//
// Created by Nadrino on 03/12/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H

#include "atomic"

namespace GenericToolbox{

  template <typename T>
  struct AtomicWrapper
  {
    std::atomic<T> atomicValue;

    AtomicWrapper() : atomicValue() {}
    explicit AtomicWrapper(const std::atomic<T> &a) : atomicValue(a.load()){}
    AtomicWrapper(const AtomicWrapper &other): atomicValue(other.atomicValue.load()){}
    AtomicWrapper &operator=(const AtomicWrapper &other){
      atomicValue.store(other.atomicValue.load());
      return *this;
    }

  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
