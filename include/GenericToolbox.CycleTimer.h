//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H

#include "GenericToolbox.h"

#include <string>
#include <vector>
#include <chrono>
#include <utility>
#include <sstream>


// Classes : VariablesMonitor
namespace GenericToolbox{

  struct CycleTimer{
    long long counts{0};
    long long cumulated{0};
    friend std::ostream& operator<< (std::ostream& stream, const CycleTimer& timer_) {
      if(timer_.counts == 0) stream << "0s";
      else stream << GenericToolbox::parseTimeUnit(double(timer_.cumulated) / double(timer_.counts));
      return stream;
    }
  };

  class CycleClock{
  public:
    CycleClock() = default;
    explicit CycleClock(std::string unit_) : _unit_(std::move(unit_)) {}
    ~CycleClock() = default;

    // setters
    void setUnit(const std::string &unit_) { _unit_ = unit_; }

    // getters
    long long int getCounts() const { return _counts_; }
    double* getCountSpeedPtr(){ return &_countSpeed_; }

    // measure
    void start(){ _startTime_ = std::chrono::high_resolution_clock::now(); _hasStarted_ = true; }
    void stopAndCumulate(long long counts_=1){
      if( not _hasStarted_ ){ throw std::runtime_error("clock not started."); }
      _cumulatedSeconds_ += std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-_startTime_).count();
      cumulate( counts_ );
      _hasStarted_ = false;
    }
    void cycle(long long int counts_=1){
      if( not _hasStarted_ ){
        cumulate(counts_);
        start();
      }
      else{
        // stop-start
        stopAndCumulate( counts_ );
        start();
      }
    }
    void cumulate(long long int counts_){ _counts_ += counts_; }

    // output
    std::string getCountingSpeed() const{
      std::stringstream ss;
      if( _counts_ == 0 or not _hasStarted_ ) ss << "0 " << _unit_ << "/s";
      else{
        this->updateCountingSpeed();
        ss << GenericToolbox::parseUnitPrefix(_countSpeed_) << " " << _unit_ << "/s";
      }
      return ss.str();
    }
    friend std::ostream& operator<< (std::ostream& stream, const CycleClock& cCock_) {
      stream << cCock_.getCountingSpeed();
      return stream;
    }

    // cache
    void updateCountingSpeed() const {
      _countSpeed_ = double(_counts_)/_cumulatedSeconds_;
    }

  private:
    // parameters
    std::string _unit_{"counts"};

    // internals
    bool _hasStarted_{false};
    long long int _counts_{0};
    double _cumulatedSeconds_{0};

    // caches
    std::chrono::high_resolution_clock::time_point _startTime_;
    mutable double _countSpeed_{0};
  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
