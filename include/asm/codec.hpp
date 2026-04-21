#pragma once

#include "asm/common.hpp"
#include "klv/klv.hpp"

namespace asmproto {

struct CommandDescriptor {
    Command command;
    bool response;
};

class Codec {
public:
    static KlvPacket encode_request(const StandardRequest& request);
    static StandardRequest decode_request(const KlvPacket& packet);

    static KlvPacket encode_response(const StandardResponse& response);
    static StandardResponse decode_response(const KlvPacket& packet);

    static CommandDescriptor describe_ul(const Ul& ul);
    static const Ul& ul_for(Command command, bool response);
};

}  // namespace asmproto
