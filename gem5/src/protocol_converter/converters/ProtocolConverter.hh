/*
 * ProtocolConverter.hh
 * 协议转换器基类头文件
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__

#include <memory>
#include <string>
#include <map>
#include <queue>
#include <functional>
#include "protocol_converter/upf/UPF.hh"
#include "sim/sim_object.hh"
#include "base/statistics.hh"
#include "base/trace.hh"

namespace gem5 {

// 协议转换器基类
class ProtocolConverter : public SimObject
{
  public:
    // 转换结果枚举
    enum class ConversionResult {
        SUCCESS,                // 转换成功
        UNSUPPORTED_FEATURE,    // 不支持的功能
        INCOMPATIBLE_FIELDS,    // 不兼容的字段
        BUFFER_FULL,            // 缓冲区已满
        ERROR                   // 其他错误
    };
    
    // 转换策略枚举
    enum class ConversionPolicy {
        CONSERVATIVE,           // 保守策略 - 不支持的功能直接失败
        AGGRESSIVE,             // 激进策略 - 尝试功能降级
        BEST_EFFORT             // 尽力而为 - 尝试多种方法实现转换
    };
    
    // 构造函数
    ProtocolConverter(const std::string& name, 
                      const std::string& sourceProtocol,
                      const std::string& targetProtocol);
    
    virtual ~ProtocolConverter() {}
    
    // 获取协议信息
    const std::string& getSourceProtocol() const { return sourceProtocol; }
    const std::string& getTargetProtocol() const { return targetProtocol; }
    
    // 设置转换策略
    void setConversionPolicy(ConversionPolicy policy) { conversionPolicy = policy; }
    ConversionPolicy getConversionPolicy() const { return conversionPolicy; }
    
    // 源协议到UPF的转换 (由子类实现)
    virtual ConversionResult sourceToUPF(void* sourcePacket, 
                                        std::shared_ptr<upf::Packet>& upfPacket) = 0;
    
    // UPF到目标协议的转换 (由子类实现)
    virtual ConversionResult UPFToTarget(const std::shared_ptr<upf::Packet>& upfPacket,
                                        void*& targetPacket) = 0;
    
    // 事务上下文管理
    std::shared_ptr<upf::TransactionContext> getTransactionContext(uint64_t transactionId);
    void addTransactionContext(std::shared_ptr<upf::TransactionContext> context);
    void removeTransactionContext(uint64_t transactionId);
    
    // 统计信息
    void regStats() override;
    
  protected:
    // 协议信息
    std::string sourceProtocol;
    std::string targetProtocol;
    
    // 转换策略
    ConversionPolicy conversionPolicy;
    
    // 事务上下文映射表
    std::map<uint64_t, std::shared_ptr<upf::TransactionContext>> transactionContexts;
    
    // 统计信息
    Stats::Scalar packetsConverted;
    Stats::Scalar conversionFailures;
    Stats::Scalar unsupportedFeatures;
    Stats::Scalar featureDowngrades;
    
    // 辅助方法
    bool isFeatureSupported(const std::string& feature);
    ConversionResult handleUnsupportedFeature(const std::string& feature);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_PROTOCOL_CONVERTER_HH__