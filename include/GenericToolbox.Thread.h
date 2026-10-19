//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_THREAD_H
#define CPP_GENERIC_TOOLBOX_THREAD_H

#include "GenericToolbox.Build.h"


#include "GenericToolbox.Wrappers.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Time.h"
#include "GenericToolbox.Macro.h"

#include <exception>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <future>
#include <mutex>
#include <queue>
#include <map>


// WorkerEntry
namespace GenericToolbox{
  struct WorkerEntry{
    int index{-1};
    std::shared_ptr<std::future<void>> thread{nullptr};
    std::exception_ptr exception{nullptr};

    // signal handler
    std::function<void(int)>* fctToRunPtr{nullptr};
    GenericToolbox::Atomic<bool> isEngaged{false};
    GenericToolbox::Atomic<bool> isRunning{false};
  };
}

// JobEntry
namespace GenericToolbox{
  struct JobEntry{
    std::string name{};
    std::function<void(int)> function{};
    std::function<void()> functionPreParallel{};
    std::function<void()> functionPostParallel{};

    JobEntry() = delete;
    explicit JobEntry(std::string  name_) : name(std::move(name_)) {}
  };
}

// ParallelWorker
namespace GenericToolbox{

  class ParallelWorker {

  public:
    template<typename T> struct ThreadBounds{ T beginIndex{}; T endIndex{}; };
    template<typename T> static ThreadBounds<T> getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_);

  public:
    inline ParallelWorker() = default;
    inline virtual ~ParallelWorker(){ if( not _workerList_.empty() ){ this->stopThreads(); } };

    inline void setIsVerbose(bool isVerbose_){ _isVerbose_ = isVerbose_; }
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void setNThreads(int nThreads_);
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_);

    // const getters
    [[nodiscard]] inline int getNbThreads() const{ return _nbThreads_; }
    [[nodiscard]] CPP_GENERIC_TOOLBOX_THREAD_INLINE int getJobIdx(const std::string& name_) const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_THREAD_INLINE const JobEntry* getJobPtr(const std::string& name_) const;
    [[nodiscard]] inline const GenericToolbox::Time::Timer& getLastJobTimer() const { return _lastJobTimer_; }

    // getters
    CPP_GENERIC_TOOLBOX_THREAD_INLINE JobEntry* getJobPtr(const std::string& name_);

    // core
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void addJob(const std::string& jobName_, const std::function<void(int)>& function_); // int arg is supposed to be the thread id
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void runJob(const std::function<void(int)>& function_);
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void runJob(const std::string& jobName_);
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void removeJob(const std::string& jobName_);

  protected:
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void startThreads();
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void stopThreads();

    CPP_GENERIC_TOOLBOX_THREAD_INLINE void threadWorker(int iThread_);

  private:
    // Parameters
    bool _isVerbose_{false};
    bool _cpuTimeSaverIsEnabled_{true}; // shutdown the threads when no job is running - put it to false for better performances
    int _nbThreads_{1};

    // Internals
    GenericToolbox::CopiableAtomic<bool> _stopThreads_{false};

    std::vector<WorkerEntry> _workerList_{};
    std::vector<JobEntry> _jobEntryList_{};

    // Monitoring
    Time::Timer _lastJobTimer_{};

  };

  // statics
  template<typename T> inline ParallelWorker::ThreadBounds<T> ParallelWorker::getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_){
    // first index, last index: for( int i = out.first ; i < out.second ; i++){}
    ParallelWorker::ThreadBounds<T> out{0, nTotal_};

    if( iThread_ == -1 or nThreads_ == 1 ){ return out; }

    int nEventsPerThread = nTotal_ / nThreads_;
    int nExtraEvents = nTotal_ % nThreads_;

    // make sure we get the right shift event if nTotal_ < nThreads_
    out.beginIndex =
        std::min(iThread_, nExtraEvents) * (nEventsPerThread + 1) +
        std::max(0, iThread_ - nExtraEvents) * nEventsPerThread;

    // adjust such the first threads are sharing the numExtraEvents
    out.endIndex =
        out.beginIndex +
        ((iThread_ < nExtraEvents) ? nEventsPerThread + 1 : nEventsPerThread );

    // OLD METHOD:
//    out.first = T(iThread_)*( nTotal_ / T(nThreads_) );
//    if( iThread_+1 != nThreads_ ){
//      out.second = (T(iThread_) + 1) * (nTotal_ / T(nThreads_));
//    }

    return out;
  }

  // setters


  // const getters


  // getters


}

// OrderedLock
namespace GenericToolbox{
  class OrderedLock {

  public:
    inline OrderedLock() = default;

    [[nodiscard]] inline bool isLocked() const { return _isLocked_; }

    CPP_GENERIC_TOOLBOX_THREAD_INLINE void lock();
    CPP_GENERIC_TOOLBOX_THREAD_INLINE void unlock();

  private:
    bool _isLocked_{false};
    NoCopyWrapper<std::mutex> _lock_{};
    std::queue<std::condition_variable *> _conditionVariable_{};
  };


}


#ifndef CPP_GENERIC_TOOLBOX_THREAD_COMPILED
#include "implementation/GenericToolbox.Thread.impl.h"
#endif

#endif //CPP_GENERIC_TOOLBOX_THREAD_H
