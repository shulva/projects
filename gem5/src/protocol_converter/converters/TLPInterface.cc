/*
 * TLPInterface.cc
 * PCIe TLP接口实现文件
 */

#include "expr/protocol_converter/converters/ProtocolConverter.hh"
#include "expr/protocol_converter/converters/TLPInterface.hh"
#include "debug/TLPInterface.hh"
#include <cstring>

namespace gem5 {

// 构造函数
gem5::PCIe::TLPInterface::TLPInterface()
{
    DPRINTF(TLPInterface, "创建TLP接口\n");
}

// 析构函数
gem5::PCIe::TLPInterface::~TLPInterface()
{
    // 清空TLP队列
    while (!tlpQueue.empty()) {
        PCIe::TLP* tlp = tlpQueue.front();
        tlpQueue.pop();
        delete tlp;
    }
}

// 从链路层接收TLP
bool
gem5::PCIe::TLPInterface::receiveTLPFromLinkLayer(PCIe::TLP* tlp)
{
    if (!tlp) {
        return false;
    }

    DPRINTF(TLPInterface, "从链路层接收TLP: FMT_TYPE=0x%x, 地址=0x%lx\n",
            tlp->fmt_type, tlp->address);

    // 处理TLP
    bool result = processTLP(tlp);

    // 如果设置了回调，则调用回调
    if (tlpCallback) {
        tlpCallback(tlp);
    }

    return result;
}

// 向链路层发送TLP
bool
gem5::PCIe::TLPInterface::sendTLPToLinkLayer(PCIe::TLP* tlp)
{
    if (!tlp) {
        return false;
    }

    DPRINTF(TLPInterface, "向链路层发送TLP: FMT_TYPE=0x%x, 地址=0x%lx\n",
            tlp->fmt_type, tlp->address);

    // 这里应该调用链路层的接口发送TLP
    // 暂时将TLP放入队列
    tlpQueue.push(tlp);

    return true;
}

// 设置TLP处理回调
void
gem5::PCIe::TLPInterface::setTLPCallback(TLPCallback callback)
{
    tlpCallback = callback;
}

// 判断是否为内存读TLP
bool
gem5::PCIe::TLPInterface::isTLPMemoryRead(uint8_t fmt_type)
{
    return (fmt_type == PCIe::TLPFmtType::MRD_3DW) ||
           (fmt_type == PCIe::TLPFmtType::MRD_4DW);
}

// 判断是否为内存写TLP
bool
gem5::PCIe::TLPInterface::isTLPMemoryWrite(uint8_t fmt_type)
{
    return (fmt_type == PCIe::TLPFmtType::MWR_3DW) ||
           (fmt_type == PCIe::TLPFmtType::MWR_4DW);
}

// 判断是否为完成TLP
bool
gem5::PCIe::TLPInterface::isTLPCompletion(uint8_t fmt_type)
{
    return (fmt_type == PCIe::TLPFmtType::CPL) ||
           (fmt_type == PCIe::TLPFmtType::CPLD);
}

// 判断是否为配置TLP
bool
gem5::PCIe::TLPInterface::isTLPConfiguration(uint8_t fmt_type)
{
    return (fmt_type == PCIe::TLPFmtType::CFGRD0) ||
           (fmt_type == PCIe::TLPFmtType::CFGRD1) ||
           (fmt_type == PCIe::TLPFmtType::CFGWR0) ||
           (fmt_type == PCIe::TLPFmtType::CFGWR1);
}

// 判断是否为消息TLP
bool
gem5::PCIe::TLPInterface::isTLPMessage(uint8_t fmt_type)
{
    return (fmt_type == PCIe::TLPFmtType::MSG) ||
           (fmt_type == PCIe::TLPFmtType::MSGD);
}

// 获取TLP地址类型
uint8_t
gem5::PCIe::TLPInterface::getTLPAddressType(uint8_t fmt_type)
{
    if (isTLPMemoryRead(fmt_type) || isTLPMemoryWrite(fmt_type)) {
        return 0; // 内存地址
    } else if ((fmt_type == PCIe::TLPFmtType::IORD) ||
               (fmt_type == PCIe::TLPFmtType::IOWR)) {
        return 1; // IO地址
    } else if (isTLPConfiguration(fmt_type)) {
        return 2; // 配置空间地址
    } else {
        return 0; // 默认为内存地址
    }
}

// 获取TLP流量类别
uint8_t
gem5::PCIe::TLPInterface::getTLPTrafficClass(uint8_t tc_attr)
{
    return (tc_attr >> 4) & 0x07;
}

// 获取TLP缓存属性
uint8_t
gem5::PCIe::TLPInterface::getTLPCacheAttr(uint8_t tc_attr)
{
    return tc_attr & 0x0F;
}

} // namespace gem5