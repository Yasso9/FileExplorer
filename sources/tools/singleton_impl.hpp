#pragma once

#include "singleton.hpp"

template< typename T >
T & Singleton< T >::get_instance()
{
    static T instance {};
    return instance;
}
