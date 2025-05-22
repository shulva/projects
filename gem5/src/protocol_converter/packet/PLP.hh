/*
 * PCIePhysicalLayerPacket.hh
 * PCIe物理层数据包 (PLP) 结构定义
 */

#pragma once

#include "base/types.hh" // 用于 uint8_t, Tick 等
#include <vector>        // 用于 std::vector

namespace gem5 {

// PCIe K-Codes (控制符号)
// 注意: 这些值是8b/10b编码中的示例。
// 实际的比特模式和128b/130b编码中的帧机制更为复杂。
// 此枚举提供了一个高层级的符号表示。
enum class PCIeKCode : uint8_t {
    // 常见的8b/10b编码K-Codes示例
    STP = 0xFB, // K27.7 - Start of TLP Packet (TLP起始)
    SDP = 0xFA, // K27.6 - Start of DLLP Packet (DLLP起始)
    END = 0xFD, // K29.7 - End Packet (有效结束)
    EDB = 0xFE, // K30.7 - End Packet (错误结束)

    // 其他重要的K-Codes/Ordered Sets
    SKP = 0x1C, // K28.0 - Skip Ordered Set (时钟补偿)
    FTS = 0x3C, // K28.2 - Fast Training Sequence (快速训练序列)
    COM = 0xBC, // K28.5 - Comma (符号对齐)
    // 根据仿真所需的详细程度添加更多

    NONE = 0x00 // 占位符，表示无特定K-code
};

// PCIe物理层数据包 (PLP) 结构
// 这个结构表示在物理链路上经过帧界定符包装的数据包。
struct PCIePhysicalLayerPacket {
    PCIeKCode startSymbol;          // 包起始符号 (例如 STP, SDP)
    std::vector<uint8_t> payload;   // 实际的有效载荷数据 (TLP 或 DLLP 的字节流)
    PCIeKCode endSymbol;            // 包结束符号 (例如 END, EDB)

    // 默认构造函数
    PCIePhysicalLayerPacket()
        : startSymbol(PCIeKCode::NONE),
          endSymbol(PCIeKCode::NONE),
         {}

    // 带参数的构造函数
    PCIePhysicalLayerPacket(PCIeKCode start,
                            const std::vector<uint8_t>& p_payload,
                            PCIeKCode end
                            )
        : startSymbol(start),
          payload(p_payload),
          endSymbol(end),
           {}

    // 获取有效载荷大小
    size_t getPayloadSize() const {
        return payload.size();
    }

    // 获取有效载荷数据的常量指针
    const uint8_t* getPayloadData() const {
        if (payload.empty()) {
            return nullptr;
        }
        return payload.data();
    }

    // 获取有效载荷数据的可变指针 (如果需要修改)
    uint8_t* getPayloadDataMutable() {
        if (payload.empty()) {
            return nullptr;
        }
        return payload.data();
    }

    // 设置有效载荷
    void setPayload(const uint8_t* data, size_t size) {
        payload.assign(data, data + size);
    }
};

} // namespace gem5
