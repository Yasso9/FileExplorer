#pragma once

#include <istream>
#include <vector>

namespace stream
{
    // If the next character is character, it is ignored int the stream
    bool ignore_next ( std::istream & stream, char character );
    // Return the next character without ignoring it
    char peek_next ( std::istream & stream );
}  // namespace stream
