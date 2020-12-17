#include <x509.hpp>

#include <iostream>
#include <vector>
#include <cstring>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/pem.h>

namespace mvj {
 
//    const char* kSgxExtensionOidX = "1.2.840.113556.10.1.1";

    X509Cert::X509Cert() {

    }

    X509Cert::~X509Cert() {
        clear();
    }

    bool X509Cert::deserialize(const std::string& cert_str) {
        clear();
        init();

        std::string cert_content = "-----BEGIN CERTIFICATE-----\n" + cert_str + "\n-----END CERTIFICATE-----";

        // Put the certificate contents into an openssl IO stream (BIO)
        BIO_ptr input(BIO_new(BIO_s_mem()), BIO_free);
        BIO_write(input.get(), cert_content.c_str(), (int)cert_content.size());

        // Create an openssl certificate from the BIO
        X509_ptr cert(PEM_read_bio_X509_AUX(input.get(), NULL, NULL, NULL), X509_free);

        // Create a BIO to hold info from the cert
        BIO_ptr output_bio(BIO_new(BIO_s_mem()), BIO_free);

        //
        // Get the full human readable representation of the certificate
        //
        X509_print_ex(output_bio.get(), cert.get(), 0, 0);
        
        // Put the contents of the BIO into a C++ string
        char buffer[32768]; //TODO: change the size
        memset(buffer, 0, 32768);
        BIO_read(output_bio.get(), buffer, 32768 - 1);
        std::string cert_details = std::string(buffer);
        
        BIO_reset(output_bio.get());
        std::cout << "========================================" << std::endl;
        std::cout << "  Certificate details:" << std::endl;
        std::cout << cert_details << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;


        //
        // Get the subject of the certificate.
        //

        // According to the openssl documentation:
        // The returned value is an internal pointer which MUST NOT be freed
        X509_NAME* subject = X509_get_subject_name(cert.get());

        // Print the subject into a BIO and then get a string
        X509_NAME_print_ex(output_bio.get(), subject, 0, 0);


        char cert_buffer[4096];
        memset(cert_buffer, 0, 4096);
        BIO_read(output_bio.get(), cert_buffer, 4096 - 1);
        std::string cert_subject = std::string(cert_buffer);


        // Max subject length should be 555 bytes with ascii characters or 3690
        // bytes with unicode, based on the max allowed lengths for each component
        // of the subject plus the formatting.
        // Country code - 2 bytes
        // State/locality name - 128 bytes
        // Organization name - 64 bytes
        // Organizational unit name - 64 bytes
        // Common name - 64 bytes
        // email address - 64 bytes

        std::cout << "Subject:" << std::endl;
        std::cout << cert_subject << std::endl;
        std::cout << std::endl;


        //
        // Get the expiration date of the certificate
        //

        // X509_get_notAfter returns the time that the cert expires, in Abstract
        // Syntax Notation
        // According to the openssl documentation:
        // The returned value is an internal pointer which MUST NOT be freed
        ASN1_TIME* expires = X509_get_notAfter(cert.get());

        // Construct another ASN1_TIME for the unix epoch, get the difference
        // between them and use that to calculate a unix timestamp representing
        // when the cert expires
        ASN1_TIME_ptr epoch(ASN1_TIME_new(), ASN1_STRING_free);
        ASN1_TIME_set_string(epoch.get(), "700101000000Z");
        int days, seconds;
        ASN1_TIME_diff(&days, &seconds, epoch.get(), expires);
        time_t expire_timestamp = (days * 24 * 60 * 60) + seconds;

        std::cout << "Expiration timestamp:" << std::endl;
        std::cout << expire_timestamp << std::endl;
        std::cout << std::endl;
        return true;
    }

    void X509Cert::init() {
        // Adds all algorithms to the table (digests and ciphers).
        // In versions prior to 1.1.0 EVP_cleanup() removed all ciphers and digests from the table.
        // It no longer has any effect in OpenSSL 1.1.0.
        OpenSSL_add_all_algorithms();

        // ERR_load_crypto_strings() Registers the error strings for all libcrypto functions.
        // ERR_free_strings() frees all previously loaded error strings.
        ERR_load_crypto_strings();

        // Disables configuration. If called before OPENSSL_config() no configuration takes place.
        OPENSSL_no_config();
    }

    void X509Cert::clear() {
        FIPS_mode_set(0);
        CONF_modules_unload(1);
        CONF_modules_free();
        ERR_free_strings();
        EVP_cleanup();
        CRYPTO_cleanup_all_ex_data(); // CRYPTO_cleanup_all_ex_data and ERR_remove_state should be called on each thread, and not just the main thread.
    }
}
