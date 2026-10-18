#pragma once

namespace GenericToolbox {
namespace LoggerUtils {
CPP_GENERIC_TOOLBOX_LOG_INLINE std::string getTimeStr() {
      std::stringstream ss;
      time_t rawTime = std::time(nullptr);
      struct tm timeInfo = *localtime(&rawTime);
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4)
      char buffer[128];
      std::strftime(buffer, sizeof(buffer), "%Y.%m.%d %H:%M:%S", &timeInfo);
      ss << buffer;
#else
      ss << std::put_time(&timeInfo, "%Y.%m.%d %H:%M:%S");
#endif
      return ss.str();
    }
}

namespace LoggerUtils {
CPP_GENERIC_TOOLBOX_LOG_INLINE std::string getFctName(const std::string &func_) {
      if(func_ == "operator()"){ return "<lambda>"; }
      return func_;
    }
}
}
