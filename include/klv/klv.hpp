#pragma once

#include "asm/common.hpp"

namespace asmproto {

struct KlvPacket {
    Ul ul;
    std::uint32_t length {0};
    ByteBuffer value;
};

class Klv {
public:
    static ByteBuffer encode(const KlvPacket& packet);
    static KlvPacket decode(std::span<const std::uint8_t> bytes);
    static ByteBuffer encode_ber_length(std::uint32_t length);
    static std::pair<std::uint32_t, std::size_t> decode_ber_length(std::span<const std::uint8_t> bytes);
};

}  // namespace asmproto
