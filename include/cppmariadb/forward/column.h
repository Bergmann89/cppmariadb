#pragma once

#include <vector>
#include <cppmariadb/config.h>

namespace cppmariadb
{

    struct column;

    using column_vector = std::vector<column>;

}