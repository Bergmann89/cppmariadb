#pragma once

#include <mariadb/config.h>
#include <mariadb/enums.h>
#include <cpputils/misc/exception.h>

namespace mariadb
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