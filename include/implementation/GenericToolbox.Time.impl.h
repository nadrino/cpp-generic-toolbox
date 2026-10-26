#pragma once

namespace GenericToolbox {
namespace Time {
namespace Internals {
#ifdef CPP_GENERIC_TOOLBOX_TIME_COMPILED
std::map<int, std::chrono::high_resolution_clock::time_point> lastTimePointMap{};
#endif
}
}

namespace Time {
namespace Internals {
#ifdef CPP_GENERIC_TOOLBOX_TIME_COMPILED
std::map<std::string, std::chrono::high_resolution_clock::time_point> lastTimePointMapStr{};
#endif
}
}

CPP_GENERIC_TOOLBOX_TIME_STATIC std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_) {

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

CPP_GENERIC_TOOLBOX_TIME_STATIC std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(double(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_)));
  }

CPP_GENERIC_TOOLBOX_TIME_STATIC std::string getElapsedTimeSinceLastCallStr(int instance_) {
    return GenericToolbox::parseTimeUnit(double(getElapsedTimeSinceLastCallInMicroSeconds(instance_)));
  }

CPP_GENERIC_TOOLBOX_TIME_STATIC long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMapStr[key_]
    );
    Time::Internals::lastTimePointMapStr[key_] = newTimePoint;
    return microseconds.count();
  }

CPP_GENERIC_TOOLBOX_TIME_STATIC long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMap[instance_]
    );
    Time::Internals::lastTimePointMap[instance_] = newTimePoint;
    return microseconds.count();
  }

CPP_GENERIC_TOOLBOX_TIME_STATIC std::string getNowDateString(const std::string& dateFormat_) {
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

CPP_GENERIC_TOOLBOX_TIME_STATIC std::string toString(std::chrono::duration<double> duration_) {
    std::stringstream ss;
    using namespace std::chrono;

    auto h = duration_cast<hours>(duration_);
    duration_ -= h;
    auto m = duration_cast<minutes>(duration_);
    duration_ -= m;
    auto s = duration_cast<seconds>(duration_);
    duration_ -= s;
    auto ms = duration_cast<milliseconds>(duration_);

    bool started = false;

    if (h.count() > 0) {
      ss << h.count() << "h:";
      started = true;
    }
    if (m.count() > 0 || started) {  // Print minutes if hours were printed
      ss << m.count() << "m:";
      started = true;
    }
    if (s.count() > 0 || started) {  // Print seconds if minutes or hours were printed
      ss << s.count() << "s.";
      started = true;
    }
    // Always print milliseconds if everything else is zero
    ss << ms.count() << "ms";

    return ss.str();
  }
}
