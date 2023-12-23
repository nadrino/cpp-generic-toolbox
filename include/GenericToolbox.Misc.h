//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_MISC_H
#define CPP_GENERIC_TOOLBOX_MISC_H

#include <typeindex>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

// Declaration section
namespace GenericToolbox{

  //! Misc Tools
  static inline std::string getClassName(const std::string& PRETTY_FUNCTION_); // When calling this functions, provide __PRETTY_FUNCTION__ macro
  static inline std::string getMethodName(const std::string& PRETTY_FUNCTION_);

  template<class Derived, class Base> static inline bool isDerivedFrom(Base* objPtr_); // slow...
  template<class Derived, class Base> static inline bool isDerivedType(Base* objPtr_); // works when Derived is the final type

  template<typename T> inline auto getTypedArray(size_t nBytes_, char* byteArray_) -> std::vector<T>;

}

// Implementation section
namespace GenericToolbox{

  template<typename T> inline auto getTypedArray(size_t nBytes_, char* byteArray_) -> std::vector<T>{
    std::vector<T> out;
    if( nBytes_ % sizeof(T) != 0 ){
      throw std::runtime_error(std::to_string(nBytes_) + " not a multiple of typed data of size: " + std::to_string(sizeof(T)));
    }

    size_t size{nBytes_ / sizeof(T)};

    out.reserve( size );
    for( size_t iEntry = 0 ; iEntry < size ; iEntry++ ){
      out.emplace_back();
      std::memcpy( &out.back(), byteArray_, sizeof(T) );
      byteArray_ += sizeof(T);
    }

    return out;
  }


  template<class Derived, class ObjClass> static inline bool isDerivedFrom(ObjClass* objPtr_){
    return dynamic_cast< Derived* >( objPtr_ ) != nullptr;
  }
  template<class Derived, class Base> static inline bool isDerivedType(Base* objPtr_){
    return std::type_index(typeid(*objPtr_)) == std::type_index(typeid(Derived));
  }

  static inline std::string getClassName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    if (colons == std::string::npos)
      return "::";
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = colons - begin;

    return PRETTY_FUNCTION_.substr(begin, end);
  }
  static inline std::string getMethodName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = PRETTY_FUNCTION_.rfind('(') - begin;

    return PRETTY_FUNCTION_.substr(begin, end) + "()";
  }

}

//! MACROS Tools
#define __CLASS_NAME__ GenericToolbox::getClassName(__PRETTY_FUNCTION__)
//#define __CLASS_NAME__ ( this != nullptr ? typeid(*this).name() )
#define __METHOD_NAME__ GenericToolbox::getMethodName(__PRETTY_FUNCTION__)


#endif // CPP_GENERIC_TOOLBOX_MISC_H
