//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H

#include "../GenericToolbox.h"

// Classes: ParallelWorker
namespace GenericToolbox{

  inline ParallelWorker::ParallelWorker(){
    this->reset();
  }
  inline ParallelWorker::~ParallelWorker() {
    std::cout << "Stopping ParallelWorkers..." << std::endl;
    stopThreads();
  }

  inline void ParallelWorker::reset() {
    _isInitialized_ = false;
    _isVerbose_ = false;
    _checkHardwareCurrency_ = false;
    _nThreads_ = -1;

    stopThreads();
    _jobNameList_.clear();
    _jobTriggerList_.clear();
    _jobFunctionList_.clear();
    _jobFunctionPostParallelList_.clear();
    _jobFunctionPreParallelList_.clear();
  }

  inline void ParallelWorker::setIsVerbose(bool isVerbose) {
    _isVerbose_ = isVerbose;
  }
  inline void ParallelWorker::setCheckHardwareCurrency(bool checkHardwareCurrency) {
    _checkHardwareCurrency_ = checkHardwareCurrency;
  }
  inline void ParallelWorker::setNThreads(int nThreads) {
    if(_isInitialized_){
      throw std::logic_error("Can't set the number of threads while already initialized.");
    }
    if( _checkHardwareCurrency_ and nThreads > std::thread::hardware_concurrency() ){
      std::cout << GET_VAR_NAME_VALUE(std::thread::hardware_concurrency()) << std::endl;
      throw std::logic_error("Too much threads wrt your hardware.");
    }
    _nThreads_ = nThreads;
  }
  inline void ParallelWorker::setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled) {
    if(cpuTimeSaverIsEnabled != _cpuTimeSaverIsEnabled_){
      if(cpuTimeSaverIsEnabled) this->stopThreads();
      if(not cpuTimeSaverIsEnabled and not _jobNameList_.empty()) this->startThreads();
    }
    _cpuTimeSaverIsEnabled_ = cpuTimeSaverIsEnabled;
  }

  inline void ParallelWorker::initialize() {
    if( _nThreads_ < 1 ){
      throw std::logic_error("_nThreads_ should be >= 1");
    }
    _isInitialized_ = true;
  }

  inline void ParallelWorker::addJob(const std::string &jobName_, const std::function<void(int)> &function_) {
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
    _jobFunctionPreParallelList_.emplace_back();
    _jobTriggerList_.emplace_back(std::vector<bool>(_nThreads_, false));
    this->unPauseParallelThreads();

    if( not _cpuTimeSaverIsEnabled_ and _threadsList_.empty() ){
      // start the parallel threads if not already
      startThreads();
    }
  }
  inline void ParallelWorker::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_){
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
  inline void ParallelWorker::setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_){
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
    _jobFunctionPreParallelList_.at(jobIndex) = function_;
    this->unPauseParallelThreads();
  }
  inline void ParallelWorker::runJob(const std::string &jobName_) {
    if( _isVerbose_ ) std::cout << "Running \"" << jobName_ << "\" on " << _nThreads_ << " parallel threads..." << std::endl;
    if( not _isInitialized_ ){
      throw std::logic_error("Can't run job while not initialized");
    }
    int jobIndex = GenericToolbox::findElementIndex(jobName_, _jobNameList_);
    if( jobIndex == -1 ){
      throw std::logic_error(jobName_ + ": is not in the available jobsList");
    }

    if( _jobFunctionPreParallelList_.at(jobIndex) ){ // is it callable?
      _jobFunctionPreParallelList_.at(jobIndex)();
    }

    if(_cpuTimeSaverIsEnabled_) this->startThreads();
    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      _jobTriggerList_.at(jobIndex).at(iThread) = true;
    }

    _jobFunctionList_.at(jobIndex)(_nThreads_-1); // do the last job in the main thread

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
      if( _isVerbose_ ) std::cout << "Waiting for thread #" << iThread << " to be finish the job..." << std::endl;
      while( _jobTriggerList_.at(jobIndex).at(iThread) ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait
    }
    if(_cpuTimeSaverIsEnabled_) this->stopThreads();

    if( _jobFunctionPostParallelList_.at(jobIndex) ){ // is it callable?
      _jobFunctionPostParallelList_.at(jobIndex)();
    }

  }
  inline void ParallelWorker::removeJob(const std::string& jobName_){
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
    _jobFunctionPreParallelList_.erase(_jobFunctionPreParallelList_.begin() + jobIndex);
    _jobTriggerList_.erase(_jobTriggerList_.begin() + jobIndex);
    this->unPauseParallelThreads();

    if( _jobNameList_.empty() ){
      // stop the parallel threads if no job is in the pool
      stopThreads();
    }
  }

  inline void ParallelWorker::pauseParallelThreads(){
    _pauseThreads_.atomicValue = true; // prevent the threads to loop over the available jobs
    for( const auto& threadTriggers : _jobTriggerList_ ){
      for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){
        if( _isVerbose_ ) std::cout << "Waiting for thread #" << iThread << " to be on paused state..." << std::endl;
        while( threadTriggers.at(iThread) ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait for every thread to finish its current job
      }
    }
  }
  inline void ParallelWorker::unPauseParallelThreads(){
//    std::unique_lock<std::mutex> lock(_workerMutex_);
    _pauseThreads_.atomicValue = false;
//    _conditionVariable_.notify_all();
//    lock.unlock();
  }

  inline const std::vector<std::string> &ParallelWorker::getJobNameList() const {
    return _jobNameList_;
  }
  inline int ParallelWorker::getNThreads() const {
    return _nThreads_;
  }

  inline void ParallelWorker::reStartThreads() {
    stopThreads();
    startThreads();
  }
  inline void ParallelWorker::startThreads(){
    _stopThreads_ = false;
    unPauseParallelThreads(); // make sure

    for( int iThread = 0 ; iThread < _nThreads_-1 ; iThread++ ){

      std::function<void()> asyncLoop = [this, iThread](){
        size_t jobIndex;
        while( not _stopThreads_ ){

          std::this_thread::sleep_for( std::chrono::microseconds(100) ); // let space for other threads...
          while( _pauseThreads_.atomicValue ) std::this_thread::sleep_for( std::chrono::microseconds(100) ); // wait

          // dev
//          if( _pauseThreads_.atomicValue ){
//            std::unique_lock<std::mutex> lock(_workerMutex_);
//            _conditionVariable_.wait(lock, [&](){ return !_pauseThreads_.atomicValue; });
//            lock.unlock();
//          }

          if( _stopThreads_ ) break; // if stop requested while in pause
          _threadStatusList_.at(iThread) = ThreadStatus::Idle;

          for( jobIndex = 0 ; jobIndex < _jobTriggerList_.size() ; jobIndex++ ){
            if( _pauseThreads_.atomicValue ) break; // jump out!
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
  inline void ParallelWorker::stopThreads(){
    _stopThreads_ = true;
    this->unPauseParallelThreads(); // is the threads were on pause state
    for( int iThread = 0 ; iThread < int(_threadsList_.size()) ; iThread++ ){
      _threadsList_.at(iThread).get();
    }
    _threadsList_.clear();
    _threadStatusList_.clear();
  }


}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_THREADPOOL_IMPL_H
