#pragma once

#include <mariadb/config.h>

namespace mariadb {
namespace __impl
{

    template<class T>
    struct mariadb_handle
    {
    private:
        T _handle;

    protected:
        inline void handle(T h)
            { _handle = h; }

    public:
        inline operator T() const
            { return _handle; }

        inline const T& handle() const
            { return _handle; }

        mariadb_handle(T h) :
            _handle(h)
            { };

        mariadb_handle(mariadb_handle&& other) :
            _handle(other._handle)
            { other._handle = nullptr; }

    private:
        mariadb_handle(const mariadb_handle&) = delete;
    };

} }