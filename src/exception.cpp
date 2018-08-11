#include <cppmariadb/exception.h>
#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>

using namespace ::cppmariadb;

void exception::print_message(std::ostream& os) const
{
    os << message;
    if (    error != error_code::NoError
        || !query.empty())
    {
        os << " (";
        bool first = true;
        if (error != error_code::NoError)
        {
            os << "error_code=";
            utl::to_string(os, error);
            first = false;
        }
        if (!query.empty())
        {
            if (!first)
                os << "; ";
            os << "Query=" << query;
        }
        os << ")";
    }
}