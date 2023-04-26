#include "stream.hpp"

#include <iostream>

namespace stream
{
    bool ignore_next ( std::istream & stream, char character )
    {
        // Ignore spaces
        while ( stream.peek() == ' ' )
        {
            // Advence of one character
            stream.seekg( 1 + stream.tellg() );
        }

        if ( stream.eof() )
        {
            std::cerr << "End of file could not read character '" << character
                      << "'" << std::endl;
            return false;
        }

        if ( stream.peek() != character )
        {
            std::cerr << "We should have the character : '" << character
                      << "' and not '" << static_cast< char >( stream.peek() )
                      << "'" << std::endl;

            return false;
        }

        // Advence of one character
        stream.seekg( 1 + stream.tellg() );

        return true;
    }

    char peek_next ( std::istream & stream )
    {
        auto baseIndex = stream.tellg();

        // Ignore spaces
        while ( stream.peek() == ' ' )
        {
            // Advence of one character
            stream.seekg( 1 + stream.tellg() );
        }

        if ( stream.eof() )
        {
            return '\0';
        }

        auto character = stream.peek();

        stream.seekg( baseIndex );

        return static_cast< char >( character );
    }
}  // namespace stream
