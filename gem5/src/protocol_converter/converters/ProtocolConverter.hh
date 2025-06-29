#pragma once

#include "mem/packet.hh"
#include "mem/port.hh"
#include "params/ProtocolConverter.hh"
#include "sim/sim_object.hh"
#include "debug/ProtocolConverter.hh"

#include <queue>

namespace gem5
{

class ProtocolConverter : public SimObject
{
  public:
    ProtocolConverter(const ProtocolConverterParams &p);

    void startup() override;

  private:
    // 上行端口，连接到linkl
    class Link_to_Protocol : public SlavePort
    {
      private:
        ProtocolConverter *owner;
      public:
        Link_to_Protocol(const std::string &name, ProtocolConverter *owner)
            : SlavePort(name, owner), owner(owner) {}

      protected:
        Tick recvAtomic(PacketPtr pkt) override;
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override {}
        AddrRangeList getAddrRanges() const override;
    };

    // 下行端口，连接到link
    class Protocol_to_Link : public MasterPort
    {
      private:
        ProtocolConverter *owner;
      public:
        Protocol_to_Link(const std::string &name, ProtocolConverter *owner)
            : MasterPort(name, owner), owner(owner) {}

      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
    };

    Link_to_Protocol link_to_protocol_port;
    Protocol_to_Link protocol_to_link_port;

    Port &getPort(const std::string &if_name, PortID idx = InvalidPortID) override;

    // 协议名称
    const std::string sourceProtocolName;
    const std::string targetProtocolName;
};

}