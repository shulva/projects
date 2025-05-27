#pragma once

#include "mem/packet.hh"
#include "mem/port.hh"
#include "params/PCIeLinkLayer.hh"
#include "sim/sim_object.hh"
#include "debug/PCIeLinkLayer.hh"

#include <queue>

namespace gem5
{

class PCIeLinkLayer : public SimObject
{
  public:
    PCIeLinkLayer(const PCIeLinkLayerParams &p);


    void startup() override;

  private:
    // port--------------------------------------------------------------------
    // 上行端口，连接到协议转换控制器
    class ControllerPort : public MasterPort
    {
      private:
        PCIeLinkLayer *owner;
      public:
        ControllerPort(const std::string &name, PCIeLinkLayer *owner)
            : MasterPort(name, owner), owner(owner) {}

      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
    };

    // 下行端口，连接到物理层 (SimpleChipletLink)
    class PhyPort : public SlavePort
    {
      private:
        PCIeLinkLayer *owner;
      public:
        PhyPort(const std::string &name, PCIeLinkLayer *owner)
            : SlavePort(name, owner), owner(owner) {}

      protected:
        Tick recvAtomic(PacketPtr pkt) override;
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        AddrRangeList getAddrRanges() const override;
    };

    ControllerPort controllerPort;
    PhyPort phyPort;

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

    // 处理从物理层收到的数据包
    void handleFromPhy(PacketPtr pkt);
    // 处理从控制器收到的数据包
    void handleFromController(PacketPtr pkt);

    // 尝试向上层发送数据包
    void trySendToController();
    // 尝试向物理层发送数据包
    void trySendToPhy();

    // 事件处理
    EventFunctionWrapper sendToControllerEvent;
    EventFunctionWrapper sendToPhyEvent;
};

}