#include "PacketGenerator.hh"

namespace gem5 {

PacketGenerator::PacketGenerator(const PacketGeneratorParams &p)
    : SimObject(p),
      Protocol(p.Protocol),
      plp_port("PacketGenerator.plp_port", this),
      event([this]{processEvent();},name()),
      latency(p.latency),
      packet_num(p.packet_num)
{
    DPRINTF(PacketGenerator, "packet gen initialize \n");
}

TLP
PacketGenerator::generateTLP(const std::vector<uint32_t>& header_data,
                            const std::vector<uint32_t>& payload_data,
                            uint32_t digest_data)
{
    TLP tlp;
    
    // 设置header
    tlp.header = header_data;
    
    // 设置payload（如果有）
    if (!payload_data.empty()) {
        tlp.payload = payload_data;
    }
    
    // 设置digest（如果有）
    if (digest_data != 0) {
        tlp.digest = digest_data;
    }
    
    return tlp;
}

PhysicalLayerPacket
PacketGenerator::generatePLP(const TLP& tlp)
{
    std::vector<uint8_t> plp_payload;
    
    // 转换TLP header
    for (const auto& dw : tlp.header) {
        plp_payload.push_back((dw >> 24) & 0xFF);
        plp_payload.push_back((dw >> 16) & 0xFF);
        plp_payload.push_back((dw >> 8) & 0xFF);
        plp_payload.push_back(dw & 0xFF);
    }
    
    // 转换TLP payload
    for (const auto& dw : tlp.payload) {
        plp_payload.push_back((dw >> 24) & 0xFF);
        plp_payload.push_back((dw >> 16) & 0xFF);
        plp_payload.push_back((dw >> 8) & 0xFF);
        plp_payload.push_back(dw & 0xFF);
    }
    
    // 如果存在，添加digest
    if (tlp.hasDigest()) {
        plp_payload.push_back((tlp.digest >> 24) & 0xFF);
        plp_payload.push_back((tlp.digest >> 16) & 0xFF);
        plp_payload.push_back((tlp.digest >> 8) & 0xFF);
        plp_payload.push_back(tlp.digest & 0xFF);
    }
    
    // 创建PLP包
    return PhysicalLayerPacket(
        PCIeKCode::STP,  // TLP包的起始标记
        plp_payload,     // 包含TLP所有数据的字节流
        PCIeKCode::END   // 正常结束标记
    );
}

bool
PacketGenerator::sendPLP(const PhysicalLayerPacket& plp)
{
    // 创建请求
    RequestPtr req = std::make_shared<Request>(0x1010, plp.getSize(), 0, 0);
    
    // 创建数据包
    PacketPtr pkt = new Packet(req, MemCmd::WriteReq);
    
    // 设置数据
    uint8_t* pkt_data = new uint8_t[plp.getSize()];
    std::memcpy(pkt_data, plp.getData(), plp.getSize());
    pkt->dataDynamic(pkt_data);
    
    // 通过端口发送
    return plp_port.sendTimingReq(pkt);
}
//----------------------------------------------------- 事件机制
void
PacketGenerator::processEvent()
{
    packet_num--;
    DPRINTF(PacketGenerator, "Processing event! Packets need send: %d\n", packet_num);
    
    // 生成示例TLP包
    std::vector<uint32_t> header = {0x01020304, 0x05060708};
    std::vector<uint32_t> payload = {0x11223344, 0x55667788};
    
    // 生成TLP
    TLP tlp = generateTLP(header, payload);
    
    // 转换为PLP
    PhysicalLayerPacket plp = generatePLP(tlp);
    
    // 发送PLP
    bool success = sendPLP(plp);
    
    if (!success) {
        // 发送失败，重新调度
        DPRINTF(PacketGenerator, "Failed to send packet, will retry\n");
        schedule(event, curTick() + latency/2); // 短时间后重试
    } else if (packet_num <= 0) {
        DPRINTF(PacketGenerator, "Done firing!\n");
    } else {
        // 发送成功，调度下一个事件
        schedule(event, curTick() + latency);
    }
}

void
PacketGenerator::startup()
{
    schedule(event, 100);
}

} // namespace gem5