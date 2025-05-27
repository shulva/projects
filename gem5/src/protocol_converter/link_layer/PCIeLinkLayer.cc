#include "protocol_converter/link_layer/PCIeLinkLayer.hh"

#include "base/trace.hh"
#include "sim/system.hh"

namespace gem5
{

PCIeLinkLayer::PCIeLinkLayer(const PCIeLinkLayerParams &p)
    : SimObject(p),
      controllerPort(p.name + ".controller_port", this),
      phyPort(p.name + ".phy_port", this),
      toControllerBufferSize(p.to_controller_buffer_size),
      toPhyBufferSize(p.to_phy_buffer_size),
      sendToControllerEvent([this]{ trySendToController(); }, name()),
      sendToPhyEvent([this]{ trySendToPhy(); }, name())
{
    DPRINTF(PCIeLinkLayer, "Created PCIeLinkLayer\n");
}

Port &
PCIeLinkLayer::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "controller_port") {
        return controllerPort;
    } else if (if_name == "phy_port") {
        return phyPort;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

void
PCIeLinkLayer::startup()
{
    SimObject::startup();
    DPRINTF(PCIeLinkLayer, "PCIeLinkLayer startup\n");
    // TODO: 初始化流控和重传机制
}

// --- ControllerPort --- 
bool
PCIeLinkLayer::ControllerPort::recvTimingResp(PacketPtr pkt)
{
    DPRINTF(PCIeLinkLayer, "ControllerPort: Received timing response\n");
    // 协议转换控制器通常不发送响应给链路层，除非是流控信息
    // 这里暂时简单丢弃或根据需要处理
    delete pkt;
    return true;
}

void
PCIeLinkLayer::ControllerPort::recvReqRetry()
{
    DPRINTF(PCIeLinkLayer, "ControllerPort: Received request retry\n");
    // 收到上层重试请求，安排稍后重发
    // owner->scheduleSendToController();
    // TODO: 实现重试逻辑
}

// --- PhyPort --- 
Tick
PCIeLinkLayer::PhyPort::recvAtomic(PacketPtr pkt)
{
    panic("PCIeLinkLayer::PhyPort::recvAtomic not implemented");
    return 0;
}

void
PCIeLinkLayer::PhyPort::recvFunctional(PacketPtr pkt)
{
    panic("PCIeLinkLayer::PhyPort::recvFunctional not implemented");
}

bool
PCIeLinkLayer::PhyPort::recvTimingReq(PacketPtr pkt)
{
    DPRINTF(PCIeLinkLayer, "PhyPort: Received timing request from PHY\n");
    owner->handleFromPhy(pkt);
    return true; // 假设总能接受
}

AddrRangeList
PCIeLinkLayer::PhyPort::getAddrRanges() const
{
    // 链路层不直接处理地址范围，它透传数据包
    // 但端口需要返回一个范围，否则会出错
    // 可以返回一个全地址范围，或者根据连接的物理层来决定
    return AddrRangeList(AddrRange::Max());
}


void
PCIeLinkLayer::handleFromPhy(PacketPtr pkt)
{
    DPRINTF(PCIeLinkLayer, "Handling packet from PHY, size: %d\n", pkt->getSize());
    if (toControllerBuffer.size() < toControllerBufferSize) {
        toControllerBuffer.push(pkt);
        if (!챔sendToControllerEvent.scheduled()) {
            schedule(챔sendToControllerEvent, curTick() + 1); // 模拟处理延迟
        }
    } else {
        DPRINTF(PCIeLinkLayer, "toControllerBuffer full, dropping packet from PHY\n");
        // TODO: 实现流控，发送 NACK 或其他机制
        delete pkt; // 简单丢弃
    }
}

void
PCIeLinkLayer::handleFromController(PacketPtr pkt)
{
    DPRINTF(PCIeLinkLayer, "Handling packet from Controller, size: %d\n", pkt->getSize());
    if (toPhyBuffer.size() < toPhyBufferSize) {
        toPhyBuffer.push(pkt);
        if (!sendToPhyEvent.scheduled()) {
            schedule(sendToPhyEvent, curTick() + 1); // 模拟处理延迟
        }
    } else {
        DPRINTF(PCIeLinkLayer, "toPhyBuffer full, signaling retry to Controller\n");
        // TODO: 实现流控，向上层发送重试请求
        // controllerPort.sendRetryReq(); // 假设有这个方法
        delete pkt; // 简单丢弃，上层应重发
    }
}

void
PCIeLinkLayer::trySendToController()
{
    if (!toControllerBuffer.empty()) {
        PacketPtr pkt = toControllerBuffer.front();
        DPRINTF(PCIeLinkLayer, "Trying to send packet to Controller\n");
        if (controllerPort.sendTimingReq(pkt)) {
            DPRINTF(PCIeLinkLayer, "Packet sent to Controller\n");
            toControllerBuffer.pop();
            if (!toControllerBuffer.empty() && !sendToControllerEvent.scheduled()) {
                 schedule(sendToControllerEvent, curTick() + 1);
            }
        } else {
            DPRINTF(PCIeLinkLayer, "ControllerPort busy, will retry sending to Controller\n");
            if (!sendToControllerEvent.scheduled()) {
                 schedule(sendToControllerEvent, curTick() + Cycles(10)); // 稍后重试
            }
        }
    }
}

void
PCIeLinkLayer::trySendToPhy()
{
    if (!toPhyBuffer.empty()) {
        PacketPtr pkt = toPhyBuffer.front();
        DPRINTF(PCIeLinkLayer, "Trying to send packet to PHY\n");
        if (phyPort.sendTimingResp(pkt)) { // 链路层作为Master，物理层作为Slave，所以是sendTimingResp
            DPRINTF(PCIeLinkLayer, "Packet sent to PHY\n");
            toPhyBuffer.pop();
            if (!toPhyBuffer.empty() && !sendToPhyEvent.scheduled()) {
                 schedule(sendToPhyEvent, curTick() + 1);
            }
        } else {
            DPRINTF(PCIeLinkLayer, "PhyPort busy, will retry sending to PHY\n");
            // 物理层通常不应该拒绝，除非有特殊流控
            // 这里假设物理层总能接受，或者需要物理层实现sendRetryResp
            // 如果物理层拒绝，需要链路层实现重试逻辑
            if (!sendToPhyEvent.scheduled()) {
                 schedule(sendToPhyEvent, curTick() + Cycles(10)); // 稍后重试
            }
        }
    }
}

}