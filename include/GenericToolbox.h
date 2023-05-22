//
// Created by Nadrino on 28/08/2020.
//

#pragma once
#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

// Pre-proc parameters
#include "implementation/GenericToolbox.param.h"


#define HAS_CPP_17 (__cplusplus >= 201703L)
#define HAS_CPP_14 (__cplusplus >= 201300L)
#define HAS_CPP_11 (__cplusplus >= 201103L)


#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <chrono>
#include <map>
#include "list"

#define USE_FILESYSTEM 0
#if HAS_CPP_17 && USE_FILESYSTEM
#include "filesystem"
#endif



//! User Parameters
//! (CAVEAT: only set for a given source file)
namespace GenericToolbox{
  namespace Parameters{
    static int _verboseLevel_ = 0;
  }
}


//! Progress bar
namespace GenericToolbox{

  template<typename T, typename TT> inline static void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> inline static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> inline static std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> inline static bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  inline static void resetLastDisplayedValue();

}


//! Conversion Tools
namespace GenericToolbox {

  template<typename T, typename TT> inline static std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true);

}

//! Printout Tools
namespace GenericToolbox {

  namespace ColorCodes{
#ifndef CPP_GENERIC_TOOLBOX_NOCOLOR
    // reset
    static const char* resetColor{"\x1b[0m"};

    // background color
    static const char* redBackground{"\x1b[41m"};
    static const char* greenBackground{"\x1b[42m"};
    static const char* yellowBackground{"\x1b[43m"};
    static const char* blueBackground{"\x1b[44m"};
    static const char* magentaBackground{"\x1b[45m"};
    static const char* cyanBackground{"\x1b[46m"};
    static const char* greyBackground{"\x1b[47m"};

    // text color
    static const char* redText{"\x1b[1;31m"};
    static const char* greenText{"\x1b[1;32m"};
    static const char* yellowText{"\x1b[1;33m"};
    static const char* blueText{"\x1b[1;34m"};
    static const char* magentaText{"\x1b[1;35m"};
    static const char* cyanText{"\x1b[1;36m"};
    static const char* whiteText{"\x1b[1;37m"};

    // lighter text color
    static const char* redLightText{"\x1b[1;91m"};
    static const char* greenLightText{"\x1b[1;92m"};
    static const char* yellowLightText{"\x1b[1;93m"};
    static const char* blueLightText{"\x1b[1;94m"};
    static const char* magentaLightText{"\x1b[1;95m"};
    static const char* cyanLightText{"\x1b[1;96m"};
    static const char* whiteLightText{"\x1b[1;97m"};
    static const char* greyText{"\x1b[1;90m"};
#else
    // reset
    static const char* resetColor{};

    // background color
    static const char* redBackground{};
    static const char* greenBackground{};
    static const char* yellowBackground{};
    static const char* blueBackground{};
    static const char* magentaBackground{};
    static const char* cyanBackground{};
    static const char* greyBackground{};

    // text color
    static const char* redText{};
    static const char* greenText{};
    static const char* yellowText{};
    static const char* blueText{};
    static const char* magentaText{};
    static const char* cyanText{};
    static const char* whiteText{};

    static const char* redLightText{};
    static const char* greenLightText{};
    static const char* yellowLightText{};
    static const char* blueLightText{};
    static const char* magentaLightText{};
    static const char* cyanLightText{};
    static const char* whiteLightText{};
    static const char* greyText{};
#endif
    static const std::array<const char*, 6> rainbowColorList{redText, greenText, yellowText, blueText, magentaText, cyanText};
  }


  inline static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");
  inline static std::string parseIntAsString(int intToFormat_);
  inline static std::string highlightIf(bool condition_, const std::string& text_);
  inline static std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_ = true);

}


//! Vector management
namespace GenericToolbox{

  // Content management
  template <typename T> inline static bool doesElementIsInVector( const T& element_, const std::vector<T>& vector_ );
  inline static bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_);
  template <typename Elm, typename Val, typename Lambda> inline static int doesElementIsInVector(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_);
  template <typename T> inline static int findElementIndex( const T& element_, const std::vector<T>& vector_ );
  inline static int findElementIndex(const char* element_, const std::vector<std::string>& vector_ );
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> inline static void addIfNotInVector(const T& element_, std::vector<T> &vector_);
  inline static void addIfNotInVector(const char* element_, std::vector<std::string> &vector_);
  template <typename Elm, typename Val, typename Lambda> inline static int findElementIndex(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_);

  // Generators
  template<typename T> inline static std::vector<size_t> indices(const std::vector<T> &vector_);
  template<typename T> inline static std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ = -1 );
  template<typename T, typename TT> static inline std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ );

  // Printout / to string conversions
  template<typename T> static inline void printVector(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_=true);
  template<typename T> static inline std::string parseVectorAsString(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_=true);
  static inline std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_ = false, bool indentLine_=true);

  // Stats
  template <typename T> static inline double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_ = [](const T& var){return var;});
  template<typename T> static inline double getAveragedSlope(const std::vector<T> &yValues_);
  template<typename T, typename TT> static inline double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_);
  template <typename T> static inline double getStdDev(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_ = [](const T& var){return var;});

  // Sorting
  template <typename T, typename Lambda> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_ );
  template <typename T> static inline std::vector<T> getSortedVector(const std::vector<T>& unsortedVector_, const std::vector<std::size_t>& sortPermutation_);
  template <typename T> static inline void applyPermutation(std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_);
  template <typename T, typename Lambda> static inline void sortVector(std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_);
  template <typename T, typename Lambda> static inline void removeEntryIf(std::vector<T>& vector_, const Lambda& removeIfFct_);

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
  template <typename T1, typename T2, typename T3> static inline std::string parseMapAsString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_ = true);
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
  static inline std::string repeatString(const std::string &inputStr_, int amount_);
  static inline std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  static inline std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar = ' ');
  static inline std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  static inline std::string removeRepeatedCharacters(const std::string& inputStr_, const std::string &repeatedChar_);
  template<typename T> static inline std::string joinVectorString(const std::vector<T> &stringList_, const std::string &delimiter_, int beginIndex_ = 0, int endIndex_ = 0);
  template<typename... Args> static inline std::string joinAsString(const std::string &delimiter_, const Args&... args);
  static inline std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static inline std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_);
  static inline std::vector<std::string> splitString(const std::string& inputString_, const std::string &delimiter_, bool removeEmpty_ = false);

  // -- Transformations (Fast)
  static inline void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static inline void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  static inline void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  static inline void trimInputString(std::string &inputStr_, const std::string &strToTrim_);

  // -- Parsing
  static inline size_t getPrintSize(const std::string& str_);
  static inline size_t getNLines(const std::string& str_);
  static inline std::string parseDecimalValue(double val_, const std::string& format_="%s%s", bool allowSubOne_ = true);
  static inline std::string parseUnitPrefix(double val_, int maxPadSize_=-1);
  static inline std::string parseSizeUnits(double sizeInBytes_);
  static inline std::string formatString( const std::string& strToFormat_ ); // overrider: make sure this is the one used when no extra args are provided.
  template<typename ... Args> static inline std::string formatString(const std::string& strToFormat_, Args ... args );

  // -- Conversion Tools
  static inline std::string toHex(const void* address_, size_t nBytes_);
  template<typename T> static inline std::string toHex(const T& val_);
  template<typename T> static inline std::string toHexString(T integerVal_, size_t nbDigit_ = 0);
  template<typename T> static inline std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_);
  static inline bool toBool(const std::string& str);

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
  static inline std::string getFileExtension(const std::string& filePath_);
  static inline std::string getFolderPathFromFilePath(const std::string &filePath_);
  static inline std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ = true);
  static inline std::string replaceFileExtension(const std::string& filePath_, const std::string& newExtension_);
  static inline std::string joinPath(const std::vector<std::string>& vec_);
  template<typename... Args> static inline std::string joinPath(const Args&... args_);
  template<typename... Args> static inline std::string joinPath(const std::vector<std::string>& vec_, const Args&... args_);
#if HAS_CPP_17 && USE_FILESYSTEM
  static inline std::filesystem::file_type fileTypeFromDt(int dt_);
#endif

  // -- with direct IO dependencies
  static inline bool doesPathIsValid(const std::string &filePath_);
  static inline bool doesPathIsFile(const std::string &filePath_);
  static inline bool doesPathIsFolder(const std::string &folderPath_);
  static inline bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_);
  static inline bool mkdirPath(const std::string &newFolderPath_);
  static inline bool deleteFile(const std::string &filePath_);
  static inline bool isFolderEmpty(const std::string &dirPath_);
  static inline bool deleteEmptyDirectory(const std::string &dirPath_);
  static inline bool copyFile(const std::string &source_file_path_, const std::string &destination_file_path_, bool force_ = false);
  static inline bool mvFile(const std::string &sourceFilePath_, const std::string &destinationFilePath_, bool force_ = false);
  static inline size_t getHashFile(const std::string &filePath_);
  static inline ssize_t getFileSize(const std::string& path_);
  static inline long int getFileSizeInBytes(const std::string &filePath_);
  static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  static inline std::string dumpFileAsString(const std::string &filePath_);
  static inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_=false);
  static inline std::vector<std::string> getListOfEntriesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "", int type_=-1, size_t maxEntries_ = 0);
  static inline std::vector<std::string> getListOfSubFoldersInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  static inline std::vector<std::string> getListOfFilesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);

  // -- with indirect IO dependencies
  static inline bool doesFolderIsEmpty(const std::string &folderPath_);
  static inline std::vector<std::string> getListOfEntriesInSubFolders(const std::string &folderPath_, int type_ = -1);
  static inline std::vector<std::string> getListOfFilesInSubFolders(const std::string &folderPath_);
  static inline std::vector<std::string> getListOfFoldersInSubFolders(const std::string &folderPath_);

}


//! Hardware Tools
namespace GenericToolbox{

  static inline size_t getProcessMemoryUsage();
  static inline size_t getProcessMaxMemoryUsage();
  static inline double getCpuUsageByProcess();
  static inline long getProcessMemoryUsageDiffSinceLastCall();
  static inline double getFreeDiskSpacePercent( const std::string& path_ );
  static inline unsigned long long getFreeDiskSpace( const std::string& path_ );
  static inline unsigned long long getTotalDiskSpace( const std::string& path_ );
  static inline int getTerminalWidth();
  static inline int getTerminalHeight();
  static inline std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);

  namespace Hardware{
      static size_t lastProcessMemoryUsage = 0;
  }
}


//! Time Tools
namespace GenericToolbox{

  static inline std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_=-1);
  static inline std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static inline std::string getNowDateString(const std::string& dateFormat_="%Y_%m_%d-%H_%M_%S");

}


//! Polymorphism Tools
namespace GenericToolbox{

  template<class Derived, class Base> static inline bool isDerivedFrom(Base* objPtr_); // slow...
  template<class Derived, class Base> static inline bool isDerivedType(Base* objPtr_); // works when Derived is the final type

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


#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

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
