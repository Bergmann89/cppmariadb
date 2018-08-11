#pragma once

#include <mariadb/database.h>

namespace mariadb
{

    /* database ***********************************************************************************/

    inline connection database::connect(
        const std::string&  host,
        const uint&         port,
        const std::string&  user,
        const std::string&  password,
        const std::string&  database,
        const client_flags& flags)
    {
        auto handle = mysql_init(nullptr);
        if (!handle)
            throw exception("unable to initialize connection handle", error_code::Unknown);

        if (!mysql_real_connect(
                handle,
                host.c_str(),
                user.c_str(),
                password.c_str(),
                database.empty() ? static_cast<const char*>(nullptr) : database.c_str(),
                port,
                nullptr,
                flags.value))
            throw exception(database::error_msg(handle), database::error_code(handle));

        return connection(handle);
    }

    inline error_code database::error_code(MYSQL* handle)
    {
        auto ret = mysql_errno(handle);
        return static_cast<enum error_code>(ret);
    }

    inline std::string database::error_msg(MYSQL* handle)
    {
        auto ret = mysql_error(handle);
        return (ret ? std::string(ret) : std::string());
    }
    
}