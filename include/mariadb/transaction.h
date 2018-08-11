#pragma once

#include <mariadb/config.h>
#include <mariadb/enums.h>
#include <mariadb/forward/connection.h>
#include <mariadb/forward/statement.h>
#include <mariadb/forward/transaction.h>

namespace mariadb
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