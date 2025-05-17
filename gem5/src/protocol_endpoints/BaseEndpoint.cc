#include "protocol_endpoints/BaseEndpoint.hh"

namespace gem5
{

namespace protocol_endpoints
{

BaseEndpoint::BaseEndpoint(const BaseEndpointParams &params)
    : SimObject(params),
      startAddr(params.start_addr),
      endAddr(params.end_addr),
      masterPort(name() + ".master_port", this),
      slavePort(name() + ".slave_port", this)
{
}

void
BaseEndpoint::regStats()
{
    SimObject::regStats();
    
    stats.txPackets
        .name(name() + ".tx_packets")
        .desc("发送的数据包数量")
        .flags(Stats::nozero);
    
    stats.rxPackets
        .name(name() + ".rx_packets")
        .desc("接收的数据包数量")
        .flags(Stats::nozero);
    
    stats.txByType
        .name(name() + ".tx_by_type")
        .desc("按类型分类的发送数据包")
        .flags(Stats::nozero);
    
    stats.rxByType
        .name(name() + ".rx_by_type")
        .desc("按类型分类的接收数据包")
        .flags(Stats::nozero);
    
    stats.latency
        .name(name() + ".latency")
        .desc("数据包处理延迟")
        .flags(Stats::nozero);
}

Port &
BaseEndpoint::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "master_port") {
        return masterPort;
    } else if (if_name == "slave_port") {
        return slavePort;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

// MasterPort 实现
BaseEndpoint::MasterPort::MasterPort(const std::string &name, BaseEndpoint *_owner)
    : RequestPort(name), owner(_owner)
{
}

bool
BaseEndpoint::MasterPort::recvTimingResp(PacketPtr pkt)
{
    // 处理接收到的响应
    return owner->handlePacket(pkt);
}

void
BaseEndpoint::MasterPort::recvReqRetry()
{
    // 处理请求重试
}

// SlavePort 实现
BaseEndpoint::SlavePort::SlavePort(const std::string &name, BaseEndpoint *_owner)
    : ResponsePort(name), owner(_owner)
{
}

bool
BaseEndpoint::SlavePort::recvTimingReq(PacketPtr pkt)
{
    // 处理接收到的请求
    return owner->handlePacket(pkt);
}

void
BaseEndpoint::SlavePort::recvRespRetry()
{
    // 处理响应重试
}

Tick
BaseEndpoint::SlavePort::recvAtomic(PacketPtr pkt)
{
    // 原子模式接收
    return 0;
}

void
BaseEndpoint::SlavePort::recvFunctional(PacketPtr pkt)
{
    // 功能模式接收
}

AddrRangeList
BaseEndpoint::SlavePort::getAddrRanges() const
{
    AddrRangeList ranges;
    ranges.push_back(AddrRange(owner->startAddr, owner->endAddr));
    return ranges;
}

} // namespace protocol_endpoints
} // namespace gem5