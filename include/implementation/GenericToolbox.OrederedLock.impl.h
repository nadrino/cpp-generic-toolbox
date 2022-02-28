//
// Created by Adrien BLANCHET on 01/02/2022.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H

#include "GenericToolbox.OrderedLock.h"

namespace GenericToolbox{

  OrderedLock::OrderedLock() : _mutexLockPtr_(std::make_shared<std::mutex>()) {}
  void OrderedLock::lock() {
    std::unique_lock<std::mutex> acquire(*_mutexLockPtr_);
    if (_isLocked_) {
      std::condition_variable signal{};
      _conditionVariable_.emplace(&signal);
      signal.wait(acquire);
    }
    else {
      _isLocked_ = true;
    }
  }
  void OrderedLock::unlock() {
    std::unique_lock<std::mutex> acquire(*_mutexLockPtr_);
    if (_conditionVariable_.empty()) {
      _isLocked_ = false;
    }
    else {
      _conditionVariable_.front()->notify_one();
      _conditionVariable_.pop();
    }
  }

  bool OrderedLock::isLocked() {
    return _isLocked_;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H
