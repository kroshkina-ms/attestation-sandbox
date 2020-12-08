#include <vector>
#include <cctype>
#include <options.hpp>


// file reading stuff 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Parse JWT
#include <base64.hpp>
#include <regex>

// PArse JSON - for this sample no need to fully parse JSON, this sample is meant to show token validation, while using limited number of third-party libraries

// utils
inline void split_string(const std::string& str, const std::string& delim, std::vector<std::string>& result) {
    std::regex regex(delim);
    result = std::vector<std::string>( std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
}

struct JwtMaster {
    std::string encoded_token;
    
    std::string encoded_header;
    std::string encoded_payload;
    std::string encoded_signature;

    std::string decoded_header;
    std::string decoded_payload;

    std::string jku; // TODO olga write me
    std::string kid; // tODO olga write description
    std::string attest_dns;
    std::string tenant;

    JwtMaster(const std::string& token): encoded_token(token) {
        std::vector<std::string> hps; // hps = header, payload, signature
        split_string(token, "\\.", hps);
        
        if (hps.size() != 3) 
            throw std::runtime_error("Invalid token!");
        
        encoded_header = hps[0];
        encoded_payload = hps[1];
        encoded_signature = hps[2];

        decoded_header = base64::uri_decode(encoded_header);
        decoded_payload = base64::uri_decode(encoded_payload);
        
        jku = retrieve_jku();
        kid = retrieve_kid();
        attest_dns = retreive_dns();
        tenant = retreive_tenant();
    }

private:
    std::string retrieve_jku() const {
        if (decoded_header.empty()) {
            std::cout << "Empty decoded JWT header, cannot retrieve 'jku' token" << std::endl;
            return "";
        }
        std::regex rgx(".*[jJ][kK][uU] *\" *: *\" *([0-9a-zA-Z\:\/\.]*)");
        std::smatch match;
        std::string result = (std::regex_search(decoded_header.begin(), decoded_header.end(), match, rgx)) ? std::string(match[1]) : "";
        return result;
    }

    std::string retrieve_kid() const {
        if (decoded_header.empty()) {
            std::cout << "Empty decoded JWT header, cannot retrieve 'kid' token" << std::endl;
            return "";
        }
        std::regex rgx(".*[kK][iI][dD] *\" *: *\" *([0-9a-zA-Z=]*)");
        std::smatch match;
        std::string result = (std::regex_search(decoded_header.begin(), decoded_header.end(), match, rgx)) ? std::string(match[1]) : "";
        return result;
    }

    std::string retreive_dns() const {
        if (decoded_header.empty()) {
            std::cout << "Empty decoded JWT header, cannot retrieve attest DNS" << std::endl;
            return "";
        }
        std::regex rgx("https:\/\/([0-9a-zA-Z\.]*)");
        std::smatch match;
        std::string result = (std::regex_search(decoded_header.begin(), decoded_header.end(), match, rgx)) ? std::string(match[1]) : "";
        return result;
    }

    std::string retreive_tenant() const {
        if (attest_dns.empty()) {
            std::cout << "Empty attest DNS, cannot retrieve tenant name" << std::endl;
            return "";
        }
        std::string result = attest_dns.substr(0, attest_dns.find_first_of("."));
        return result;
    }
};

int main(int argc, char* argv[], char* envp[]) {
    std::vector<std::string> argvec(argc, "");
    for (int count = 0; count < argc; count++) argvec[count] = argv[count];
    Options opts(argvec);
    opts.maybe_dump();
    
    // Read the JWT file
    std::vector<std::string> maybe_encoded_tokens; // do for many tokens
    std::ifstream infile;
    
    // TODO: move to a different file, it should be for win and linux
    infile.open(opts.jwt_filename);
    if (infile) {
        while (!infile.eof())
        {
            std::string encoded_jwt;
            getline(infile, encoded_jwt);
            maybe_encoded_tokens.push_back(encoded_jwt);
        }
    } 
    else {
        std::cout << "Failed to open file '" << opts.jwt_filename << "'" << std::endl;
    }
    infile.close();

    JwtMaster jwt(maybe_encoded_tokens[0]);

    std::cout << jwt.decoded_header << std::endl;
    // Call certs
    // ...
   
    // TODO: WRITE ME
    return EXIT_SUCCESS;
}

