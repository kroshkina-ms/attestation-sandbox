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

// Parse JSON
#include <unordered_set>

enum class JsonType {
    kJsonTypeUndef = 0,
    kJsonTypeBool = 1,
    kJsonTypeInt = 2,
    kJsonTypeFloat = 3,
    kJsonTypeString = 4,
    kJsonTypeObject = 5
};

const std::unordered_set<char> kEscapeCharacters = {'\\'};
const std::unordered_set<char> kSpecialCharacters = { '{', '}', '"', ',', ':', '[', ']'};

struct Json {

    std::vector<Json*> json_values;
    std::vector<bool> bool_values;
    std::vector<int> int_values;
    std::vector<double> double_values;
    std::vector<std::string> string_values;

    JsonType my_type;

    std::unordered_map<std::string, Json> object_data;

    Json() {}

    explicit Json(const std::string& doc): my_type(JsonType::kJsonTypeObject) {
        const size_t sz = doc.size();
        for (size_t i = 0; i < sz; ++i) {
            switch (doc[i]) {
            case '{':
                my_type = JsonType::kJsonTypeObject;
                size_t start = i + 1;
                size_t end = doc.find_last_of('}');
                if (start < sz && end != std::string::npos) {
                    json_values.push_back(new Json());
                    parse_object(doc.substr(start, end-start), object_data);
                }
                i = end;
                break;
            case '\\':


            default:

            }
        }
        
    }

    ~Json() {
        if (!json_values.empty()) {
            for (int i = 0; i < json_values.size(); ++i) {
                delete json_values[i];
                json_values[i] = nullptr;
            }
            json_values.clear();
        }
    }

    // I don't think I need to write here a real JSONparser
    void parse_object(const std::string& value, std::unordered_map<std::string, Json>& obj) {
        const size_t sz = value.size();
        for (size_t i = 0; i < sz - 1; ++i) {
            size_t start = i + 1;
            switch (value[i]) {
            case '{':
                my_type = JsonType::kJsonTypeObject;
                size_t end = value.find_last_of('}');
                if (end != std::string::npos) {
                    json_values.push_back(new Json());
                    parse_object(value.substr(start, end - start), object_data);
                }
                i = end;
                break;
            case '\\':

                parse_string(start, );


            default:

            }
            
        }

    }

};

// utils
inline void split_string(const std::string& str, const std::string& delim, std::vector<std::string>& result) {
    std::regex regex(delim);
    result = std::vector<std::string>( std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
}

struct Jwt {
    std::string encoded_token;
    std::string encoded_header;
    std::string encoded_payload;
    std::string encoded_signature;
    std::string decoded_header;
    std::string decoded_payload;

    Jwt(const std::string& token): encoded_token(token) {
        std::vector<std::string> hps; // hps = header, payload, signature
        split_string(token, "\\.", hps);
        
        if (hps.size() != 3) 
            throw std::runtime_error("Invalid token!");
        
        encoded_header = hps[0];
        encoded_payload = hps[1];
        encoded_signature = hps[2];
        decoded_header = base64::uri_decode(encoded_header);
        decoded_payload = base64::uri_decode(encoded_payload);
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

    Jwt jwt(maybe_encoded_tokens[0]);

    std::cout << jwt.decoded_header << std::endl;
    // Call certs
    // ...
   
    // TODO: WRITE ME
    return EXIT_SUCCESS;
}

