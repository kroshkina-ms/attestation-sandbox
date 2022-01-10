#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <openssl/bio.h>
#include <openssl/x509.h>

namespace mvj {

    using BIO_ptr = std::unique_ptr<BIO, decltype(&BIO_free)>;
    using X509_ptr = std::unique_ptr<X509, decltype(&X509_free)>;
    using ASN1_TIME_ptr = std::unique_ptr<ASN1_TIME, decltype(&ASN1_STRING_free)>;

    class X509QuoteExt {
        std::unordered_map< std::string, std::vector<uint8_t> > extensions_;

    public:
        X509QuoteExt();
        ~X509QuoteExt();

        bool deserialize(const std::string& cert_content);
        std::vector<uint8_t> find_extension(const std::string& extension_oid) const;

    private:
        void init();
        void clear();
        bool parse_extension(const STACK_OF(X509_EXTENSION) *exts, int n);
    };
}

