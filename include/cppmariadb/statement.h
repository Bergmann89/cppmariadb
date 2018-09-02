#pragma once

#include <string>
#include <vector>
#include <cppmariadb/config.h>
#include <cppmariadb/forward/connection.h>
#include <cppmariadb/forward/statement.h>

namespace cppmariadb
{

    struct statement
    {
    public:
        static constexpr size_t npos = std::numeric_limits<size_t>::max();

    private:
        struct parameter
        {
            bool        has_value { false };
            bool        unescaped { false };
            std::string value;
        };

    private:
        mutable bool                _changed;
        mutable std::string         _query;
        mutable const connection*   _connection;

        std::vector<std::string>                        _code;
        std::vector<std::pair<std::string, parameter>>  _parameters;

        void parse(const std::string& query);
        void build(const connection& con) const;

    public:
        inline void                 assign  (const std::string& query);
        inline const std::string&   query   (const connection& con) const;
        inline size_t               find    (const std::string& param);
        inline void                 set_null(const std::string& param);
        inline void                 set_null(size_t index);
        inline bool                 empty   () const;
        inline void                 clear   ();

        template<class T>
        inline void set(const std::string& param, const T& value);

        template<class T>
        inline void set(size_t index, const T& value);

        inline statement();
        inline statement(const std::string& query);
    };

}