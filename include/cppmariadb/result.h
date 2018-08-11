#pragma once

#include <memory>
#include <cppmariadb/config.h>
#include <cppmariadb/impl/mariadb_handle.h>
#include <cppmariadb/forward/column.h>
#include <cppmariadb/forward/row.h>

namespace cppmariadb
{

    struct result :
        public __impl::mariadb_handle<MYSQL_RES*>
    {
    private:
        std::unique_ptr<row>    _row;
        mutable column_vector   _columns;
        unsigned long long      _rowindex;

        void update_columns() const;

    protected:
        inline void rowindex(unsigned long long value);

    public:
        inline unsigned int         columncount () const;
        inline const column_vector& columns     () const;
               row*                 next        ();
        inline row*                 current     () const;
        inline unsigned long long   rowindex    () const;
        inline void                 free        ();

        inline result(MYSQL_RES* h);
        virtual ~result();
    };

    struct result_stored
        : public result
    {
        inline MYSQL_ROW_OFFSET     rowoffset   () const;
        inline void                 rowoffset   (MYSQL_ROW_OFFSET offset);
        inline void                 rowindex    (unsigned long long index);
        inline unsigned long long   rowindex    () const;
        inline unsigned long long   rowcount    () const;

        inline result_stored(MYSQL_RES* h);
    };

    struct result_used
        : public result
    {
        inline result_used(MYSQL_RES* h);
        virtual ~result_used() override;
    };

}