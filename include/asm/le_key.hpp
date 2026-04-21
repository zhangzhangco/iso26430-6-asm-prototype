#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "asm/common.hpp"

namespace asmproto {

struct LeKeyRecord {
    std::uint32_t key_id {0};
    std::array<std::uint8_t, 16> key {};
    std::uint32_t expire_time {0};
    std::uint64_t attribute_data {0};
};

struct LeKeyLoadRequestPayload {
    std::vector<LeKeyRecord> keys;
};

struct LeKeyLoadResponsePayload {
    bool overflow {false};
};

struct LeKeyQueryIdRequestPayload {
    std::uint32_t key_id {0};
};

struct LeKeyQueryIdResponsePayload {
    bool key_present {false};
};

struct LeKeyQueryAllResponsePayload {
    std::vector<std::uint32_t> key_ids;
};

struct LeKeyPurgeIdRequestPayload {
    std::uint32_t key_id {0};
};

struct LeKeyPurgeIdResponsePayload {
    bool no_key_id {false};
};

class LeKeyStore {
public:
    virtual ~LeKeyStore() = default;
    virtual bool load_keys(std::span<const LeKeyRecord> keys) = 0;
    virtual bool contains_key(std::uint32_t key_id) const = 0;
    virtual std::vector<std::uint32_t> list_key_ids() const = 0;
    virtual bool purge_key(std::uint32_t key_id) = 0;
    virtual void purge_all() = 0;
};

class InMemoryLeKeyStore final : public LeKeyStore {
public:
    bool load_keys(std::span<const LeKeyRecord> keys) override;
    bool contains_key(std::uint32_t key_id) const override;
    std::vector<std::uint32_t> list_key_ids() const override;
    bool purge_key(std::uint32_t key_id) override;
    void purge_all() override;

private:
    std::vector<LeKeyRecord> keys_;
    static constexpr std::size_t kCapacity = 16U;
};

class JsonlLeKeyStore final : public LeKeyStore {
public:
    explicit JsonlLeKeyStore(const std::filesystem::path& path);

    bool load_keys(std::span<const LeKeyRecord> keys) override;
    bool contains_key(std::uint32_t key_id) const override;
    std::vector<std::uint32_t> list_key_ids() const override;
    bool purge_key(std::uint32_t key_id) override;
    void purge_all() override;

private:
    void persist() const;

    std::filesystem::path path_;
    std::vector<LeKeyRecord> keys_;
    static constexpr std::size_t kCapacity = 16U;
};

class LeKeyPayloadCodec {
public:
    static ByteBuffer encode_load_request(const LeKeyLoadRequestPayload& payload);
    static LeKeyLoadRequestPayload decode_load_request(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_load_response(const LeKeyLoadResponsePayload& payload);
    static LeKeyLoadResponsePayload decode_load_response(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_query_id_request(const LeKeyQueryIdRequestPayload& payload);
    static LeKeyQueryIdRequestPayload decode_query_id_request(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_query_id_response(const LeKeyQueryIdResponsePayload& payload);
    static LeKeyQueryIdResponsePayload decode_query_id_response(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_query_all_response(const LeKeyQueryAllResponsePayload& payload);
    static LeKeyQueryAllResponsePayload decode_query_all_response(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_purge_id_request(const LeKeyPurgeIdRequestPayload& payload);
    static LeKeyPurgeIdRequestPayload decode_purge_id_request(std::span<const std::uint8_t> bytes);

    static ByteBuffer encode_purge_id_response(const LeKeyPurgeIdResponsePayload& payload);
    static LeKeyPurgeIdResponsePayload decode_purge_id_response(std::span<const std::uint8_t> bytes);
};

}  // namespace asmproto
