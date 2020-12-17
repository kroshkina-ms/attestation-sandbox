#include <context.hpp>
#include <utils.hpp>

#include <functional>
#include <iostream>
#include <chrono>

namespace mvj {

    typedef void (Context::* ContextMemFn)(const std::string&);

    Context::Context() {
        this->is_verbose_ = true;// false;
        this->jwt_filename_ = "";
        this->mrsigner_ = "";
        this->productid_ = "";
        this->mrenclave_ = "";
        this->svn_ = "";
        this->debuggable_ = -1;
    }

    void Context::log(const std::string& message) {
        if (Context::instance().is_verbose()) {
 //           unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//            std::cout << now << "\t---\t" << message << std::endl;
            std::cout << "\t---\t" << message << std::endl;
        }
    }

    void Context::set(const std::vector<std::string>& args) {
        const size_t sz = args.size();
        if (sz < 2) {
            this->help_and_exit();
        }
        this->reset();

        const std::unordered_map<std::string, ContextMemFn> opt_setters({
            {"-v", &Context::set_verbose},
            {"--verbose", &Context::set_verbose},
            {"-h", &Context::help_and_exit},
            {"--help", &Context::help_and_exit}
            });
        const std::unordered_map<std::string, ContextMemFn> arg_setters({
            {"-mrsigner", &Context::set_mrsigner},
            {"-mrenclave", &Context::set_mrenclave},
            {"-productid", &Context::set_productid},
            {"-svn", &Context::set_svn},
            {"-isdebuggable", &Context::set_debuggable},
            });

        for (size_t i = 1; i < sz; ++i) {
            std::string arg = args[i];
            strings::tolower(arg);

            auto opt_it = opt_setters.find(arg);
            if (opt_it != opt_setters.end()) {
                std::invoke(opt_it->second, this, "");
                continue;
            }

            auto arg_it = arg_setters.find(arg);
            if (arg_it != arg_setters.end()) {
                if (i < (sz - 1)) {
                    std::invoke(arg_it->second, this, args[1 + i++]);
                    continue;
                }
                else {
                    this->help_and_exit();
                }
            }

            if (this->jwt_filename_.empty()) {
                this->jwt_filename_ = args[i];
                continue;
            }

            this->help_and_exit();
        }
    }

    void Context::dump() const {
        if (this->is_verbose_) {
            std::cout << std::endl << "Arguments for this run:" << std::endl;
            std::cout << '\t' << "jwt_filename " << '\t' << ":" << '\t' << this->jwt_filename_ << std::endl;
            std::cout << '\t' << "mrsigner     " << '\t' << ":" << '\t' << this->mrsigner_ << std::endl;
            std::cout << '\t' << "productid    " << '\t' << ":" << '\t' << this->productid_ << std::endl;
            std::cout << '\t' << "mrenclave    " << '\t' << ":" << '\t' << this->mrenclave_ << std::endl;
            std::cout << '\t' << "svn          " << '\t' << ":" << '\t' << this->svn_ << std::endl;
            std::cout << '\t' << "debuggable   " << '\t' << ":" << '\t' << this->debuggable_ << std::endl;
        }
    }

    bool Context::is_verbose() const { return this->is_verbose_; }

    std::string Context::get_jwt_filename() const { return this->jwt_filename_; }
    std::string Context::get_mrsigner() const { return this->mrsigner_; }
    std::string Context::get_productid() const { return this->productid_; }
    std::string Context::get_mrenclave() const { return this->mrenclave_; }
    std::string Context::get_svn() const { return this->svn_; }
    int Context::get_debuggable() const { return this->debuggable_; }

    void Context::set_verbose(const std::string&) { this->is_verbose_ = true; }
    void Context::set_jwt_filename(const std::string& v) { this->jwt_filename_ = v; }
    void Context::set_mrsigner(const std::string& v) { this->mrsigner_ = v; }
    void Context::set_productid(const std::string& v) { this->productid_ = v; }
    void Context::set_mrenclave(const std::string& v) { this->mrenclave_ = v; }
    void Context::set_svn(const std::string& v) { this->svn_ = v; }
    void Context::set_debuggable(const std::string& v) {
        this->debuggable_ = !v.empty(); //TODO: fix me
    }

    void Context::help_and_exit(const std::string&) {
        std::cout << std::endl;
        std::cout << "Usage: maavalidatejwt [options] file" << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "    -mrsigner <value>        Verify MAA MRSIGNER value is <value>" << std::endl;
        std::cout << "    -mrenclave <value>       Verify MAA MRENCLAVE value is <value>" << std::endl;
        std::cout << "    -productid <value>       Verify MAA PRODUCTID value is <value>" << std::endl;
        std::cout << "    -svn <value>             Verify MAA SVN value is <value>" << std::endl;
        std::cout << "    -isdebuggable <value>    Verify MAA ISDEBUGGABLE value is <value>" << std::endl;
        std::cout << "    -v or --verbose          Include verbose messages during validation" << std::endl;
        std::cout << "    -h or --help             Print Help (this message) and exit" << std::endl;
        std::cout << std::endl;
        exit(EXIT_SUCCESS);
    }

    void Context::reset() {
        this->is_verbose_ = false;
        this->jwt_filename_.clear();
        this->mrsigner_.clear();
        this->productid_.clear();
        this->mrenclave_.clear();
        this->svn_.clear();
        this->debuggable_ = -1;
    }

}
