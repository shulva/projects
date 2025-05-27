
#pragma once

#include <cstdint>
#include <vector>


namespace gem5 {

class TLP {
public:
    // TLP的三个基本组成部分
    std::vector<uint32_t> header;     // Header (3或4 DW)
    std::vector<uint32_t> payload;    // Data Payload (0-N DW)
    uint32_t digest;                  // TLP Digest (1 DW)

    // 构造函数
    TLP() : digest(0) {}

    // 析构函数
    virtual ~TLP() = default;

    // 获取Header大小(以DW为单位)
    uint8_t getHeaderSize() const {
        return header.size();
    }

    // 判断是否包含数据载荷
    bool hasPayload() const {
        return !payload.empty();
    }

    // 判断是否包含TLP Digest
    bool hasDigest() const {
        return digest != 0;
    }
};

}
