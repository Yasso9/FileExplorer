#pragma once

#include "array2D.hpp"

#include <cmath>  // for floor

#include "tools/assertion.hpp"  // for ASSERTION
#include "tools/stream.hpp"     // for ignore_next
#include "tools/traces.hpp"     // for Trace
#include "tools/vector.hpp"     // for vector::append, vector::extract

namespace
{
    template< typename T >
    bool check_compat ( std::vector< T > array, ImVec2 size )
    {
        return size.x * size.y == array.size();
    }
}  // namespace

template< typename T >
Array2D< T >::Array2D() : m_array {}, m_size { 0u, 0u }
{}

template< typename T >
Array2D< T >::Array2D( std::vector< T > array, ImVec2 size )
  : m_array { array }, m_size { size }
{
    ASSERTION( check_compat( m_array, m_size ),
               "Size and Array not comptatible" );
}

template< typename T >
Array2D< T >::Array2D( std::vector< std::vector< T > > array ) : Array2D {}
{
    if ( array.empty() )
    {
        return;
    }

    ASSERTION( vector::is_rectangle( array ),
               "Each columns must have the same size" );

    m_size = ImVec2 { array[0].size(), array.size() };
    for ( std::vector< T > const & subvector : array )
    {
        m_array.insert( m_array.end(), subvector.begin(), subvector.end() );
    }
}

template< typename T >
ImVec2 Array2D< T >::get_size() const
{
    return m_size;
}

template< typename T >
std::vector< T > const & Array2D< T >::get_data() const
{
    return m_array;
}

template< typename T >
std::vector< T > Array2D< T >::operator[] ( unsigned int line ) const
{
    ASSERTION( line < m_size.y, "Line out of range" );
    return vector::extract( m_array, line * m_size.x, line * ( m_size.x + 1 ) );
}

template< typename T >
std::vector< T > Array2D< T >::get_row( unsigned int line ) const
{
    return this->operator[] ( line );
}

template< typename T >
T const & Array2D< T >::get_element( unsigned int column,
                                     unsigned int line ) const
{
    return const_cast< Array2D< T > * >( this )->get_element( column, line );
}

template< typename T >
T & Array2D< T >::get_element( unsigned int column, unsigned int line )
{
    ASSERTION( column < m_size.x && line < m_size.y, "Element out of range" );
    return m_array[this->get_index( column, line )];
}

template< typename T >
ImVec2 Array2D< T >::get_position( unsigned int index ) const
{
    return ImVec2 {
        index % static_cast< unsigned int >( m_size.x ),
        std::floor( index / static_cast< unsigned int >( m_size.x ) ) };
}

template< typename T >
unsigned int Array2D< T >::get_index( unsigned int column,
                                      unsigned int line ) const
{
    return column + line * m_size.x;
}

template< typename T >
unsigned int Array2D< T >::get_length() const
{
    ASSERTION( check_compat( m_array, m_size ),
               "Size and Array not comptatible" );
    return m_array.length();
}

template< typename T >
void Array2D< T >::set_size( ImVec2 size, T defaultValue )
{
    // Changment in lines, we add element in columns
    if ( m_size.y < size.y )
    {
        // Differences between the 2 sizes
        unsigned int diff = size.y - m_size.y;

        // We add lines at the end of the array so we can just append a vector
        // of the desized size
        std::vector< T > newVector {};
        newVector.assign( diff * m_size.x, defaultValue );

        // vector::append( m_array, newVector );

        // Update the size
        m_size.y = size.y;
    }
    else if ( m_size.y > size.y )
    {
        m_size.y = size.y;

        // We remove lines at the end of the array so we erase element to the
        // specified index
        m_array.erase( m_array.begin() + this->get_length() + 1,
                       m_array.end() );
    }

    // Changment in columns, we add element in lines
    if ( m_size.x < size.x )
    {
        // Number of element to add in each lines
        unsigned int diff = size.x - m_size.x;

        for ( unsigned int index = m_size.x; index <= size.x * m_size.y;
              index += size.x )
        {
            m_array.insert( m_array.begin() + index, diff, defaultValue );
        }
    }
    else if ( m_size.x > size.x )
    {
        int diff = static_cast< int >( m_size.x - size.x );

        for ( int index = static_cast< int >( this->get_length() ) + 1;
              index >= diff; index -= static_cast< int >( m_size.x ) )
        {
            m_array.erase( m_array.begin() + index - diff,
                           m_array.begin() + index );
        }
    }

    m_size.x = size.x;

    if ( m_size.x * m_size.y != m_array.size() )
    {
        Trace::Error( "Size and Array not comptatible" );
    }
}

template< typename T >
std::ostream & operator<< ( std::ostream & stream, Array2D< T > const & array )
{
    return stream << "{ " << array.m_size << " | " << array.m_array << " }";
}

template< typename T >
std::istream & operator>> ( std::istream & stream, Array2D< T > & array )
{
    stream::ignore_next( stream, '{' );
    stream >> array.m_size;
    stream::ignore_next( stream, '|' );
    stream >> array.m_array;
    stream::ignore_next( stream, '}' );

    ASSERTION( check_compat( array.m_array, array.m_size ),
               "Size and Array not comptatible" );

    return stream;
}
