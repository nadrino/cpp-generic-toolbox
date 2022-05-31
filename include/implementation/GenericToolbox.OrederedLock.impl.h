//
// Created by Adrien BLANCHET on 01/02/2022.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H


namespace GenericToolbox{

  OrderedLock::OrderedLock() = default;
  void OrderedLock::lock() {
    std::unique_lock<std::mutex> acquire(_lock_);
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
    std::unique_lock<std::mutex> acquire(_lock_);
    if (_conditionVariable_.empty()) {
      _isLocked_ = false;
    }
    else {
      _conditionVariable_.front()->notify_one();
      _conditionVariable_.pop();
    }
  }

  bool OrderedLock::isLocked() const {
    return _isLocked_;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_OREDEREDLOCK_IMPL_H
