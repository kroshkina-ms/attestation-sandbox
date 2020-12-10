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


// Curl-s 
#include <curl/curl.h> 
#include <cstdlib>

namespace strings {

    inline void split(const std::string& str, const std::string& delim, std::vector<std::string>& result) {
        std::regex regex(delim);
        result = std::vector<std::string>(std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
    }

    inline void remove_char(std::string& str, char c) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }

    inline void remove_spaces(std::string& str) {
        str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
    }
}

namespace json {

    // TODO: move to string or parsing utils/collection.
    // TODO: reuse in JWT token parsing.
    static std::string retrieve_value(const std::string& str, const std::string& key) {
        if (str.empty()) {
            // TODO: add verbose
            std::cout << "Input string is empty, cannot retrieve value for key '" << key << "'" << std::endl;
            return "";
        }
        std::regex rgx(".*" + key + "[ \n\r]*\"[ \n\r]*:[ \n\r]*\"[ \n\r]*([^\"]*)");
        std::smatch match;
        std::string result = (std::regex_search(str.begin(), str.end(), match, rgx)) ? std::string(match[1]) : "";
        return result;
    }

    static std::vector<std::string> retrieve_array(const std::string& str, const std::string& key) {
        if (str.empty()) {
            // TODO: add verbose
            std::cout << "Input string is empty, cannot retrieve array of value for key '" << key << "'" << std::endl;
            return std::vector<std::string>();
        }
        std::regex rgx(".*" + key + "[ \n\r]*\"[ \n\r]*:[ \n\r]*[[ \n\r]*([^\\]]*)");
        std::smatch match;
        std::string values = (std::regex_search(str.begin(), str.end(), match, rgx)) ? std::string(match[1]) : "";
        strings::remove_char(values, '"');
        std::vector<std::string> result;
        strings::split(values, ",", result);
        return result;
    }

}

// PArse JSON - for this sample no need to fully parse JSON, this sample is meant to show token validation, while using limited number of third-party libraries

// utils


struct JwtHelper {
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

    JwtHelper(const std::string& token): encoded_token(token) {
        std::vector<std::string> hps; // hps = header, payload, signature
        strings::split(token, "\\.", hps);
        
        if (hps.size() != 3) 
            throw std::runtime_error("Invalid token!");
        
        encoded_header = hps[0];
        encoded_payload = hps[1];
        encoded_signature = hps[2];

        decoded_header = base64::uri_decode(encoded_header);
        decoded_payload = base64::uri_decode(encoded_payload);
        
        jku = json::retrieve_value(decoded_header, "jku");
        kid = json::retrieve_value(decoded_header, "kid");
        attest_dns = retreive_dns();
        tenant = retreive_tenant();
    }

private:
    std::string retreive_dns() const {
        if (decoded_header.empty()) {
            std::cout << "Empty decoded JWT header, cannot retrieve attest DNS" << std::endl;
            return "";
        }
        std::regex rgx("https://([0-9a-zA-Z.]*)");
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

struct MaaJwk {
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

    MaaJwk() : kid(), kty(), x5c() {}

    explicit MaaJwk(const std::string& str) : kid(), kty(), x5c() {
        kid = json::retrieve_value(str, "kid");
        kty = json::retrieve_value(str, "kty");
        x5c = json::retrieve_array(str, "x5c");
    }

private:
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

struct JwkHelper {

    std::unordered_map<std::string, MaaJwk> keys;

    explicit JwkHelper(const std::string& raw_json_web_keys) {    
        std::vector<std::string> raw_keys;
        strings::split(raw_json_web_keys, "\\}[ \n\r]*,", raw_keys);
        for (auto raw_key : raw_keys) {
            MaaJwk key(raw_key);
            keys[key.kid] = key;
        }
    }
};

// https://curl.se/libcurl/c/getinmemory.html

struct MemoryStruct {
    char* memory;
    size_t size;

    std::string tostring() const {
        return (this->memory != nullptr) ? std::string(this->memory) : "";
    }
};

static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, char* argv[]) {
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

    JwtHelper jwt(maybe_encoded_tokens[0]);

    std::cout << jwt.decoded_header << std::endl;


    // Get certs

    CURL* curl;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    /* Minimalistic http request */
    //const char* request = "GET /certs HTTP/1.1\r\nHost: shareduks.uks.attest.azure.net\r\n\r\n";
    //size_t request_len = strlen(request);

    std::string response = "";

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    if (curl) {
    
        curl_easy_setopt(curl, CURLOPT_URL, jwt.jku.c_str());
  
        // [optional] set header
        struct curl_slist* header = NULL;
        std::string header_str = std::string("tenantName: ") + ((jwt.tenant.size() > 24) ? jwt.tenant.substr(0, 24) : jwt.tenant);
        header = curl_slist_append(header, header_str.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        // Options
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // [optional]
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // [required] follow HTTP 3xx redirects
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2); // [required] MAA supports only TLSv1.2
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // [per system] Do not verify the peer's SSL certificate
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); // [required] send all data to this function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk); // [required] we pass our 'chunk' struct to the callback function

        CURLcode res = curl_easy_perform(curl);

        if (CURLE_OK != res) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res)); //TODO: std::cout
            curl_easy_cleanup(curl);
            curl_slist_free_all(header);
            curl_global_cleanup();
            // TODO: cleanup
            return EXIT_FAILURE;
        }
        else {
            std::cout << "Received " << chunk.size << " bytes" << std::endl;
            response = chunk.tostring();
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(header);
    }
    free(chunk.memory);
    curl_global_cleanup();

    JwkHelper jwks(response);
    auto it = jwks.keys.find(jwt.kid);
    if (it != jwks.keys.end()) {
        std::cout << "Great succeess, the key is found: " <<  it->second.kty << std::endl;
    }
    else {
        std::cout << "that makes me a sad panda" << std::endl;
    }
    //  curl --location --request GET 'https://shareduks.uks.attest.azure.net/certs' --header 'tenantName: shareduks'
    // ...
    // TODO: WRITE ME
    // So here we go

    
    
    
    
    return EXIT_SUCCESS;
}

