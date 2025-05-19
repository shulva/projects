/*
 * ProtocolConverter.cc
 * 协议转换器基类实现文件
 */

#include "expr/protocol_converter/converters/ProtocolConverter.hh"
#include "base/trace.hh"
#include "debug/ProtocolConverter.hh"

namespace gem5 {

ProtocolConverter::ProtocolConverter(const ProtocolConverterParams& params)
    : SimObject(params),
      sourceProtocolName(params.sourceProtocolName),
      targetProtocolName(params.targetProtocolName)
{
    DPRINTF(ProtocolConverter, "创建协议转换器: 源协议=%s, 目标协议=%s\n", 
            sourceProtocolName.c_str(), targetProtocolName.c_str());
}

void 
ProtocolConverter::init()
{
    // 调用父类初始化
    SimObject::init();
    
    DPRINTF(ProtocolConverter, "协议转换器初始化\n");
}

void 
ProtocolConverter::startup()
{
    // 调用父类启动
    SimObject::startup();
    
    DPRINTF(ProtocolConverter, "协议转换器启动\n");
}

void 
ProtocolConverter::setLinkLayerA(PCIeLinkLayer* linkLayer)
{
    linkLayerA = linkLayer;
    
    // 设置链路层回调
    if (linkLayerA) {
        linkLayerA->setProtocolCallback(
            [this](PCIe::TLP* tlp) {
                return this->handlePacketFromLinkLayerA(tlp);
            });
    }
    
    DPRINTF(ProtocolConverter, "设置链路层A: %s\n", 
            linkLayerA ? "成功" : "失败");
}

void 
ProtocolConverter::setLinkLayerB(PCIeLinkLayer* linkLayer)
{
    linkLayerB = linkLayer;
    
    // 设置链路层回调
    if (linkLayerB) {
        linkLayerB->setProtocolCallback(
            [this](PCIe::TLP* tlp) {
                return this->handlePacketFromLinkLayerB(tlp);
            });
    }
    
    DPRINTF(ProtocolConverter, "设置链路层B: %s\n", 
            linkLayerB ? "成功" : "失败");
}

bool 
ProtocolConverter::handlePacketFromLinkLayerA(PCIe::TLP* tlp)
{
    // 处理从链路层A接收的数据包
    DPRINTF(ProtocolConverter, "处理来自链路层A的数据包\n");
    
    if (!tlp) {
        DPRINTF(ProtocolConverter, "数据包为空\n");
        return false;
    }
    
    // 将源协议A转换为UPF
    std::shared_ptr<upf::Packet> upfPacket;
    ConversionResult sourceToUPFResult = sourceToUPF(tlp, upfPacket);
    
    if (sourceToUPFResult != ConversionResult::SUCCESS) {
        DPRINTF(ProtocolConverter, "源协议A到UPF的转换失败: %d\n", 
                static_cast<int>(sourceToUPFResult));
        return false;
    }
    
    // 将UPF转换为目标协议B
    void* targetPacket = nullptr;
    ConversionResult UPFToTargetResult = UPFToTarget(upfPacket, targetPacket);
    
    if (UPFToTargetResult != ConversionResult::SUCCESS || !targetPacket) {
        DPRINTF(ProtocolConverter, "UPF到目标协议B的转换失败: %d\n", 
                static_cast<int>(UPFToTargetResult));
        return false;
    }
    
    // 将转换后的数据包发送到链路层B
    if (linkLayerB) {
        return linkLayerB->processTLP(static_cast<PCIe::TLP*>(targetPacket));
    } else {
        DPRINTF(ProtocolConverter, "链路层B未设置，无法发送数据包\n");
        return false;
    }
}

bool 
ProtocolConverter::handlePacketFromLinkLayerB(PCIe::TLP* tlp)
{
    // 处理从链路层B接收的数据包
    DPRINTF(ProtocolConverter, "处理来自链路层B的数据包\n");
    
    if (!tlp) {
        DPRINTF(ProtocolConverter, "数据包为空\n");
        return false;
    }
    
    // 将源协议B转换为UPF
    std::shared_ptr<upf::Packet> upfPacket;
    ConversionResult sourceToUPFResult = sourceToUPF(tlp, upfPacket);
    
    if (sourceToUPFResult != ConversionResult::SUCCESS) {
        DPRINTF(ProtocolConverter, "源协议B到UPF的转换失败: %d\n", 
                static_cast<int>(sourceToUPFResult));
        return false;
    }
    
    // 将UPF转换为目标协议A
    void* targetPacket = nullptr;
    ConversionResult UPFToTargetResult = UPFToTarget(upfPacket, targetPacket);
    
    if (UPFToTargetResult != ConversionResult::SUCCESS || !targetPacket) {
        DPRINTF(ProtocolConverter, "UPF到目标协议A的转换失败: %d\n", 
                static_cast<int>(UPFToTargetResult));
        return false;
    }
    
    // 将转换后的数据包发送到链路层A
    if (linkLayerA) {
        return linkLayerA->processTLP(static_cast<PCIe::TLP*>(targetPacket));
    } else {
        DPRINTF(ProtocolConverter, "链路层A未设置，无法发送数据包\n");
        return false;
    }
}

bool 
ProtocolConverter::sendPacketToLinkLayerA(void* packet)
{
    // 发送数据包到链路层A
    DPRINTF(ProtocolConverter, "发送数据包到链路层A\n");
    
    if (!packet) {
        DPRINTF(ProtocolConverter, "数据包为空\n");
        return false;
    }
    
    if (!linkLayerA) {
        DPRINTF(ProtocolConverter, "链路层A未设置，无法发送数据包\n");
        return false;
    }
    
    // 将数据包转换为TLP类型
    PCIe::TLP* tlp = static_cast<PCIe::TLP*>(packet);
    
    // 发送到链路层A
    return linkLayerA->processTLP(tlp);
}

bool 
ProtocolConverter::sendPacketToLinkLayerB(void* packet)
{
    // 发送数据包到链路层B
    DPRINTF(ProtocolConverter, "发送数据包到链路层B\n");
    
    if (!packet) {
        DPRINTF(ProtocolConverter, "数据包为空\n");
        return false;
    }
    
    if (!linkLayerB) {
        DPRINTF(ProtocolConverter, "链路层B未设置，无法发送数据包\n");
        return false;
    }
    
    // 将数据包转换为TLP类型
    PCIe::TLP* tlp = static_cast<PCIe::TLP*>(packet);
    
    // 发送到链路层B
    return linkLayerB->processTLP(tlp);
}

bool 
ProtocolConverter::convertAndForward(void* sourcePacket, bool fromA)
{
    // 转换并转发数据包
    DPRINTF(ProtocolConverter, "转换并转发数据包，来源: %s\n", 
            fromA ? "链路层A" : "链路层B");
    
    if (!sourcePacket) {
        DPRINTF(ProtocolConverter, "源数据包为空\n");
        return false;
    }
    
    // 将源协议转换为UPF
    std::shared_ptr<upf::Packet> upfPacket;
    ConversionResult sourceToUPFResult = sourceToUPF(sourcePacket, upfPacket);
    
    if (sourceToUPFResult != ConversionResult::SUCCESS) {
        DPRINTF(ProtocolConverter, "源协议到UPF的转换失败: %d\n", 
                static_cast<int>(sourceToUPFResult));
        return false;
    }
    
    // 将UPF转换为目标协议
    void* targetPacket = nullptr;
    ConversionResult UPFToTargetResult = UPFToTarget(upfPacket, targetPacket);
    
    if (UPFToTargetResult != ConversionResult::SUCCESS || !targetPacket) {
        DPRINTF(ProtocolConverter, "UPF到目标协议的转换失败: %d\n", 
                static_cast<int>(UPFToTargetResult));
        return false;
    }
    
    // 根据来源选择目标链路层
    if (fromA) {
        return sendPacketToLinkLayerB(targetPacket);
    } else {
        return sendPacketToLinkLayerA(targetPacket);
    }
}

} // namespace gem5