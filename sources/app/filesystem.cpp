#include "filesystem.hpp"

#include <iostream>
#include <vector>

#include <fmt/core.h>
#include <imgui/imgui.h>

#include "tools/traces.hpp"

namespace ds
{
    fs::path get_home_directory ()
    {
        return fs::path( std::getenv( "HOME" ) );
    }

    bool is_showed_gui ( fs::directory_entry entry )
    {
        return entry.is_regular_file() || entry.is_directory()
               || entry.is_symlink();
    }

    bool is_hidden ( fs::directory_entry entry )
    {
        return entry.path().filename().string().find( '.' ) == 0;
    }

    std::string get_type ( fs::directory_entry entry )
    {
        std::string extension { entry.path().extension().string() };

        if ( extension == "txt" )
        {
            return "Text";
        }
        else if ( extension == "pdf" )
        {
            return "PDF";
        }
        else if ( extension == "jpg" || extension == "jpeg"
                  || extension == "png" )
        {
            return "Image";
        }
        else
        {
            return "Others";
        }
    }

    uintmax_t get_folder_size ( fs::path folder )
    {
        uintmax_t size = 0;

        for ( const auto & entry : fs::recursive_directory_iterator( folder ) )
        {
            if ( entry.is_regular_file() )
            {
                size += entry.file_size();
            }
        }

        return size;
    }

    unsigned int get_nb_files ( fs::path folder )
    {
        unsigned int nbFiles = 0;

        for ( const auto & entry : fs::recursive_directory_iterator( folder ) )
        {
            if ( entry.is_regular_file() )
            {
                ++nbFiles;
            }
        }

        return nbFiles;
    }

    uintmax_t get_size ( fs::directory_entry entry )
    {
        if ( entry.is_directory() )
        {
            Trace::Warning( "get_size() not implemented for directories" );
            return 0;
        }

        return entry.file_size();
    }

    std::string get_size_pretty_print ( fs::directory_entry entry )
    {
        if ( entry.is_directory() )
        {
            return fmt::format( "{} files", ds::get_nb_files( entry.path() ) );
        }

        static std::vector< std::string > units { "B",  "KB", "MB", "GB", "TB",
                                                  "PB", "EB", "ZB", "YB" };

        // return the size in Ko, Mo, Go, etc ...
        uintmax_t size = get_size( entry );

        unsigned int unitIndex = 0;
        while ( size >= 1024.0 && unitIndex < units.size() )
        {
            size /= 1024.0;
            ++unitIndex;
        }

        return fmt::format( "{} {}", size, units[unitIndex] );
    }

    std::string get_open_command ()
    {
        std::string command;
#if defined( _WIN32 )
        command = "start";
#elif defined( __APPLE__ )
        command = "open";
#elif defined( __linux__ )
        command = "xdg-open";
#else
        Trace::Error( "Unsupported operating system" );
#endif
        return command;
    }

    bool open ( fs::path const & file )
    {
        std::string command { get_open_command() + " \"" + file.string()
                              + "\"" };

        int result = std::system( command.c_str() );
        if ( result )
        {
            Trace::Error( "Error opening file with default program." );
            return false;
        }
        return true;
    }
}  // namespace ds
