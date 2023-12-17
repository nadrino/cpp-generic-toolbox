//
// Created by Nadrino on 15/10/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_IMPL_H

#include "../GenericToolbox.Wrappers.h"

namespace GenericToolbox{

  template<typename T> CopiableAtomic<T>& CopiableAtomic<T>::operator=(const CopiableAtomic<T>& other) {
    this->store(other.load(std::memory_order_acquire), std::memory_order_release);
    return *this;
  }
  template<typename T> CopiableAtomic<T>& CopiableAtomic<T>::operator=(const T& other) {
    this->store(other, std::memory_order_release);
    return *this;
  }

  template<typename T> PolymorphicObjectWrapper<T>& PolymorphicObjectWrapper<T>::operator=(const PolymorphicObjectWrapper& other){
    if (this != &other) { dialPtr = other.dialPtr->clone(); } return *this;
  }


  template<typename T> void Atomic<T>::setValue(const T& val_){
    // meant to be executed in the main thread
    {
      // scope lock
      std::unique_lock<std::mutex> lock(_mutex_);
      _variable_ = val_;
    }
    // tell the parallel thread to check the "engageThread" variable
    _notifier_.notify_one();
  }
  template<typename T> const T& Atomic<T>::getValue() const {
    // scope lock
    std::unique_lock<std::mutex> lock(_mutex_);
    return _variable_;
  }
  template<typename T> void Atomic<T>::waitUntilEqual(const T& val_) const {
    std::unique_lock<std::mutex> lock(_mutex_);
    _notifier_.wait(lock, [&]{ return _variable_ == val_; });
  }

  template<typename T> template<typename U>
  typename std::enable_if<std::is_integral<U>::value, T>::type Atomic<T>::operator++(int) {
    std::unique_lock<std::mutex> lock(_mutex_);
    T out(_variable_++);
    return out;
  }

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_WRAPPERS_IMPL_H
