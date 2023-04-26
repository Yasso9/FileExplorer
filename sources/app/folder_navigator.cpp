#include "folder_navigator.hpp"

#include <imgui/imgui.h>  // for ImGui::Text, ImGui::Begin, ImGui::End

FolderNavigator::FolderNavigator( fs::path const &         baseDirectory,
                                  ExplorerSettings const & settings )
  : m_settings { settings },
    m_currentDirectory { baseDirectory },
    m_searchBox { m_currentDirectory },
    m_previousDirectories {},
    m_nextDirectories {},
    m_structure {},
    m_nbColumns { 4 }
{
    this->refresh();
}

fs::path const & FolderNavigator::get_directory() const
{
    return m_currentDirectory;
}

fs::path const & FolderNavigator::get_search_box() const
{
    return m_searchBox;
}

// fs::path & FolderNavigator::get_search_box()
// {
//     return m_searchBox;
// }

std::vector< fs::path > const & FolderNavigator::get_previous_directories()
    const
{
    return m_previousDirectories;
}

std::vector< fs::path > const & FolderNavigator::get_next_directories() const
{
    return m_nextDirectories;
}

boost::multi_array< std::string, 2 > const & FolderNavigator::get_structure()
    const
{
    return m_structure;
}

void FolderNavigator::change_directory( fs::path const & path )
{
    m_previousDirectories.push_back( m_currentDirectory );
    m_nextDirectories.clear();
    this->set_current_dir( path );
}

void FolderNavigator::change_to_previous_dir()
{
    if ( ! m_previousDirectories.empty() )
    {
        this->add_to_next_dir( m_currentDirectory );
        this->set_current_dir( m_previousDirectories.back() );
        m_previousDirectories.pop_back();
    }
}

void FolderNavigator::change_to_next_dir()
{
    if ( ! m_nextDirectories.empty() )
    {
        this->add_to_previous_dir( m_currentDirectory );
        this->set_current_dir( m_nextDirectories.back() );
        m_nextDirectories.pop_back();
    }
}

void FolderNavigator::to_parent_dir()
{
    if ( m_currentDirectory.has_parent_path() )
    {
        this->change_directory( m_currentDirectory.parent_path() );
    }
}

void FolderNavigator::set_search_box( fs::path const & path )
{
    m_searchBox = path;
}

void FolderNavigator::refresh()
{
    // Get all entries to show in the current directory
    std::vector< fs::directory_entry > entries {};
    for ( auto const & entry : fs::directory_iterator( this->get_directory() ) )
    {
        if ( ! ds::is_showed_gui( entry )
             || ( ! m_settings.showHidden && ds::is_hidden( entry ) ) )
        {
            continue;
        }
        entries.push_back( entry );
    }

    m_structure.resize( boost::extents[entries.size()][m_nbColumns] );
    for ( std::size_t row = 0; row < entries.size(); ++row )
    {
        for ( std::size_t column = 0; column < m_nbColumns; column++ )
        {
            switch ( column )
            {
            case 0 :
                // Used internally to store the path
                m_structure[row][column] = entries[row].path().string();
                break;
            case 1 :
                m_structure[row][column] =
                    entries[row].path().filename().string();
                break;
            case 2 :
                m_structure[row][column] =
                    ds::get_size_pretty_print( entries[row] );
                break;
            case 3 :
                m_structure[row][column] = ds::get_type( entries[row] );
                break;
            default :
                m_structure[row][column] = "N/A";
                break;
            }
        }
    }
}

void FolderNavigator::gui_info()
{
    ImGui::Text( "Current directory: %s", m_currentDirectory.string().c_str() );
    ImGui::Text( "Search box: %s", m_searchBox.string().c_str() );

    if ( ImGui::Button( "Reset Previous/Next" ) )
    {
        m_previousDirectories.clear();
        m_nextDirectories.clear();
    }

    ImGui::Text( "Previous directories:" );
    for ( auto const & dir : m_previousDirectories )
    {
        ImGui::Text( "%s", dir.string().c_str() );
    }
    ImGui::Text( "Next directories:" );
    for ( auto const & dir : m_nextDirectories )
    {
        ImGui::Text( "%s", dir.string().c_str() );
    }
}

void FolderNavigator::add_to_previous_dir( fs::path const & path )
{
    m_previousDirectories.push_back( fs::path { path } );
    if ( m_previousDirectories.size() > m_settings.maxHistorySize )
    {
        m_previousDirectories.erase( m_previousDirectories.begin() );
    }
}

void FolderNavigator::add_to_next_dir( fs::path const & path )
{
    // todo check if it's necessary to copy the path ?
    m_nextDirectories.push_back( fs::path { path } );
    if ( m_nextDirectories.size() > m_settings.maxHistorySize )
    {
        m_nextDirectories.erase( m_nextDirectories.begin() );
    }
}

void FolderNavigator::set_current_dir( fs::path const & path )
{
    m_currentDirectory = path;
    m_searchBox        = m_currentDirectory;
    this->refresh();
}
