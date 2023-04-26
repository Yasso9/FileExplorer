#include "display.hpp"

#include <algorithm>
#include <array>

#include "tools/traces.hpp"

namespace
{
    std::string gsettings ( std::string const & schema,
                            std::string const & key )
    {
        std::string             command = "gsettings get " + schema + " " + key;
        std::array< char, 128 > buffer;
        std::string             result;

        FILE * pipe = popen( command.c_str(), "r" );
        if ( ! pipe )
        {
            Trace::Error( "popen() failed!" );
        }
        while ( fgets( buffer.data(), buffer.size(), pipe ) != nullptr )
        {
            result += buffer.data();
        }
        pclose( pipe );

        // Remove newline and trailing whitespace
        result.erase( std::find_if( result.rbegin(), result.rend(),
                                    [] ( unsigned char ch ) {
                                        return ! std::isspace( ch );
                                    } )
                          .base(),
                      result.end() );

        // Run command and get output
        // std::string result = exec( command.c_str() );

        return result;
    }
}  // namespace

namespace display
{
    std::string get_font_name ()
    {
        return gsettings( "org.gnome.desktop.interface", "font-name" );
    }

    unsigned int get_font_size ()
    {
        std::size_t sizePos = get_font_name().find_last_of( ' ' );
        return std::stoul( get_font_name().substr( sizePos + 1 ) );
    }

    float get_text_scaling_factor ()
    {
        // Get the text scaling factor from GNOME settings
        std::string textScalingFactorStr =
            gsettings( "org.gnome.desktop.interface", "text-scaling-factor" );
        return std::stof( textScalingFactorStr );
    }

}  // namespace display
