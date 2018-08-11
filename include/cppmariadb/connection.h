#pragma once

#include <memory>
#include <cppmariadb/config.h>
#include <cppmariadb/impl/mariadb_handle.h>
#include <cppmariadb/forward/connection.h>
#include <cppmariadb/forward/result.h>
#include <cppmariadb/forward/statement.h>

namespace cppmariadb
{

    struct connection
        : public __impl::mariadb_handle<MYSQL*>
    {
    private:
        std::unique_ptr<result>     _result;

        template<class T>
        typename T::result_type*    execute_internal(const std::string& cmd);

    public:
        inline void                 execute         (const std::string& cmd);
        inline unsigned long long   execute_id      (const std::string& cmd);
        inline unsigned long long   execute_rows    (const std::string& cmd);
        inline result_stored*       execute_stored  (const std::string& cmd);
        inline result_used*         execute_used    (const std::string& cmd);

        inline void                 execute         (const statement& s);
        inline unsigned long long   execute_id      (const statement& s);
        inline unsigned long long   execute_rows    (const statement& s);
        inline result_stored*       execute_stored  (const statement& s);
        inline result_used*         execute_used    (const statement& s);

        inline result*              result          () const;
        inline uint                 fieldcount      () const;
        inline std::string          escape          (const std::string& value) const;
        inline void                 close           ();

        inline connection& operator =(connection&& other);

        inline connection();
        inline connection(MYSQL* h);
        inline connection(connection&& other);
        inline ~connection();
    };

}