//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_TIME_H
#define CPP_GENERIC_TOOLBOX_TIME_H

// ***************************
//! Time related tools
// ***************************

#include <sstream>
#include <iomanip>
#include <utility>
#include <string>
#include <chrono>
#include <vector>
#include <map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration section
namespace GenericToolbox{

  namespace Time{

    struct CycleTimer{
      long long counts{0};
      long long cumulated{0};
      inline friend std::ostream& operator<< (std::ostream& stream, const CycleTimer& timer_);
    };

    class StopWatch {

    public:
      StopWatch() = default;

      void reset(){ _startTime_ = std::chrono::high_resolution_clock::now(); }
      std::chrono::duration<double> get(){ return std::chrono::high_resolution_clock::now()-_startTime_; }

    private:
      std::chrono::high_resolution_clock::time_point _startTime_{std::chrono::high_resolution_clock::now()};

    };


    template <size_t N> class AveragedTimer{

    public:
      void start(){ _stopWatch_.reset(); _isStarted_ = true; }
      void stop(){
        if( not _isStarted_ ){ return; }
        _durationsBuffer_[_cursorIndex_++] = _stopWatch_.get();
        if( _cursorIndex_ >= _durationsBuffer_.size() ){ _cursorIndex_ = 0; _isCycleCompleted_ = true; }
        _isStarted_ = false;
      }

      [[nodiscard]] std::chrono::duration<double> calcAverage() const {
        if( not _isCycleCompleted_ ){
          return std::accumulate(_durationsBuffer_.begin(), _durationsBuffer_.begin()+_cursorIndex_+1)/(_cursorIndex_+1);
        }
        return std::accumulate(_durationsBuffer_.begin(), _durationsBuffer_.end())/_durationsBuffer_.size();
      }
      inline friend std::ostream& operator<< (std::ostream& stream, const AveragedTimer& cCock_) {
        stream << cCock_.calcAverage().count(); return stream;
      }

    private:
      StopWatch _stopWatch_{};

      bool _isStarted_{false};
      bool _isCycleCompleted_{false};
      int _cursorIndex_{0};
      std::array<std::chrono::duration<double>, N> _durationsBuffer_{};
    };


    class CycleCounterClock{
    public:
      CycleCounterClock() = default;
      explicit CycleCounterClock( std::string unit_) : _unit_(std::move(unit_)) {}
      ~CycleCounterClock() = default;

      // setters
      inline void setUnit(const std::string &unit_) { _unit_ = unit_; }

      // getters
      inline long long int getCounts() const { return _counts_; }
      inline double* getCountSpeedPtr(){ return &_countSpeed_; }

      // measure
      inline void start(){ _stopWatch_.reset(); _hasStarted_ = true; }
      inline void stopAndCumulate(long long counts_=1);
      inline void cycle(long long int counts_=1);
      inline void cumulate(long long int counts_){ _counts_ += counts_; }

      // output
      inline std::string getCountingSpeed() const;
      inline friend std::ostream& operator<< (std::ostream& stream, const CycleCounterClock& cCock_) { stream << cCock_.getCountingSpeed(); return stream; }

      // cache
      inline void updateCountingSpeed() const { _countSpeed_ = double(_counts_)/_cumulatedSeconds_; }

    private:
      // parameters
      std::string _unit_{"counts"};

      // internals
      bool _hasStarted_{false};
      long long int _counts_{0};
      double _cumulatedSeconds_{0};

      // caches
      StopWatch _stopWatch_{};
      mutable double _countSpeed_{0};
    };
  }



  static std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_=-1);
  static std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static std::string getNowDateString(const std::string& dateFormat_="%Y_%m_%d-%H_%M_%S");

}


// Implementation section
namespace GenericToolbox{

  namespace Time{
    inline std::ostream& operator<< (std::ostream& stream, const CycleTimer& timer_) {
      if(timer_.counts == 0) stream << "0s";
      else stream << GenericToolbox::parseTimeUnit(double(timer_.cumulated) / double(timer_.counts));
      return stream;
    }

    inline void CycleCounterClock::stopAndCumulate( long long counts_){
      if( not _hasStarted_ ){ throw std::runtime_error("clock not started."); }
      _cumulatedSeconds_ += _stopWatch_.get().count();
      cumulate( counts_ );
      _hasStarted_ = false;
    }
    inline void CycleCounterClock::cycle( long long int counts_){
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
    inline std::string CycleCounterClock::getCountingSpeed() const{
      std::stringstream ss;
      if( _counts_ == 0 or not _hasStarted_ ) ss << "0 " << _unit_ << "/s";
      else{
        this->updateCountingSpeed();
        ss << GenericToolbox::parseUnitPrefix(_countSpeed_) << " " << _unit_ << "/s";
      }
      return ss.str();
    }

    namespace Internals{
      static std::map<int, std::chrono::high_resolution_clock::time_point> lastTimePointMap{};
      static std::map<std::string, std::chrono::high_resolution_clock::time_point> lastTimePointMapStr{};
    }
  }



  static std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_){

    std::stringstream ss;

    if( nbMicroSec_ < 0 ){
      ss << "-";
      nbMicroSec_ = -nbMicroSec_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(std::abs(nbMicroSec_));
    if     ( (reducedVal = (reducedVal / 1000)) < 9 ){ // print in ms?
      ss << nbMicroSec_ << "us";                       // <- no
    }
    else if( (reducedVal = (reducedVal / 1000)) < 3 ){ // print in s?
      ss << nbMicroSec_/1E3 << "ms";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in min?
      ss << nbMicroSec_/1E6 << "s";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in h?
      ss << nbMicroSec_/1E6/60. << "min";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in d?
      ss << nbMicroSec_/1E6/3600. << "h";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in y?
      ss << nbMicroSec_/1E6/3600./24. << "d";
    }
    else {
      ss << nbMicroSec_/1E6/3600./24./365.25 << "y";
    }
    return ss.str();
  }
  static std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(double(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_)));
  }
  static std::string getElapsedTimeSinceLastCallStr(int instance_){
    return GenericToolbox::parseTimeUnit(double(getElapsedTimeSinceLastCallInMicroSeconds(instance_)));
  }
  static long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMapStr[key_]
    );
    Time::Internals::lastTimePointMapStr[key_] = newTimePoint;
    return microseconds.count();
  }
  static long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_){
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMap[instance_]
    );
    Time::Internals::lastTimePointMap[instance_] = newTimePoint;
    return microseconds.count();
  }
  static std::string getNowDateString(const std::string& dateFormat_){
    std::stringstream ss;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4)
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    char buffer[128];
    std::strftime(buffer, sizeof(buffer), dateFormat_.c_str(), timeinfo);

    ss << buffer;
#else
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    ss << std::put_time(std::localtime(&in_time_t), dateFormat_.c_str());
#endif
    return ss.str();
  }



}


#endif // CPP_GENERIC_TOOLBOX_TIME_H
