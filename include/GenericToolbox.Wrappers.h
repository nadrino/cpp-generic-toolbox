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

  template<class T> class CopiableAtomic : public std::atomic<T> {
  public:
    CopiableAtomic() = default;
    constexpr explicit CopiableAtomic(T desired) : std::atomic<T>(desired) {}
    constexpr CopiableAtomic(const CopiableAtomic<T>& other) : CopiableAtomic(other.load(std::memory_order_relaxed)){}
    CopiableAtomic& operator=(const CopiableAtomic<T>& other) {
      this->store(other.load(std::memory_order_acquire), std::memory_order_release);
      return *this;
    }
    CopiableAtomic& operator=(const T& other) {
      this->store(other, std::memory_order_release);
      return *this;
    }
  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
