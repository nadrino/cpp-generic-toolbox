//
// Created by Nadrino on 13/11/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOG_H
#define GUNDAM_GENERICTOOLBOX_LOG_H

#include "GenericToolbox.Macro.h"

#include <iostream>
#include <exception>


// a very simple logger

// internals
#define GTLogBase std::string(__FILE_NAME__) + ":" + std::to_string(__LINE__) + ": "

// basics
#define GTLogError   std::cout << "[ERROR] " << GTLogBase
#define GTLogAlert   std::cout << "[ALERT] " << GTLogBase
#define GTLogWarning std::cout << "[ WARN] " << GTLogBase
#define GTLogInfo    std::cout << "[ INFO] " << GTLogBase
#define GTLogDebug   std::cout << "[DEBUG] " << GTLogBase
#define GTLogTrace   std::cout << "[TRACE] " << GTLogBase

// throw
#define GTLogThrow(message_) GTLogError << message_ << std::endl; throw std::runtime_error("exception thrown by the logger.")
#define GTLogThrowIf(condition_, message_) if(condition_){ GTLogThrow( "[" << #condition_ << "] " << message_); }

// debug tools
#define GTDebugVar(var_) GTLogDebug << GET_VAR_NAME_VALUE(var_) << std::endl;


#endif //GUNDAM_GENERICTOOLBOX_LOG_H
