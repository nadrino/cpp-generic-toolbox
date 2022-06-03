//
// Created by Nadrino on 28/08/2020.
//

#pragma once
#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

// Pre-proc parameters
#include "implementation/GenericToolbox.param.h"

#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <chrono>
#include <map>
#include "list"


//! User Parameters
//! (CAVEAT: only set for a given source file)
namespace GenericToolbox{
  namespace Parameters{
    static int _verboseLevel_ = 0;
  }
}


//! Progress bar
namespace GenericToolbox{

  template<typename T, typename TT> static inline void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> static inline std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> static inline std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> static inline bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  static inline void resetLastDisplayedValue();

}


//! Conversion Tools
namespace GenericToolbox {

  template<typename T, typename TT> static inline std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true);

}

//! Printout Tools
namespace GenericToolbox {

  namespace ColorCodes{
#ifndef CPP_GENERIC_TOOLBOX_NOCOLOR
    static const char* redBackground = "\e[41m";
    static const char* yellowBackground = "\033[43m";
    static const char* greyText = "\x1b[90m";
    static const char* resetColor = "\e[0m";
    static const std::vector<std::string> rainbowColorList{"\033[1;31m", "\033[1;32m", "\033[1;33m", "\033[1;34m", "\033[1;35m", "\033[1;36m"};
#else
    static const char* redBackground = "";
    static const char* yellowBackground = "";
    static const char* greyText = "";
    static const char* resetColor = "";
    static const std::vector<std::string> rainbowColorList{"", "", "", "", "", ""};
#endif
  }


  static inline void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");
  static inline std::string parseIntAsString(int intToFormat_);
  static inline std::string highlightIf(bool condition_, const std::string& text_);
  static inline std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_ = true);

}


//! Vector management
namespace GenericToolbox{

  // Content management
  template <typename T> static inline bool doesElementIsInVector( const T& element_, const std::vector<T>& vector_ );
  static inline bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_);
  template <typename T> static inline int findElementIndex( const T& element_, const std::vector<T>& vector_ );
  static inline int findElementIndex(const char* element_, const std::vector<std::string>& vector_ );
  template<typename T> static inline void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> static inline void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_);

  // Generators
  template<typename T> static inline std::vector<size_t> indices(const std::vector<T> &vector_);
  template<typename T> static inline std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ = -1 );
  template<typename T, typename TT> static inline std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ );

  // Printout / to string conversions
  template<typename T> static inline void printVector(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_=true);
  template<typename T> static inline std::string parseVectorAsString(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_=true);
  static inline std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_ = false, bool indentLine_=true);

  // Stats
  template <typename T> static inline double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_ = [](const T& var){return var;});
  template<typename T> static inline double getAveragedSlope(const std::vector<T> &yValues_);
  template<typename T, typename TT> static inline double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_);

  // Sorting
  template <typename T> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T&, const T&)> firstArgGoesFirstFct_ );
  template <typename T> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T, const T)> firstArgGoesFirstFct_ );
  template <typename T> static inline std::vector<T> applyPermutation(const std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_ );

  // Others
  template<typename T, typename TT> static inline T& getListEntry(std::list<T>& list_, TT index_);
  template<typename T, typename TT> static inline const T& getListEntry(const std::list<T>& list_, TT index_);

}


//! Map management
namespace GenericToolbox{

  template <typename K, typename T> static inline bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ );
  template <typename K, typename T> static inline T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ );
  template <typename T1, typename T2> static inline void appendToMap( std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_ = true );
  template <typename T> static inline std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ );
  template <typename T1, typename T2> static inline std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_ = true);
  template <typename T1, typename T2> static inline void printMap(const std::map<T1, T2>& map_, bool enableLineJump_ = true);

}


//! String Management Tools
namespace GenericToolbox{

  // -- Aesthetic
  static inline std::string addUpDownBars(const std::string& str_, bool stripUnicode_ = true);

  // -- Transformations
  static inline bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  static inline bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  static inline bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  static inline std::string toLowerCase(const std::string &inputStr_);
  static inline std::string stripStringUnicode(const std::string &inputStr_);
  static inline std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_ = true, std::vector<std::string>* argBuffer_ = nullptr);
  static inline size_t getPrintSize(const std::string& str_);
  static inline std::string repeatString(const std::string &inputStr_, int amount_);
  static inline std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  static inline std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar = ' ');
  static inline std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  static inline std::string removeRepeatedCharacters(const std::string& inputStr_, const std::string &repeatedChar_);
  static inline std::string joinVectorString(const std::vector<std::string> &string_list_, const std::string &delimiter_, int begin_index_ = 0, int end_index_ = 0);
  static inline std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static inline std::vector<std::string> splitString(const std::string& inputString_, const std::string &delimiter_, bool removeEmpty_ = false);

  // -- Transformations (Fast)
  static inline void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static inline void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  static inline void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");

  // -- Parsing
  static inline std::string parseUnitPrefix(double val_, int maxPadSize_=-1);
  static inline std::string parseSizeUnits(unsigned int sizeInBytes_);
  static inline std::string formatString( const std::string& strToFormat_ ); // overrider: make sure this is the one used when no extra args are provided.
  template<typename ... Args> static inline std::string formatString(const std::string& strToFormat_, Args ... args );

  // -- Conversion Tools
  static inline std::string toHex(const void* address_, size_t nBytes_);
  template<typename T> static inline std::string toHex(const T& val_);
  template<typename T> static inline std::string toHexString(T integerVal_, size_t nbDigit_ = 0);
  template<typename T> static inline std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_);
  static inline bool toBool(std::string str);

}


//! Operating System Tools
namespace GenericToolbox{

  static inline std::string getHomeDirectory();
  static inline std::string getCurrentWorkingDirectory();
  static inline std::string expandEnvironmentVariables(const std::string &filePath_);
  static inline std::string getExecutableName(); // untested on windows platform

}


//! File System Tools
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  static inline bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_);
  static inline std::string getFolderPathFromFilePath(const std::string &filePath_);
  static inline std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ = true);

  // -- with direct IO dependencies
  static inline bool doesPathIsFile(const std::string &filePath_);
  static inline bool doesPathIsFolder(const std::string &folderPath_);
  static inline bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_);
  static inline bool mkdirPath(const std::string &newFolderPath_);
  static inline bool deleteFile(const std::string &filePath_);
  static inline bool copyFile(const std::string &source_file_path_, const std::string &destination_file_path_, bool force_ = false);
  static inline bool mvFile(const std::string &sourceFilePath_, const std::string &destinationFilePath_, bool force_ = false);
  static inline size_t getHashFile(const std::string &filePath_);
  static inline long int getFileSizeInBytes(const std::string &filePath_);
  static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  static inline std::string dumpFileAsString(const std::string &filePath_);
  static inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_);
  static inline std::vector<std::string> getListOfEntriesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");
  static inline std::vector<std::string> getListOfSubfoldersInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");
  static inline std::vector<std::string> getListOfFilesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");

  // -- with indirect IO dependencies
  static inline bool doesFolderIsEmpty(const std::string &folderPath_);
  static inline std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_);

}


//! Hardware Tools
namespace GenericToolbox{

  static inline size_t getProcessMemoryUsage();
  static inline size_t getProcessMaxMemoryUsage();
  static inline double getCpuUsageByProcess();
  static inline long getProcessMemoryUsageDiffSinceLastCall();
  static inline int getTerminalWidth();
  static inline int getTerminalHeight();
  static inline std::string parseTimeUnit(long long nbMicroSec_);
  static inline std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static inline std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);

  namespace Hardware{
      static size_t lastProcessMemoryUsage = 0;
  }
}


//! Misc Tools
namespace GenericToolbox{

  //! Misc Tools
  static inline std::string getClassName(const std::string& PRETTY_FUNCTION_); // When calling this functions, provide __PRETTY_FUNCTION__ macro
  static inline std::string getMethodName(const std::string& PRETTY_FUNCTION_);

  // Not intended to be managed by the user
  namespace Internals{
    static std::map<int, std::chrono::high_resolution_clock::time_point> _lastTimePointMap_;
    static std::map<std::string, std::chrono::high_resolution_clock::time_point> _lastTimePointMapStr_;
  }

}


/* The actual implementation is done under .impl files.
 * For more details, checkout this file
 * */
#include "implementation/GenericToolbox.impl.h"


//! MACROS Tools
#define __CLASS_NAME__ GenericToolbox::getClassName(__PRETTY_FUNCTION__)
//#define __CLASS_NAME__ ( this != nullptr ? typeid(*this).name() )
#define __METHOD_NAME__ GenericToolbox::getMethodName(__PRETTY_FUNCTION__)

#define GET_VAR_NAME_VALUE(var) ( ((std::stringstream&) (std::stringstream() << #var << " = " << (var)) ).str() )
#define GET_VAR_NAME_VALUE_STREAM(var) #var << " = " << var
#define GET_VAR_NAME(var) std::string(#var)

/*
 * ENUM_EXPANDER will create the desired enum and will automatically attach a namespace which provides additional methods
 * For example: "ENUM_EXPAND(MyEnumType, 1, state1, state2, state3)" is equivalent to "enum MyEnumType{state1 = 1, state2, state3};"
 * In addition, you can search of any enum name as a string: "MyEnumTypeEnumNamespace::toString(1)" which will return "state1".
 * */
#define ENUM_EXPANDER(enumName_, intOffset_, v1_, ...) GT_INTERNALS_ENUM_EXPANDER(enumName_, intOffset_, v1_, __VA_ARGS__)

#define BIND_VAR_NAME(var) var, #var
#define BIND_VAR_REF_NAME(var) &(var), #var


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
