#include "klv/klv.hpp"

namespace asmproto {

ByteBuffer Klv::encode_ber_length(std::uint32_t length) {
    if (length > 0x00ffffffU) {
        throw AsmError("KLV length exceeds fixed 3-byte BER payload");
    }
    return ByteBuffer {
        0x83U,
        static_cast<std::uint8_t>((length >> 16U) & 0xffU),
        static_cast<std::uint8_t>((length >> 8U) & 0xffU),
        static_cast<std::uint8_t>(length & 0xffU),
    };
}

std::pair<std::uint32_t, std::size_t> Klv::decode_ber_length(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < 4U) {
        throw AsmError("missing BER length");
    }
    if (bytes[0] != 0x83U) {
        throw AsmError("invalid fixed BER length header");
    }
    const std::uint32_t length =
        (static_cast<std::uint32_t>(bytes[1]) << 16U)
        | (static_cast<std::uint32_t>(bytes[2]) << 8U)
        | static_cast<std::uint32_t>(bytes[3]);
    return {length, 4U};
}

ByteBuffer Klv::encode(const KlvPacket& packet) {
    if (packet.value.size() != packet.length) {
        throw AsmError("KLV packet length/value mismatch");
    }

    ByteBuffer out;
    const auto encoded_length = encode_ber_length(packet.length);
    out.reserve(packet.ul.size() + encoded_length.size() + packet.value.size());
    out.insert(out.end(), packet.ul.begin(), packet.ul.end());
    out.insert(out.end(), encoded_length.begin(), encoded_length.end());
    out.insert(out.end(), packet.value.begin(), packet.value.end());
    return out;
}

KlvPacket Klv::decode(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < 17U) {
        throw AsmError("KLV packet too short");
    }

    KlvPacket packet;
    std::copy_n(bytes.begin(), packet.ul.size(), packet.ul.begin());

    const auto [length, consumed] = decode_ber_length(bytes.subspan(packet.ul.size()));
    const auto value_offset = packet.ul.size() + consumed;
    if (bytes.size() != value_offset + length) {
        throw AsmError("KLV value length mismatch");
    }

    packet.length = length;
    packet.value.assign(bytes.begin() + static_cast<std::ptrdiff_t>(value_offset), bytes.end());
    return packet;
}

}  // namespace asmproto
