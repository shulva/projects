#ifndef __PROTOCOL_ENDPOINTS_BASE_ENDPOINT_HH__
#define __PROTOCOL_ENDPOINTS_BASE_ENDPOINT_HH__

#include "mem/port.hh"
#include "params/BaseEndpoint.hh"
#include "sim/sim_object.hh"

namespace gem5
{

namespace protocol_endpoints
{

// 基础端点类，作为所有协议端点的父类
class BaseEndpoint : public SimObject
{
  protected:
    // 端点地址范围
    Addr startAddr;
    Addr endAddr;
    
    // 统计计数器
    struct EndpointStats {
        // 发送和接收的包计数
        Stats::Scalar txPackets;
        Stats::Scalar rxPackets;
        
        // 按事务类型分类的计数
        Stats::Vector txByType;
        Stats::Vector rxByType;
        
        // 延迟统计
        Stats::Histogram latency;
    } stats;
    
  public:
    // 主端口 (发起请求)
    class MasterPort : public RequestPort
    {
      private:
        BaseEndpoint *owner;
        
      public:
        MasterPort(const std::string &name, BaseEndpoint *owner);
        
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
    };
    
    // 从端口 (接收请求)
    class SlavePort : public ResponsePort
    {
      private:
        BaseEndpoint *owner;
        
      public:
        SlavePort(const std::string &name, BaseEndpoint *owner);
        
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
        Tick recvAtomic(PacketPtr pkt) override;
        void recvFunctional(PacketPtr pkt) override;
        AddrRangeList getAddrRanges() const override;
    };
    
    // 端口实例
    MasterPort masterPort;
    SlavePort slavePort;
    
    // 构造函数
    BaseEndpoint(const BaseEndpointParams &params);
    
    // 统计函数
    void regStats() override;
    
    Port &getPort(const std::string &if_name,
                 PortID idx = InvalidPortID) override;
                 
    // 处理数据包的虚函数，由子类实现
    virtual bool handlePacket(PacketPtr pkt) = 0;
};

} // namespace protocol_endpoints
} // namespace gem5

#endif // __PROTOCOL_ENDPOINTS_BASE_ENDPOINT_HH__