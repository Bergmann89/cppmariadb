#pragma once

#include <cppmariadb/result.h>

namespace cppmariadb
{

    /* result ************************************************************************************/

    inline void result::rowindex(unsigned long long value)
        { _rowindex = value; }

    inline unsigned int result::columncount() const
        { return mysql_num_fields(*this); }

    inline const column_vector& result::columns() const
    {
        if (_columns.empty())
            update_columns();
        return _columns;
    }

    inline row* result::current() const
        { return _row.get(); }

    inline unsigned long long result::rowindex() const
        { return _rowindex; }

    inline void result::free()
    {
        auto h = handle();
        handle(nullptr);
        mysql_free_result(h);
    }

    inline result::result(MYSQL_RES* h)
        : mariadb_handle    (h)
        , _is_initialized   (false)
        , _rowindex         (static_cast<unsigned long long>(-1))
        { }

    /* result_stored ******************************************************************************/

    inline MYSQL_ROW_OFFSET result_stored::rowoffset() const
        { return mysql_row_tell(*this); }

    inline void result_stored::rowoffset(MYSQL_ROW_OFFSET offset)
        { mysql_row_seek(*this, offset); }

    inline void result_stored::rowindex(unsigned long long index)
    {
        result::rowindex(index);
        mysql_data_seek(*this, result::rowindex());
    }

    inline unsigned long long result_stored::rowindex() const
        { return result::rowindex(); }

    inline unsigned long long result_stored::rowcount() const
        { return mysql_num_rows(*this); }

    inline result_stored::result_stored(MYSQL_RES* h)
        : result(h)
        { }

    /* result_used *******************************************************************************/

    inline result_used::result_used(MYSQL_RES* h)
        : result(h)
        { }

}