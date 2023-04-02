#include "traces.hpp"

#include <iostream>

namespace Trace
{
    void Error ( std::string const & message )
    {
        std::cout << "ERROR          : " << message << std::endl;
    }

    void Warning ( std::string const & message )
    {
        std::cout << "WARNING        : " << message << std::endl;
    }

    void Info ( std::string const & message )
    {
        std::cout << "INFO           : " << message << std::endl;
    }

    void Debug ( std::string const & message )
    {
        std::cout << "DEBUG          : " << message << std::endl;
    }

    void FileNotFound ( std::string const & filePath,
                        std::string const & message )
    {
        std::cout << "FILE NOT FOUND : '" << filePath << "' - " << message
                  << std::endl;
    }

    void FileIssue ( std::string const & filePath, std::string const & message )
    {
        std::cout << "FILE ISSUE     : '" << filePath << "' - " << message
                  << std::endl;
    }
}  // namespace Trace