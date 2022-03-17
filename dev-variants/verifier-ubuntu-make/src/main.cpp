#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <context.hpp>
#include <utils.hpp>
#include <jwt.hpp>
#include <jwks.hpp>
#include <curl.hpp>
#include <x509.hpp>
#include <base64.hpp>

#include <openenclave/host_verify.h>
#include <openenclave/attestation/verifier.h>
#include <openenclave/attestation/sgx/evidence.h>

using namespace mvj;

uint32_t _id_version;
uint64_t _attributes;
uint32_t _security_version;
std::array<uint8_t, OE_UNIQUE_ID_SIZE> _unique_id{};
std::array<uint8_t, OE_SIGNER_ID_SIZE> _signer_id{};
std::array<uint8_t, OE_PRODUCT_ID_SIZE> _product_id{};

std::string _str_unique_id;
std::string _str_signer_id;
std::string _str_product_id;

std::string uint8_to_hex_string(const uint8_t *v, const size_t s) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i = 0; i < s; i++) {
    ss << std::hex << std::setw(2) << static_cast<int>(v[i]);
  }
  return ss.str();
}

oe_result_t enclave_identity_verifier(oe_identity_t* identity, void* arg)
{
    _attributes = identity->attributes;
    _id_version = identity->id_version;
    _security_version = identity->security_version;

    std::memcpy(_unique_id.data(), identity->unique_id, OE_UNIQUE_ID_SIZE);
    std::memcpy(_signer_id.data(), identity->signer_id, OE_SIGNER_ID_SIZE);
    std::memcpy(_product_id.data(), identity->product_id, OE_PRODUCT_ID_SIZE);

    _str_unique_id = uint8_to_hex_string(identity->unique_id, OE_UNIQUE_ID_SIZE);
    _str_signer_id = uint8_to_hex_string(identity->signer_id, OE_SIGNER_ID_SIZE);
    _str_product_id = uint8_to_hex_string(identity->product_id, OE_PRODUCT_ID_SIZE);
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
    X509QuoteExt x509(certs[0]);
    auto quote_ext = x509.find_extension("1.3.6.1.4.1.311.105.1");
    if (quote_ext.empty()) {
        Context::always_log("ERROR - Failed to find wanted quote extension");
        return EXIT_FAILURE;
    }

    std::cout << "NUMBER OF CERTS = " << certs.size() << std::endl;

    // Verify the certificate.
    auto cert = certs[0];
    auto decoded_cert = base64::decode(cert);
    auto rv = oe_verify_attestation_certificate(decoded_cert.data(), decoded_cert.size(), enclave_identity_verifier, nullptr);
    if (rv != OE_OK) {
        Context::always_log("ERROR - Failed to verify attestation certificate");
    } else {
        Context::always_log("SUCCESS - Verified attestation certificate quote");
    }
    std::cout << "_str_unique_id " << std::endl << _str_unique_id  << std::endl << "size=" << OE_UNIQUE_ID_SIZE << std::endl;
    std::cout << "_str_signer_id " << std::endl << _str_signer_id  << std::endl << "size=" << OE_SIGNER_ID_SIZE  << std::endl;
    std::cout << "_str_product_id" << std::endl << _str_product_id << std::endl << "size=" << OE_PRODUCT_ID_SIZE << std::endl;

    if (!Context::instance().get_mrsigner().empty()) {
		// TODO: writeme
    }

    if (!Context::instance().get_productid().empty()) {
		// TODO: writeme
    }

    if (!Context::instance().get_mrenclave().empty()) {
		// TODO: writeme
    }

    if (!Context::instance().get_svn().empty()) {
		// TODO: writeme
    }

    if ( Context::instance().get_debuggable()) {
		// TODO: writeme
    }

    return EXIT_SUCCESS;
}
