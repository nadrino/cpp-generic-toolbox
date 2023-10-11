//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_H

#include "GenericToolbox.h"
#include "GenericToolbox.Wrappers.h"

#include <map>
#include <mutex>
#include <future>
#include <utility>
#include <vector>
#include <string>
#include <memory>


// Classes : ParallelWorker
namespace GenericToolbox{

  ENUM_EXPANDER(
      WorkerState, 0,
      Unset,
      Set
  );

  ENUM_EXPANDER(
      ThreadStatus, 0,
      Stopped,
      Idle,
      Running
  );

  struct ThreadEntry{
    std::shared_ptr<std::future<void>> thread{nullptr};
    ThreadStatus threadStatus{ThreadStatus::Stopped};
  };

  struct JobEntry{
    std::string name{};
    std::function<void(int)> function{};
    std::function<void()> functionPreParallel{};
    std::function<void()> functionPostParallel{};
    std::vector<bool> jobRequestedThreadList{false};

    JobEntry() = delete;
    explicit JobEntry(std::string  name_) : name(std::move(name_)) {}
  };

  class ParallelWorker {

  public:
    inline ParallelWorker() = default;
    inline virtual ~ParallelWorker(){ if( _workerState_ == WorkerState::Set ){ this->stopThreads(); } };

    inline void setIsVerbose(bool isVerbose){ _isVerbose_ = isVerbose; }
    inline void setCheckHardwareCurrency(bool checkHardwareCurrency){ _checkHardwareCurrency_ = checkHardwareCurrency; }
    inline void setNThreads(int nThreads){ _nThreads_ = nThreads; }
    inline void setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled){ _cpuTimeSaverIsEnabled_ = cpuTimeSaverIsEnabled; }

    // const getters
    inline int getNThreads() const{ return _nThreads_; }
    inline int getJobIdx(const std::string& name_) const;
    inline const JobEntry* getJobPtr(const std::string& name_) const;

    // getters
    inline JobEntry* getJobPtr(const std::string& name_);

    // core
    inline void addJob(const std::string& jobName_, const std::function<void(int)>& function_); // int arg is supposed to be the thread id
    inline void setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void runJob(const std::string& jobName_);
    inline void removeJob(const std::string& jobName_);

    inline void pauseParallelThreads();
    inline void unPauseParallelThreads(){ _pauseThreads_ = false; }

  protected:
    inline void reStartThreads();
    inline void startThreads();
    inline void stopThreads();

    inline void threadWorker(int iThread_);

  private:
    // Parameters
    bool _isVerbose_{true};
    bool _checkHardwareCurrency_{true};
    bool _cpuTimeSaverIsEnabled_{false};
    int _nThreads_{1};

    // Internals
    bool _stopThreads_{false};
    bool _pauseThreads_{false};

    WorkerState _workerState_{WorkerState::Unset};
    std::vector<ThreadEntry> _threadsList_{};
    std::vector<JobEntry> _jobEntryList_{};

  };

}

#include "implementation/GenericToolbox.ParallelWorker.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_H
