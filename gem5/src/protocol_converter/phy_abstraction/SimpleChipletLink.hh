/*
 * SimpleChipletLink.hh
 * 简单芯粒链路物理层抽象模型头文件
 */

#pragma once

#include "base/types.hh"
#include "sim/sim_object.hh"
#include "params/SimpleChipletLink.hh"
#include "../packet/PLP.hh"
#include <functional>
#include <queue>
#include <random>
#include <vector>

namespace gem5 {

// 物理层传输模式枚举
enum PhyTransferMode {
    MODE_NON_FLIT,  // 非Flit模式
    MODE_FLIT       // Flit模式
};

// 物理层状态枚举
enum PhyLinkState {
    LINK_DOWN,      // 链路断开
    LINK_TRAINING,  // 链路训练中
    LINK_UP         // 链路正常
};

// 物理层错误类型枚举
enum PhyErrorType {
    ERROR_NONE,     // 无错误
    ERROR_BIT,      // 位错误
    ERROR_LINK      // 链路错误
};

// 协议类型枚举
enum PhyProtocolType {
    PROTOCOL_UNKNOWN, // 未知协议
    PROTOCOL_PCIE,    // PCIe协议
    PROTOCOL_CXL,     // CXL协议
    PROTOCOL_UCIE     // UCIe协议
};

// 数据包类型枚举
enum PhyPacketType {
    PACKET_UNKNOWN, // 未知类型
    PACKET_TLP,     // TLP类型
    PACKET_DLLP,    // DLLP类型
    PACKET_FLIT     // FLIT类型
};


// 简单芯粒链路物理层类
class SimpleChipletLink : public SimObject
{
  public:
    // 构造函数
    SimpleChipletLink(const SimpleChipletLinkParams &p);

    // 初始化
    void init() ;

    // 启动
    void startup() ;

    // 设置链路状态
    void setLinkState(PhyLinkState state);

    // 获取链路状态
    PhyLinkState getLinkState() const;

    // 设置传输模式
    void setTransferMode(PhyTransferMode mode);

    // 获取传输模式
    PhyTransferMode getTransferMode() const;
    
    // 检查是否有数据包可接收
    bool hasPacket() const;

    // 计算传输延迟
    Tick calculateTransferDelay(uint32_t size) const;

    // 模拟位错误
    PhyErrorType simulateBitError(uint32_t size) const;

    // 识别协议类型
    PhyProtocolType identifyProtocol(const PhyPacket* packet) const;

    // 识别数据包类型
    PhyPacketType identifyPacketType(const PhyPacket* packet) const;

    // 设置协议类型
    void setProtocolType(PhyProtocolType protocol);

    // 获取协议类型
    PhyProtocolType getProtocolType() const; // 移除参数

     // 发送TLP
    bool sendTLP(void* tlp, uint32_t size, PhyProtocolType protocol);

    // 发送DLLP
    bool sendDLLP(void* dllp, uint32_t size, PhyProtocolType protocol);

    //处理发送的物理层数据包 //phy->link
    void send_phy2link(const void* data, uint32_t size, bool isDLLP, PhyProtocolType protocol); // 修正参数类型和名称

    // 处理接收的物理层数据包 // port->phy PLP包
    void receive_PLP(PhyPacket* packet);

    //

  protected:
    // 链路参数
    uint64_t bandwidth;       // 带宽 (Gbps)
    Tick baseLatency;         // 基础延迟 (ps)
    double bitErrorRate;      // 位错误率
    uint32_t linkWidth;       // 链路宽度 (bits)
    double encodingOverhead;  // 编码开销 (如8b/10b为1.25)
    PhyProtocolType protocolType; // 协议类型?存疑
    
    // 链路状态
    PhyLinkState linkState;   // 当前链路状态
    PhyTransferMode transferMode; // 传输模式

    // 检查是否为PCIe TLP
    bool isPCIeTLP(const PhyPacket* packet) const;
    
    // 检查是否为PCIe DLLP
    bool isPCIeDLLP(const PhyPacket* packet) const;
    
    // 检查是否为CXL TLP
    bool isCXLTLP(const PhyPacket* packet) const;
    
    // 检查是否为CXL DLLP
    bool isCXLDLLP(const PhyPacket* packet) const;
    
    // 检查是否为UCIe Flit
    bool isUCIeFlit(const PhyPacket* packet) const;
};

} // namespace gem5
