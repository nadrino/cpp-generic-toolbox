//
// Created by Nadrino on 13/11/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOG_H
#define GUNDAM_GENERICTOOLBOX_LOG_H

#include <iostream>
#include <exception>


// a very simple logger

// internals
#define GTLogBase __FILE_NAME__ << ":" << __LINE__ << ": "

// basics
#define GTLogError   std::cout << "ERROR " << GTLogBase
#define GTLogAlert   std::cout << "ALERT " << GTLogBase
#define GTLogWarning std::cout << " WARN " << GTLogBase
#define GTLogInfo    std::cout << " INFO " << GTLogBase
#define GTLogDebug   std::cout << "DEBUG " << GTLogBase
#define GTLogTrace   std::cout << "TRACE " << GTLogBase

// conditions
#define GTLogThrow(message_) GTLogError << message_ << std::endl; throw std::runtime_error("exception thrown by the logger.")
#define GTLogThrowIf(condition_, message_) if(condition_){ GTLogThrow(message_); }



#endif //GUNDAM_GENERICTOOLBOX_LOG_H
