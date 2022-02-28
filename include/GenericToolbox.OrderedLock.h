//
// Created by Adrien BLANCHET on 01/02/2022.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ORDEREDLOCK_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ORDEREDLOCK_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>


namespace GenericToolbox{

  class OrderedLock {

  public:
    inline OrderedLock();
    inline void lock();
    inline void unlock();
    inline bool isLocked();

  private:
    bool _isLocked_{false};
    std::shared_ptr<std::mutex> _mutexLockPtr_{};
    std::queue<std::condition_variable *> _conditionVariable_{};
  };

}

#include "implementation/GenericToolbox.OrederedLock.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ORDEREDLOCK_H
