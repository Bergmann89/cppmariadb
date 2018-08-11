#pragma once

#include <cppmariadb/row.h>
#include <cppmariadb/field.h>
#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>

namespace cppmariadb
{

    /* op_field_converter ************************************************************************/

    template<class T, class Enable = void>
    struct op_field_converter;

    template<typename T>
    struct op_field_converter<T, void>
    {
        inline T operator()(const char* c, size_t s) const
        {
            T tmp;
            std::string data(c, s);
            if (!utl::try_from_string(data, tmp))
                throw exception(std::string("unable to convert field data (data=") + data + ")", error_code::UnknownError);
            return tmp;
        }
    };

    template<>
    struct op_field_converter<const char*, void>
    {
        inline const char* operator()(const char* c, size_t s) const
            { return c; }
    };

    template<>
    struct op_field_converter<std::string, void>
    {
        inline std::string operator()(const char* c, size_t s) const
            { return std::string(c, s); }
    };

    /* field *************************************************************************************/

    inline size_t field::index() const
        { return _index; }

    inline const column& field::column() const
        { return _row.columns().at(_index); }

    inline bool field::is_null() const
        { return (_data == nullptr); }

    inline bool field::is_empty() const
        { return (_size == 0); }

    inline const char* field::data() const
        { return _data; }

    inline size_t field::size() const
        { return _size; }

    template <class T>
    inline T field::get() const
    {
        if (is_null())
            throw exception("field is null", error_code::UnknownError);
        return op_field_converter<T>()(_data, _size);
    }

    inline field::operator bool() const
        { return !is_null() && !is_empty(); }

    inline field::field(const row& r, size_t i, const char* d, size_t s)
            : _row  (r)
            , _index(i)
            , _data (d)
            , _size (s)
            { }

}