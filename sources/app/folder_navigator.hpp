#pragma once

#include <vector>  // for vector

#include <boost/multi_array.hpp>  // for multi_array

#include "app/explorer_settings.hpp"  // for ExplorerSettings
#include "app/filesystem.hpp"         // for fs::path

class FolderNavigator
{
    // Pointer because we need move assignment operator
    ExplorerSettings const * m_settings;

    fs::path m_currentDirectory;
    fs::path m_searchBox;

    std::vector< fs::path > m_previousDirectories;
    std::vector< fs::path > m_nextDirectories;

    boost::multi_array< std::string, 2 > m_structure;
    unsigned int                         m_nbColumns;

  public:
    FolderNavigator( fs::path const &         baseDirectory,
                     ExplorerSettings const & settings );
    virtual ~FolderNavigator()                              = default;
    FolderNavigator( FolderNavigator const & )              = default;
    FolderNavigator( FolderNavigator && )                   = default;
    FolderNavigator & operator= ( FolderNavigator const & ) = default;
    FolderNavigator & operator= ( FolderNavigator && )      = default;

    fs::path const &                get_directory () const;
    fs::path const &                get_search_box () const;
    // fs::path &                      get_search_box ();
    std::vector< fs::path > const & get_previous_directories () const;
    std::vector< fs::path > const & get_next_directories () const;
    boost::multi_array< std::string, 2 > const & get_structure () const;

    void change_directory ( fs::path const & path );
    void change_to_previous_dir ();
    void change_to_next_dir ();
    void to_parent_dir ();
    void set_search_box ( fs::path const & path );

    void refresh ();
    void gui_info ();

  private:
    void add_to_previous_dir ( fs::path const & path );
    void add_to_next_dir ( fs::path const & path );
    void set_current_dir ( fs::path const & path );
};
