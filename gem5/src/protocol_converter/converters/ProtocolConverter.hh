/*
 * ProtocolConverter.hh
 * 协议转换器基类头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__

#include <memory>
#include <string>
#include "sim/sim_object.hh"
#include "../link_layer/PCIeLinkLayer.hh"
#include "params/ProtocolConverter.hh"
#include "TLPInterface.hh"
#include "../upf/UPF.hh"

namespace gem5 {
    enum class ConversionResult {
        SUCCESS,                // 转换成功
        ERROR,                  // 一般错误
        UNSUPPORTED_FEATURE,    // 不支持的特性
        INVALID_PACKET,         // 无效的数据包
        BUFFER_OVERFLOW         // 缓冲区溢出
    };
// 协议转换器基类
class ProtocolConverter : public SimObject
{
  public:
    // 转换结果枚举
    
    // 构造函数
    ProtocolConverter(const ProtocolConverterParams& params);
    
    // 初始化
    void init() override;
    
    // 启动
    void startup() override;
    
    // 设置链路层A
    void setLinkLayerA(PCIeLinkLayer* linkLayer);
    
    // 设置链路层B
    void setLinkLayerB(PCIeLinkLayer* linkLayer);
    
    // 源协议到UPF的转换（纯虚函数）
    virtual ConversionResult sourceToUPF(void* sourcePacket, 
                                       std::shared_ptr<upf::Packet>& upfPacket) = 0;
    
    // UPF到目标协议的转换（纯虚函数）
    virtual ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket,
                                       void*& targetPacket) = 0;
    
    // 处理从链路层A接收的数据包
    bool handlePacketFromLinkLayerA(PCIe::TLP* tlp);
    
    // 处理从链路层B接收的数据包
    bool handlePacketFromLinkLayerB(PCIe::TLP* tlp);
    
    // 发送数据包到链路层A
    bool sendPacketToLinkLayerA(void* packet);
    
    // 发送数据包到链路层B
    bool sendPacketToLinkLayerB(void* packet);
    
    // 转换并转发数据包
    bool convertAndForward(void* sourcePacket, bool fromA);
    
  protected:
    // 协议名称
    std::string sourceProtocolName;
    std::string targetProtocolName;
    
    // 链路层指针
    PCIeLinkLayer* linkLayerA;
    PCIeLinkLayer* linkLayerB;
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__