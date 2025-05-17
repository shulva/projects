/*
 * PCIeConverter.hh
 * PCIe协议转换器头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__

#include "protocol_converter/converters/ProtocolConverter.hh"
#include "protocol_converter/converters/TLPInterface.hh"
#include <map>

namespace gem5 {

// PCIe TLP类型到UPF事务类型的映射
struct PCIeTLPTypeMapping {
    static std::map<uint8_t, upf::TransactionType> tlpTypeToUPF;
    static upf::TransactionType mapTLPTypeToUPF(uint8_t tlpType);
};

// PCIe协议转换器类
class PCIeConverter : public ProtocolConverter, public TLPInterface
{
  public:
    // 构造函数
    PCIeConverter(const std::string& name, const std::string& targetProtocol);
    
    // 源协议到UPF的转换
    ConversionResult sourceToUPF(void* sourcePacket, 
                                std::shared_ptr<upf::Packet>& upfPacket) override;
    
    // UPF到目标协议的转换
    ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket,
                                void*& targetPacket) override;
    
    // TLP处理接口实现
    bool processTLP(PCIe::TLP* tlp) override;
    
    // TLP到UPF的转换
    ConversionResult TLPToUPF(PCIe::TLP* tlp, 
                             std::shared_ptr<upf::Packet>& upfPacket) override;
    
    // UPF到TLP的转换
    ConversionResult UPFToTLP(const std::shared_ptr<upf::Packet>& upfPacket,
                             PCIe::TLP*& tlp) override;
    
  private:
    // PCIe TLP到UPF的转换
    ConversionResult PCIeTLPToUPF(void* tlpPacket, 
                                 std::shared_ptr<upf::Packet>& upfPacket);
    
    // PCIe Flit到UPF的转换
    ConversionResult PCIeFlitToUPF(void* flitPacket, 
                                  std::shared_ptr<upf::Packet>& upfPacket);
    
    // UPF到PCIe TLP的转换
    ConversionResult UPFToPCIeTLP(const std::shared_ptr<upf::Packet>& upfPacket,
                                 void*& tlpPacket);
    
    // UPF到PCIe Flit的转换
    ConversionResult UPFToPCIeFlit(const std::shared_ptr<upf::Packet>& upfPacket,
                                  void*& flitPacket);
    
    // 辅助方法
    upf::AddressSpace mapPCIeAddressSpace(uint8_t addressType);
    upf::CacheAttribute mapPCIeCacheAttribute(uint8_t cacheAttr);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_PCIE_CONVERTER_HH__