//
// Created by Nadrino on 03/12/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H

#include "atomic"

namespace GenericToolbox{

  template <class T> class NoCopyWrapper: public T{
  public:
    NoCopyWrapper() = default;
    NoCopyWrapper(const NoCopyWrapper&){}
  };

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

  template<class T> class CopyableAtomic : public std::atomic<T> {
  public:
    CopyableAtomic() = default;
    constexpr explicit CopyableAtomic(T desired) :std::atomic<T>(desired) {}
    constexpr CopyableAtomic(const CopyableAtomic<T>& other) : CopyableAtomic(other.load(std::memory_order_relaxed)){}
    CopyableAtomic& operator=(const CopyableAtomic<T>& other) {
      this->store(other.load(std::memory_order_acquire), std::memory_order_release);
      return *this;
    }
    CopyableAtomic& operator=(const T& other) {
      this->store(other, std::memory_order_release);
      return *this;
    }
  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
