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
#include <chrono>
#include <condition_variable>


// Classes : ParallelWorker
namespace GenericToolbox{

  struct WorkerEntry{
    int index{-1};
    std::shared_ptr<std::future<void>> thread{nullptr};

    // signal handler
    std::function<void(int)>* fctToRunPtr{nullptr};
    GenericToolbox::Atomic<bool> isEngaged{false};
    GenericToolbox::Atomic<bool> isRunning{false};
  };

  struct JobEntry{
    std::string name{};
    std::function<void(int)> function{};
    std::function<void()> functionPreParallel{};
    std::function<void()> functionPostParallel{};

    JobEntry() = delete;
    explicit JobEntry(std::string  name_) : name(std::move(name_)) {}
  };

  class ParallelWorker {

  public:
    template<typename T> inline static std::pair<T, T> getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_);

  public:
    inline ParallelWorker() = default;
    inline virtual ~ParallelWorker(){ if( not _workerList_.empty() ){ this->stopThreads(); } };

    inline void setIsVerbose(bool isVerbose_){ _isVerbose_ = isVerbose_; }
    inline void setNThreads(int nThreads_);
    inline void setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_);

    // const getters
    [[nodiscard]] inline int getNbThreads() const{ return _nbThreads_; }
    [[nodiscard]] inline int getJobIdx(const std::string& name_) const;
    [[nodiscard]] inline const JobEntry* getJobPtr(const std::string& name_) const;

    // getters
    inline JobEntry* getJobPtr(const std::string& name_);

    // core
    inline void addJob(const std::string& jobName_, const std::function<void(int)>& function_); // int arg is supposed to be the thread id
    inline void setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void runJob(const std::string& jobName_);
    inline void removeJob(const std::string& jobName_);

  protected:
    inline void startThreads();
    inline void stopThreads();

    inline void threadWorker(int iThread_);

  private:
    // Parameters
    bool _isVerbose_{false};
    bool _cpuTimeSaverIsEnabled_{false};
    int _nbThreads_{1};

    // Internals
    bool _stopThreads_{false};

    std::vector<WorkerEntry> _workerList_{};
    std::vector<JobEntry> _jobEntryList_{};

  };

}

#include "implementation/GenericToolbox.ParallelWorker.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_H
