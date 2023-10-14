//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H

#include "GenericToolbox.h"


// Classes: ParallelWorker
namespace GenericToolbox{

  // statics
  template<typename T> inline std::pair<T, T> ParallelWorker::getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_){
    // first index, last index: for( int i = out.first ; i < out.second ; i++){}
    std::pair<T, T> out{0, nTotal_};

    if( iThread_ == -1 or nThreads_ == 1 ){ return out; }

    int nEventsPerThread = nTotal_ / nThreads_;
    int nExtraEvents = nTotal_ % nThreads_;

    // make sure we get the right shift event if nTotal_ < nThreads_
    out.first =
        std::min(iThread_, nExtraEvents) * (nEventsPerThread + 1) +
        std::max(0, iThread_ - nExtraEvents) * nEventsPerThread;

    // adjust such the first threads are sharing the numExtraEvents
    out.second =
        out.first +
        ((iThread_ < nExtraEvents) ? nEventsPerThread + 1 : nEventsPerThread );

    // OLD METHOD:
//    out.first = T(iThread_)*( nTotal_ / T(nThreads_) );
//    if( iThread_+1 != nThreads_ ){
//      out.second = (T(iThread_) + 1) * (nTotal_ / T(nThreads_));
//    }

    return out;
  }

  // setters
  inline void ParallelWorker::setNThreads(int nThreads_){
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _nbThreads_ = nThreads_;
  }
  inline void ParallelWorker::setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_){
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _cpuTimeSaverIsEnabled_ = cpuTimeSaverIsEnabled_;
  }

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

    // now emplace_back()
    _jobEntryList_.emplace_back( jobName_ );
    jobPtr = this->getJobPtr( jobName_ );
    jobPtr->function = function_;
  }
  inline void ParallelWorker::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPostParallel = function_;
  }
  inline void ParallelWorker::setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPreParallel = function_;
  }
  inline void ParallelWorker::runJob(const std::string &jobName_) {
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
      thread.fctToRunPtr = &jobPtr->function;
      thread.isEngaged.setValue(true );
    }

    // do the last job in the main thread
    jobPtr->function(_nbThreads_ - 1);

    for( auto& worker : _workerList_ ){
      if( _isVerbose_ ) std::cout << "Waiting for worker #" << worker.index << " to be finish the job..." << std::endl;
      worker.isEngaged.waitUntilEqual( false );
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

    _jobEntryList_.erase(_jobEntryList_.begin() + jobIdx);

    // stop the parallel threads if no job is in the pool
    if( _jobEntryList_.empty() ){ this->stopThreads(); }
  }

  inline void ParallelWorker::startThreads(){
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
  inline void ParallelWorker::stopThreads(){
    if( _isVerbose_ ){ std::cout << __METHOD_NAME__ << std::endl; }

    // set stop signal
    _stopThreads_ = true;

    // stop waiting for the signal
    for( auto& worker : _workerList_ ){ worker.isEngaged.setValue( true ); }

    // wait for all to close
    for( auto& worker : _workerList_ ){ worker.thread.get(); }

    // an empty worker list is saying the threads are not running. So clearing it
    _workerList_.clear();
  }


  inline void ParallelWorker::threadWorker(int iThread_){

    auto* thisWorker{&_workerList_[iThread_]};
    thisWorker->isRunning.setValue( true );

    while( not _stopThreads_ ){

      // release with signal is set
      thisWorker->isEngaged.waitUntilEqual( true );

      if( _stopThreads_ ){ break; } // if stop requested while in pause

      // run job
      (*thisWorker->fctToRunPtr)(iThread_);

      // reset
      thisWorker->isEngaged.setValue(false );

    } // not stop

  }
}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
