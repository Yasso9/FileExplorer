#include "assertion.hpp"

#include <iostream>  // for operator<<, basic_ostream, basic_ostream::operat...
#include <stdlib.h>  // for abort

#include <fmt/core.h>

#include "tools/traces.hpp"

namespace too
{
    void assertion ( bool const & expression, std::string const & fileName,
                     int const & line, std::string const & functionName,
                     std::string const & expressionString,
                     std::string const & message )
    {
        if ( ! expression )
        {
            Trace::Error( fmt::format(
                "Assertion failed : {}\n"
                "Expected : ({}) == true\n"
                "Source : {}:{}\n"
                "Function Call : {}\n",
                message, expressionString, fileName, line, functionName ) );
        }
    }
}  // namespace too
