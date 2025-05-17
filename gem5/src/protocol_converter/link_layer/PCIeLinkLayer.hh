/*
 * PCIeLinkLayer.hh
 * PCIe链路层实现头文件
 */

#ifndef __PROTOCOL_CONVERTER_LINK_LAYER_PCIE_LINK_LAYER_HH__
#define __PROTOCOL_CONVERTER_LINK_LAYER_PCIE_LINK_LAYER_HH__

#include "protocol_converter/converters/TLPInterface.hh"
#include "base/types.hh"
#include "sim/sim_object.hh"
#include <queue>
#include <map>
#include <functional>

namespace gem5 {

// 控制DLLP类型枚举
enum DLLPType {
    DLLP_TYPE_FC_UPDATE,    // 流控更新
    DLLP_TYPE_ACK,          // 确认
    DLLP_TYPE_NAK           // 否认
};

// 接收缓冲区
struct ReceiveBuffer {
    std::queue<PCIe::TLP*> tlpQueue;      // TLP队列
    uint32_t maxSize;                      // 最大容量
    uint32_t currentSize;                  // 当前使用量
    FlowControlStatus flowControl;         // 流控状态
};

// 发送缓冲区
struct SendBuffer {
    std::queue<PCIe::TLP*> tlpQueue;      // TLP队列
    uint32_t maxSize;                      // 最大容量
    uint32_t currentSize;                  // 当前使用量
    FlowControlStatus flowControl;         // 流控状态
};

// 在PCIeLinkLayer类中添加
ReceiveBuffer rxBuffer;
SendBuffer txBuffer;
// 控制DLLP结构(流控和ACK/NAK共用)
struct ControlDLLP {
    uint8_t type;           // DLLP类型
    union {
        // 流控信息
        struct {
            uint8_t vcID;       // 虚拟通道ID
            uint8_t fcType;     // 流控类型(PH/PD/NPH/NPD/CH/CD)
            uint16_t credits;   // 信用点数量
        } fc;
        
        // ACK/NAK信息
        struct {
            uint16_t sequenceNum;  // 序列号
            uint16_t reserved;     // 保留字段
        } ack_nak;
    };
    uint16_t crc;           // CRC校验
};

// 数据DLP结构
struct DataDLP {
    uint16_t header;        // 包含序列号和链路控制位
    PCIe::TLP* tlp;         // TLP指针
    uint32_t lcrc;          // 链路CRC
    
    // 构造函数
    DataDLP() : header(0), tlp(nullptr), lcrc(0) {}
    
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

// PCIe链路层类
class PCIeLinkLayer : public SimObject, public TLPInterface
{
  public:
    // 构造函数
    PCIeLinkLayer(const PCIeLinkLayerParams &p);
    
    // 初始化
    void init() override;
    
    // 启动
    void startup() override;
    
    // TLP处理接口实现
    bool processTLP(PCIe::TLP* tlp) override;
    
    // 接收数据DLP的接口
    bool receiveDataDLP(DataDLP* dataDlp);
    
    // 接收控制DLLP的接口
    bool receiveControlDLLP(ControlDLLP* controlDllp);
    
    // 发送数据DLP的接口
    bool sendDataDLP(DataDLP* dataDlp);
    
    // 发送控制DLLP的接口
    bool sendControlDLLP(ControlDLLP* controlDllp);
    
    // 辅助方法 - 创建流控DLLP
    ControlDLLP* createFlowControlDLLP(uint8_t vcID, uint8_t fcType, uint16_t credits);
    
    // 辅助方法 - 创建ACK DLLP
    ControlDLLP* createAckDLLP(uint16_t sequenceNum);
    
    // 辅助方法 - 创建NAK DLLP
    ControlDLLP* createNakDLLP(uint16_t sequenceNum);
    
    // 设置物理层回调
    using PhyCallback = std::function<bool(void*)>;
    void setPhyCallback(PhyCallback callback);
    
    // 设置协议层回调
    using ProtocolCallback = std::function<bool(PCIe::TLP*)>;
    void setProtocolCallback(ProtocolCallback callback);
    
  protected:
    // 流控状态
    FlowControlStatus txFlowControl;  // 发送流控
    FlowControlStatus rxFlowControl;  // 接收流控
    
    // 序列号管理
    uint16_t nextTxSequenceNumber;    // 下一个发送序列号
    uint16_t expectedRxSequenceNumber; // 期望的接收序列号
    
    // 重传缓冲区
    std::map<uint16_t, DataDLP*> retryBuffer;
    
    // 回调函数
    PhyCallback phyCallback;
    ProtocolCallback protocolCallback;
    
    // 辅助方法 - 封装TLP为数据DLP
    DataDLP* encapsulateTLP(PCIe::TLP* tlp);
    
    // 辅助方法 - 从数据DLP提取TLP
    PCIe::TLP* extractTLP(DataDLP* dataDlp);
    
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
    void updateFlowControlCredits(uint8_t vcID, uint8_t fcType, uint16_t credits);
    
    // 辅助方法 - 检查流控状态
    bool checkFlowControl(PCIe::TLP* tlp);
    
    // 辅助方法 - 定期更新流控
    void periodicFlowControlUpdate();
    
    // 辅助方法 - 重传超时处理
    void handleRetryTimeout();
    
    // 辅助方法 - 计算TLP大小
    uint32_t calculateTLPSize(PCIe::TLP* tlp);
};

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_LINK_LAYER_PCIE_LINK_LAYER_HH__