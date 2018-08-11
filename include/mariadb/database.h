#pragma once

#include <string>
#include <mariadb/config.h>
#include <mariadb/enums.h>
#include <mariadb/forward/connection.h>

namespace mariadb
{

    struct database
    {
        static inline connection  connect   (const std::string&     host,
                                             const uint&            port,
                                             const std::string&     user,
                                             const std::string&     password,
                                             const std::string&     database,
                                             const client_flags&    flags);
        static inline error_code  error_code(MYSQL* handle);
        static inline std::string error_msg (MYSQL* handle);
    };

}