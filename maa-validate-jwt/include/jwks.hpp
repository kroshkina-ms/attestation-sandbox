#pragma once

#include <unordered_map>
#include <string>

namespace mvj {

    struct Jwk {
        // A JSON Web Key (JWK) is a JavaScript Object Notation (JSON) data
        // structure that represents a cryptographic key.
        // See https://tools.ietf.org/html/rfc7517

        // TODO: does MAA use only the following parameters for the keys?

        std::string kid; // Key ID
        // See JWK specification https://tools.ietf.org/html/rfc7517#section-4.1
            // The "kty" values should
            // either be registered in the IANA "JSON Web Key Types" registry
            //    established by[JWA] or be a value that contains a Collision -
            //    Resistant Name.
            //    https://tools.ietf.org/html/rfc7517#ref-JWA

        std::string kty; // Key Type
        std::vector<std::string> x5c; //  The "x5c" (X.509 certificate chain) parameter contains a chain of one or more PKIX certificates[RFC5280].
        Jwk() {}
        explicit Jwk(const std::string& str);
        
        // TODO : JWK names are case sensitive (see https://tools.ietf.org/html/rfc7517#section-8.1.1)


        // TODO: move to string or parsing utils/collection.
        // TODO: reuse in JWT token parsing.



        // See JWK specification https://tools.ietf.org/html/rfc7517#section-4.1
            // The "kty" values should
            // either be registered in the IANA "JSON Web Key Types" registry
            //    established by[JWA] or be a value that contains a Collision -
            //    Resistant Name.
            //    https://tools.ietf.org/html/rfc7517#ref-JWA
    };

    class Jwks {
        std::unordered_map<std::string, Jwk> keys_;

    public:
        explicit Jwks(const std::string& keys_str);
        std::vector<std::string> get_certs(const std::string& key) const;
    };

}
