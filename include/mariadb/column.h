#pragma once

#include <string>
#include <vector>
#include <mariadb/config.h>
#include <mariadb/enums.h>
#include <mariadb/forward/column.h>

namespace mariadb
{

    struct column
    {
        std::string     name;
        std::string     original_name;
        std::string     table;
        std::string     original_table;
        std::string     database;
        unsigned long   length;
        unsigned long   max_length;
        column_flags    flags;
        unsigned int    decimals;
        unsigned int    charset_number;
        column_type     type;

        inline column(MYSQL_FIELD& f)
            : name          (f.name,        f.name_length)
            , original_name (f.org_name,    f.org_name_length)
            , table         (f.table,       f.table_length)
            , original_table(f.org_table,   f.org_table_length)
            , database      (f.db,          f.db_length)
            , length        (f.length)
            , max_length    (f.max_length)
            , flags         (f.flags)
            , decimals      (f.decimals)
            , charset_number(f.charsetnr)
            , type          (static_cast<column_type>(f.type))
            { }
    };

}