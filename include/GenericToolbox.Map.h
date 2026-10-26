//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_MAP_H
#define CPP_GENERIC_TOOLBOX_MAP_H

// ***************************
//! Map related tools
// ***************************

#include "GenericToolbox.String.h"

#include <string>
#include <map>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


// Declaration section
namespace GenericToolbox{

  template <typename K, typename T> static bool isIn( const K& key_, const std::map<K,T>& map_ );
  template <typename K, typename T> static T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ );

  template <typename K, typename T> [[deprecated("use isIn()")]] static bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ ){ return isIn(key_, map_);}

}


// Implementation section
namespace GenericToolbox {

  template <typename K, typename  T> static bool isIn( const K& key_, const std::map<K,T>& map_ ){
    return ( map_.find(key_) != map_.end() );
  }
  template <typename K, typename T> static T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ ){
    auto it = map_.find(key_);
    return it == map_.end() ? nullptr : &it->second;
  }

}


#endif // CPP_GENERIC_TOOLBOX_MAP_H
