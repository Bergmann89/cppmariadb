#include <cppmariadb/row.h>
#include <cppmariadb/column.h>
#include <cppmariadb/exception.h>

#include <cppmariadb/inline/row.inl>
#include <cppmariadb/inline/field.inl>
#include <cppmariadb/inline/result.inl>

using namespace ::cppmariadb;

size_t row::find(const std::string& name) const
{
    auto& columns = _result.columns();
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (columns.at(i).name == name)
            return i;
    }
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (columns.at(i).original_name == name)
            return i;
    }
    return npos;
}

field row::at(size_t i) const
{
    if (i >= size())
        throw exception("row index out of range", error_code::UnknownError);
    if (!_lengths)
    {
        _lengths = mysql_fetch_lengths(_result.handle());
        if (!_lengths)
            throw exception("unble to fetch lenghts for row", error_code::UnknownError);
    }
    return field(*this, i, handle()[i], _lengths[i]);
}

field row::at(const std::string name) const
{
    auto i = find(name);
    if (i == npos)
        throw exception(std::string("unknown field name: ") + name, error_code::UnknownError);
    return at(i);
}
