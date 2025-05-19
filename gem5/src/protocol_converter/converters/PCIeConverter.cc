/*
 * PCIeConverter.cc
 * PCIe协议转换器实现文件
 */

#include "expr/protocol_converter/converters/PCIeConverter.hh"
#include "debug/PCIeConverter.hh"
#include <cstring>

namespace gem5 {

// 初始化静态映射表
std::map<uint8_t, upf::TransactionType> PCIeTLPTypeMapping::tlpTypeToUPF = {
    // 内存事务
    {0x00, upf::TransactionType::MEMORY_READ},         // MRd - 内存读 (3DW, 无数据)
    {0x20, upf::TransactionType::MEMORY_READ},         // MRd - 内存读 (4DW, 无数据)
    {0x40, upf::TransactionType::MEMORY_WRITE},        // MWr - 内存写 (3DW, 有数据)
    {0x60, upf::TransactionType::MEMORY_WRITE},        // MWr - 内存写 (4DW, 有数据)
    
    // IO事务
    {0x02, upf::TransactionType::IO_READ},             // IORd - IO读
    {0x42, upf::TransactionType::IO_WRITE},            // IOWr - IO写
    
    // 配置事务
    {0x04, upf::TransactionType::CONFIG_READ},         // CfgRd0 - 配置读类型0
    {0x05, upf::TransactionType::CONFIG_READ},         // CfgRd1 - 配置读类型1
    {0x44, upf::TransactionType::CONFIG_WRITE},        // CfgWr0 - 配置写类型0
    {0x45, upf::TransactionType::CONFIG_WRITE},        // CfgWr1 - 配置写类型1
    
    // 完成事务
    {0x0A, upf::TransactionType::FLOW_CONTROL},        // Cpl - 完成包(无数据)
    {0x4A, upf::TransactionType::FLOW_CONTROL},        // CplD - 带数据的完成包
    
    // 消息事务
    {0x30, upf::TransactionType::MESSAGE},             // Msg - 消息(无数据)
    {0x70, upf::TransactionType::MESSAGE}              // MsgD - 带数据的消息
};

// 映射TLP类型到UPF事务类型
upf::TransactionType
PCIeTLPTypeMapping::mapTLPTypeToUPF(uint8_t tlpType)
{
    auto it = tlpTypeToUPF.find(tlpType);
    if (it != tlpTypeToUPF.end()) {
        return it->second;
    }
    
    // 默认返回未知类型
    return upf::TransactionType::UNKNOWN;
}

// 构造函数
PCIeConverter::PCIeConverter(const PCIeConverterParams &params)
    : SimObject(params)
{
    DPRINTF(PCIeConverter, "创建PCIe协议转换器: 目标协议=%s\n", targetProtocol.c_str());
}

// 源协议到UPF的转换
gem5::ConversionResult
PCIeConverter::sourceToUPF(void* sourcePacket, std::shared_ptr<upf::Packet>& upfPacket)
{
    if (!sourcePacket) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 直接调用TLP转换函数，暂时不处理Flit
    DPRINTF(PCIeConverter, "处理PCIe TLP数据包转换\n");
    return PCIeTLPToUPF(sourcePacket, upfPacket);
}

// UPF到目标协议的转换
gem5::ConversionResult
PCIeConverter::UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket, void*& targetPacket)
{
    if (!upfPacket) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 直接调用TLP转换函数，暂时不处理Flit
    DPRINTF(PCIeConverter, "处理UPF到PCIe TLP的转换\n");
    return UPFToPCIeTLP(upfPacket, targetPacket);
}

// PCIe TLP到UPF的转换
gem5::ConversionResult
PCIeConverter::PCIeTLPToUPF(void* tlpPacket, std::shared_ptr<upf::Packet>& upfPacket)
{
    if (!tlpPacket) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 转换为PCIe TLP类型
    PCIe::TLP* tlp = static_cast<PCIe::TLP*>(tlpPacket);
    
    // 创建UPF数据包
    upfPacket = std::make_shared<upf::Packet>(tlp->transactionId, 
                                             PCIeTLPTypeMapping::mapTLPTypeToUPF(tlp->fmt_type));
    
    if (upfPacket->getType() == upf::TransactionType::UNKNOWN) {
        return gem5::ConversionResult::UNSUPPORTED_FEATURE;
    }
    
    // 设置通用字段
    upfPacket->setAddress(tlp->address);
    upfPacket->setAddressSpace(mapPCIeAddressSpace(getTLPAddressType(tlp->fmt_type)));
    
    // 设置请求者和完成者ID
    upfPacket->setSourceId(tlp->requesterID);
    
    // 对于完成包，设置完成者ID
    if (isTLPCompletion(tlp->fmt_type)) {
        upfPacket->setDestinationId(tlp->completerID);
    }
    
    // 设置标签
    upfPacket->getMetadata().setInt("tag", tlp->tag);
    
    // 设置字节使能
    if (isTLPMemoryWrite(tlp->fmt_type)) {
        upfPacket->getMetadata().setInt("first_be", tlp->firstDWBE);
        upfPacket->getMetadata().setInt("last_be", tlp->lastDWBE);
    } else if (isTLPMemoryRead(tlp->fmt_type)) {
        upfPacket->getMetadata().setInt("first_be", 0xF); // 默认全部使能
        upfPacket->getMetadata().setInt("last_be", tlp->lastDWBE);
    }
    
    // 设置缓存属性
    upfPacket->setCacheAttribute(mapPCIeCacheAttribute(getTLPCacheAttr(tlp->tc_attr)));
    
    // 设置优先级
    upfPacket->setPriority(static_cast<upf::PriorityLevel>(getTLPTrafficClass(tlp->tc_attr)));
    
    // 复制数据(如果有)
    if (hasTLPData(tlp->fmt_type) && tlp->dataSize > 0) {
        //TODO检查函数调用
        // upfPacket->setData(tlp->data); 
        std::vector<uint8_t> data(tlp->data, tlp->data + tlp->dataSize);
        upfPacket->setData(data);
    }
    
    // 设置原始协议信息
    upfPacket->getMetadata().setString("source_protocol", "PCIe");
    
    DPRINTF(PCIeConverter, "PCIe TLP转换为UPF: 类型=%d, 地址=0x%lx\n",
            static_cast<int>(upfPacket->getType()), upfPacket->getAddress());
    
    return gem5::ConversionResult::SUCCESS;
}

// UPF到PCIe TLP的转换
gem5::ConversionResult
PCIeConverter::UPFToPCIeTLP(const std::shared_ptr<upf::Packet>& upfPacket, void*& tlpPacket)
{
    if (!upfPacket) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 创建PCIe TLP数据包
    PCIe::TLP* tlp = new PCIe::TLP();
    tlpPacket = tlp;
    
    // 设置事务ID
    tlp->transactionId = upfPacket->getTransactionId();
    
    // 设置地址
    tlp->address = upfPacket->getAddress();
    
    // 设置请求者ID
    tlp->requesterID = upfPacket->getSourceId();
    
    // 设置标签
    tlp->tag = upfPacket->getMetadata().getInt("tag", 0);
    
    // 设置字节使能
    tlp->firstDWBE = upfPacket->getMetadata().getInt("first_be", 0xF);
    tlp->lastDWBE = upfPacket->getMetadata().getInt("last_be", 0xF);
    
    // 设置FMT和TYPE字段
    uint8_t fmt_type = 0;
    
    // 根据UPF事务类型设置TLP类型
    switch (upfPacket->getType()) {
      case upf::TransactionType::MEMORY_READ:
        // 根据地址大小选择3DW或4DW头
        if (upfPacket->getAddress() > 0xFFFFFFFF) {
            fmt_type = 0x20; // MRd 4DW, 无数据
        } else {
            fmt_type = 0x00; // MRd 3DW, 无数据
        }
        break;
        
      case upf::TransactionType::MEMORY_WRITE:
        // 根据地址大小选择3DW或4DW头
        if (upfPacket->getAddress() > 0xFFFFFFFF) {
            fmt_type = 0x60; // MWr 4DW, 有数据
        } else {
            fmt_type = 0x40; // MWr 3DW, 有数据
        }
        break;
        
      case upf::TransactionType::IO_READ:
        fmt_type = 0x02; // IORd
        break;
        
      case upf::TransactionType::IO_WRITE:
        fmt_type = 0x42; // IOWr
        break;
        
      case upf::TransactionType::CONFIG_READ:
        // 默认使用类型0
        fmt_type = 0x04; // CfgRd0
        break;
        
      case upf::TransactionType::CONFIG_WRITE:
        // 默认使用类型0
        fmt_type = 0x44; // CfgWr0
        break;
        
      case upf::TransactionType::FLOW_CONTROL:
        // 根据是否有数据选择Cpl或CplD
        if (!upfPacket->getData().empty()) {
            fmt_type = 0x4A; // CplD
        } else {
            fmt_type = 0x0A; // Cpl
        }
        break;
        
      case upf::TransactionType::MESSAGE:
        // 根据是否有数据选择Msg或MsgD
        if (!upfPacket->getData().empty()) {
            fmt_type = 0x70; // MsgD
        } else {
            fmt_type = 0x30; // Msg
        }
        break;
        
      default:
        delete tlp;
        return gem5::ConversionResult::UNSUPPORTED_FEATURE;
    }
    
    tlp->fmt_type = fmt_type;
    
    // 设置TC和Attr字段
    tlp->tc_attr = (static_cast<uint8_t>(upfPacket->getPriority()) << 4) | 
                   (static_cast<uint8_t>(mapUPFToPCIeCacheAttr(upfPacket->getCacheAttribute())));
    
    // 复制数据(如果有)
    const std::vector<uint8_t>& data = upfPacket->getData();
    if (!data.empty()) {
        tlp->dataSize = data.size();
        tlp->data = new uint8_t[tlp->dataSize];
        std::memcpy(tlp->data, data.data(), tlp->dataSize);
    } else {
        tlp->dataSize = 0;
        tlp->data = nullptr;
    }
    
    DPRINTF(PCIeConverter, "UPF转换为PCIe TLP: FMT_TYPE=0x%x, 地址=0x%lx\n",
            tlp->fmt_type, tlp->address);
    
    return gem5::ConversionResult::SUCCESS;
}

// PCIe Flit到UPF的转换 - 暂未实现
gem5::ConversionResult
PCIeConverter::PCIeFlitToUPF(void* flitPacket, std::shared_ptr<upf::Packet>& upfPacket)
{
    // 暂未实现
    return gem5::ConversionResult::UNSUPPORTED_FEATURE;
}

// UPF到PCIe Flit的转换 - 暂未实现
gem5::ConversionResult
PCIeConverter::UPFToPCIeFlit(const std::shared_ptr<upf::Packet>& upfPacket, void*& flitPacket)
{
    // 暂未实现
    return gem5::ConversionResult::UNSUPPORTED_FEATURE;
}

// 辅助方法 - 映射PCIe地址空间类型到UPF地址空间
upf::AddressSpace
PCIeConverter::mapPCIeAddressSpace(uint8_t addressType)
{
    switch (addressType) {
      case 0: // 内存地址
        return upf::AddressSpace::MEMORY;
      case 1: // IO地址
        return upf::AddressSpace::IO;
      case 2: // 配置空间地址
        return upf::AddressSpace::CONFIG;
      default:
        return upf::AddressSpace::UNDEFINED;
    }
}

// 辅助方法 - 映射PCIe缓存属性到UPF缓存属性
upf::CacheAttribute
PCIeConverter::mapPCIeCacheAttribute(uint8_t cacheAttr)
{
    switch (cacheAttr) {
      case 0: // 非缓存
        return upf::CacheAttribute::NON_CACHEABLE;
      case 1: // 可缓存
        return upf::CacheAttribute::CACHEABLE;
      case 2: // 可缓存且可写回
        return upf::CacheAttribute::WRITE_BACK;
      case 3: // 写合并
        return upf::CacheAttribute::WRITE_COMBINING;
      default:
        return upf::CacheAttribute::UNDEFINED;
    }
}

// 辅助方法 - 映射UPF缓存属性到PCIe缓存属性
uint8_t
PCIeConverter::mapUPFToPCIeCacheAttr(upf::CacheAttribute cacheAttr)
{
    switch (cacheAttr) {
      case upf::CacheAttribute::NON_CACHEABLE:
        return 0;
      case upf::CacheAttribute::CACHEABLE:
        return 1;
      case upf::CacheAttribute::WRITE_BACK:
        return 2;
      case upf::CacheAttribute::WRITE_COMBINING:
        return 3;
      default:
        return 0; // 默认为非缓存
    }
}

// 辅助方法 - 从FMT_TYPE字段获取地址类型
uint8_t
PCIeConverter::getTLPAddressType(uint8_t fmt_type)
{
    // 根据FMT_TYPE字段判断地址类型
    if ((fmt_type & 0x1C) == 0x00) {
        return 0; // 内存地址
    } else if ((fmt_type & 0x1E) == 0x02) {
        return 1; // IO地址
    } else if ((fmt_type & 0x1E) == 0x04) {
        return 2; // 配置空间地址
    } else {
        return 0; // 默认为内存地址
    }
}

// 辅助方法 - 从TC_ATTR字段获取缓存属性
uint8_t
PCIeConverter::getTLPCacheAttr(uint8_t tc_attr)
{
    return tc_attr & 0x03;
}

// 辅助方法 - 从TC_ATTR字段获取流量类别
uint8_t
PCIeConverter::getTLPTrafficClass(uint8_t tc_attr)
{
    return (tc_attr >> 4) & 0x07;
}

// 辅助方法 - 判断TLP是否有数据
bool
PCIeConverter::hasTLPData(uint8_t fmt_type)
{
    return (fmt_type & 0x40) != 0;
}

// 辅助方法 - 判断TLP是否为内存读
bool
PCIeConverter::isTLPMemoryRead(uint8_t fmt_type)
{
    return (fmt_type & 0x1F) == 0x00;
}

// 辅助方法 - 判断TLP是否为内存写
bool
PCIeConverter::isTLPMemoryWrite(uint8_t fmt_type)
{
    return (fmt_type & 0x3F) == 0x40 || (fmt_type & 0x3F) == 0x60;
}

// 辅助方法 - 判断TLP是否为完成包
bool
PCIeConverter::isTLPCompletion(uint8_t fmt_type)
{
    return (fmt_type & 0x1E) == 0x0A;
}

// TLP处理接口实现
bool
PCIeConverter::processTLP(PCIe::TLP* tlp)
{
    if (!tlp) {
        return false;
    }
    
    DPRINTF(PCIeConverter, "处理TLP: FMT_TYPE=0x%x, 地址=0x%lx\n",
            tlp->fmt_type, tlp->address);
    
    // 将TLP转换为UPF
    std::shared_ptr<upf::Packet> upfPacket;
    gem5::ConversionResult result = TLPToUPF(tlp, upfPacket);
    
    if (result != gem5::ConversionResult::SUCCESS) {
        DPRINTF(PCIeConverter, "TLP转换为UPF失败: %d\n", static_cast<int>(result));
        return false;
    }
    
    // 这里可以进一步处理UPF包，例如发送到目标协议
    
    return true;
}

// TLP到UPF的转换
gem5::ConversionResult
PCIeConverter::TLPToUPF(PCIe::TLP* tlp, std::shared_ptr<upf::Packet>& upfPacket)
{
    if (!tlp) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 创建UPF数据包
    upfPacket = std::make_shared<upf::Packet>(tlp->transactionId, 
                                             PCIeTLPTypeMapping::mapTLPTypeToUPF(tlp->fmt_type));
    
    if (upfPacket->getType() == upf::TransactionType::UNKNOWN) {
        return gem5::ConversionResult::UNSUPPORTED_FEATURE;
    }
    
    // 设置通用字段
    upfPacket->setAddress(tlp->address);
    upfPacket->setAddressSpace(mapPCIeAddressSpace(getTLPAddressType(tlp->fmt_type)));
    
    // 设置请求者和完成者ID
    upfPacket->setSourceId(tlp->requesterID);
    
    // 对于完成包，设置完成者ID
    if (isTLPCompletion(tlp->fmt_type)) {
        upfPacket->setDestinationId(tlp->completerID);
    }
    
    // 设置标签
    upfPacket->getMetadata().setInt("tag", tlp->tag);
    
    // 设置字节使能
    upfPacket->getMetadata().setInt("first_be", tlp->firstDWBE);
    upfPacket->getMetadata().setInt("last_be", tlp->lastDWBE);
    
    // 设置缓存属性
    upfPacket->setCacheAttribute(mapPCIeCacheAttribute(getTLPCacheAttr(tlp->tc_attr)));
    
    // 设置优先级
    upfPacket->setPriority(static_cast<upf::PriorityLevel>(getTLPTrafficClass(tlp->tc_attr)));
    
    // 复制数据(如果有)
    if (tlp->hasData() && tlp->dataSize > 0) {
        std::vector<uint8_t> data(tlp->data, tlp->data + tlp->dataSize);
        upfPacket->setData(data);
    }
    
    // 设置原始协议信息
    upfPacket->setSourceProtocol("PCIe");
    
    DPRINTF(PCIeConverter, "TLP转换为UPF: 类型=%d, 地址=0x%lx\n",
            static_cast<int>(upfPacket->getType()), upfPacket->getAddress());
    
    return gem5::ConversionResult::SUCCESS;
}

// UPF到TLP的转换
gem5::ConversionResult
PCIeConverter::UPFToTLP(const std::shared_ptr<upf::Packet>& upfPacket, PCIe::TLP*& tlp)
{
    if (!upfPacket) {
        return gem5::ConversionResult::ERROR;
    }
    
    // 创建PCIe TLP数据包
    tlp = new PCIe::TLP();
    
    // 设置事务ID
    tlp->transactionId = upfPacket->getTransactionId();
    
    // 设置地址
    tlp->address = upfPacket->getAddress();
    
    // 设置请求者ID
    tlp->requesterID = upfPacket->getSourceId();
    
    // 设置标签
    tlp->tag = upfPacket->getMetadata().getInt("tag", 0);
    
    // 设置字节使能
    tlp->firstDWBE = upfPacket->getMetadata().getInt("first_be", 0xF);
    tlp->lastDWBE = upfPacket->getMetadata().getInt("last_be", 0xF);
    
    // 设置FMT和TYPE字段
    uint8_t fmt_type = 0;
    
    // 根据UPF事务类型设置TLP类型
    switch (upfPacket->getType()) {
      case upf::TransactionType::MEMORY_READ:
        // 根据地址大小选择3DW或4DW头
        if (upfPacket->getAddress() > 0xFFFFFFFF) {
            fmt_type = PCIe::TLPFmtType::MRD_4DW;
        } else {
            fmt_type = PCIe::TLPFmtType::MRD_3DW;
        }
        break;
        
      case upf::TransactionType::MEMORY_WRITE:
        // 根据地址大小选择3DW或4DW头
        if (upfPacket->getAddress() > 0xFFFFFFFF) {
            fmt_type = PCIe::TLPFmtType::MWR_4DW;
        } else {
            fmt_type = PCIe::TLPFmtType::MWR_3DW;
        }
        break;
        
      case upf::TransactionType::IO_READ:
        fmt_type = PCIe::TLPFmtType::IORD;
        break;
        
      case upf::TransactionType::IO_WRITE:
        fmt_type = PCIe::TLPFmtType::IOWR;
        break;
        
      case upf::TransactionType::CONFIG_READ:
        // 默认使用类型0
        fmt_type = PCIe::TLPFmtType::CFGRD0;
        break;
        
      case upf::TransactionType::CONFIG_WRITE:
        // 默认使用类型0
        fmt_type = PCIe::TLPFmtType::CFGWR0;
        break;
        
      case upf::TransactionType::FLOW_CONTROL:
        // 根据是否有数据选择Cpl或CplD
        if (!upfPacket->getData().empty()) {
            fmt_type = PCIe::TLPFmtType::CPLD;
        } else {
            fmt_type = PCIe::TLPFmtType::CPL;
        }
        break;
        
      case upf::TransactionType::MESSAGE:
        // 根据是否有数据选择Msg或MsgD
        if (!upfPacket->getData().empty()) {
            fmt_type = PCIe::TLPFmtType::MSGD;
        } else {
            fmt_type = PCIe::TLPFmtType::MSG;
        }
        break;
        
      default:
        delete tlp;
        return gem5::ConversionResult::UNSUPPORTED_FEATURE;
    }
    
    tlp->fmt_type = fmt_type;
    
    // 设置TC和Attr字段
    tlp->tc_attr = (static_cast<uint8_t>(upfPacket->getPriority()) << 4) | 
                   (static_cast<uint8_t>(mapUPFToPCIeCacheAttr(upfPacket->getCacheAttribute())));
    
    // 复制数据(如果有)
    const std::vector<uint8_t>& data = upfPacket->getData();
    if (!data.empty()) {
        tlp->setData(data.data(), data.size());
    }
    
    DPRINTF(PCIeConverter, "UPF转换为TLP: FMT_TYPE=0x%x, 地址=0x%lx\n",
            tlp->fmt_type, tlp->address);
    
    return gem5::ConversionResult::SUCCESS;
}

} // namespace gem5