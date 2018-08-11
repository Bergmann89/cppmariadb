#pragma once

#include <cppmariadb/config.h>
#include <cppmariadb/enums.h>
#include <cpputils/misc/exception.h>

namespace cppmariadb
{

    struct exception : public utl::exception
    {
    protected:
        void print_message(std::ostream& os) const override;

    public:
        error_code  error;
        std::string query;

        inline exception(const std::string& msg, error_code err, const std::string& q = std::string())
            : utl::exception(msg)
            , error         (err)
            , query         (q)
            { }
    };

}