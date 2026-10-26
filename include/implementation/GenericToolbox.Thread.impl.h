#pragma once

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::setNThreads(int nThreads_) {
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _nbThreads_ = nThreads_;
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_) {
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _cpuTimeSaverIsEnabled_ = cpuTimeSaverIsEnabled_;
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE int ParallelWorker::getJobIdx(const std::string& name_) const {
    return GenericToolbox::findElementIndex(name_, _jobEntryList_, [](const JobEntry& job){ return job.name; });
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE const JobEntry* ParallelWorker::getJobPtr(const std::string& name_) const {
    auto idx = this->getJobIdx( name_ );
    if( idx == -1 ){ return nullptr; }
    return &_jobEntryList_[idx];
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE JobEntry* ParallelWorker::getJobPtr(const std::string& name_) {
    return const_cast<JobEntry*>(const_cast<const ParallelWorker*>(this)->getJobPtr(name_));
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::addJob(const std::string &jobName_, const std::function<void(int)> &function_) {
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr != nullptr ){ throw std::logic_error("A job with the same name has already been added: " + jobName_); }

    // now emplace_back()
    _jobEntryList_.emplace_back( jobName_ );
    jobPtr = &_jobEntryList_.back();
    jobPtr->function = function_;
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_) {
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPostParallel = function_;
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_) {
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPreParallel = function_;
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::runJob(const std::function<void(int)>& function_) {
    std::string tempName{std::to_string((unsigned long long)(void**)this)};
    this->addJob(tempName, function_);
    try{ this->runJob(tempName); }
    catch(...){
      this->removeJob(tempName);
      throw;
    }
    this->removeJob(tempName);
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::runJob(const std::string &jobName_) {
    _lastJobTimer_.start();
    if( _isVerbose_ ){ std::cout << "Running \"" << jobName_ << "\" on " << _nbThreads_ << " parallel threads..." << std::endl; }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // runs the pre-job if set
    if( jobPtr->functionPreParallel ){ jobPtr->functionPreParallel(); }

    // launch threads if not already
    if( _workerList_.empty() ){ this->startThreads(); }

    // request the jobs
    for( auto& thread : _workerList_ ){
      thread.exception = nullptr;
      thread.fctToRunPtr = &jobPtr->function;
      thread.isEngaged.setValue(true );
    }

    // do the last job in the main thread
    std::exception_ptr exception;
    try{ jobPtr->function(_nbThreads_ - 1); }
    catch(...){ exception = std::current_exception(); }

    for( auto& worker : _workerList_ ){
      if( _isVerbose_ ) std::cout << "Waiting for worker #" << worker.index << " to be finish the job..." << std::endl;
      worker.isEngaged.waitUntilEqual( false );
      if( not exception and worker.exception ){ exception = worker.exception; }
    }

    // CPU time saver stops the parallel threads when no job is requested
    if( _cpuTimeSaverIsEnabled_ or exception ){ this->stopThreads(); }

    if( exception ){
      _lastJobTimer_.stop();
      std::rethrow_exception(exception);
    }

    // runs the post-job if set
    if( jobPtr->functionPostParallel ){ jobPtr->functionPostParallel(); }

    _lastJobTimer_.stop();
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::removeJob(const std::string& jobName_) {
    // existing job?
    auto jobIdx{ this->getJobIdx(jobName_) };
    if( jobIdx == -1 ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    _jobEntryList_.erase(_jobEntryList_.begin() + jobIdx);

    // stop the parallel threads if no job is in the pool
    if( _jobEntryList_.empty() ){ this->stopThreads(); }
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::startThreads() {
    if( _isVerbose_ ){ std::cout << __METHOD_NAME__ << std::endl; }

    _stopThreads_ = false;

    _workerList_.clear();
    _workerList_.resize(_nbThreads_ - 1 );
    int iThread{-1};
    for( auto& worker : _workerList_ ){
      iThread++;
      worker.index = iThread;
      worker.thread = std::make_shared<std::future<void>>(
          std::async( std::launch::async, [this, iThread]{ this->threadWorker( iThread ); } )
      );
    }

    // waiting for all of them to start
    for( auto& worker : _workerList_ ){
      worker.isRunning.waitUntilEqual( true );
    }
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::stopThreads() {
    if( _isVerbose_ ){ std::cout << __METHOD_NAME__ << std::endl; }

    // set stop signal
    _stopThreads_ = true;

    // stop waiting for the signal
    for( auto& worker : _workerList_ ){ worker.isEngaged.setValue( true ); }

    // wait for all to close
    for( auto& worker : _workerList_ ){ worker.thread->get(); }

    // an empty worker list is saying the threads are not running. So clearing it
    _workerList_.clear();
  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void ParallelWorker::threadWorker(int iThread_) {

    auto* thisWorker{&_workerList_[iThread_]};
    thisWorker->isRunning.setValue( true );

    while( not _stopThreads_ ){

      // release with signal is set
      thisWorker->isEngaged.waitUntilEqual( true );

      if( _stopThreads_ ){ break; } // if stop requested while in pause

      // run job
      // Keep the worker alive and always release the caller, even when its job fails.
      try{ (*thisWorker->fctToRunPtr)(iThread_); }
      catch(...){ thisWorker->exception = std::current_exception(); }

      // reset
      thisWorker->isEngaged.setValue(false );

    } // not stop

  }

CPP_GENERIC_TOOLBOX_THREAD_INLINE void OrderedLock::lock() {
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

CPP_GENERIC_TOOLBOX_THREAD_INLINE void OrderedLock::unlock() {
    std::unique_lock<std::mutex> acquire(_lock_);
    if (_conditionVariable_.empty()) {
      _isLocked_ = false;
    }
    else {
      _conditionVariable_.front()->notify_one();
      _conditionVariable_.pop();
    }
  }
}
