#include "assertion.hpp"

#include <iostream>  // for operator<<, basic_ostream, basic_ostream::operat...
#include <stdlib.h>  // for abort

namespace too
{
    void assertion ( bool const & expression, std::string const & fileName,
                     int const & line, std::string const & functionName,
                     std::string const & expressionString,
                     std::string const & message )
    {
        if ( ! expression )
        {
            std::cerr << "Assertion failed : " << message << "\n"
                      << "Expected : (" << expressionString << ") == true\n"
                      << "Source : " << fileName << ":" << line << "\n"
                      << "Function Call : " << functionName << "\n";

            abort();
        }
    }
}  // namespace too