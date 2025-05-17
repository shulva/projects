/*
 * CXLConverter.hh
 * CXL协议转换器头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_CXL_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_CXL_CONVERTER_HH__

#include "protocol_converter/converters/ProtocolConverter.hh"
#include <map>

namespace gem5 {

// CXL操作类型到UPF事务类型的映射
struct CXLOpTypeMapping {
    static std::map<uint8_t, upf::TransactionType> cxlOpToUPF;
    static upf::TransactionType mapCXLOpToUPF(uint8_t opType);
};

// CXL协议转换器类
class CXLConverter : public ProtocolConverter
{
  public:
    // 构造函数
    CXLConverter(const std::string& name, const std::string& targetProtocol);
    
    // 源协议到UPF的转换
    ConversionResult sourceToUPF(void* sourcePacket, 
                                std::shared_ptr<upf::Packet>& upfPacket) override;
    
    // UPF到目标协议的转换
    ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket,
                                void*& targetPacket) override;
    
  private:
    // CXL.io包到UPF的转换
    ConversionResult CXLIOToUPF(void* cxlIOPacket, 
                               std::shared_ptr<upf::Packet>& upfPacket);
    
    // CXL.mem包到UPF的转换
    ConversionResult CXLMemToUPF(void* cxlMemPacket, 
                                std::shared_ptr<upf::Packet>& upfPacket);
    
    // CXL.cache包到UPF的转换
    ConversionResult CXLCacheToUPF(void* cxlCachePacket, 
                                  std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到CXL.io包的转换
    ConversionResult UPFToCXLIO(const std::shared_ptr<upf::Packet>& upfPacket,
                               void*& cxlIOPacket);
    
    // UPF到CXL.mem包的转换
    ConversionResult UPFToCXLMem(const std::shared_ptr<upf::Packet>& upfPacket,
                                void*& cxlMemPacket);
    
    // UPF到CXL.cache包的转换
    ConversionResult UPFToCXLCache(const std::shared_ptr<upf::Packet>& upfPacket,
                                  void*& cxlCachePacket);
    
    // 辅助方法
    upf::AddressSpace mapCXLAddressSpace(uint8_t addressType);
    upf::CacheAttribute mapCXLCacheAttribute(uint8_t cacheAttr);
    upf::PriorityLevel mapCXLPriority(uint8_t priorityLevel);
    
    // 判断CXL包类型
    enum CXLPacketType {
        CXL_IO,
        CXL_MEM,
        CXL_CACHE,
        CXL_UNKNOWN
    };
    
    CXLPacketType determineCXLPacketType(void* cxlPacket);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_CXL_CONVERTER_HH__