#pragma once

#include <string>  // for allocator, string

#ifndef NDEBUG
#    define ASSERTION( expression, message )                                   \
        too::assertion( expression, __FILE__, __LINE__, __func__, #expression, \
                        message )
#else
#    define ASSERTION( expression, message )
#endif

namespace too
{
    void assertion ( bool const & expression, std::string const & fileName,
                     int const & line, std::string const & functionName,
                     std::string const & expressionString,
                     std::string const & message = "" );
}  // namespace too