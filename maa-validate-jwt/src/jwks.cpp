#include <jwks.hpp>

#include <utils.hpp>
#include <context.hpp>
#include <regex>

namespace mvj {

    Jwk::Jwk(const std::string& str) : kid(), kty(), x5c() {
        kid = json::get_value(str, "kid");
        kty = json::get_value(str, "kty");
        x5c = json::get_array(str, "x5c");
    }

    Jwks::Jwks(const std::string& str) {
        std::vector<std::string> raw_keys;
        mvj::strings::split(str, "\\}[ \n\r]*,", raw_keys);
        for (auto raw_key : raw_keys) {
            Jwk key(raw_key);
            keys_[key.kid] = key;
        }
    }
    
    std::vector<std::string> Jwks::get_certs(const std::string& key) const {
        auto it = keys_.find(key);
        if (it != keys_.end()) {
            return it->second.x5c;
        }
        else {
            Context::log("Failed to find key " + key);
            return std::vector<std::string>();
        }
    }

}
