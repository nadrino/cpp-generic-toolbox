//
// Created by Nadrino on 13/11/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOG_H
#define GUNDAM_GENERICTOOLBOX_LOG_H

#include "GenericToolbox.Build.h"

#include "GenericToolbox.Macro.h"

#include <ctime>
#include <iostream>
#include <exception>
#include <sstream>
#include <string>
#include <iomanip>


// a very simple logger
namespace GenericToolbox{
  namespace LoggerUtils{

    CPP_GENERIC_TOOLBOX_LOG_INLINE std::string getTimeStr();

    CPP_GENERIC_TOOLBOX_LOG_INLINE std::string getFctName(const std::string &func_);

  }
}

// internals
#define GTLogBase std::string(__FILENAME__) + ":" + std::to_string(__LINE__) + "/" + GenericToolbox::LoggerUtils::getFctName(__func__) + ": "

// basics
#define GTLogError   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " ERROR " << GTLogBase
#define GTLogAlert   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " ALERT " << GTLogBase
#define GTLogWarning std::cout << GenericToolbox::LoggerUtils::getTimeStr() << "  WARN " << GTLogBase
#define GTLogInfo    std::cout << GenericToolbox::LoggerUtils::getTimeStr() << "  INFO " << GTLogBase
#define GTLogDebug   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " DEBUG " << GTLogBase
#define GTLogTrace   std::cout << GenericToolbox::LoggerUtils::getTimeStr() << " TRACE " << GTLogBase

// conditionals
#define GTLogErrorIf(enable_)   if(enable_) GTLogError
#define GTLogAlertIf(enable_)   if(enable_) GTLogAlert
#define GTLogWarningIf(enable_) if(enable_) GTLogWarning
#define GTLogInfoIf(enable_)    if(enable_) GTLogInfo
#define GTLogDebugIf(enable_)   if(enable_) GTLogDebug
#define GTLogTraceIf(enable_)   if(enable_) GTLogTrace

// throw
#define GTLogThrow(message_) GTLogError << message_ << std::endl; throw std::runtime_error("exception thrown by the logger.")
#define GTLogThrowIf2(condition_, message_) if(condition_){ GTLogThrow( "[" << #condition_ << "] " << message_); }
#define GTLogThrowIf1(condition_) GTLogThrowIf(condition_, "Exception thrown by the logger.");
#define GTLogThrowIf(...) GT_GET_OVERLOADED_MACRO2(__VA_ARGS__, GTLogThrowIf2, GTLogThrowIf1)(__VA_ARGS__)

// debug tools
#define GTDebugVar(var_) GTLogDebug << GET_VAR_NAME_VALUE(var_) << std::endl;


#ifndef CPP_GENERIC_TOOLBOX_LOG_COMPILED
#include "implementation/GenericToolbox.Log.impl.h"
#endif

#endif //GUNDAM_GENERICTOOLBOX_LOG_H
