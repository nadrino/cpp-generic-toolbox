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

  static std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_=-1);
  static std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static std::string getNowDateString(const std::string& dateFormat_="%Y_%m_%d-%H_%M_%S");

  namespace Time{

    class StopWatch {

    public:
      StopWatch() = default;

      void reset(){ _startTime_ = std::chrono::high_resolution_clock::now(); }
      [[nodiscard]] std::chrono::duration<double> get() const { return std::chrono::high_resolution_clock::now()-_startTime_; }

      [[nodiscard]] inline std::string toString() const {
        // .count() returns a double expressing the time in seconds
        return GenericToolbox::parseTimeUnit( this->get().count()*1E6 );
      }
      inline friend std::ostream& operator<< (std::ostream& stream, const StopWatch& stopWatch_) {
        stream << stopWatch_.toString(); return stream;
      }

    private:
      std::chrono::high_resolution_clock::time_point _startTime_{std::chrono::high_resolution_clock::now()};

    };

    template <size_t N> class AveragedTimer{

    public:
      inline void start(){ _stopWatch_.reset(); _isStarted_ = true; }
      inline void stop( size_t nbCycles_=1 ){
        if( not _isStarted_ or nbCycles_ == 0 ){ return; }
        _nbCumulated_ += nbCycles_;
        _durationsBuffer_[_cursorIndex_++] = _stopWatch_.get()/nbCycles_;
        if( _cursorIndex_ >= _durationsBuffer_.size() ){ _cursorIndex_ = 0; _isBufferFilled_ = true; }
        _isStarted_ = false;
      }
      inline void cycle( size_t nbCycles_=1 ){
        this->stop(nbCycles_);
        this->start();
      }
      inline void count( size_t nbCumulated_=1 ){
        if( nbCumulated_ > _nbCumulated_ ){ this->cycle(nbCumulated_ - _nbCumulated_);  }
        else{ _nbCumulated_ = nbCumulated_; }
      }

      [[nodiscard]] inline std::chrono::duration<double> calcAverage() const {
        std::chrono::duration<double> out{0};
        if( not _isBufferFilled_ ){
          for( int iSlot = 0 ; iSlot < _cursorIndex_+1 ; iSlot++ ){ out += _durationsBuffer_[iSlot]; }
          return out/(_cursorIndex_+1);
        }

        for( auto& duration : _durationsBuffer_ ){ out += duration; }

        // in seconds
        return out/_durationsBuffer_.size();
      }
      template<typename T> [[nodiscard]] inline ssize_t calcAverage() const{
        return std::chrono::duration_cast<T>( this->calcAverage() ).count();
      }

      [[nodiscard]] inline double calcCountSpeed() const{
        // per second
        return 1./this->calcAverage().count();
      }
      template<typename T> [[nodiscard]] inline double calcCountSpeed() const{
        return 1./this->calcAverage<T>();
      }

      [[nodiscard]] inline std::string toString() const {
        // .count() returns a double expressing the time in seconds
        return GenericToolbox::parseTimeUnit( this->calcAverage().count()*1E6 );
      }
      inline friend std::ostream& operator<< (std::ostream& stream, const AveragedTimer& aTimer_) {
        stream << aTimer_.toString(); return stream;
      }

    private:
      StopWatch _stopWatch_{};

      bool _isStarted_{false};
      bool _isBufferFilled_{false};
      int _cursorIndex_{0};
      size_t _nbCumulated_{0};
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

}


// Implementation section
namespace GenericToolbox{

  namespace Time{

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
