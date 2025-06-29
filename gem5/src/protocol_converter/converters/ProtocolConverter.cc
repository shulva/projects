#include "ProtocolConverter.hh"

#include "base/trace.hh"
#include "sim/system.hh"

namespace gem5
{

ProtocolConverter::ProtocolConverter(const ProtocolConverterParams &p)
    : SimObject(p),
      link_to_protocol_port(p.name + ".link_to_protocol_port", this),
      protocol_to_link_port(p.name + ".protocol_to_link_port", this),
      sourceProtocolName(p.sourceProtocolName),
      targetProtocolName(p.targetProtocolName)
{
    DPRINTF(ProtocolConverter, "Created ProtocolConverter\n");
}

Port &
ProtocolConverter::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "link_to_protocol_port") {
        return link_to_protocol_port;
    } else if (if_name == "protocol_to_link_port") {
        return protocol_to_link_port;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

void
ProtocolConverter::startup()
{
    SimObject::startup();
    DPRINTF(ProtocolConverter, "ProtocolConverter startup\n");
}

// --- Link_to_Protocol ---
Tick
ProtocolConverter::Link_to_Protocol::recvAtomic(PacketPtr pkt)
{
    panic("ProtocolConverter::Link_to_Protocol::recvAtomic not implemented");
    return 0;
}

void
ProtocolConverter::Link_to_Protocol::recvFunctional(PacketPtr pkt)
{
    panic("ProtocolConverter::Link_to_Protocol::recvFunctional not implemented");
}

bool
ProtocolConverter::Link_to_Protocol::recvTimingReq(PacketPtr pkt)
{
    DPRINTF(ProtocolConverter, "收到来自链路层的Packet\n");

    // TODO: 在这里添加协议转换逻辑

    // 将接收到的Packet通过 protocol_to_link_port 发送出去
    if (owner->protocol_to_link_port.sendTimingReq(pkt)) {
        DPRINTF(ProtocolConverter, "成功通过protocol_to_link_port发送Packet\n");
        return true; // 成功发送
    } else {
        DPRINTF(ProtocolConverter, "无法通过protocol_to_link_port发送Packet，端口忙\n");
        // 如果发送失败，通常需要处理重试或丢弃
        // 这里暂时返回false表示未能立即处理
        return false;
    }
}

AddrRangeList
ProtocolConverter::Link_to_Protocol::getAddrRanges() const
{
    AddrRangeList ranges;
    ranges.push_back(AddrRange(0x1000, 0x1FFF));
    return ranges;
}

// --- Protocol_to_Link ---
bool
ProtocolConverter::Protocol_to_Link::recvTimingResp(PacketPtr pkt)
{
    DPRINTF(ProtocolConverter, "Received timing response from downstream\n");
    delete pkt;
    return true;
}

void
ProtocolConverter::Protocol_to_Link::recvReqRetry()
{
    DPRINTF(ProtocolConverter, "Received request retry from downstream\n");
}

}