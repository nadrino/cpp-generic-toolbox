//
// Created by Nadrino on 13/11/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOG_H
#define GUNDAM_GENERICTOOLBOX_LOG_H

#include "GenericToolbox.Macro.h"

#include <iostream>
#include <exception>
#include <sstream>
#include <string>
#include <iomanip>


// a very simple logger
namespace GenericToolbox{
  namespace LoggerUtils{

    std::string getTimeStr(){
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
}

// internals
#define GTLogBase std::string(__FILENAME__) + ":" + std::to_string(__LINE__) + "/" + __func__ + ": "

// basics
#define GTLogError   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " ERROR " << GTLogBase
#define GTLogAlert   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " ALERT " << GTLogBase
#define GTLogWarning std::cout << GenericToolbox::LoggerUtils::getTimeStr() << "  WARN " << GTLogBase
#define GTLogInfo    std::cout << GenericToolbox::LoggerUtils::getTimeStr() << "  INFO " << GTLogBase
#define GTLogDebug   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " DEBUG " << GTLogBase
#define GTLogTrace   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " TRACE " << GTLogBase

// throw
#define GTLogThrow(message_) GTLogError << message_ << std::endl; throw std::runtime_error("exception thrown by the logger.")
#define GTLogThrowIf(condition_, message_) if(condition_){ GTLogThrow( "[" << #condition_ << "] " << message_); }

// debug tools
#define GTDebugVar(var_) GTLogDebug << GET_VAR_NAME_VALUE(var_) << std::endl;


#endif //GUNDAM_GENERICTOOLBOX_LOG_H
