/*
 * GenericPacketProcessor.hh
 * 通用数据包处理器头文件
 */

#ifndef __PROTOCOL_CONVERTER_GENERIC_PACKET_PROCESSOR_HH__
#define __PROTOCOL_CONVERTER_GENERIC_PACKET_PROCESSOR_HH__

#include "protocol_converter/phy_abstraction/SimpleChipletLink.hh"
#include "protocol_converter/link_layer/PCIeLinkLayer.hh"
#include "protocol_converter/converters/ProtocolConverter.hh"
#include "sim/sim_object.hh"
#include "params/GenericPacketProcessor.hh"
#include <memory>
#include <queue>

namespace gem5 {

// 通用数据包处理器类
class GenericPacketProcessor : public SimObject
{
  public:
    // 构造函数
    GenericPacketProcessor(const GenericPacketProcessorParams &p);
    
    // 初始化
    void init() override;
    
    // 启动
    void startup() override;
    
    // 接收数据包
    bool receivePacket(void* packet, uint32_t size, PhyProtocolType protocol);
    
    // 发送数据包
    bool sendPacket(void* packet, uint32_t size, PhyProtocolType protocol);
    
  protected:
    // 组件指针
    SimpleChipletLink* physicalLayer;
    PCIeLinkLayer* linkLayer;
    ProtocolConverter* protocolConverter;
    
    // 处理物理层数据包
    bool handlePhyPacket(PhyPacket* packet);
    
    // 处理链路层数据包
    bool handleLinkLayerPacket(void* data, uint32_t size);
    
    // 处理协议转换
    bool handleProtocolConversion(void* sourcePacket, void*& targetPacket);
    
    // 数据包队列
    std::queue<std::pair<void*, uint32_t>> packetQueue;
    
    // 处理队列中的数据包
    void processQueuedPackets();
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_GENERIC_PACKET_PROCESSOR_HH__