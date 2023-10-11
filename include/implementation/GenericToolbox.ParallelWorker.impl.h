//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H

#include "GenericToolbox.h"


// Classes: ParallelWorker
namespace GenericToolbox{

  // const getters
  inline int ParallelWorker::getJobIdx(const std::string& name_) const{
    return GenericToolbox::findElementIndex(name_, _jobEntryList_, [](const JobEntry& job){ return job.name; });
  }
  inline const JobEntry* ParallelWorker::getJobPtr(const std::string& name_) const {
    auto idx = this->getJobIdx( name_ );
    if( idx == -1 ){ return nullptr; }
    return &_jobEntryList_[idx];
  }

  // getters
  inline JobEntry* ParallelWorker::getJobPtr(const std::string& name_){
    return const_cast<JobEntry*>(const_cast<const ParallelWorker*>(this)->getJobPtr(name_));
  }

  inline void ParallelWorker::addJob(const std::string &jobName_, const std::function<void(int)> &function_) {
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr != nullptr ){ throw std::logic_error("A job with the same name has already been added: " + jobName_); }

    // make sure we don't loop over the job entries
    this->pauseParallelThreads();

    // now emplace_back()
    _jobEntryList_.emplace_back( jobName_ );
    jobPtr = this->getJobPtr( jobName_ );
    jobPtr->function = function_;
    jobPtr->jobRequestedThreadList.resize(_nThreads_, false);

    // let's allow the job loop again
    this->unPauseParallelThreads();

    if( not _cpuTimeSaverIsEnabled_ and _workerState_ == Unset ){
      // start the parallel threads if not already
      this->startThreads();
    }
  }
  inline void ParallelWorker::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // make sure we don't loop over the job entries
    this->pauseParallelThreads();

    jobPtr->functionPostParallel = function_;

    // let's allow the job loop again
    this->unPauseParallelThreads();
  }
  inline void ParallelWorker::setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // make sure we don't loop over the job entries
    this->pauseParallelThreads();

    jobPtr->functionPreParallel = function_;

    // let's allow the job loop again
    this->unPauseParallelThreads();
  }
  inline void ParallelWorker::runJob(const std::string &jobName_) {
    if( _isVerbose_ ){ std::cout << "Running \"" << jobName_ << "\" on " << _nThreads_ << " parallel threads..." << std::endl; }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // runs the pre-job if set
    if( jobPtr->functionPreParallel ){ jobPtr->functionPreParallel(); }

    // request the jobs
    for( int iThread = 0 ; iThread < _nThreads_ ; iThread++ ){
      jobPtr->jobRequestedThreadList[ iThread ] = true;
    }

    // launch threads if not already
    if( _workerState_ == Unset ){ this->startThreads(); }

    // do the last job in the main thread
    jobPtr->function(_nThreads_-1);
    jobPtr->jobRequestedThreadList.back() = false; // flag as done

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      if( _isVerbose_ ) std::cout << "Waiting for thread #" << iThread << " to be finish the job..." << std::endl;
      while( jobPtr->jobRequestedThreadList[iThread] ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait
    }

    // CPU time saver stops the parallel threads when no job is requested
    if( _cpuTimeSaverIsEnabled_ ){ this->stopThreads(); }

    // runs the post-job if set
    if( jobPtr->functionPostParallel ){ jobPtr->functionPostParallel(); }

  }
  inline void ParallelWorker::removeJob(const std::string& jobName_){
    // existing job?
    auto jobIdx{ this->getJobIdx(jobName_) };
    if( jobIdx == -1 ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // make sure we don't loop over the job entries
    this->pauseParallelThreads();

    _jobEntryList_.erase(_jobEntryList_.begin() + jobIdx);

    // let's allow the job loop again
    this->unPauseParallelThreads();

    // stop the parallel threads if no job is in the pool
    if( _jobEntryList_.empty() ){ this->stopThreads(); }
  }

  inline void ParallelWorker::pauseParallelThreads(){
    // prevent the threads to loop over the available jobs
    _pauseThreads_ = true;

    // wait for every thread to finish its current job
    for( auto& job : _jobEntryList_ ){
      for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
        if( _isVerbose_ ) std::cout << "Waiting for thread #" << iThread << " to be on paused state..." << std::endl;
        while( job.jobRequestedThreadList[iThread] ) std::this_thread::sleep_for( std::chrono::microseconds(100) );
      }
    }
  }

  inline void ParallelWorker::reStartThreads() {
    stopThreads();
    startThreads();
  }
  inline void ParallelWorker::startThreads(){
    _stopThreads_ = false;
    unPauseParallelThreads(); // make sure

    _threadsList_.clear();
    _threadsList_.reserve(_nThreads_);
    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      _threadsList_.emplace_back();
      _threadsList_.back().thread = std::make_shared<std::future<void>>(
          std::async( std::launch::async, [this, iThread]{ this->threadWorker( iThread ); } )
      );
    }

    // waiting for all of them to start
    for( auto& thread : _threadsList_ ){
      while( thread.threadStatus == ThreadStatus::Stopped ){ std::this_thread::sleep_for( std::chrono::microseconds(100) ); }
    }

    _workerState_ = WorkerState::Set;
  }
  inline void ParallelWorker::stopThreads(){
    _stopThreads_ = true;
    this->unPauseParallelThreads(); // is the threads were on pause state

    for( auto& thread : _threadsList_ ){ thread.thread.get(); }

    _threadsList_.clear();
    _workerState_ = WorkerState::Unset;
  }


  inline void ParallelWorker::threadWorker(int iThread_){

    auto* thisWorker{&_threadsList_[iThread_]};

    size_t jobIndex;
    while( not _stopThreads_ ){
      thisWorker->threadStatus = ThreadStatus::Idle;

      std::this_thread::sleep_for( std::chrono::microseconds(100) ); // let space for other threads...
      while( _pauseThreads_ ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait

      if( _stopThreads_ ) break; // if stop requested while in pause

      for( auto& job : _jobEntryList_ ){
        if( _pauseThreads_ ){ break; } // jump out!
        if( job.jobRequestedThreadList[iThread_] ){
          thisWorker->threadStatus = ThreadStatus::Running;
          job.function(iThread_); // run
          thisWorker->threadStatus = ThreadStatus::Idle;
          job.jobRequestedThreadList[iThread_] = false; // this thread has done its job
        }
      }

    } // not stop

    thisWorker->threadStatus = ThreadStatus::Stopped;
  }


}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
