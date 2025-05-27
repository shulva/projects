#pragma once

#include "TLP.hh"
#include "PLP.hh"

#include "base/types.hh"
#include "sim/sim_object.hh"
#include "params/PacketGenerator.hh"
#include "debug/PacketGenerator.hh"
#include "mem/port.hh"
#include "mem/packet.hh"  // 引入Packet类
#include "mem/request.hh" // 引入Request类（用于创建Packet）
#include "sim/eventq.hh" // 引入事件队列
#include "sim/sim_events.hh" // 引入事件类

namespace gem5 {

class PacketGenerator : public SimObject {
public:
    // 默认构造函数
    PacketGenerator(const PacketGeneratorParams &p);

    // 生成基础TLP包
    TLP generateTLP(const std::vector<uint32_t>& header_data,
                    const std::vector<uint32_t>& payload_data = {},
                    uint32_t digest_data = 0);

    // 将TLP转换为PLP
    PhysicalLayerPacket generatePLP(const TLP& tlp);

    // 发送PLP包
    bool sendPLP(const PhysicalLayerPacket& plp);

    //-----------------------------------------------------port
    // 添加发送PLP的端口
    class PLPPort : public MasterPort {
    public:
        PLPPort(const std::string &name, PacketGenerator *owner)
            : MasterPort(name, owner) {
            this->owner = owner;
        }
    protected:
        bool recvTimingResp(PacketPtr pkt) override { return true; }
        void recvReqRetry() override {};
        PacketGenerator *owner;
    };

    Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override {
        if (if_name == "plp_port")
            return plp_port;
        else
            return SimObject::getPort(if_name, idx);
    }

    PLPPort plp_port;
    //-----------------------------------------------------port

    //-----------------------------------------------------事件机制
    void startup() override;

    void processEvent();

    EventFunctionWrapper event;

    const Tick latency;

    int packet_num;

    u_int8_t Protocol; //协议参数 1=pcie tlp 2=pcie flit ..

};

} // namespace gem5