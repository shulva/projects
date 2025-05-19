/*
 * GenericPacketProcessor.cc
 * 通用数据包处理器实现文件
 */

#include "expr/protocol_converter/GenericPacketProcessor.hh"
#include "base/trace.hh"
#include "debug/GenericPacketProcessor.hh"
#include "upf/UPF.hh"
#include "expr/protocol_converter/link_layer/PCIeLinkLayer.hh" // 包含PCIeLinkLayer头文件以使用DataDLP和ControlDLLP结构

namespace gem5 {

// 注册调试标志

GenericPacketProcessor::GenericPacketProcessor(const GenericPacketProcessorParams& params)
    : SimObject(params),
      physicalLayer(params.physical_layer),
      linkLayer(params.link_layer),
      protocolConverter(params.protocol_converter)
{
    DPRINTF(GenericPacketProcessor, "创建通用数据包处理器\n");

    // 设置物理层回调
    physicalLayer->setLinkLayerCallback(
        [this](PhyPacket* packet) {
            return this->handlePhyPacket(packet);
        });

    // 设置链路层回调
    linkLayer->setPhyCallback(
        [this](PhyPacket* packet) { // 修改 lambda 签名以匹配 PhyCallback
            return this->handleLinkLayerPacket(packet->data.data(),packet->getSize()); 
        });

    // 设置协议转换器回调
    // ... existing code ...
}

void 
GenericPacketProcessor::init()
{
    // 调用父类初始化
    SimObject::init();
    
    DPRINTF(GenericPacketProcessor, "通用数据包处理器初始化\n");
}

void 
GenericPacketProcessor::startup()
{
    // 调用父类启动
    SimObject::startup();
    
    DPRINTF(GenericPacketProcessor, "通用数据包处理器启动\n");
    
    // 注册定期处理队列的事件
    schedule(new EventFunctionWrapper([this]{ processQueuedPackets(); }, 
                                     name() + ".processQueueEvent", true), 
             curTick() + 100);
}

bool 
GenericPacketProcessor::receivePacket(void* packet, uint32_t size, PhyProtocolType protocol)
{
    // 接收数据包
    DPRINTF(GenericPacketProcessor, "接收数据包: 大小=%d 字节, 协议=%d\n", size, protocol);
    
    // 检查物理层状态
    if (physicalLayer->getLinkState() != LINK_UP) {
        DPRINTF(GenericPacketProcessor, "物理层链路未就绪，无法接收数据包\n");
        return false;
    }
    
    // 将数据包发送到物理层
    return physicalLayer->sendPacket(packet, size);
}

bool 
GenericPacketProcessor::sendPacket(void* packet, uint32_t size, PhyProtocolType protocol)
{
    // 发送数据包
    DPRINTF(GenericPacketProcessor, "发送数据包: 大小=%d 字节, 协议=%d\n", size, protocol);
    
    // 检查物理层状态
    if (physicalLayer->getLinkState() != LINK_UP) {
        DPRINTF(GenericPacketProcessor, "物理层链路未就绪，无法发送数据包\n");
        return false;
    }
    
    // 将数据包加入队列
    packetQueue.push(std::make_pair(packet, size));
    
    return true;
}

bool 
GenericPacketProcessor::handlePhyPacket(PhyPacket* packet)
{
    // 处理从物理层接收的数据包
    DPRINTF(GenericPacketProcessor, "处理物理层数据包: 大小=%d 字节\n", packet->getSize());
    
    // 识别协议和数据包类型
    PhyProtocolType protocol = physicalLayer->identifyProtocol(packet);
    PhyPacketType packetType = physicalLayer->identifyPacketType(packet);
    
    // 提取数据
    uint8_t* data = packet->data.data();
    uint32_t size = packet->getSize();
    
    // 创建数据副本
    void* dataCopy = malloc(size);
    memcpy(dataCopy, data, size);
    
    // 将数据包传递给链路层处理
    bool result = linkLayer->processTLP(static_cast<PCIe::TLP*>(dataCopy));
    
    if (!result) {
        DPRINTF(GenericPacketProcessor, "链路层处理数据包失败\n");
        free(dataCopy);
        return false;
    }
    
    return true;
}

bool 
GenericPacketProcessor::handleLinkLayerPacket(void* data, uint32_t size)
{
    // 处理从链路层接收的数据包
    DPRINTF(GenericPacketProcessor, "处理链路层数据包: 大小=%d 字节\n", size);
    
    // 进行协议转换
    void* convertedPacket = nullptr;
    bool conversionResult = handleProtocolConversion(data, convertedPacket);
    
    if (!conversionResult || !convertedPacket) {
        DPRINTF(GenericPacketProcessor, "协议转换失败\n");
        return false;
    }
    
    // 将转换后的数据包发送到物理层
    return physicalLayer->sendPacket(convertedPacket, size);
}

bool 
GenericPacketProcessor::handleProtocolConversion(void* sourcePacket, void*& targetPacket)
{
    // 处理协议转换
    DPRINTF(GenericPacketProcessor, "处理协议转换\n");
    
    // 创建UPF数据包
    std::shared_ptr<upf::Packet> upfPacket;
    
    // 源协议到UPF的转换
    gem5::ConversionResult sourceToUPFResult = 
        protocolConverter->sourceToUPF(sourcePacket, upfPacket);
    
    if (sourceToUPFResult !=gem5::ConversionResult::SUCCESS) {
        DPRINTF(GenericPacketProcessor, "源协议到UPF的转换失败: %d\n", 
                static_cast<int>(sourceToUPFResult));
        return false;
    }
    
    // UPF到目标协议的转换
    gem5::ConversionResult UPFToTargetResult = 
        protocolConverter->UPFToTarget(upfPacket, targetPacket);
    
    if (UPFToTargetResult !=gem5::ConversionResult::SUCCESS) {
        DPRINTF(GenericPacketProcessor, "UPF到目标协议的转换失败: %d\n", 
                static_cast<int>(UPFToTargetResult));
        return false;
    }
    
    return true;
}

void 
GenericPacketProcessor::processQueuedPackets()
{
    // 处理队列中的数据包
    DPRINTF(GenericPacketProcessor, "处理队列中的数据包，队列大小: %d\n", packetQueue.size());
    
    // 处理队列中的所有数据包
    while (!packetQueue.empty()) {
        // 获取队列中的第一个数据包
        std::pair<void*, uint32_t> packetPair = packetQueue.front();
        packetQueue.pop();
        
        // 发送数据包
        bool result = physicalLayer->sendPacket(packetPair.first, packetPair.second);
        
        if (!result) {
            DPRINTF(GenericPacketProcessor, "发送数据包失败，重新加入队列\n");
            packetQueue.push(packetPair);
            break;
        }
    }
    
    // 重新调度事件
    schedule(new EventFunctionWrapper([this]{ processQueuedPackets(); }, 
                                     name() + ".processQueueEvent", true), 
             curTick() + 100);
}

} // namespace gem5