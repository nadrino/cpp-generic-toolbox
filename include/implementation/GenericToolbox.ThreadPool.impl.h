//
// Created by Adrien BLANCHET on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H

#include "../GenericToolbox.h"

// Classes: ThreadPool
namespace GenericToolbox{

  inline ThreadPool::ThreadPool(){
    this->reset();
  }
  inline ThreadPool::~ThreadPool() {
    this->reset();
  }

  inline void ThreadPool::reset() {
    _isInitialized_ = false;
    _nThreads_ = -1;

    stopThreads();
    _jobNameList_.clear();
    _jobTriggerList_.clear();
    _jobFunctionList_.clear();
    _jobFunctionPostParallelList_.clear();
  }

  inline void ThreadPool::setIsVerbose(bool isVerbose) {
    _isVerbose_ = isVerbose;
  }
  inline void ThreadPool::setNThreads(int nThreads) {
    if(_isInitialized_){
      throw std::logic_error("Can't set the number of threads while already initialized.");
    }
    if( nThreads > std::thread::hardware_concurrency() ){
      std::cout << GET_VAR_NAME_VALUE(std::thread::hardware_concurrency()) << std::endl;
      throw std::logic_error("Too much threads wrt your hardware.");
    }
    _nThreads_ = nThreads;
  }

  inline void ThreadPool::initialize() {

    if( _nThreads_ < 1 ){
      throw std::logic_error("_nThreads_ should be >= 1");
    }

    _isInitialized_ = true;
  }

  inline void ThreadPool::addJob(const std::string &jobName_, const std::function<void(int)> &function_) {
    if( not _isInitialized_ ){
      throw std::logic_error("Can't add job while not initialized");
    }
    if( GenericToolbox::doesElementIsInVector(jobName_, _jobNameList_) ){
      throw std::logic_error("A job with the same name has already been added: " + jobName_);
    }
    if( not function_ ){ // is it callable?
      throw std::logic_error("the provided function is not callable");
    }

    this->pauseParallelThreads();
    _jobNameList_.emplace_back(jobName_);
    _jobFunctionList_.emplace_back(function_);
    _jobFunctionPostParallelList_.emplace_back();
    _jobTriggerList_.emplace_back(std::vector<bool>(_nThreads_, false));
    this->unPauseParallelThreads();

    if( _threadsList_.empty() ){
      // start the parallel threads if not already
      reStartThreads();
    }
  }
  inline void ThreadPool::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    if( not _isInitialized_ ){
      throw std::logic_error("Can't add post parallel job while not initialized");
    }
    int jobIndex = GenericToolbox::findElementIndex(jobName_, _jobNameList_);
    if( jobIndex == -1 ){
      throw std::logic_error(jobName_ + ": is not in the available jobsList");
    }
    if( not function_ ){ // is it callable?
      throw std::logic_error("the provided post parallel function is not callable");
    }

    this->pauseParallelThreads();
    _jobFunctionPostParallelList_.at(jobIndex) = function_;
    this->unPauseParallelThreads();
  }
  inline void ThreadPool::runJob(const std::string &jobName_) {
    if( not _isInitialized_ ){
      throw std::logic_error("Can't run job while not initialized");
    }
    int jobIndex = GenericToolbox::findElementIndex(jobName_, _jobNameList_);
    if( jobIndex == -1 ){
      throw std::logic_error(jobName_ + ": is not in the available jobsList");
    }

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      _jobTriggerList_.at(jobIndex).at(iThread) = true;
    }

    _jobFunctionList_.at(jobIndex)(_nThreads_-1); // do the last job in the main thread

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      while( _jobTriggerList_.at(jobIndex).at(iThread) ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait
    }

    if( _jobFunctionPostParallelList_.at(jobIndex) ){ // is it callable?
      _jobFunctionPostParallelList_.at(jobIndex)();
    }
  }
  inline void ThreadPool::removeJob(const std::string& jobName_){
    if( not _isInitialized_ ){
      throw std::logic_error("Can't run job while not initialized");
    }
    int jobIndex = GenericToolbox::findElementIndex(jobName_, _jobNameList_);
    if( jobIndex == -1 ){
      throw std::logic_error(jobName_ + ": is not in the available jobsList");
    }

    this->pauseParallelThreads();
    _jobNameList_.erase(_jobNameList_.begin() + jobIndex);
    _jobFunctionList_.erase(_jobFunctionList_.begin() + jobIndex);
    _jobFunctionPostParallelList_.erase(_jobFunctionPostParallelList_.begin() + jobIndex);
    _jobTriggerList_.erase(_jobTriggerList_.begin() + jobIndex);
    this->unPauseParallelThreads();

    if( _jobNameList_.empty() ){
      // stop the parallel threads if no job is in the pool
      stopThreads();
    }
  }

  inline void ThreadPool::pauseParallelThreads(){
    _pauseThreads_ = true; // prevent the threads to loop over the available jobs
    for( const auto& threadTriggers : _jobTriggerList_ ){
      for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
        while( threadTriggers.at(iThread) ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait for every thread to finish its current job
      }
    }
  }
  inline void ThreadPool::unPauseParallelThreads(){
    _pauseThreads_ = false; // that's all
  }

  inline const std::vector<std::string> &ThreadPool::getJobNameList() const {
    return _jobNameList_;
  }

  inline void ThreadPool::reStartThreads() {

    stopThreads();

    _stopThreads_ = false;
    _threadMutexPtr_ = new std::mutex(); // We have the ownership
    unPauseParallelThreads(); // make sure

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){

      std::function<void()> asyncLoop = [this, iThread](){
        size_t jobIndex = 0;
        while( not _stopThreads_ ){

          std::this_thread::sleep_for( std::chrono::microseconds(100) ); // let space for other threads...

          while( _pauseThreads_ ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait
          if( _stopThreads_ ) break; // if stop requested while in pause
          _threadStatusList_.at(iThread) = ThreadStatus::Idle;

          jobIndex = 0;
          for( jobIndex = 0 ; jobIndex < _jobTriggerList_.size() ; jobIndex++ ){
            if( _pauseThreads_ ) break; // jump out!
            if( _jobTriggerList_[jobIndex][iThread] ){ // is it triggered?
              _threadStatusList_.at(iThread) = ThreadStatus::Running;
              _jobFunctionList_.at(jobIndex)(iThread); // run
              _jobTriggerList_[jobIndex][iThread] = false; // un-trigger this thread
            }
          } // jobIndex
          _threadStatusList_.at(iThread) = ThreadStatus::Idle;

        } // not stop
        _threadStatusList_.at(iThread) = ThreadStatus::Stopped;
      };

      _threadStatusList_.emplace_back(ThreadStatus::Stopped);
      _threadsList_.emplace_back( std::async( std::launch::async, asyncLoop ) );

    }

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      while( _threadStatusList_.at(iThread) == ThreadStatus::Stopped ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait to be in Idle state
    }

  }
  inline void ThreadPool::stopThreads(){
    _stopThreads_ = true;
    this->unPauseParallelThreads(); // is the threads were on pause state
    for( int iThread = 0 ; iThread < int(_threadsList_.size()) ; iThread++ ){
      _threadsList_.at(iThread).get();
    }
    _threadsList_.clear();
    _threadStatusList_.clear();
    delete _threadMutexPtr_; _threadMutexPtr_ = nullptr;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
