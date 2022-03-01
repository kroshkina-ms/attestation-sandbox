#include <vector>
#include <string>
#include <iostream>

#include <context.hpp>
#include <utils.hpp>
#include <jwt.hpp>
#include <jwks.hpp>
#include <curl.hpp>
#include <x509.hpp>

using namespace mvj;

int main(int argc, char* argv[]) {
    std::vector<std::string> argvec(argc, "");
    for (int count = 0; count < argc; count++) argvec[count] = argv[count];

    // Parse arguments.
    Context::instance().set(argvec);

    // Read JWT from file.
    std::vector<std::string> str_tokens;
    if (!file::get_lines(Context::instance().get_jwt_filename(), str_tokens)) {
        return EXIT_FAILURE;
    }

    // Deserialize JWT.
    Jwt jwt;
    if (!jwt.deserialize(str_tokens[0])) {
        return EXIT_FAILURE;
    }

    // Send request to MAA to get certficates.
    Curl curl;
    std::string response = curl.get(jwt.get_jku());
    if (response.empty()) {
        Context::log("ERROR - Failed to retrieve certificates");
        return EXIT_FAILURE;
    }

    // Deserialize JSON Web Keys and find x.509 certificates for the key.
    Jwks jwks(response);
    std::vector<std::string> certs;
    if (!jwks.get_certs(jwt.get_kid(), certs)) {
        Context::log("ERROR - Failed to find x509 certificates for the key");
        return EXIT_FAILURE;
    }

    // X.509.
    X509QuoteExt x509;
    if (!x509.deserialize(certs[0])) {
        Context::log("ERROR - Failed to deserialize x509 cert");
        return EXIT_FAILURE;
    }

    // Validate.

    // 1. Verify if quote extension is in certificate
    auto quote_ext = x509.find_extension("1.3.6.1.4.1.311.105.1");
    if (quote_ext.size() > 0) {
        //std::string s(quote_ext.begin(), quote_ext.end());
        //std::cout << "quote=" << s << std::endl;
        Context::log("Embedded quote found in certificate");
    } else {
        Context::log("ERROR - Failed to find wanted quote extension");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
