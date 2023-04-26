#pragma once

#include <ostream>  // for ostream
#include <vector>   // for vector

#include <imgui/imgui.h>  // for ImVec2

template< typename T >
class Vector2D
{
  protected:
    // One dimensionnal array representing a two dimensionnal array
    std::vector< T > m_array;
    // x : index of columns, y : index of lines
    ImVec2           m_size;

  public:
    Vector2D();
    Vector2D( std::vector< T > array, ImVec2 size );
    Vector2D( std::vector< std::vector< T > > array );
    virtual ~Vector2D() = default;

    ImVec2                   get_size () const;
    std::vector< T > const & get_data () const;

    std::vector< T > operator[] ( unsigned int line ) const;
    std::vector< T > get_row ( unsigned int line ) const;
    T const & get_element ( unsigned int column, unsigned int line ) const;
    T &       get_element ( unsigned int column, unsigned int line );

    ImVec2       get_position ( unsigned int index ) const;
    unsigned int get_index ( unsigned int column, unsigned int line ) const;
    unsigned int get_length () const;

    void set_size ( ImVec2 size, T defaultValue = T {} );

    template< typename T2 >
    friend std::ostream & operator<< ( std::ostream &         stream,
                                       Vector2D< T2 > const & array );
    template< typename T2 >
    friend std::istream & operator>> ( std::istream &   stream,
                                       Vector2D< T2 > & array );
};

#include "vector2D.tpp"
