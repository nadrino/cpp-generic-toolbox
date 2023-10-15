//
// Created by Nadrino on 03/12/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H


#include <condition_variable>
#include <type_traits>
#include <atomic>
#include <memory>
#include <mutex>

namespace GenericToolbox{

  template<typename T> class NoCopyWrapper: public T {
  public:
    NoCopyWrapper() = default;
    NoCopyWrapper(const NoCopyWrapper&){}
    NoCopyWrapper& operator=(const NoCopyWrapper&){ return *this; }
  };

  template<typename T> class CopiableAtomic : public std::atomic<T> {
  public:
    CopiableAtomic() = default;
    constexpr explicit CopiableAtomic(T desired) : std::atomic<T>(desired) {}
    constexpr CopiableAtomic(const CopiableAtomic<T>& other) : CopiableAtomic(other.load(std::memory_order_relaxed)){}
    CopiableAtomic& operator=(const CopiableAtomic<T>& other);
    CopiableAtomic& operator=(const T& other);
  };

  template<typename T> class PolymorphicObjectWrapper{

  public:
    PolymorphicObjectWrapper() = default;

    // Handling copy
    PolymorphicObjectWrapper(const PolymorphicObjectWrapper& src_): dialPtr{src_.dialPtr->clone()} {  }
    PolymorphicObjectWrapper& operator=(const PolymorphicObjectWrapper& other);
    PolymorphicObjectWrapper(PolymorphicObjectWrapper&&)  noexcept = default;
    PolymorphicObjectWrapper& operator=(PolymorphicObjectWrapper&&)  noexcept = default;

#if HAS_CPP_14
    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(const DerivedT& src_): dialPtr{src_.clone()} {  }
#else
    template <typename DerivedT>
    explicit PolymorphicObjectWrapper(const DerivedT& src_): dialPtr{src_.clone()} {
      static_assert(std::is_base_of<T, DerivedT>::value, "Class doesn't inherit from Base!");
    }
#endif


#if HAS_CPP_14
    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(std::unique_ptr<DerivedT> def): dialPtr{std::move(def)} {};
#else
    template <typename DerivedT>
    explicit PolymorphicObjectWrapper(std::unique_ptr<DerivedT> def): dialPtr{std::move(def)} {
      static_assert(std::is_base_of<T, DerivedT>::value, "Class doesn't inherit from Base!");
    }
#endif

    T& operator*() const { return *dialPtr; }
    T* operator->() const { return dialPtr.get(); }
    [[nodiscard]] T* get() const{ return dialPtr.get(); }

    std::unique_ptr<T> dialPtr;

  };

  template<typename T> class Atomic{

  public:
    Atomic() = default;
    explicit Atomic(const T& val_): _variable_(val_) {}

    // setter
    void setValue(const T& val_);

    // getter
    const T& getValue() const;

    // core
    void waitUntilEqual(const T& val_) const;

    // Define operator++ (post-increment) using SFINAE
    // Using operator++(int) and not operator++(T) to stay consistent with C++ conventions */
    template<typename U = T>
    typename std::enable_if<std::is_integral<U>::value, T>::type operator++(int);

  private:
    T _variable_{};
    mutable GenericToolbox::NoCopyWrapper<std::mutex> _mutex_{};
    mutable GenericToolbox::NoCopyWrapper<std::condition_variable> _notifier_{};
  };

}

#include "implementation/GenericToolbox.Wrappers.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_H
