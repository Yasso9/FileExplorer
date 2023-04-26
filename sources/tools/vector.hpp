#pragma once

#include <vector>

namespace vector
{
    // Get the subvector between the two index of vector
    template< typename Type >
    std::vector< Type > extract ( std::vector< Type > vector,
                                  unsigned int begin, unsigned int end );

    // Check if a vector contain an element
    template< typename Type >
    bool contains ( std::vector< Type > const & vector,
                    Type const &                elementToSearch );

    // Add vectorB to vectorA
    template< typename Type >
    void append ( std::vector< Type > &       vectorA,
                  std::vector< Type > const & vectorB );

    // Check if the all the size of the subvectors are equal
    template< typename Type >
    bool is_rectangle (
        std::vector< std::vector< Type > > const & dimensionnalVector );
}  // namespace vector

#include "vector.tpp"
