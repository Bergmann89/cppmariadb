#pragma once

#include <cppmariadb/transaction.h>

namespace cppmariadb
{

    /* transaction *******************************************************************************/

    inline void transaction::begin()
    {
        static const statement sCommit("START TRANSACTION");
        _connection.execute(sCommit);
    }

    inline void transaction::commit()
    {
        static const statement sCommit("COMMIT");
        if (_closed)
            throw exception("transaction is already closed", error_code::Unknown);
        _connection.execute(sCommit);
        _closed = true;
    }

    inline void transaction::rollback()
    {
        static const statement sRollback("ROLLBACK");
        if (_closed)
            throw exception("transaction is already closed", error_code::Unknown);
        _connection.execute(sRollback);
        _closed = true;
    }

    inline transaction::transaction(connection& connection) :
        _connection (connection),
        _closed     (false)
        { begin(); }

    inline transaction::~transaction()
    {
        if (!_closed)
            rollback();
    }


}