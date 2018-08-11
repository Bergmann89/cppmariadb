#pragma once

#include <mariadb/config.h>
#include <mariadb/forward/column.h>
#include <mariadb/forward/field.h>
#include <mariadb/forward/row.h>

namespace mariadb
{

    struct field
    {
    private:
        const row&      _row;
        const size_t    _index;
        const char*     _data;
        size_t          _size;

    public:
        inline size_t           index   () const;
        inline const column&    column  () const;
        inline bool             is_null () const;
        inline bool             is_empty() const;
        inline const char*      data    () const;
        inline size_t           size    () const;
        inline operator         bool    () const;

        template <class T>
        inline T get() const;

        inline field(const row& r, size_t i, const char* d, size_t s);
    };

}