#pragma once

#include <string>

namespace Trace
{
    void Error ( std::string const & message = "" );
    void Warning ( std::string const & message = "" );
    void Info ( std::string const & message = "" );
    void Debug ( std::string const & message = "" );

    void FileNotFound ( std::string const & filePath,
                        std::string const & message = "" );
    void FileIssue ( std::string const & filePath,
                     std::string const & message = "" );
}  // namespace Trace