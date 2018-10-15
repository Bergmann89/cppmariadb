# cppmariadb

A C++11 class wrapper for the mysql/mariadb C connector.

## Getting Started

### Prerequisites

Before you can use cppmariadb you need to download and install [the official mariadb C connector](https://downloads.mariadb.org/connector-c/)). And if you want to run the tests you additionally need [the google testing framework](https://github.com/google/googletest).

[The cpputils libary](https://git.bergmann89.de/cpp/cpputils) will be automatically downladed during the build.

### Small Usage Example

```
#include <iostream>
#include <exception>
#include <cppmariadb.h>

using namespace ::cppmariadb;

int main(int argc, char** argv)
{
    try
    {
        /* establish connection to database */
        connection c = database::connect("localhost", 3306, "testuser", "password", "database", client_flags::empty());

        /* prepare a query */
        statement s("SELECT * FROM my_table WHERE id=?id?");
        s.set("id", 5);

        /* execute the query */
        result_used* res = con.execute_used(s);
        if (res == nullptr)
        {
            throw std::exception("unable to fetch from table from database!");
        }

        /* iterate over result set */
        row* r = res->next();
        while(r != nullptr)
        {
            std::cout << "new dataset:" << std::endl;
            for (size_t i = 0; i < r->size(); ++i)
            {
                field f = r->at(i);
                std::cout << "  " << f.column().name << ": " << f.get<std::string>() << std::endl;
            }
            std::cout << std::endl;
            r = res->next();
        }

        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 1
}
```

## License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details