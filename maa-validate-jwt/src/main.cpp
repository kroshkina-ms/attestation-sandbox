#include <context.hpp>
#include <utils.hpp>
#include <jwt.hpp>
#include <jwks.hpp>

#include <vector>
#include <cctype>

// file reading stuff 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Curl-s 
#include <curl/curl.h> 
#include <cstdlib>

// TODO: delete all this stuff

// x.509
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>

using std::cout;
using std::endl;
using std::stringstream;
using std::map;
using std::vector;
using std::string;

// fixme 


// PArse JSON - for this sample no need to fully parse JSON, this sample is meant to show token validation, while using limited number of third-party libraries

// utils

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

using namespace mvj;

int main(int argc, char* argv[]) {
    std::vector<std::string> argvec(argc, "");
    for (int count = 0; count < argc; count++) argvec[count] = argv[count];
    
    Context& ctx = Context::instance();
    ctx.set(argvec);
    
    // Read the JWT file
    std::vector<std::string> raw_tokens;
    std::ifstream infile;
    infile.open(ctx.get_jwt_filename());
    if (infile) {
        while (!infile.eof())
        {
            std::string encoded_jwt;
            getline(infile, encoded_jwt);
            raw_tokens.push_back(encoded_jwt);
        }
    } 
    else {
        Context::log("Failed to open file " + ctx.get_jwt_filename());
    }
    infile.close();

    Jwt jwt(raw_tokens[0]);


    // Get certs

    CURL* curl;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::string response = "";

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    if (curl) {
    
        curl_easy_setopt(curl, CURLOPT_URL, jwt.get_jku().c_str());
  
        // [optional] set header
        struct curl_slist* header = NULL;
        std::string header_str = std::string("tenantName: ") + jwt.get_tenant();
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

    Jwks jwks(response);

    OpenSSL_add_all_algorithms();
    
    std::string pemCertString = "-----BEGIN CERTIFICATE-----\n" + jwks.get_certs(jwt.get_kid())[0] +"\n-----END CERTIFICATE-----";
    int certLen = static_cast<int>(pemCertString.size());
    
    BIO* certBio = BIO_new(BIO_s_mem());
    BIO_write(certBio, pemCertString.c_str(), certLen);
    X509* certX509 = PEM_read_bio_X509(certBio, NULL, NULL, NULL);
    if (!certX509) {
        fprintf(stderr, "unable to parse certificate in memory\n");
        return EXIT_FAILURE;
    }
    char* subj = X509_NAME_oneline(X509_get_subject_name(certX509), NULL, 0);
    char* issuer = X509_NAME_oneline(X509_get_issuer_name(certX509), NULL, 0);
    std::string ttt(subj);
    std::string ttt2(issuer);
    cout << "ttt=" << ttt << " ttt2=" << ttt2 << endl;

    return EXIT_SUCCESS;
}

