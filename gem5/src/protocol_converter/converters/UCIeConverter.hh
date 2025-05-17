/*
 * UCIeConverter.hh
 * UCIe协议转换器头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_UCIE_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_UCIE_CONVERTER_HH__

#include "protocol_converter/converters/ProtocolConverter.hh"
#include <map>

namespace gem5 {

// UCIe操作类型到UPF事务类型的映射
struct UCIeOpTypeMapping {
    static std::map<uint8_t, upf::TransactionType> ucieOpToUPF;
    static upf::TransactionType mapUCIeOpToUPF(uint8_t opType);
};

// UCIe协议转换器类
class UCIeConverter : public ProtocolConverter
{
  public:
    // 构造函数
    UCIeConverter(const std::string& name, const std::string& targetProtocol);
    
    // 源协议到UPF的转换
    ConversionResult sourceToUPF(void* sourcePacket, 
                                std::shared_ptr<upf::Packet>& upfPacket) override;
    
    // UPF到目标协议的转换
    ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket,
                                void*& targetPacket) override;
    
  private:
    // UCIe包到UPF的转换
    ConversionResult UCIePacketToUPF(void* uciePacket, 
                                    std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到UCIe包的转换
    ConversionResult UPFToUCIePacket(const std::shared_ptr<upf::Packet>& upfPacket,
                                    void*& uciePacket);
    
    // 辅助方法
    upf::AddressSpace mapUCIeAddressSpace(uint8_t addressType);
    upf::CacheAttribute mapUCIeCacheAttribute(uint8_t cacheAttr);
    upf::PriorityLevel mapUCIePriority(uint8_t priorityLevel);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_UCIE_CONVERTER_HH__