/*
 * UnifiedLinkLayer.hh
 * 统一链路层实现头文件 - 支持多种协议(PCIe, CXL, UCIE等)
 */

#ifndef __PROTOCOL_CONVERTER_LINK_LAYER_UNIFIED_LINK_LAYER_HH__
#define __PROTOCOL_CONVERTER_LINK_LAYER_UNIFIED_LINK_LAYER_HH__

#include "protocol_converter/converters/TLPInterface.hh"
#include "base/types.hh"
#include "sim/sim_object.hh"
#include <queue>
#include <map>
#include <functional>

namespace gem5 {

// 协议类型枚举
enum ProtocolType {
    PROTOCOL_PCIE,    // PCIe协议
    PROTOCOL_CXL,     // CXL协议
    PROTOCOL_UCIE,    // UCIe协议
    PROTOCOL_GENERIC  // 通用协议
};

// 控制DLLP类型枚举
enum DLLPType {
    DLLP_TYPE_FC_UPDATE,    // 流控更新
    DLLP_TYPE_ACK,          // 确认
    DLLP_TYPE_NAK,          // 否认
    DLLP_TYPE_PROTOCOL_SPECIFIC // 协议特定DLLP
};

// 接收缓冲区
struct ReceiveBuffer {
    std::queue<void*> tlpQueue;      // 通用TLP队列
    uint32_t maxSize;                // 最大容量
    uint32_t currentSize;            // 当前使用量
    FlowControlStatus flowControl;   // 流控状态
    ProtocolType protocol;           // 缓冲区协议类型
    
    ReceiveBuffer() : maxSize(0), currentSize(0), protocol(PROTOCOL_GENERIC) {}
};

// 发送缓冲区
struct SendBuffer {
    std::queue<void*> tlpQueue;      // 通用TLP队列
    uint32_t maxSize;                // 最大容量
    uint32_t currentSize;            // 当前使用量
    FlowControlStatus flowControl;   // 流控状态
    ProtocolType protocol;           // 缓冲区协议类型
    
    SendBuffer() : maxSize(0), currentSize(0), protocol(PROTOCOL_GENERIC) {}
};

// 控制DLLP结构(通用版本)
struct ControlDLLP {
    uint8_t type;           // DLLP类型
    ProtocolType protocol;  // 协议类型
    
    union {
        // 流控信息
        struct {
            uint8_t vcID;       // 虚拟通道ID
            uint8_t fcType;     // 流控类型
            uint16_t credits;   // 信用点数量
        } fc;
        
        // ACK/NAK信息
        struct {
            uint16_t sequenceNum;  // 序列号
            uint16_t reserved;     // 保留字段
        } ack_nak;
        
        // 协议特定信息
        struct {
            uint8_t subtype;     // 子类型
            uint8_t reserved[3]; // 保留字段
            void* protocolData;  // 协议特定数据
        } protocol_specific;
    };
    
    uint16_t crc;           // CRC校验
};

// 数据DLP结构(通用版本)
struct DataDLP {
    uint16_t header;        // 包含序列号和链路控制位
    ProtocolType protocol;  // 协议类型
    void* tlp;              // 通用TLP指针
    uint32_t lcrc;          // 链路CRC
    
    // 构造函数
    DataDLP() : header(0), protocol(PROTOCOL_GENERIC), tlp(nullptr), lcrc(0) {}
    
    // 析构函数
    ~DataDLP() {
        // 注意：不负责删除tlp，由调用者管理
    }
    
    // 获取序列号
    uint16_t getSequenceNumber() const {
        return (header >> 4) & 0xFFF; // 12位序列号
    }
    
    // 设置序列号
    void setSequenceNumber(uint16_t seqNum) {
        header = (header & 0x000F) | ((seqNum & 0xFFF) << 4);
    }
    
    // 获取链路控制位
    uint8_t getLinkControl() const {
        return header & 0x0F;
    }
    
    // 设置链路控制位
    void setLinkControl(uint8_t lc) {
        header = (header & 0xFFF0) | (lc & 0x0F);
    }
};

// 流控状态结构
struct FlowControlStatus {
    uint16_t availableCredits;  // 可用信用点
    uint16_t consumedCredits;   // 已消耗信用点
    uint16_t returnedCredits;   // 已返回信用点
    uint8_t bufferType;         // buffer类型标识
    
    FlowControlStatus() 
        : availableCredits(0), consumedCredits(0), 
          returnedCredits(0), bufferType(0) {}
};

// 统一链路层类
class UnifiedLinkLayer : public SimObject, public TLPInterface
{
  public:
    // 构造函数
    UnifiedLinkLayer(const UnifiedLinkLayerParams &p);
    
    // 初始化
    void init() override;
    
    // 启动
    void startup() override;
    
    // TLP处理接口实现
    bool processTLP(PCIe::TLP* tlp) override;
    
    // TLP到UPF的转换
    ProtocolConverter::ConversionResult TLPToUPF(
        PCIe::TLP* tlp, 
        std::shared_ptr<upf::Packet>& upfPacket) override;
    
    // UPF到TLP的转换
    ProtocolConverter::ConversionResult UPFToTLP(
        const std::shared_ptr<upf::Packet>& upfPacket,
        PCIe::TLP*& tlp) override;
    
    // 接收数据DLP的接口
    bool receiveDataDLP(DataDLP* dataDlp);
    
    // 接收控制DLLP的接口
    bool receiveControlDLLP(ControlDLLP* controlDllp);
    
    // 发送数据DLP的接口
    bool sendDataDLP(DataDLP* dataDlp);
    
    // 发送控制DLLP的接口
    bool sendControlDLLP(ControlDLLP* controlDllp);
    
    // 辅助方法 - 创建流控DLLP
    ControlDLLP* createFlowControlDLLP(ProtocolType protocol, uint8_t vcID, uint8_t fcType, uint16_t credits);
    
    // 辅助方法 - 创建ACK DLLP
    ControlDLLP* createAckDLLP(ProtocolType protocol, uint16_t sequenceNum);
    
    // 辅助方法 - 创建NAK DLLP
    ControlDLLP* createNakDLLP(ProtocolType protocol, uint16_t sequenceNum);
    
    // 设置物理层回调
    using PhyCallback = std::function<bool(void*, ProtocolType)>;
    void setPhyCallback(PhyCallback callback);
    
    // 设置协议层回调
    using ProtocolCallback = std::function<bool(void*, ProtocolType)>;
    void setProtocolCallback(ProtocolCallback callback);
    
    // 设置协议类型
    void setProtocolType(ProtocolType rxProtocol, ProtocolType txProtocol);
    
  protected:
    // 接收和发送缓冲区
    ReceiveBuffer rxBuffer;
    SendBuffer txBuffer;
    
    // 协议类型
    ProtocolType rxProtocolType;  // 接收协议类型
    ProtocolType txProtocolType;  // 发送协议类型
    
    // 序列号管理
    uint16_t nextTxSequenceNumber;    // 下一个发送序列号
    uint16_t expectedRxSequenceNumber; // 期望的接收序列号
    
    // 重传缓冲区
    std::map<uint16_t, DataDLP*> retryBuffer;
    
    // 回调函数
    PhyCallback phyCallback;
    ProtocolCallback protocolCallback;
    
    // 辅助方法 - 封装TLP为数据DLP
    DataDLP* encapsulateTLP(void* tlp, ProtocolType protocol);
    
    // 辅助方法 - 从数据DLP提取TLP
    void* extractTLP(DataDLP* dataDlp);
    
    // 辅助方法 - 计算CRC
    uint32_t calculateLCRC(DataDLP* dataDlp);
    uint16_t calculateDLLPCRC(ControlDLLP* controlDllp);
    
    // 辅助方法 - 验证CRC
    bool validateLCRC(DataDLP* dataDlp);
    bool validateDLLPCRC(ControlDLLP* controlDllp);
    
    // 辅助方法 - 处理确认
    void processAcknowledgement(uint16_t sequenceNum);
    
    // 辅助方法 - 处理否认
    void processNegativeAcknowledgement(uint16_t sequenceNum);
    
    // 辅助方法 - 更新流控状态
    void updateFlowControlCredits(ProtocolType protocol, uint8_t vcID, uint8_t fcType, uint16_t credits);
    
    // 辅助方法 - 检查流控状态
    bool checkFlowControl(void* tlp, ProtocolType protocol);
    
    // 辅助方法 - 定期更新流控
    void periodicFlowControlUpdate();
    
    // 辅助方法 - 重传超时处理
    void handleRetryTimeout();
    
    // 辅助方法 - 计算TLP大小
    uint32_t calculateTLPSize(void* tlp, ProtocolType protocol);
    
    // 协议转换辅助方法
    void* convertTLP(void* tlp, ProtocolType srcProtocol, ProtocolType dstProtocol);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_LINK_LAYER_UNIFIED_LINK_LAYER_HH__