/*
 * UPF.hh
 * 统一协议格式(Unified Protocol Format)头文件
 */

#ifndef __PROTOCOL_CONVERTER_UPF_HH__
#define __PROTOCOL_CONVERTER_UPF_HH__

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace gem5 {

namespace upf {

// 事务类型枚举
enum class TransactionType {
    MEMORY_READ,           // 内存读
    MEMORY_WRITE,          // 内存写
    MEMORY_ATOMIC,         // 内存原子操作
    IO_READ,               // IO读
    IO_WRITE,              // IO写
    CONFIG_READ,           // 配置空间读
    CONFIG_WRITE,          // 配置空间写
    MESSAGE,               // 消息传递
    CACHE_COHERENCE,       // 缓存一致性操作
    FLOW_CONTROL,          // 流控制
    LINK_MANAGEMENT,       // 链路管理
    UNKNOWN                // 未知类型
};

// 地址空间类型
enum class AddressSpace {
    MEMORY,                // 内存空间
    IO,                    // IO空间
    CONFIG,                // 配置空间
    SPECIAL,               // 特殊地址空间
    UNDEFINED              // 未定义
};

// 缓存属性
enum class CacheAttribute {
    CACHEABLE,             // 可缓存
    NON_CACHEABLE,         // 不可缓存
    WRITE_COMBINING,       // 写合并
    WRITE_THROUGH,         // 写通
    WRITE_BACK,            // 写回
    UNDEFINED              // 未定义
};

// 优先级级别
enum class PriorityLevel {
    HIGHEST,               // 最高优先级
    HIGH,                  // 高优先级
    MEDIUM,                // 中等优先级
    LOW,                   // 低优先级
    LOWEST,                // 最低优先级
    UNDEFINED              // 未定义
};

// 原子操作类型
enum class AtomicOperation {
    COMPARE_SWAP,          // 比较交换
    FETCH_ADD,             // 获取并加
    FETCH_AND,             // 获取并与
    FETCH_OR,              // 获取并或
    FETCH_XOR,             // 获取并异或
    SWAP,                  // 交换
    NONE,                  // 无原子操作
    UNDEFINED              // 未定义
};

// 元数据类 - 存储无法直接映射的协议特定信息
class Metadata {
  private:
    std::map<std::string, std::string> stringValues;
    std::map<std::string, uint64_t> intValues;
    std::map<std::string, double> floatValues;
    std::map<std::string, std::vector<uint8_t>> binaryValues;

  public:
    // 设置和获取不同类型的元数据
    void setString(const std::string& key, const std::string& value) {
        stringValues[key] = value;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = stringValues.find(key);
        return (it != stringValues.end()) ? it->second : defaultValue;
    }
    
    void setInt(const std::string& key, uint64_t value) {
        intValues[key] = value;
    }
    
    uint64_t getInt(const std::string& key, uint64_t defaultValue = 0) const {
        auto it = intValues.find(key);
        return (it != intValues.end()) ? it->second : defaultValue;
    }
    
    void setFloat(const std::string& key, double value) {
        floatValues[key] = value;
    }
    
    double getFloat(const std::string& key, double defaultValue = 0.0) const {
        auto it = floatValues.find(key);
        return (it != floatValues.end()) ? it->second : defaultValue;
    }
    
    void setBinary(const std::string& key, const std::vector<uint8_t>& value) {
        binaryValues[key] = value;
    }
    
    std::vector<uint8_t> getBinary(const std::string& key) const {
        auto it = binaryValues.find(key);
        return (it != binaryValues.end()) ? it->second : std::vector<uint8_t>();
    }
    
    // 检查键是否存在
    bool hasKey(const std::string& key) const {
        return stringValues.find(key) != stringValues.end() ||
               intValues.find(key) != intValues.end() ||
               floatValues.find(key) != floatValues.end() ||
               binaryValues.find(key) != binaryValues.end();
    }
    
    // 清除所有元数据
    void clear() {
        stringValues.clear();
        intValues.clear();
        floatValues.clear();
        binaryValues.clear();
    }
};

// UPF数据包类 - 统一协议格式的核心数据结构
class Packet {
  private:
    // 基本字段
    uint64_t transactionId;        // 事务ID
    TransactionType type;          // 事务类型
    AddressSpace addrSpace;        // 地址空间
    uint64_t address;              // 地址
    uint32_t size;                 // 数据大小(字节)
    std::vector<uint8_t> data;     // 数据内容
    
    // 扩展字段
    CacheAttribute cacheAttr;      // 缓存属性
    PriorityLevel priority;        // 优先级
    AtomicOperation atomicOp;      // 原子操作类型
    bool bypassCache;              // 是否绕过缓存
    bool ordered;                  // 是否有序
    
    // 源和目标信息
    uint16_t sourceId;             // 源ID
    uint16_t destinationId;        // 目标ID
    
    // 协议特定元数据
    std::string sourceProtocol;    // 源协议
    std::string targetProtocol;    // 目标协议
    Metadata metadata;             // 元数据存储
    
    // 状态跟踪
    bool isResponse;               // 是否为响应包
    uint64_t requestId;            // 对应请求的ID(如果是响应包)
    bool hasError;                 // 是否有错误
    std::string errorMessage;      // 错误信息
    
  public:
    // 构造函数
    Packet(uint64_t _transactionId, TransactionType _type)
        : transactionId(_transactionId),
          type(_type),
          addrSpace(AddressSpace::UNDEFINED),
          address(0),
          size(0),
          cacheAttr(CacheAttribute::UNDEFINED),
          priority(PriorityLevel::MEDIUM),
          atomicOp(AtomicOperation::NONE),
          bypassCache(false),
          ordered(false),
          sourceId(0),
          destinationId(0),
          isResponse(false),
          requestId(0),
          hasError(false)
    {}
    
    // 基本字段访问器
    uint64_t getTransactionId() const { return transactionId; }
    TransactionType getType() const { return type; }
    AddressSpace getAddressSpace() const { return addrSpace; }
    uint64_t getAddress() const { return address; }
    uint32_t getSize() const { return size; }
    const std::vector<uint8_t>& getData() const { return data; }
    
    void setTransactionId(uint64_t id) { transactionId = id; }
    void setType(TransactionType t) { type = t; }
    void setAddressSpace(AddressSpace as) { addrSpace = as; }
    void setAddress(uint64_t addr) { address = addr; }
    void setSize(uint32_t s) { size = s; }
    void setData(const std::vector<uint8_t>& d) { data = d; }
    
    // 扩展字段访问器
    CacheAttribute getCacheAttribute() const { return cacheAttr; }
    PriorityLevel getPriority() const { return priority; }
    AtomicOperation getAtomicOperation() const { return atomicOp; }
    bool isBypassCache() const { return bypassCache; }
    bool isOrdered() const { return ordered; }
    
    void setCacheAttribute(CacheAttribute ca) { cacheAttr = ca; }
    void setPriority(PriorityLevel p) { priority = p; }
    void setAtomicOperation(AtomicOperation ao) { atomicOp = ao; }
    void setBypassCache(bool bypass) { bypassCache = bypass; }
    void setOrdered(bool order) { ordered = order; }
    
    // 源和目标信息访问器
    uint16_t getSourceId() const { return sourceId; }
    uint16_t getDestinationId() const { return destinationId; }
    
    void setSourceId(uint16_t id) { sourceId = id; }
    void setDestinationId(uint16_t id) { destinationId = id; }
    
    // 协议信息访问器
    const std::string& getSourceProtocol() const { return sourceProtocol; }
    const std::string& getTargetProtocol() const { return targetProtocol; }
    Metadata& getMetadata() { return metadata; }
    const Metadata& getMetadata() const { return metadata; }
    
    void setSourceProtocol(const std::string& protocol) { sourceProtocol = protocol; }
    void setTargetProtocol(const std::string& protocol) { targetProtocol = protocol; }
    
    // 状态跟踪访问器
    bool isResponsePacket() const { return isResponse; }
    uint64_t getRequestId() const { return requestId; }
    bool hasErrorFlag() const { return hasError; }
    const std::string& getErrorMessage() const { return errorMessage; }
    
    void setIsResponse(bool resp) { isResponse = resp; }
    void setRequestId(uint64_t id) { requestId = id; }
    void setError(bool err, const std::string& msg = "") { 
        hasError = err; 
        errorMessage = msg;
    }
    
    // 创建响应包
    std::shared_ptr<Packet> createResponse() const {
        auto response = std::make_shared<Packet>(transactionId, type);
        response->setIsResponse(true);
        response->setRequestId(transactionId);
        response->setSourceId(destinationId);
        response->setDestinationId(sourceId);
        response->setSourceProtocol(targetProtocol);
        response->setTargetProtocol(sourceProtocol);
        return response;
    }
    
    // 打印包信息(用于调试)
    std::string toString() const;
};

// 事务上下文类 - 用于跟踪跨多个包的事务
class TransactionContext {
  private:
    uint64_t transactionId;                // 事务ID
    std::string sourceProtocol;            // 源协议
    std::string targetProtocol;            // 目标协议
    std::shared_ptr<Packet> originalRequest; // 原始请求包
    std::vector<std::shared_ptr<Packet>> relatedPackets; // 相关包
    Metadata contextMetadata;              // 上下文元数据
    bool completed;                        // 事务是否完成
    
  public:
    TransactionContext(uint64_t id, const std::string& srcProto, const std::string& tgtProto)
        : transactionId(id),
          sourceProtocol(srcProto),
          targetProtocol(tgtProto),
          completed(false)
    {}
    
    // 访问器
    uint64_t getTransactionId() const { return transactionId; }
    const std::string& getSourceProtocol() const { return sourceProtocol; }
    const std::string& getTargetProtocol() const { return targetProtocol; }
    bool isCompleted() const { return completed; }
    
    void setCompleted(bool complete) { completed = complete; }
    
    // 原始请求包管理
    void setOriginalRequest(std::shared_ptr<Packet> request) {
        originalRequest = request;
    }
    
    std::shared_ptr<Packet> getOriginalRequest() const {
        return originalRequest;
    }
    
    // 相关包管理
    void addRelatedPacket(std::shared_ptr<Packet> packet) {
        relatedPackets.push_back(packet);
    }
    
    const std::vector<std::shared_ptr<Packet>>& getRelatedPackets() const {
        return relatedPackets;
    }
    
    // 元数据访问
    Metadata& getContextMetadata() { return contextMetadata; }
    const Metadata& getContextMetadata() const { return contextMetadata; }
};

} // namespace upf
} // namespace gem5

#endif // __PROTOCOL_CONVERTER_UPF_HH__