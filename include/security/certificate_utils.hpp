#pragma once

#include <filesystem>

#include <openssl/x509.h>

#include "asm/common.hpp"

namespace asmproto {

PeerIdentity read_peer_identity(X509* certificate);
void validate_certificate_profile(X509* certificate);
void validate_certificate_profile_from_file(const std::filesystem::path& certificate_path);

}  // namespace asmproto
