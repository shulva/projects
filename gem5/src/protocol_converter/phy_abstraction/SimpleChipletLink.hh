/*
 * SimpleChipletLink.hh
 * 简单芯粒链路物理层抽象模型头文件
 */

#pragma once

#include "base/types.hh"
#include "sim/sim_object.hh"
#include "params/SimpleChipletLink.hh"
#include "debug/SimpleChipletLink.hh"
#include "mem/packet.hh"  // 引入Packet类
#include "mem/request.hh" // 引入Request类（用于创建Packet）
#include "mem/port.hh" // 引入Request类（用于创建Packet）
#include <base/addr_range.hh>  // 包含地址范围定义

#include "../packet/PLP.hh"
#include <functional>
#include <queue>
#include <random>
#include <vector>

namespace gem5 {

// 物理层传输模式枚举
enum PhyTransferMode {
    MODE_NON_FLIT,  // 非Flit模式
    MODE_FLIT       // Flit模式
};

// 物理层状态枚举
enum PhyLinkState {
    LINK_DOWN,      // 链路断开
    LINK_TRAINING,  // 链路训练中
    LINK_UP         // 链路正常
};

// 物理层错误类型枚举
enum PhyErrorType {
    ERROR_NONE,     // 无错误
    ERROR_BIT,      // 位错误
    ERROR_LINK      // 链路错误
};

// 协议类型枚举
enum PhyProtocolType {
    PROTOCOL_UNKNOWN, // 未知协议
    PROTOCOL_PCIE,    // PCIe协议
    PROTOCOL_CXL,     // CXL协议
    PROTOCOL_UCIE     // UCIe协议
};

// 数据包类型枚举
enum PhyPacketType {
    PACKET_UNKNOWN, // 未知类型
    PACKET_TLP,     // TLP类型
    PACKET_DLLP,    // DLLP类型
    PACKET_FLIT     // FLIT类型
};


// 简单芯粒链路物理层类
class SimpleChipletLink : public SimObject
{
  public:
    // 构造函数
    SimpleChipletLink(const SimpleChipletLinkParams &p);

    // 初始化
    void init() ;

    // 启动
    void startup() override ;

    // 设置链路状态
    void setLinkState(PhyLinkState state);

    // 获取链路状态
    PhyLinkState getLinkState() const;

    // 设置传输模式
    void setTransferMode(PhyTransferMode mode);

    // 获取传输模式
    PhyTransferMode getTransferMode() const;
    
    // 检查是否有数据包可接收
    bool hasPacket() const;

    // 计算传输延迟
    Tick calculateTransferDelay(uint32_t size) const;

    // 模拟位错误
    PhyErrorType simulateBitError(uint32_t size) const;

    // 识别协议类型
    PhyProtocolType identifyProtocol(const PhysicalLayerPacket* packet) const;

    // 识别数据包类型
    PhyPacketType identifyPacketType(const PhysicalLayerPacket* packet) const;

    // 设置协议类型
    void setProtocolType(PhyProtocolType protocol);

    // 获取协议类型
    PhyProtocolType getProtocolType() const; // 移除参数

     // 发送TLP
    bool sendTLP(void* tlp, uint32_t size, PhyProtocolType protocol);

    // 发送DLLP
    bool sendDLLP(void* dllp, uint32_t size, PhyProtocolType protocol);

    // 处理接收的物理层数据包 // port->phy PLP包
    void receive_PLP(PhysicalLayerPacket* packet);

    //接受数据包进elastic buffer
    void receivePacket();

  protected:
    // 链路参数
    uint64_t bandwidth;       // 带宽 (Gbps)
    Tick baseLatency{};         // 基础延迟 (ps)
    double bitErrorRate;      // 位错误率
    uint32_t linkWidth;       // 链路宽度 (bits)
    double encodingOverhead;  // 编码开销 (如8b/10b为1.25)
    PhyProtocolType protocolType; // 协议类型?存疑

    // 链路状态
    PhyLinkState linkState;   // 当前链路状态
    PhyTransferMode transferMode; // 传输模式

    //-----------------------------------------------port
    class PLPPort : public SlavePort { // generator to phy slave port
    public:
        PLPPort(const std::string &name, SimpleChipletLink *owner)
                : SlavePort(name, owner), owner(owner) {}

    protected:
        bool recvTimingReq(PacketPtr pkt) override {
            DPRINTF(SimpleChipletLink, "PLPPort收到请求，地址：0x%x\n", pkt->getAddr());
            
            uint8_t* data = pkt->getPtr<uint8_t>();
            size_t size = pkt->getSize();
            
            PhysicalLayerPacket* plp = new PhysicalLayerPacket(data, size);
            DPRINTF(SimpleChipletLink, "accept PLP\n");
            owner->receive_PLP(plp);
            return true;
        }

        AddrRangeList getAddrRanges() const override {
            AddrRangeList ranges;
            ranges.push_back(AddrRange(0x1000, 0x1FFF));  // 地址范围：0x1000 ~ 0x1FFF,暂时写一个
            return ranges;
        }

        Tick recvAtomic(PacketPtr pkt) override {
            // 处理atomic模式请求

        }

        // 实现TimingResponseProtocol接口
        void recvRespRetry() override {
            // TODO:处理响应重试逻辑,不过这里不负责重传
        }

        // 实现FunctionalResponseProtocol接口
        void recvFunctional(PacketPtr pkt) override {
            // 处理functional模式请求（通常用于调试）
            if (pkt->isRead()) {
                // 填充读取数据
                pkt->makeResponse();
            } else if (pkt->isWrite()) {
                // 处理写入数据
                pkt->makeResponse();
            }
        }

    private:
        SimpleChipletLink *owner;
    };

    class Link_to_Phy_Port:  public SlavePort { // generator to phy slave port
    public:
        Link_to_Phy_Port(const std::string &name, SimpleChipletLink *owner)
                : SlavePort(name, owner), owner(owner) {}

    protected:
        bool recvTimingReq(PacketPtr pkt) override {
            DPRINTF(SimpleChipletLink, "收到来自链路层的转换包\n");
            //TODO:将其重新包装成PLP并发送

            return true;
        }

        AddrRangeList getAddrRanges() const override {
            AddrRangeList ranges;
            ranges.push_back(AddrRange(0x1000, 0x1FFF));  // 地址范围：0x1000 ~ 0x1FFF,暂时写一个
            return ranges;
        }

        Tick recvAtomic(PacketPtr pkt) override {
        }

        void recvRespRetry() override {
        }

        void recvFunctional(PacketPtr pkt) override {
        }

    private:
        SimpleChipletLink *owner;
    };

    class Phy_to_Link_Port : public MasterPort { // phy to link master port
    public:
        Phy_to_Link_Port(const std::string &name, SimpleChipletLink *owner)
                : MasterPort(name, owner) {
            this->owner = owner;
        }
    protected:
        bool recvTimingResp(PacketPtr pkt) override { return true; }
        void recvReqRetry() override {};
        SimpleChipletLink  *owner;
    };

    PLPPort phy_port; //接受PLP的port
    Phy_to_Link_Port phy_to_link_port;//发送DLP/TLP的port
    Link_to_Phy_Port link_to_phy_slaveport;

    Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override {
        if (if_name == "phy_port")
            return phy_port;
        else if(if_name == "phy_to_link_port")
            return phy_to_link_port;
        else if(if_name == "link_to_phy_slaveport")
            return link_to_phy_slaveport;
        else
            return SimObject::getPort(if_name, idx);
    }

    //-----------------------------------------------port

    std::queue<PhysicalLayerPacket> rxQueue;

    // 检查是否为PCIe TLP
    bool isPCIeTLP(const PhysicalLayerPacket* packet) const;
    
    // 检查是否为PCIe DLLP
    bool isPCIeDLLP(const PhysicalLayerPacket* packet) const;
    
    // 检查是否为CXL TLP
    bool isCXLTLP(const PhysicalLayerPacket* packet) const;
    
    // 检查是否为CXL DLLP
    bool isCXLDLLP(const PhysicalLayerPacket* packet) const;
    
    // 检查是否为UCIe Flit
    bool isUCIeFlit(const PhysicalLayerPacket* packet) const;

};


// 在类定义中添加：


} // namespace gem5
