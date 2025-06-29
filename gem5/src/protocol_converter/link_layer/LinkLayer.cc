#include "LinkLayer.hh"

#include "base/trace.hh"
#include "sim/system.hh"

namespace gem5
{

LinkLayer::LinkLayer(const LinkLayerParams &p)
    : SimObject(p),
      link_to_converter_Port(p.name + ".controller_port", this),
      link_to_phy_Port(p.name + ".phy_port", this),
      converter_to_link_slaveport(p.name + ".converter_slaveport", this),
      link_to_phy_masterport(p.name + ".phy_masterport", this),
      toControllerBufferSize(p.to_converter_buffer_size),
      toPhyBufferSize(p.to_phy_buffer_size)
{
    DPRINTF(LinkLayer, "Created LinkLayer\n");
}

Port &
LinkLayer::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "link_to_converter_Port") {
        return link_to_converter_Port;
    } else if (if_name == "link_to_phy_Port") {
        return link_to_phy_Port;
    } else if(if_name=="converter_to_link_slaveport"){
        return converter_to_link_slaveport;
    } else if(if_name=="link_to_phy_masterport"){
        return link_to_phy_masterport;
    }
    else {
        return SimObject::getPort(if_name, idx);
    }
}

void
LinkLayer::startup()
{
    SimObject::startup();
    DPRINTF(LinkLayer, "LinkLayer startup\n");
    // TODO: 初始化流控和重传机制
}

// --- Link_to_Converter ---
bool
LinkLayer::Link_to_Converter::recvTimingResp(PacketPtr pkt)
{
    DPRINTF(LinkLayer, "Link_to_Converter: Received timing response\n");
    // 协议转换控制器通常不发送响应给链路层，除非是流控信息
    // 这里暂时简单丢弃或根据需要处理
    delete pkt;
    return true;
}

void
LinkLayer::Link_to_Converter::recvReqRetry()
{
    DPRINTF(LinkLayer, "Link_to_Converter: Received request retry\n");
    // 收到上层重试请求，安排稍后重发
    // owner->scheduleSendToController();
    // TODO: 实现重试逻辑
}

// --- Link_to_Phy ---
Tick
LinkLayer::Link_to_Phy::recvAtomic(PacketPtr pkt)
{
    panic("LinkLayer::Link_to_Phy::recvAtomic not implemented");
    return 0;
}

void
LinkLayer::Link_to_Phy::recvFunctional(PacketPtr pkt)
{
    panic("LinkLayer::Link_to_Phy::recvFunctional not implemented");
}

bool
LinkLayer::Link_to_Phy::recvTimingReq(PacketPtr pkt)
{
    DPRINTF(LinkLayer, "link layer: 成功接收Packet\n");

    if(true){//owner->isTLP(pkt) isTLP尚有问题
        //TODO 先直接转发到上层，buffer暂时不设置
        DPRINTF(LinkLayer, "link layer: 发送TLP至协议层\n");
        owner->link_to_converter_Port.sendTimingReq(pkt);
    }
    else if(owner->isDLP(pkt)){
        //TODO
    }
    else
        return false;

    return true; // 假设总能接受
}

AddrRangeList
LinkLayer::Link_to_Phy::getAddrRanges() const
{
    // 链路层不直接处理地址范围，它透传数据包
    // 但端口需要返回一个范围，否则会出错
    // 可以返回一个全地址范围，或者根据连接的物理层来决定

    AddrRangeList ranges;
    ranges.push_back(AddrRange(0x1000, 0x1FFF));  // 地址范围：0x1000 ~ 0x1FFF,暂时写一个
    return ranges;
}

bool LinkLayer::Converter_to_Link::recvTimingReq(PacketPtr pkt) {
    DPRINTF(LinkLayer, "接收到来自协议层的转换包\n");
    owner->link_to_phy_masterport.sendTimingReq(pkt);
    return true;
}

bool LinkLayer::isDLP(PacketPtr pkt) {
    // 1. 大小判断：DLLP是6字节（1字节类型 + 3字节数据 + 2字节CRC）
    if (pkt->getSize() != 6) {
        return false;
    }
    
    // 2. 格式判断：检查DLLP特征
    uint8_t* data = pkt->getPtr<uint8_t>();
    
    // DLLP头部格式：
    // - 前2位是类型(Type)
    // - 接下来6位是子类型(Subtype)
    uint8_t type = (data[0] >> 6) & 0x3;     // 取前2位
    uint8_t subtype = data[0] & 0x3F;        // 取后6位
    
    // DLLP的Type字段固定为01b
    // Subtype范围通常在0-15之间
    return (type == 1) && (subtype <= 15);
}

bool LinkLayer::isTLP(PacketPtr pkt) {
    // 如果是DLLP就一定不是TLP
    if (isDLP(pkt)) {
        return false;
    }
    
    // TLP特征判断
    uint8_t* data = pkt->getPtr<uint8_t>();
    uint8_t fmt = (data[0] >> 5) & 0x7;   // 前3位
    uint8_t type = data[0] & 0x1F;        // 后5位
    
    // TLP的Type字段范围通常在0-5之间
    // Fmt字段对于常见TLP类型为0-4
    return (type <= 5) && (fmt <= 4);
}
}