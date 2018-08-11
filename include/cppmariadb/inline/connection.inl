#pragma once

#include <cppmariadb/result.h>
#include <cppmariadb/connection.h>

#include <cppmariadb/inline/result.inl>
#include <cppmariadb/inline/database.inl>
#include <cppmariadb/inline/statement.inl>

namespace cppmariadb
{

    /* op_store_result ***************************************************************************/
        
    struct op_store_result
    {
        using result_type = result_stored;

        inline MYSQL_RES* operator()(MYSQL* handle) const
            { return mysql_store_result(handle); }
    };

    /* op_use_result *****************************************************************************/

    struct op_use_result
    {
        using result_type = result_used;

        inline MYSQL_RES* operator()(MYSQL* handle) const
            { return mysql_use_result(handle); }
    };

    /* connection ********************************************************************************/

    template<class T>
    typename T::result_type* connection::execute_internal(const std::string& cmd)
    {
#ifdef MARIADB_DEBUG
        log_global_message(debug) << "execute cppmariadb query: " << std::endl << cmd;
#endif
        if (!handle())
            throw exception("invalid handle", error_code::Unknown, cmd);
        using result_type = typename T::result_type;
        _result.reset();
        if (mysql_real_query(*this, cmd.data(), cmd.size()) != 0)
            throw exception(database::error_msg(*this), database::error_code(*this), cmd);
        auto ret = T()(*this);
        if (!ret)
        {
            if (mysql_field_count(*this) > 0)
                throw exception(database::error_msg(*this), database::error_code(*this), cmd);
            return nullptr;
        }
        _result.reset(new result_type(ret));
        return static_cast<result_type*>(_result.get());
    }

    inline void connection::execute(const std::string& cmd)
        { execute_internal<op_store_result>(cmd); }

    inline unsigned long long connection::execute_id(const std::string& cmd)
    {
        execute_internal<op_store_result>(cmd);
        auto id = mysql_insert_id(*this);
        if (id == static_cast<unsigned long long>(-1))
            throw exception(database::error_msg(*this), database::error_code(*this), cmd);
        return id;
    }

    inline unsigned long long connection::execute_rows(const std::string& cmd)
    {
        execute_internal<op_store_result>(cmd);
        auto rows = mysql_affected_rows(*this);
        if (rows == static_cast<unsigned long long>(-1))
            throw exception(database::error_msg(*this), database::error_code(*this), cmd);
        return rows;
    }

    inline result_stored* connection::execute_stored(const std::string& cmd)
        { return execute_internal<op_store_result>(cmd); }

    inline result_used* connection::execute_used(const std::string& cmd)
        { return execute_internal<op_use_result>(cmd); }

    inline void connection::execute(const statement& s)
        { return execute(s.query(*this)); }

    inline unsigned long long connection::execute_id(const statement& s)
        { return execute_id(s.query(*this)); }

    inline unsigned long long connection::execute_rows(const statement& s)
        { return execute_rows(s.query(*this)); }

    inline result_stored* connection::execute_stored(const statement& s)
        { return execute_stored(s.query(*this)); }

    inline result_used* connection::execute_used(const statement& s)
        { return execute_used(s.query(*this)); }

    inline result* connection::result() const
        { return _result.get(); }

    inline uint connection::fieldcount() const
        { return mysql_field_count(handle()); }

    inline std::string connection::escape(const std::string& value) const
    {
        if (handle())
        {
            std::string ret;
            ret.resize(2 * value.size() + 1);
            auto len = mysql_real_escape_string(handle(), const_cast<char*>(ret.data()), value.c_str(), value.size());
            ret.resize(len);
            return ret;
        }
        return value;
    }

    inline void connection::close()
    {
        _result.reset();
        auto h = handle();
        handle(nullptr);
        if (h)
            mysql_close(h);
    }

    inline connection& connection::operator =(connection&& other)
    {
        close();
        handle(other.handle());
        other.handle(nullptr);
        return *this;
    }

    inline connection::connection()
        : connection(nullptr)
        { }

    inline connection::connection(MYSQL* h)
        : mariadb_handle(h)
        { }

    inline connection::connection(connection&& other)
        : mariadb_handle(std::move(other))
        , _result       (std::move(other)._result)
        { }

    inline connection::~connection()
        { close(); }

}