//
// Created by Nadrino on 03/12/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H

#include "atomic"
#include "memory"

namespace GenericToolbox{

  template <class T> class NoCopyWrapper: public T{
  public:
    NoCopyWrapper() = default;
    NoCopyWrapper(const NoCopyWrapper&){}
    NoCopyWrapper& operator=(const NoCopyWrapper&){ return *this; }
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

  template<class T> class PolymorphicObjectWrapper{

  public:
    PolymorphicObjectWrapper() = default;

    // Handling copy
    PolymorphicObjectWrapper(const PolymorphicObjectWrapper& src_): dialPtr{src_.dialPtr->clone()} {  }
    PolymorphicObjectWrapper& operator=(const PolymorphicObjectWrapper& other) { if (this != &other) { dialPtr = other.dialPtr->clone(); } return *this; }
    PolymorphicObjectWrapper(PolymorphicObjectWrapper&&)  noexcept = default;
    PolymorphicObjectWrapper& operator=(PolymorphicObjectWrapper&&)  noexcept = default;

    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(const DerivedT& src_): dialPtr{src_.clone()} {  }

    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(std::unique_ptr<DerivedT> def): dialPtr{std::move(def)} {};

    T& operator*() const { return *dialPtr; }
    T* operator->() const { return dialPtr.get(); }
    [[nodiscard]] T* get() const{ return dialPtr.get(); }

    std::unique_ptr<T> dialPtr;

  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
