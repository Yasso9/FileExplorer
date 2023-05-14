#pragma once

#include <type_traits>

template< typename T >
requires ( std::is_class_v< T > )
class Singleton
{
  public:
    Singleton( Singleton const & )       = delete;
    void operator= ( Singleton const & ) = delete;

    static T & get_instance ();

  protected:
    Singleton()          = default;
    virtual ~Singleton() = default;
};

// Use this macro inside the inherited singleton class
#define ENABLE_SINGLETON( Type ) friend Type & Singleton< Type >::get_instance()

#include "singleton_impl.hpp"
