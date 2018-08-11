#pragma once

#include <cppmariadb/statement.h>
#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>

namespace cppmariadb
{

    /* statement *********************************************************************************/
        
    inline void statement::assign(const std::string& query)
    {
        _changed = true;
        parse(query);
    }

    inline const std::string& statement::query(const connection& con) const
    {
        if (_changed || &con != _connection)
            build(con);
        return _query;
    }

    inline size_t statement::find(const std::string& param)
    {
        for (size_t i = 0; i < _parameters.size(); ++i)
        {
            if (_parameters.at(i).first == param)
                return i;
        }
        return npos;
    }

    inline void statement::set_null(const std::string& param)
    {
        auto i = find(param);
        if (i == npos)
            throw exception(std::string("unknown parameter name in query: ") + param, error_code::Unknown);
        set_null(i);
    }

    inline void statement::set_null(size_t index)
    {
        if (index >= _parameters.size())
            throw exception(std::string("unknown parameter index in query: ") + std::to_string(index), error_code::Unknown);
        auto& param = _parameters.at(index).second;
        param.has_value = false;
        _changed = true;
    }

    inline void statement::clear()
    {
        for (auto& param : _parameters)
            param.second.has_value = false;
    }

    template<class T>
    inline void statement::set(const std::string& param, const T& value)
    {
        auto i = find(param);
        if (i == npos)
            throw exception(std::string("unknown parameter name in query: ") + param, error_code::Unknown);
        set<T>(i, value);
    }

    template<class T>
    inline void statement::set(size_t index, const T& value)
    {
        if (index >= _parameters.size())
            throw exception(std::string("unknown parameter index in query: ") + std::to_string(index), error_code::Unknown);
        auto& param = _parameters.at(index).second;
        param.has_value = true;
        param.value = utl::to_string(value);
        _changed = true;
    }

    inline statement::statement()
        : _changed      (true)
        , _connection   (nullptr)
        { }

    inline statement::statement(const std::string& query)
        : _changed      (true)
        , _connection   (nullptr)
        { parse(query); }

        
}