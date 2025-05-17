/*
 * PCIeLinkLayer.cc
 * PCIe链路层实现文件
 */

#include "protocol_converter/link_layer/PCIeLinkLayer.hh"
#include "base/trace.hh"
#include "debug/PCIeLinkLayer.hh"
#include "params/PCIeLinkLayer.hh"
#include "sim/system.hh"

namespace gem5 {

// 注册调试标志
Trace::CreateDebugFlag("PCIeLinkLayer", "PCIe链路层调试信息");

PCIeLinkLayer::PCIeLinkLayer(const PCIeLinkLayerParams &p)
    : SimObject(p),
      nextTxSequenceNumber(0),
      expectedRxSequenceNumber(0),
      phyCallback(nullptr),
      protocolCallback(nullptr)
{
    // 初始化接收缓冲区
    rxBuffer.maxSize = p.rxBufferSize;
    rxBuffer.currentSize = 0;
    
    // 初始化发送缓冲区
    txBuffer.maxSize = p.txBufferSize;
    txBuffer.currentSize = 0;
    
    // 初始化流控状态
    txFlowControl.availableCredits = p.initialTxCredits;
    rxFlowControl.availableCredits = p.initialRxCredits;
    
    DPRINTF(PCIeLinkLayer, "PCIe链路层创建: rxBufferSize=%d, txBufferSize=%d\n", 
            p.rxBufferSize, p.txBufferSize);
}

void 
PCIeLinkLayer::init()
{
    // 调用父类初始化
    SimObject::init();
    
    DPRINTF(PCIeLinkLayer, "PCIe链路层初始化\n");
}

void 
PCIeLinkLayer::startup()
{
    // 调用父类启动
    SimObject::startup();
    
    // 注册定期事件
    // 流控更新事件
    schedule(new EventFunctionWrapper([this]{ periodicFlowControlUpdate(); }, 
                                     name() + ".flowControlEvent", true), 
             curTick() + 1000);  // 1000个周期后触发
    
    // 重传超时事件
    schedule(new EventFunctionWrapper([this]{ handleRetryTimeout(); }, 
                                     name() + ".retryTimeoutEvent", true), 
             curTick() + 5000);  // 5000个周期后触发
    
    DPRINTF(PCIeLinkLayer, "PCIe链路层启动\n");
}

bool 
PCIeLinkLayer::processTLP(PCIe::TLP* tlp)
{
    // 处理来自协议层的TLP
    DPRINTF(PCIeLinkLayer, "处理来自协议层的TLP: %s\n", tlp->toString().c_str());
    
    // 检查发送缓冲区是否有空间
    if (txBuffer.currentSize >= txBuffer.maxSize) {
        DPRINTF(PCIeLinkLayer, "发送缓冲区已满，无法处理TLP\n");
        return false;
    }
    
    // 检查流控状态
    if (!checkFlowControl(tlp)) {
        DPRINTF(PCIeLinkLayer, "流控检查失败，无法发送TLP\n");
        return false;
    }
    
    // 将TLP加入发送缓冲区
    txBuffer.tlpQueue.push(tlp);
    txBuffer.currentSize++;
    
    // 更新流控状态
    txFlowControl.consumedCredits += calculateTLPSize(tlp);
    txFlowControl.availableCredits -= calculateTLPSize(tlp);
    
    // 封装TLP为数据DLP
    DataDLP* dataDlp = encapsulateTLP(tlp);
    
    // 发送数据DLP
    sendDataDLP(dataDlp);
    
    return true;
}

// 删除TLPToUPF和UPFToTLP方法

bool 
PCIeLinkLayer::receiveDataDLP(DataDLP* dataDlp)
{
    // 接收来自物理层的数据DLP
    DPRINTF(PCIeLinkLayer, "接收数据DLP，序列号: %d\n", dataDlp->getSequenceNumber());
    
    // 验证CRC
    if (!validateLCRC(dataDlp)) {
        DPRINTF(PCIeLinkLayer, "数据DLP CRC验证失败，发送NAK\n");
        
        // 发送NAK
        ControlDLLP* nakDllp = createNakDLLP(expectedRxSequenceNumber);
        sendControlDLLP(nakDllp);
        
        // 释放资源
        delete nakDllp;
        return false;
    }
    
    // 检查序列号
    uint16_t seqNum = dataDlp->getSequenceNumber();
    if (seqNum != expectedRxSequenceNumber) {
        DPRINTF(PCIeLinkLayer, "序列号不匹配，期望: %d, 实际: %d，发送NAK\n", 
                expectedRxSequenceNumber, seqNum);
        
        // 发送NAK
        ControlDLLP* nakDllp = createNakDLLP(expectedRxSequenceNumber);
        sendControlDLLP(nakDllp);
        
        // 释放资源
        delete nakDllp;
        return false;
    }
    
    // 提取TLP
    PCIe::TLP* tlp = extractTLP(dataDlp);
    
    // 更新期望的序列号
    expectedRxSequenceNumber = (expectedRxSequenceNumber + 1) & 0xFFF;
    
    // 发送ACK
    ControlDLLP* ackDllp = createAckDLLP(seqNum);
    sendControlDLLP(ackDllp);
    
    // 检查接收缓冲区是否有空间
    if (rxBuffer.currentSize >= rxBuffer.maxSize) {
        DPRINTF(PCIeLinkLayer, "接收缓冲区已满，丢弃TLP\n");
        
        // 释放资源
        delete ackDllp;
        delete tlp;
        return false;
    }
    
    // 将TLP加入接收缓冲区
    rxBuffer.tlpQueue.push(tlp);
    rxBuffer.currentSize++;
    
    // 更新流控状态
    rxFlowControl.consumedCredits += calculateTLPSize(tlp);
    rxFlowControl.returnedCredits += calculateTLPSize(tlp);
    
    // 如果有协议层回调，则调用
    if (protocolCallback) {
        // 从接收缓冲区取出TLP
        PCIe::TLP* frontTlp = rxBuffer.tlpQueue.front();
        rxBuffer.tlpQueue.pop();
        rxBuffer.currentSize--;
        
        // 调用协议层回调
        bool result = protocolCallback(frontTlp);
        
        if (!result) {
            DPRINTF(PCIeLinkLayer, "协议层处理TLP失败\n");
            // 如果协议层处理失败，将TLP重新放回接收缓冲区
            rxBuffer.tlpQueue.push(frontTlp);
            rxBuffer.currentSize++;
        }
    }
    
    // 释放资源
    delete ackDllp;
    
    return true;
}

bool 
PCIeLinkLayer::receiveControlDLLP(ControlDLLP* controlDllp)
{
    // 接收来自物理层的控制DLLP
    DPRINTF(PCIeLinkLayer, "接收控制DLLP，类型: %d\n", controlDllp->type);
    
    // 验证CRC
    if (!validateDLLPCRC(controlDllp)) {
        DPRINTF(PCIeLinkLayer, "控制DLLP CRC验证失败，忽略\n");
        return false;
    }
    
    // 根据DLLP类型处理
    switch (controlDllp->type) {
        case DLLP_TYPE_FC_UPDATE:
            // 处理流控更新
            DPRINTF(PCIeLinkLayer, "处理流控更新: VC=%d, 类型=%d, 信用点=%d\n", 
                    controlDllp->fc.vcID, controlDllp->fc.fcType, controlDllp->fc.credits);
            
            // 更新流控状态
            updateFlowControlCredits(controlDllp->fc.vcID, 
                                    controlDllp->fc.fcType, 
                                    controlDllp->fc.credits);
            break;
            
        case DLLP_TYPE_ACK:
            // 处理确认
            DPRINTF(PCIeLinkLayer, "处理ACK: 序列号=%d\n", 
                    controlDllp->ack_nak.sequenceNum);
            
            // 处理确认
            processAcknowledgement(controlDllp->ack_nak.sequenceNum);
            break;
            
        case DLLP_TYPE_NAK:
            // 处理否认
            DPRINTF(PCIeLinkLayer, "处理NAK: 序列号=%d\n", 
                    controlDllp->ack_nak.sequenceNum);
            
            // 处理否认
            processNegativeAcknowledgement(controlDllp->ack_nak.sequenceNum);
            break;
            
        default:
            // 未知类型
            DPRINTF(PCIeLinkLayer, "未知控制DLLP类型: %d\n", controlDllp->type);
            return false;
    }
    
    return true;
}

bool 
PCIeLinkLayer::sendDataDLP(DataDLP* dataDlp)
{
    // 发送数据DLP到物理层
    DPRINTF(PCIeLinkLayer, "发送数据DLP，序列号: %d\n", dataDlp->getSequenceNumber());
    
    // 计算CRC
    dataDlp->lcrc = calculateLCRC(dataDlp);
    
    // 将DLP保存到重传缓冲区
    retryBuffer[dataDlp->getSequenceNumber()] = dataDlp;
    
    // 如果有物理层回调，则调用
    if (phyCallback) {
        bool result = phyCallback(dataDlp);
        
        if (!result) {
            DPRINTF(PCIeLinkLayer, "物理层发送DLP失败\n");
            return false;
        }
    } else {
        DPRINTF(PCIeLinkLayer, "物理层回调未设置，无法发送DLP\n");
        return false;
    }
    
    return true;
}

bool 
PCIeLinkLayer::sendControlDLLP(ControlDLLP* controlDllp)
{
    // 发送控制DLLP到物理层
    DPRINTF(PCIeLinkLayer, "发送控制DLLP，类型: %d\n", controlDllp->type);
    
    // 计算CRC
    controlDllp->crc = calculateDLLPCRC(controlDllp);
    
    // 如果有物理层回调，则调用
    if (phyCallback) {
        bool result = phyCallback(controlDllp);
        
        if (!result) {
            DPRINTF(PCIeLinkLayer, "物理层发送DLLP失败\n");
            return false;
        }
    } else {
        DPRINTF(PCIeLinkLayer, "物理层回调未设置，无法发送DLLP\n");
        return false;
    }
    
    return true;
}

ControlDLLP* 
PCIeLinkLayer::createFlowControlDLLP(uint8_t vcID, uint8_t fcType, uint16_t credits)
{
    // 创建流控DLLP
    ControlDLLP* dllp = new ControlDLLP();
    
    // 设置类型
    dllp->type = DLLP_TYPE_FC_UPDATE;
    
    // 设置流控信息
    dllp->fc.vcID = vcID;
    dllp->fc.fcType = fcType;
    dllp->fc.credits = credits;
    
    // CRC将在发送时计算
    dllp->crc = 0;
    
    return dllp;
}

ControlDLLP* 
PCIeLinkLayer::createAckDLLP(uint16_t sequenceNum)
{
    // 创建ACK DLLP
    ControlDLLP* dllp = new ControlDLLP();
    
    // 设置类型
    dllp->type = DLLP_TYPE_ACK;
    
    // 设置ACK信息
    dllp->ack_nak.sequenceNum = sequenceNum;
    dllp->ack_nak.reserved = 0;
    
    // CRC将在发送时计算
    dllp->crc = 0;
    
    return dllp;
}

ControlDLLP* 
PCIeLinkLayer::createNakDLLP(uint16_t sequenceNum)
{
    // 创建NAK DLLP
    ControlDLLP* dllp = new ControlDLLP();
    
    // 设置类型
    dllp->type = DLLP_TYPE_NAK;
    
    // 设置NAK信息
    dllp->ack_nak.sequenceNum = sequenceNum;
    dllp->ack_nak.reserved = 0;
    
    // CRC将在发送时计算
    dllp->crc = 0;
    
    return dllp;
}

void 
PCIeLinkLayer::setPhyCallback(PhyCallback callback)
{
    // 设置物理层回调
    phyCallback = callback;
    DPRINTF(PCIeLinkLayer, "设置物理层回调\n");
}

void 
PCIeLinkLayer::setProtocolCallback(ProtocolCallback callback)
{
    // 设置协议层回调
    protocolCallback = callback;
    DPRINTF(PCIeLinkLayer, "设置协议层回调\n");
}

DataDLP* 
PCIeLinkLayer::encapsulateTLP(PCIe::TLP* tlp)
{
    // 封装TLP为数据DLP
    DPRINTF(PCIeLinkLayer, "封装TLP为数据DLP\n");
    
    // 创建数据DLP
    DataDLP* dataDlp = new DataDLP();
    
    // 设置序列号
    dataDlp->setSequenceNumber(nextTxSequenceNumber);
    
    // 更新下一个发送序列号
    nextTxSequenceNumber = (nextTxSequenceNumber + 1) & 0xFFF;
    
    // 设置链路控制位
    dataDlp->setLinkControl(0x5); // 示例链路控制值
    
    // 设置TLP
    dataDlp->tlp = tlp;
    
    // LCRC将在发送时计算
    dataDlp->lcrc = 0;
    
    return dataDlp;
}

PCIe::TLP* 
PCIeLinkLayer::extractTLP(DataDLP* dataDlp)
{
    // 从数据DLP提取TLP
    DPRINTF(PCIeLinkLayer, "从数据DLP提取TLP\n");
    
    // 直接返回TLP指针
    return dataDlp->tlp;
}

uint32_t 
PCIeLinkLayer::calculateLCRC(DataDLP* dataDlp)
{
    // 计算数据DLP的LCRC
    // 注意：这里只是一个简单的实现，实际CRC计算应该更复杂
    
    // 简单的CRC计算示例
    uint32_t crc = 0xFFFFFFFF;
    
    // 对头部进行CRC计算
    crc ^= dataDlp->header;
    
    // 对TLP进行CRC计算
    if (dataDlp->tlp) {
        // 这里应该对TLP的所有字段进行CRC计算
        // 简化实现，仅使用地址和数据大小
        if (dataDlp->tlp->hasAddr) {
            crc ^= dataDlp->tlp->address;
        }
        
        if (dataDlp->tlp->hasData) {
            crc ^= dataDlp->tlp->dataSize;
        }
    }
    
    return crc;
}

uint16_t 
PCIeLinkLayer::calculateDLLPCRC(ControlDLLP* controlDllp)
{
    // 计算控制DLLP的CRC
    // 注意：这里只是一个简单的实现，实际CRC计算应该更复杂
    
    // 简单的CRC计算示例
    uint16_t crc = 0xFFFF;
    
    // 对类型进行CRC计算
    crc ^= controlDllp->type;
    
    // 根据类型对不同字段进行CRC计算
    switch (controlDllp->type) {
        case DLLP_TYPE_FC_UPDATE:
            crc ^= controlDllp->fc.vcID;
            crc ^= controlDllp->fc.fcType;
            crc ^= controlDllp->fc.credits;
            break;
            
        case DLLP_TYPE_ACK:
        case DLLP_TYPE_NAK:
            crc ^= controlDllp->ack_nak.sequenceNum;
            break;
    }
    
    return crc;
}

bool 
PCIeLinkLayer::validateLCRC(DataDLP* dataDlp)
{
    // 验证数据DLP的LCRC
    uint32_t calculatedCRC = calculateLCRC(dataDlp);
    return calculatedCRC == dataDlp->lcrc;
}

bool 
PCIeLinkLayer::validateDLLPCRC(ControlDLLP* controlDllp)
{
    // 验证控制DLLP的CRC
    uint16_t calculatedCRC = calculateDLLPCRC(controlDllp);
    return calculatedCRC == controlDllp->crc;
}

void 
PCIeLinkLayer::processAcknowledgement(uint16_t sequenceNum)
{
    // 处理确认
    DPRINTF(PCIeLinkLayer, "处理确认，序列号: %d\n", sequenceNum);
    
    // 查找重传缓冲区中的DLP
    auto it = retryBuffer.find(sequenceNum);
    if (it != retryBuffer.end()) {
        // 从重传缓冲区中移除
        DataDLP* dataDlp = it->second;
        retryBuffer.erase(it);
        
        // 释放资源
        delete dataDlp;
    }
}

void 
PCIeLinkLayer::processNegativeAcknowledgement(uint16_t sequenceNum)
{
    // 处理否认
    DPRINTF(PCIeLinkLayer, "处理否认，序列号: %d\n", sequenceNum);
    
    // 查找重传缓冲区中的DLP
    auto it = retryBuffer.find(sequenceNum);
    if (it != retryBuffer.end()) {
        // 获取DLP
        DataDLP* dataDlp = it->second;
        
        // 重新发送
        sendDataDLP(dataDlp);
    }
}

void 
PCIeLinkLayer::updateFlowControlCredits(uint8_t vcID, uint8_t fcType, uint16_t credits)
{
    // 更新流控状态
    DPRINTF(PCIeLinkLayer, "更新流控状态: VC=%d, 类型=%d, 信用点=%d\n", 
            vcID, fcType, credits);
    
    // 更新可用信用点
    txFlowControl.availableCredits = credits;
}

bool 
PCIeLinkLayer::checkFlowControl(PCIe::TLP* tlp)
{
    // 检查流控状态
    uint32_t requiredCredits = calculateTLPSize(tlp);
    
    // 检查是否有足够的信用点
    return txFlowControl.availableCredits >= requiredCredits;
}

void 
PCIeLinkLayer::periodicFlowControlUpdate()
{
    // 定期更新流控
    DPRINTF(PCIeLinkLayer, "定期更新流控\n");
    
    // 检查是否需要发送流控更新
    if (rxFlowControl.returnedCredits > 0) {
        // 创建流控DLLP
        ControlDLLP* fcDllp = createFlowControlDLLP(0, 0, rxFlowControl.availableCredits);
        
        // 发送流控DLLP
        sendControlDLLP(fcDllp);
        
        // 重置已返回信用点
        rxFlowControl.returnedCredits = 0;
        
        // 释放资源
        delete fcDllp;
    }
    
    // 重新调度事件
    schedule(new EventFunctionWrapper([this]{ periodicFlowControlUpdate(); }, 
                                     name() + ".flowControlEvent", true), 
             curTick() + 1000);  // 1000个周期后再次触发
}

void 
PCIeLinkLayer::handleRetryTimeout()
{
    // 重传超时处理
    DPRINTF(PCIeLinkLayer, "处理重传超时\n");
    
    // 检查重传缓冲区中是否有需要重传的DLP
    for (auto& pair : retryBuffer) {
        // 获取DLP
        DataDLP* dataDlp = pair.second;
        
        // 重新发送
        sendDataDLP(dataDlp);
    }
    
    // 重新调度事件
    schedule(new EventFunctionWrapper([this]{ handleRetryTimeout(); }, 
                                     name() + ".retryTimeoutEvent", true), 
             curTick() + 5000);  // 5000个周期后再次触发
}

uint32_t 
PCIeLinkLayer::calculateTLPSize(PCIe::TLP* tlp)
{
    // 计算TLP大小
    // 注意：这里只是一个简单的实现，实际大小计算应该更复杂
    
    // 基本大小
    uint32_t size = 1;  // 至少1个信用点
    
    // 如果有数据，增加数据大小
    if (tlp->hasData) {
        // 每4字节数据需要1个信用点
        size += (tlp->dataSize + 3) / 4;
    }
    
    return size;
}

} // namespace gem5