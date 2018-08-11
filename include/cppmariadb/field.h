#pragma once

#include <cppmariadb/config.h>
#include <cppmariadb/forward/column.h>
#include <cppmariadb/forward/field.h>
#include <cppmariadb/forward/row.h>

namespace cppmariadb
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