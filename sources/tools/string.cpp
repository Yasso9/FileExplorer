#include "string.hpp"

#include <algorithm>  // for transform
#include <cctype>     // for tolower

namespace string
{
    std::string to_lowercase ( std::string const & str )
    {
        std::string lowercase {};
        lowercase.reserve( str.size() );
        std::transform( str.begin(), str.end(), std::back_inserter( lowercase ),
                        [] ( unsigned char c ) { return std::tolower( c ); } );
        return lowercase;
    }
}  // namespace string
