/*
 * PCIeConverter.hh
 * PCIe协议转换器头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__

#include "expr/protocol_converter/converters/ProtocolConverter.hh"
#include "expr/protocol_converter/converters/TLPInterface.hh"
#include "params/PCIeConverter.hh"
#include <map>

namespace gem5 {

// PCIe TLP类型到UPF事务类型的映射
struct PCIeTLPTypeMapping {
    static std::map<uint8_t, upf::TransactionType> tlpTypeToUPF;
    static upf::TransactionType mapTLPTypeToUPF(uint8_t tlpType);
};

// PCIe协议转换器类
class PCIeConverter : public SimObject
{
  public:
    // 构造函数
    PCIeConverter(const PCIeConverterParams &params);
    
    // 源协议到UPF的转换
    gem5::ConversionResult sourceToUPF(void* sourcePacket, 
                                     std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到目标协议的转换
    gem5::ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket, 
                                     void*& targetPacket);
    
    // TLP处理接口实现
    bool processTLP(PCIe::TLP* tlp);
    
    // TLP到UPF的转换
    gem5::ConversionResult TLPToUPF(PCIe::TLP* tlp, 
                                               std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到TLP的转换
    gem5::ConversionResult UPFToTLP(const std::shared_ptr<upf::Packet>& upfPacket,
                                               PCIe::TLP*& tlp);
    
  private:
    // PCIe TLP到UPF的转换
    gem5::ConversionResult PCIeTLPToUPF(void* tlpPacket, 
                                                   std::shared_ptr<upf::Packet>& upfPacket);
    
    // PCIe Flit到UPF的转换
    gem5::ConversionResult PCIeFlitToUPF(void* flitPacket, 
                                                    std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到PCIe TLP的转换
    gem5::ConversionResult UPFToPCIeTLP(const std::shared_ptr<upf::Packet>& upfPacket,
                                                   void*& tlpPacket);
    
    // UPF到PCIe Flit的转换
    gem5::ConversionResult UPFToPCIeFlit(const std::shared_ptr<upf::Packet>& upfPacket,
                                                    void*& flitPacket);
    
    // 辅助方法
    upf::AddressSpace mapPCIeAddressSpace(uint8_t addressType);
    upf::CacheAttribute mapPCIeCacheAttribute(uint8_t cacheAttr);
    uint8_t mapUPFToPCIeCacheAttr(upf::CacheAttribute cacheAttr);
    
    // 从TLP字段获取信息的辅助方法
    uint8_t getTLPAddressType(uint8_t fmt_type);
    uint8_t getTLPCacheAttr(uint8_t tc_attr);
    uint8_t getTLPTrafficClass(uint8_t tc_attr);
    
    // TLP特性判断辅助方法
    bool hasTLPData(uint8_t fmt_type);
    bool isTLPMemoryRead(uint8_t fmt_type);
    bool isTLPMemoryWrite(uint8_t fmt_type);
    bool isTLPCompletion(uint8_t fmt_type);
    
    // 目标协议
    std::string targetProtocol;
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__