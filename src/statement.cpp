#include <cppmariadb/row.h>
#include <cppmariadb/enums.h>
#include <cppmariadb/column.h>
#include <cppmariadb/exception.h>

#include <cppmariadb/inline/statement.inl>
#include <cppmariadb/inline/connection.inl>

using namespace ::cppmariadb;

void statement::parse(const std::string& query)
{
    auto c = query.c_str();
    auto t = c;
    bool inParam = false;
    while (*c != '\0')
    {
        switch (*c)
        {
            case '?':
                if (inParam)
                    _parameters.emplace_back(std::string(t, static_cast<std::string::size_type>(c - t)), parameter { false, false, std::string() });
                else
                    _code.emplace_back(t, c - t);
                inParam = !inParam;
                t = c + 1;
                break;

            case '!':
                if (!inParam)
                    break;
                _parameters.emplace_back(std::string(t, static_cast<std::string::size_type>(c - t)), parameter { false, true, std::string() });
                inParam = false;
                t = c + 1;
                break;
        }
        ++c;
    }
    if (inParam)
        throw exception("unclosed parameter in statement", error_code::Unknown, query);
    if (c != t)
        _code.emplace_back(t, c - t);
}

void statement::build(const connection& con) const
{
    _connection = &con;
    std::ostringstream ss;
    size_t i = 0;
    if (std::abs(static_cast<ssize_t>(_code.size()) - static_cast<ssize_t>(_parameters.size())) > 1)
        throw exception("statement::build() - internal error: code and parameter size mismatch", error_code::Unknown);
    while (     (i >> 1) < _code.size()
            ||  (i >> 1) < _parameters.size())
    {
        size_t idx = (i >> 1);
        if ((i & 1) == 0)
        {
            if (idx >= _code.size())
                break;
            ss << _code.at(i >> 1);
        }
        else
        {
            if (idx >= _parameters.size())
                break;
            auto& param = _parameters.at(i >> 1).second;
            if (param.has_value)
            {
                if (param.unescaped)
                    ss << param.value;
                else
                    ss << "'" << con.escape(param.value) << "'";
            }
            else
            {
                if (!param.unescaped)
                    ss << "null";
            }
        }
        ++i;
    }
    _changed = false;
    _query   = ss.str();
}