#pragma once

#include <cppmariadb/config.h>
#include <cppmariadb/enums.h>
#include <cppmariadb/forward/connection.h>
#include <cppmariadb/forward/statement.h>
#include <cppmariadb/forward/transaction.h>

namespace cppmariadb
{

    struct transaction
    {
    private:
        connection&     _connection;
        bool            _closed;

        inline void begin();

    public:
        inline void commit();
        inline void rollback();

        inline transaction(connection& connection);
        inline ~transaction();
    };

}