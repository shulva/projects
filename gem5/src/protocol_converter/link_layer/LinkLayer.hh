#pragma once

#include "mem/packet.hh"
#include "mem/port.hh"
#include "params/LinkLayer.hh"
#include "sim/sim_object.hh"
#include "debug/LinkLayer.hh"

#include <queue>

namespace gem5
{

class LinkLayer : public SimObject
{
  public:
    LinkLayer(const LinkLayerParams &p);

    void startup() override;

    bool isTLP(PacketPtr pkt);

    bool isDLP(PacketPtr pkt);

    // port--------------------------------------------------------------------
    // 上行端口，连接到协议转换控制器
    class Link_to_Converter : public MasterPort
    {
      private:
        LinkLayer *owner;
      public:
        Link_to_Converter(const std::string &name, LinkLayer *owner)
            : MasterPort(name, owner), owner(owner) {}

      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
    };

    class link_to_phy : public MasterPort
    {
    private:
        LinkLayer *owner;
    public:
        link_to_phy(const std::string &name, LinkLayer *owner)
                : MasterPort(name, owner), owner(owner) {}

    protected:
        bool recvTimingResp(PacketPtr pkt) override {};
        void recvReqRetry() override {};
    };

    // 下行端口，连接到协议转换控制器
    class Converter_to_Link : public SlavePort
    {
    private:
        LinkLayer *owner;
    public:
        Converter_to_Link(const std::string &name, LinkLayer *owner)
                : SlavePort(name, owner), owner(owner) {}

    protected:

        Tick recvAtomic(PacketPtr pkt) override {};
        void recvFunctional(PacketPtr pkt) override {};
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override {}  // 添加这个函数实现
        AddrRangeList getAddrRanges() const override {
            AddrRangeList ranges;
            ranges.push_back(AddrRange(0x1000, 0x1FFF));  // 地址范围：0x1000 ~ 0x1FFF,暂时写一个
            return ranges;
        };
    };

    // 下行端口，连接到物理层 (SimpleChipletLink)
    class Link_to_Phy : public SlavePort
    {
      private:
        LinkLayer *owner;
      public:
        Link_to_Phy(const std::string &name, LinkLayer *owner)
            : SlavePort(name, owner), owner(owner) {}

      protected:
        Tick recvAtomic(PacketPtr pkt) override;
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override {}  // 添加这个函数实现
        AddrRangeList getAddrRanges() const override;
    };

    Link_to_Converter link_to_converter_Port;
    Link_to_Phy link_to_phy_Port;
    Converter_to_Link converter_to_link_slaveport;
    link_to_phy link_to_phy_masterport;

private:
    Port &getPort(const std::string &if_name, PortID idx = InvalidPortID) override;
    // port--------------------------------------------------------------------

    // 内部缓冲区
    std::queue<PacketPtr> toControllerBuffer; // 发往协议转换控制器
    std::queue<PacketPtr> toPhyBuffer;      // 发往物理层

    // 缓冲区大小参数
    const unsigned int toControllerBufferSize;
    const unsigned int toPhyBufferSize;

    // 流控相关
    // TODO: 添加流控逻辑和变量

    // 重传相关
    // TODO: 添加重传逻辑和变量


};

}