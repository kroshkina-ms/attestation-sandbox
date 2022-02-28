#include <verify.hpp>

//#include <openenclave/host.h>
//#include <openenclave/host_verify.h>
//#include <openenclave/bits/defs.h>

#include <context.hpp>

namespace mvj {

    Verifier::Verifier() {
    }

    Verifier::~Verifier() {
    }

    bool Verifier::is_quote_in_extension(const std::vector<uint8_t>& ) const {
    	// The quote embedded in the extension is an SGX quote. The oe_verify_remote_report API requires an OE remote quote, so
    	// transform the SGX quote into an OE quote.
/*    	oe_report_header_t header;

    	header.version = 1;
    	header.report_size = etx.size();
    	header.report_type = oe_report_type::OE_REPORT_TYPE_SGX_REMOTE;
    	auto header_vector(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header + 1)));
		std::vector<uint8_t> _oe_quote;
    	_oe_quote.insert(_oe_quote.end(), header_vector.begin(), header_vector.end());
    	_oe_quote.insert(_oe_quote.end(), ext.begin(), ext.end());
		oe_report_t _parsedReport{ 0 };
    	auto rv = oe_verify_remote_report(_oe_quote.data(), _oe_quote.size(), nullptr, 0, &_parsedReport);
    	if (rv != OE_OK)
    	{
    	    return false;
    	}*/
    	return true;
    }
}
