#pragma once

#include <vector>
#include <stdint.h>

namespace mvj {

    class Verifier {

    public:
        Verifier();
        ~Verifier();
    
        bool is_quote_in_extension(const std::vector<uint8_t>& ext) const;

//    private:

    };

}

