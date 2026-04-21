#include "security/certificate_utils.hpp"

#include <filesystem>
#include <iomanip>
#include <memory>
#include <sstream>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include "asm/common.hpp"

namespace asmproto {

namespace {

std::string name_to_string(X509_NAME* name) {
    if (name == nullptr) {
        return {};
    }

    BIO* bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        throw AsmError("failed to allocate BIO");
    }
    std::unique_ptr<BIO, decltype(&BIO_free)> holder(bio, &BIO_free);

    if (X509_NAME_print_ex(bio, name, 0, XN_FLAG_RFC2253) <= 0) {
        throw AsmError("failed to format X509 name");
    }

    BUF_MEM* mem = nullptr;
    BIO_get_mem_ptr(bio, &mem);
    return mem != nullptr ? std::string(mem->data, mem->length) : std::string {};
}

std::string digest_to_hex(const unsigned char* digest, unsigned int len) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < len; ++i) {
        out << std::setw(2) << static_cast<int>(digest[i]);
    }
    return out.str();
}

struct BioFileCloser {
    void operator()(BIO* bio) const noexcept {
        if (bio != nullptr) {
            BIO_free(bio);
        }
    }
};

}  // namespace

PeerIdentity read_peer_identity(X509* certificate) {
    if (certificate == nullptr) {
        throw AsmError("peer certificate missing");
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0U;
    if (X509_digest(certificate, EVP_sha256(), digest, &digest_len) != 1) {
        throw AsmError("failed to digest peer certificate");
    }

    return {
        .subject = name_to_string(X509_get_subject_name(certificate)),
        .issuer = name_to_string(X509_get_issuer_name(certificate)),
        .sha256_thumbprint = digest_to_hex(digest, digest_len),
    };
}

void validate_certificate_profile(X509* certificate) {
    if (certificate == nullptr) {
        throw AsmError("certificate missing");
    }

    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(X509_get_pubkey(certificate), &EVP_PKEY_free);
    if (key == nullptr) {
        throw AsmError("failed to read certificate public key");
    }
    if (EVP_PKEY_base_id(key.get()) != EVP_PKEY_RSA) {
        throw AsmError("certificate public key must be RSA");
    }

    const RSA* rsa = EVP_PKEY_get0_RSA(key.get());
    if (rsa == nullptr) {
        throw AsmError("failed to access RSA public key");
    }

    const BIGNUM* n = nullptr;
    const BIGNUM* e = nullptr;
    RSA_get0_key(rsa, &n, &e, nullptr);
    if (n == nullptr || e == nullptr) {
        throw AsmError("RSA public key parameters missing");
    }
    if (BN_num_bits(n) != 2048) {
        throw AsmError("certificate RSA key must be 2048 bits");
    }
    if (BN_get_word(e) != 65537UL) {
        throw AsmError("certificate RSA exponent must be 65537");
    }
}

void validate_certificate_profile_from_file(const std::filesystem::path& certificate_path) {
    std::unique_ptr<BIO, BioFileCloser> bio(
        BIO_new_file(certificate_path.string().c_str(), "r"),
        BioFileCloser {});
    if (bio == nullptr) {
        throw AsmError("failed to open certificate file");
    }

    std::unique_ptr<X509, decltype(&X509_free)> certificate(
        PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr),
        &X509_free);
    if (certificate == nullptr) {
        throw AsmError("failed to parse certificate file");
    }
    validate_certificate_profile(certificate.get());
}

}  // namespace asmproto
