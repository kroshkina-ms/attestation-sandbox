#include <vector>
#include <string>
#include <iostream>

#include <context.hpp>
#include <utils.hpp>
#include <jwt.hpp>
#include <jwks.hpp>
#include <curl.hpp>
#include <x509.hpp>
#include <base64.hpp>

#include <openenclave/host_verify.h>
#include <cstring>

using namespace mvj;

uint32_t _id_version;
uint64_t _attributes;
uint32_t _security_version;
std::array<uint8_t, OE_UNIQUE_ID_SIZE> _unique_id{};
std::array<uint8_t, OE_SIGNER_ID_SIZE> _signer_id{};
std::array<uint8_t, OE_PRODUCT_ID_SIZE> _product_id{};

oe_result_t verify_metadata_quote(oe_identity_t* identity, void* arg)
{
    (void)arg;
    _attributes = identity->attributes;
    _id_version = identity->id_version;
    _security_version = identity->security_version;
    std::memcpy(_unique_id.data(), identity->unique_id, OE_UNIQUE_ID_SIZE);
    std::memcpy(_signer_id.data(), identity->signer_id, OE_SIGNER_ID_SIZE);
    std::memcpy(_product_id.data(), identity->product_id, OE_PRODUCT_ID_SIZE);
    return OE_OK;
}


int main(int argc, char* argv[]) {
    std::vector<std::string> argvec(argc, "");
    for (int count = 0; count < argc; count++) argvec[count] = argv[count];

    // Parse arguments.
    Context::instance().set(argvec);

    // Read JWT from file.
    std::vector<std::string> str_tokens;
    if (!file::get_lines(Context::instance().get_jwt_filename(), str_tokens)) {
        Context::always_log("ERROR - Failed to read input file");
        return EXIT_FAILURE;
    }

    // Deserialize JWT.
    Jwt jwt;
    if (!jwt.deserialize(str_tokens[0])) {
        Context::always_log("ERROR - Failed to deserialize JWT token");
        return EXIT_FAILURE;
    }

    // Send request to MAA to get certficates.
    Curl curl;
    std::string response = curl.get(jwt.get_jku());
    if (response.empty()) {
        Context::always_log("ERROR - Failed to retrieve certificates");
        return EXIT_FAILURE;
    }

    // Deserialize JSON Web Keys and find x.509 certificates for the key.
    Jwks jwks(response);
    std::vector<std::string> certs;
    if (!jwks.get_certs(jwt.get_kid(), certs)) {
        Context::always_log("ERROR - Failed to find x509 certificates for the key");
        return EXIT_FAILURE;
    }

    // X.509.
    X509QuoteExt x509;
    if (!x509.deserialize(certs[0])) {
        Context::always_log("ERROR - Failed to deserialize x509 cert");
        return EXIT_FAILURE;
    }

    // 1. Verify if quote extension is in certificate.
    auto quote_ext = x509.find_extension("1.3.6.1.4.1.311.105.1");
    if (quote_ext.size() > 0) {
        Context::always_log("SUCCESS - Embedded quote found in certificate");
    } else {
        Context::always_log("ERROR - Failed to find wanted quote extension");
        return EXIT_FAILURE;
    }

    // 2. Call the oe_verify_attestation_certificate on THE CERT.
    auto decoded_cert = base64::decode(certs[0]);
    auto rv = oe_verify_attestation_certificate(decoded_cert.data(), decoded_cert.size(), verify_metadata_quote, nullptr);
    if (rv != OE_OK)
    {
        Context::always_log("ERROR - Failed to verify attestation certificate");
        return EXIT_FAILURE;
    } else {
        Context::always_log("SUCCESS - Verified attestation certificate quote");
    }

    return EXIT_SUCCESS;
}
