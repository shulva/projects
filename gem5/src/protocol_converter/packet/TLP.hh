/*
 * TLPInterface.hh
 * PCIe TLP接口头文件 - 为链路层传上来的TLP提供接口
 */

#ifndef __PROTOCOL_CONVERTER_CONVERTERS_TLP_INTERFACE_HH__
#define __PROTOCOL_CONVERTER_CONVERTERS_TLP_INTERFACE_HH__

#include "../upf/UPF.hh"
#include <queue>
#include <memory>
#include <vector>
#include <cstdint>
#include <functional>
#include <cstring>  // 添加这一行，包含 memcpy 函数

namespace gem5 {

// TLP数据包结构定义
namespace PCIe {

// TLP头部格式类型
enum TLPFmtType {
    // 3DW头部，无数据
    MRD_3DW = 0x00,      // 内存读请求
    IORD = 0x02,         // IO读请求
    CFGRD0 = 0x04,       // 配置读请求类型0
    CFGRD1 = 0x05,       // 配置读请求类型1
    CPL = 0x0A,          // 完成包，无数据
    
    // 3DW头部，有数据
    MWR_3DW = 0x40,      // 内存写请求
    IOWR = 0x42,         // IO写请求
    CFGWR0 = 0x44,       // 配置写请求类型0
    CFGWR1 = 0x45,       // 配置写请求类型1
    CPLD = 0x4A,         // 完成包，有数据
    
    // 4DW头部，无数据
    MRD_4DW = 0x20,      // 内存读请求
    MSG = 0x30,          // 消息
    
    // 4DW头部，有数据
    MWR_4DW = 0x60,      // 内存写请求
    MSGD = 0x70          // 带数据的消息
};

// TLP数据包结构
struct TLP {
    // 基本字段
    uint64_t transactionId;    // 事务ID
    uint8_t fmt_type;          // 格式和类型
    uint8_t tc_attr;           // 流量类别和属性
    uint64_t address;          // 地址
    uint16_t requesterID;      // 请求者ID
    uint16_t completerID;      // 完成者ID
    uint8_t tag;               // 标签
    uint8_t firstDWBE;         // 第一个双字节使能
    uint8_t lastDWBE;          // 最后一个双字节使能

    // 数据部分
    uint32_t dataSize;         // 数据大小
    uint8_t* data;             // 数据指针

    // 构造函数
    TLP() : transactionId(0), fmt_type(0), tc_attr(0), address(0),
            requesterID(0), completerID(0), tag(0), firstDWBE(0),
            lastDWBE(0), dataSize(0), data(nullptr) {}

    // 析构函数
    ~TLP() {
        if (data) {
            delete[] data;
        }
    }

    // 复制构造函数
    TLP(const TLP& other) : transactionId(other.transactionId),
                           fmt_type(other.fmt_type),
                           tc_attr(other.tc_attr),
                           address(other.address),
                           requesterID(other.requesterID),
                           completerID(other.completerID),
                           tag(other.tag),
                           firstDWBE(other.firstDWBE),
                           lastDWBE(other.lastDWBE),
                           dataSize(other.dataSize),
                           data(nullptr) {
        if (other.data && other.dataSize > 0) {
            data = new uint8_t[dataSize];
            std::memcpy(data, other.data, dataSize);
        }
    }

    // 赋值运算符
    TLP& operator=(const TLP& other) {
        if (this != &other) {
            transactionId = other.transactionId;
            fmt_type = other.fmt_type;
            tc_attr = other.tc_attr;
            address = other.address;
            requesterID = other.requesterID;
            completerID = other.completerID;
            tag = other.tag;
            firstDWBE = other.firstDWBE;
            lastDWBE = other.lastDWBE;
            
            if (data) {
                delete[] data;
                data = nullptr;
            }
            
            dataSize = other.dataSize;
            if (other.data && other.dataSize > 0) {
                data = new uint8_t[dataSize];
                std::memcpy(data, other.data, dataSize);  // 修改这一行
            }
        }
        return *this;
    }

    // 设置数据
    void setData(const uint8_t* src, uint32_t size) {
        if (data) {
            delete[] data;
        }
        
        dataSize = size;
        if (size > 0 && src) {
            data = new uint8_t[size];
            std::memcpy(data, src, size);  // 修改这一行
        } else {
            data = nullptr;
        }
    }

    // 判断是否有数据
    bool hasData() const {
        return (fmt_type & 0x40) && (dataSize > 0);
    }

    // 判断是否有地址字段
    bool hasAddr() const {
        // 根据fmt_type判断是否包含地址字段
        // 常见的包含地址的TLP类型：内存读/写，IO读/写
        // 配置请求使用不同的地址表示方式 (Bus/Device/Function/Register)
        // 完成包和消息通常不包含地址字段
        uint8_t type = fmt_type & 0x1F; // 提取类型字段

        switch (type) {
            case 0x00: // MRd (3DW)
            case 0x20: // MRd (4DW)
            case 0x40: // MWr (3DW)
            case 0x60: // MWr (4DW)
            case 0x02: // IORd
            case 0x42: // IOWr
                return true;
            default:
                return false;
        }
    }
};



// TLP接口类
class TLPInterface {
  public:
    // 构造函数
    TLPInterface();
    virtual ~TLPInterface();
    
    // TLP处理接口
    virtual bool processTLP(PCIe::TLP* tlp) = 0;
    
    // 从链路层接收TLP
    bool receiveTLPFromLinkLayer(PCIe::TLP* tlp);
    
    // 向链路层发送TLP
    bool sendTLPToLinkLayer(PCIe::TLP* tlp);
    
    
    // 设置TLP处理回调
    using TLPCallback = std::function<void(PCIe::TLP*)>;
    void setTLPCallback(TLPCallback callback);
    
  protected:
    // TLP处理回调
    TLPCallback tlpCallback;
    
    // TLP队列
    std::queue<PCIe::TLP*> tlpQueue;
    
    // 辅助方法
    bool isTLPMemoryRead(uint8_t fmt_type);
    bool isTLPMemoryWrite(uint8_t fmt_type);
    bool isTLPCompletion(uint8_t fmt_type);
    bool isTLPConfiguration(uint8_t fmt_type);
    bool isTLPMessage(uint8_t fmt_type);
    
    uint8_t getTLPAddressType(uint8_t fmt_type);
    uint8_t getTLPTrafficClass(uint8_t tc_attr);
    uint8_t getTLPCacheAttr(uint8_t tc_attr);
};

} // namespace PCIe

} // namespace gem5

#endif // __PROTOCOL_CONVERTER_CONVERTERS_TLP_INTERFACE_HH__