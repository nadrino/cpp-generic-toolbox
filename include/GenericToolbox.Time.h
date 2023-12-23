//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_TIME_H
#define CPP_GENERIC_TOOLBOX_TIME_H

#include <string>

// ***************************
//! Time related tools
// ***************************


// Declaration section
namespace GenericToolbox{

  static inline std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_=-1);
  static inline std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static inline std::string getNowDateString(const std::string& dateFormat_="%Y_%m_%d-%H_%M_%S");

}


#include <sstream>
#include <map>


namespace GenericToolbox{

  namespace Time{
    namespace Internals{
      static std::map<int, std::chrono::high_resolution_clock::time_point> lastTimePointMap{};
      static std::map<std::string, std::chrono::high_resolution_clock::time_point> lastTimePointMapStr{};
    }
  }


  static inline std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_){

    std::stringstream ss;

    if( nbMicroSec_ < 0 ){
      ss << "-";
      nbMicroSec_ = -nbMicroSec_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(fabs(nbMicroSec_));
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
  static inline std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(double(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_)));
  }
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_){
    return GenericToolbox::parseTimeUnit(double(getElapsedTimeSinceLastCallInMicroSeconds(instance_)));
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMapStr[key_]
    );
    Time::Internals::lastTimePointMapStr[key_] = newTimePoint;
    return microseconds.count();
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_){
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMap[instance_]
    );
    Time::Internals::lastTimePointMap[instance_] = newTimePoint;
    return microseconds.count();
  }
  static inline std::string getNowDateString(const std::string& dateFormat_){
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
