/*
 * SimpleChipletLink.cc
 * 简单芯粒链路物理层抽象模型实现文件
 */

#include "SimpleChipletLink.hh"
#include "base/trace.hh"
#include "params/SimpleChipletLink.hh"
#include "sim/system.hh"
#include <cmath>

namespace gem5 {

// PhyPacket 构造函数实现
SimpleChipletLink::SimpleChipletLink(const SimpleChipletLinkParams &p)
    : SimObject(p),
      bandwidth(p.bandwidth),
      baseLatency(p.baseLatency),
      bitErrorRate(p.bitErrorRate),
      linkWidth(p.linkWidth),
      encodingOverhead(p.encodingOverhead),
      linkState(LINK_DOWN),
      transferMode(MODE_NON_FLIT),
      phy_port( "SimpleChipletLink.phy_port", this),
      phy_to_link_port( "SimpleChipletLink.phy_to_link_port", this),
      link_to_phy_slaveport( "SimpleChipletLink.link_to_phy_slaveport", this)
{
    DPRINTF(SimpleChipletLink, "创建物理层: 带宽=%llu Gbps, 基础延迟=%llu ps, BER=%e\n",bandwidth, baseLatency, bitErrorRate);
}

void
SimpleChipletLink::init()
{
    // 调用父类初始化
    SimObject::init();

    DPRINTF(SimpleChipletLink, "芯粒初始化\n");
}

void
SimpleChipletLink::startup()
{
    // 调用父类启动
    SimObject::startup();

    // 设置链路状态为训练中
    setLinkState(LINK_TRAINING);

    // 模拟链路训练过程，1000个周期后链路变为UP状态
    schedule(new EventFunctionWrapper([this]{ setLinkState(LINK_UP); },
                                     name() + ".linkTrainingEvent", true),
             curTick() + 1000);

    DPRINTF(SimpleChipletLink, "物理层启动，开始链路训练\n");
}

void
SimpleChipletLink::setLinkState(PhyLinkState state)
{
    // 设置链路状态
    linkState = state;
    DPRINTF(SimpleChipletLink, "链路状态变更为: %d\n", state);
}

PhyLinkState
SimpleChipletLink::getLinkState() const
{
    // 获取链路状态
    return linkState;
}

void
SimpleChipletLink::setTransferMode(PhyTransferMode mode)
{
    // 设置传输模式
    transferMode = mode;
    DPRINTF(SimpleChipletLink, "传输模式变更为: %d\n", mode);
}

PhyTransferMode
SimpleChipletLink::getTransferMode() const
{
    // 获取传输模式
    return transferMode;
}

void
SimpleChipletLink::setProtocolType(PhyProtocolType protocol)
{
    // 设置协议类型
    protocolType = protocol;
    DPRINTF(SimpleChipletLink, "协议类型变更为: %d\n", protocol);
}

PhyProtocolType
SimpleChipletLink::getProtocolType() const // 移除参数
{
    // 获取协议类型
    return protocolType;
}

void SimpleChipletLink::receivePacket()
{
    if (!hasPacket()) {
        return;
    }

    // 从接收队列中取出数据包
    PhysicalLayerPacket packet = rxQueue.front();
    rxQueue.pop();

    DPRINTF(SimpleChipletLink, "处理接收数据包, 大小: %d 字节\n", packet.getSize());
    
    // 根据协议类型和数据包类型进行处理
    PhyProtocolType protocol = identifyProtocol(&packet);
    PhyPacketType packetType = identifyPacketType(&packet);
    
    // 这里可以添加更多的处理逻辑，例如：
    // 1. 解析PLP包中的TLP数据
    // 2. 将TLP数据传递给上层协议处理
    // 3. 生成响应等
    
    // 如果队列中还有数据包，继续处理
    if (hasPacket()) {
        schedule(new EventFunctionWrapper([this]{ receivePacket(); },
                                         name() + ".processPacketEvent", true),
                 curTick() + baseLatency);
    }
}

bool
SimpleChipletLink::hasPacket() const
{
    // 检查是否有数据包可接收
    return !rxQueue.empty();
}

Tick
SimpleChipletLink::calculateTransferDelay(uint32_t size) const
{
    // 计算传输延迟
    // 考虑带宽、链路宽度和编码开销

    // 计算有效带宽 (bits/ps)
    double effectiveBandwidth = (double)bandwidth / (1000.0 * encodingOverhead);

    // 计算传输时间 (ps)
    // 传输时间 = 基础延迟 + 数据大小(bits) / 有效带宽(bits/ps)
    Tick transferTime = baseLatency + (Tick)ceil((size * 8.0) / effectiveBandwidth);

    DPRINTF(SimpleChipletLink, "计算传输延迟: 大小=%d 字节, 延迟=%llu ps\n",
            size, transferTime);

    return transferTime;
}

PhyErrorType
SimpleChipletLink::simulateBitError(uint32_t size) const
{
    // 模拟位错误
    // 错误概率 = 1 - (1 - BER)^(size*8)
    double errorProb = 1.0 - pow(1.0 - bitErrorRate, size * 8);

    // 生成随机数判断是否发生错误
    DPRINTF(SimpleChipletLink, "模拟位错误: 发生错误\n");

    return ERROR_NONE;
}

void
SimpleChipletLink::receive_PLP(PhysicalLayerPacket* packet)
{

    const uint8_t* data = packet->getData();
    // 从PLP中提取数据
    /* 调试用
    DPRINTF(SimpleChipletLink, "PLP起始标记: 0x%02x\n", static_cast<uint8_t>(packet->startSymbol));

    DPRINTF(SimpleChipletLink, "PLP内容: ");
    for(size_t i = 0; i < packet->getSize(); i++) {
        DPRINTF(SimpleChipletLink, "%02x \n ", data[i]);
    }

    DPRINTF(SimpleChipletLink, "PLP结束标记: 0x%02x\n", static_cast<uint8_t>(packet->endSymbol));
     */

    // 创建新的Request和Packet用于传输
    RequestPtr req = std::make_shared<Request>(0x1000, packet->getSize(), 0, 0);
    PacketPtr pkt = new Packet(req, MemCmd::WriteReq);

    // 创建新的缓冲区来存储数据副本 绕开const指针
    uint8_t* modifiable_data = new uint8_t[packet->getSize()];
    std::memcpy(modifiable_data, data, packet->getSize());
    pkt->dataDynamic(modifiable_data);


    bool success = phy_to_link_port.sendTimingReq(pkt);
    if (!success) {
        rxQueue.push(*packet);
        DPRINTF(SimpleChipletLink, "PHY:发送失败，数据包加入接收队列\n");
    } else {
        DPRINTF(SimpleChipletLink, "PHY:成功发送数据包到链路层\n");
    }

    delete packet;

}

PhyProtocolType
SimpleChipletLink::identifyProtocol(const PhysicalLayerPacket* packet) const
{
    // 根据数据包格式识别协议类型

    // 首先检查是否为UCIe Flit
    if (isUCIeFlit(packet)) {
        return PROTOCOL_UCIE;
    }

    // 检查是否为CXL TLP或DLLP
    if (isCXLTLP(packet) || isCXLDLLP(packet)) {
        return PROTOCOL_CXL;
    }

    // 检查是否为PCIe TLP或DLLP
    if (isPCIeTLP(packet) || isPCIeDLLP(packet)) {
        return PROTOCOL_PCIE;
    }

    // 无法识别协议
    return PROTOCOL_UNKNOWN;
}

PhyPacketType
SimpleChipletLink::identifyPacketType(const PhysicalLayerPacket* packet) const
{
    // 根据数据包格式识别数据包类型

    // 检查是否为UCIe Flit
    if (isUCIeFlit(packet)) {
        return PACKET_FLIT;
    }

    // 检查是否为TLP
    if (isPCIeTLP(packet) || isCXLTLP(packet)) {
        return PACKET_TLP;
    }

    // 检查是否为DLLP
    if (isPCIeDLLP(packet) || isCXLDLLP(packet)) {
        return PACKET_DLLP;
    }

    // 无法识别类型
    return PACKET_UNKNOWN;
}

bool
SimpleChipletLink::isPCIeTLP(const PhysicalLayerPacket* packet) const
{
    // 检查是否为PCIe TLP

    // 数据包至少需要4字节（TLP头部）
    if (packet->getSize() < 4) {
        return false;
    }

    const uint8_t* data = packet->getData();

    // 检查TLP格式和类型字段
    uint8_t fmt = (data[0] >> 5) & 0x7;  // 格式字段
    uint8_t type = ((data[0] & 0x1F) << 3) | ((data[1] >> 5) & 0x7); // 类型字段

    // PCIe TLP格式和类型范围检查
    // 这里需要根据PCIe规范进行详细检查
    // 简化版本：检查常见的PCIe TLP类型

    // 内存请求、IO请求、配置请求等
    if ((fmt <= 0x5) && (type <= 0x1F)) {
        return true;
    }

    return false;
}

bool
SimpleChipletLink::isPCIeDLLP(const PhysicalLayerPacket* packet) const
{
    // 检查是否为PCIe DLLP

    // DLLP通常为6字节（2字节头部+2字节数据+2字节CRC）
    if (packet->getSize() != 6) {
        return false;
    }

    const uint8_t* data = packet->getData();

    // 检查DLLP类型字段
    uint8_t type = data[0] & 0xF0;

    // PCIe DLLP类型范围检查
    // 这里需要根据PCIe规范进行详细检查
    // 简化版本：检查常见的PCIe DLLP类型

    // 流控更新、ACK、NAK等
    if (type <= 0x30) {
        return true;
    }

    return false;
}

bool
SimpleChipletLink::isCXLTLP(const PhysicalLayerPacket* packet) const
{
    // 检查是否为CXL TLP

    // 数据包至少需要4字节（TLP头部）
    if (packet->getSize() < 4) {
        return false;
    }

    const uint8_t* data = packet->getData();

    // 检查TLP格式和类型字段
    uint8_t fmt = (data[0] >> 5) & 0x7;  // 格式字段
    uint8_t type = ((data[0] & 0x1F) << 3) | ((data[1] >> 5) & 0x7); // 类型字段

    // CXL特有的TLP类型检查
    // 这里需要根据CXL规范进行详细检查
    // 简化版本：检查CXL特有的TLP类型

    // CXL.cache和CXL.mem特有的类型
    if ((fmt == 0x4 && type >= 0x18) || (fmt == 0x5 && type >= 0x18)) {
        return true;
    }

    return false;
}

bool
SimpleChipletLink::isCXLDLLP(const PhysicalLayerPacket* packet) const
{
    // 检查是否为CXL DLLP

    // DLLP通常为6字节（2字节头部+2字节数据+2字节CRC）
    if (packet->getSize() != 6) {
        return false;
    }

    const uint8_t* data = packet->getData();

    // 检查DLLP类型字段
    uint8_t type = data[0] & 0xF0;

    // CXL特有的DLLP类型检查
    // 这里需要根据CXL规范进行详细检查
    // 简化版本：检查CXL特有的DLLP类型

    // CXL特有的DLLP类型
    if (type == 0xE0 || type == 0xF0) {
        return true;
    }

    return false;
}

bool
SimpleChipletLink::isUCIeFlit(const PhysicalLayerPacket* packet) const
{
    // 检查是否为UCIe Flit

    // UCIe Flit通常有固定大小和特定的头部格式
    // 这里需要根据UCIe规范进行详细检查
    // 简化版本：检查UCIe Flit的基本特征

    // 数据包至少需要8字节（Flit头部）
    if (packet->getSize() < 8) {
        return false;
    }

    const uint8_t* data = packet->getData();

    // 检查Flit头部特征
    // 这里需要根据UCIe规范进行详细检查

    // UCIe Flit头部特征检查
    // 简化版本：检查特定的标志位或格式
    uint8_t flitType = data[0] & 0xF0;

    // UCIe特有的Flit类型
    if (flitType >= 0x80) {
        return true;
    }

    return false;
}

} // namespace gem5