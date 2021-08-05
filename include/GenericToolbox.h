//
// Created by Nadrino on 28/08/2020.
//

#pragma once
#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

#include <string>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <chrono>
#include <map>

#include "iostream"
#include "cmath"

// Pre-proc parameters
#include "implementation/GenericToolbox.param.h"


//! User Parameters
//! (CAVEAT: only set for a given source file)
namespace GenericToolbox{
  namespace Parameters{
    static int _verboseLevel_ = 0;
  }
}


//! Progress bar
namespace GenericToolbox{

  template<typename T, typename TT> inline void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> inline std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> inline std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> inline bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  inline void resetLastDisplayedValue();

}


//! Printout Tools
namespace GenericToolbox {

  namespace ColorCodes{
    static const char* redBackGround = "\e[41m";
    static const char* resetColor = "\e[0m";
  }


  inline void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");
  inline std::string parseIntAsString(int intToFormat_);
  inline std::string highlightIf(bool condition_, const std::string& text_);

}


//! Vector management
namespace GenericToolbox{

  template <typename T> inline bool doesElementIsInVector( T element_, const std::vector<T>& vector_ );
  inline bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_);
  template <typename T> inline int findElementIndex(T element_, const std::vector<T>& vector_ );
  inline int findElementIndex(const char* element_, const std::vector<std::string>& vector_ );
  template <typename T> inline double getAverage(const std::vector<T>& vector_);
  template <typename T> std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ = -1 );
  template <typename T, typename TT> inline std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ );
  template <typename T> inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T&, const T&)> compareLambda_ );
  template <typename T> inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T, const T)> compareLambda_ );
  template <typename T> inline std::vector<T> applyPermutation(const std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_ );
  template<typename T> inline void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> inline void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> inline std::string parseVectorAsString(const std::vector<T> &vector_, bool enableLineJump_ = false);
  template<typename T> inline void printVector(const std::vector<T> &vector_, bool enableLineJump_ = false);
  template<typename T> inline std::vector<size_t> indices(const std::vector<T> &vector_);
  template<typename T> inline double getAveragedSlope(const std::vector<T> &yValues_);
  template<typename T, typename TT> inline double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_);

}


//! Map management
namespace GenericToolbox{

  template <typename K, typename T> inline bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ );
  template <typename K, typename T> inline T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ );
  template <typename T1, typename T2> inline void appendToMap( std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_ = true );
  template <typename T> inline std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ );
  template <typename T1, typename T2> inline std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_ = true);
  template <typename T1, typename T2> inline void printMap(const std::map<T1, T2>& map_, bool enableLineJump_ = true);

}


//! String Management Tools
namespace GenericToolbox{

  // -- Transformations
  inline bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  inline bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  inline bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  inline std::string toLowerCase(const std::string &inputStr_);
  inline std::string stripStringUnicode(const std::string &inputStr_);
  inline size_t getPrintSize(const std::string& str_);
  inline std::string repeatString(const std::string &inputStr_, int amount_);
  inline std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  inline std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar = ' ');
  inline std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  inline std::string removeRepeatedCharacters(const std::string& inputStr_, const std::string &repeatedChar_);
  inline std::string joinVectorString(const std::vector<std::string> &string_list_, const std::string &delimiter_, int begin_index_ = 0, int end_index_ = 0);
  inline std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  inline std::vector<std::string> splitString(const std::string& inputString_, const std::string &delimiter_, bool removeEmpty_ = false);

  // -- Transformations (Fast)
  inline void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  inline void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  inline void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");

  // -- Parsing
  inline std::string parseUnitPrefix(double val_, int maxPadSize_=-1);
  inline std::string parseSizeUnits(unsigned int sizeInBytes_);
  inline std::string formatString( const std::string& strToFormat_ ); // overrider: make sure this is the one used when no extra args are provided.
  template<typename ... Args> inline std::string formatString(const std::string& strToFormat_, Args ... args );

  //! Conversion Tools
  template<typename T> inline std::string toHexString(T integerVal_, size_t nbDigit_ = 0);
  inline bool toBool(std::string str);

}


//! Operating System Tools
namespace GenericToolbox{

  inline std::string getHomeDirectory();
  inline std::string getCurrentWorkingDirectory();
  inline std::string expandEnvironmentVariables(const std::string &filePath_);
  inline std::string getExecutableName(); // untested on windows platform

}


//! File System Tools
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  inline bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_);
  inline std::string getFolderPathFromFilePath(const std::string &filePath_);
  inline std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ = true);

  // -- with direct IO dependencies
  inline bool doesPathIsFile(const std::string &filePath_);
  inline bool doesPathIsFolder(const std::string &folderPath_);
  inline bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_);
  inline bool mkdirPath(const std::string &newFolderPath_);
  inline bool deleteFile(const std::string &filePath_);
  inline bool copyFile(const std::string &source_file_path_, const std::string &destination_file_path_, bool force_ = false);
  inline bool mvFile(const std::string &sourceFilePath_, const std::string &destinationFilePath_, bool force_ = false);
  inline size_t getHashFile(const std::string &filePath_);
  inline long int getFileSizeInBytes(const std::string &filePath_);
  inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  inline std::string dumpFileAsString(const std::string &filePath_);
  inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_);
  inline std::vector<std::string> getListOfEntriesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");
  inline std::vector<std::string> getListOfSubfoldersInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");
  inline std::vector<std::string> getListOfFilesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_ = "");

  // -- with indirect IO dependencies
  inline bool doesFolderIsEmpty(const std::string &folderPath_);
  inline std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_);

}


//! Hardware Tools
namespace GenericToolbox{

  inline size_t getProcessMemoryUsage();
  inline size_t getProcessMaxMemoryUsage();
  inline long getProcessMemoryUsageDiffSinceLastCall();
  inline int getTerminalWidth();
  inline int getTerminalHeight();
  inline std::string parseTimeUnit(long long nbMicroSec_);
  inline std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  inline std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  inline long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  inline std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);

  namespace Hardware{
      static size_t lastProcessMemoryUsage = 0;
  }
}


//! Misc Tools
namespace GenericToolbox{

  //! Misc Tools
  inline std::string getClassName(const std::string& PRETTY_FUNCTION_); // When calling this functions, provide __PRETTY_FUNCTION__ macro
  inline std::string getMethodName(const std::string& PRETTY_FUNCTION_);

  // Not intended to be managed by the user
  namespace Internals{
    static std::map<int, std::chrono::high_resolution_clock::time_point> _lastTimePointMap_;
    static std::map<std::string, std::chrono::high_resolution_clock::time_point> _lastTimePointMapStr_;
  }

}


namespace GenericToolbox {

  namespace ProgressBar {

    // Parameters for the progress bar
    static bool debugMode = false;
    static bool enableRainbowProgressBar = PROGRESS_BAR_ENABLE_RAINBOW;
    static bool displaySpeed = PROGRESS_BAR_SHOW_SPEED;
    static int maxBarLength = PROGRESS_BAR_LENGTH;
    static size_t refreshRateInMilliSec = PROGRESS_BAR_REFRESH_DURATION_IN_MS;
    static std::string fillTag(PROGRESS_BAR_FILL_TAG);
    static std::ostream* outputStreamPtr{&std::cout};

    static int lastDisplayedPercentValue{-1};
    static int lastDisplayedValue{-1};
    static double lastDisplayedSpeed{0};
    static auto lastDisplayedTimePoint = std::chrono::high_resolution_clock::now();
    static std::thread::id _selectedThreadId_ = std::this_thread::get_id(); // get the main thread id
    static std::vector<std::string> rainbowColorList{"\033[1;31m", "\033[1;32m", "\033[1;33m", "\033[1;34m",
                                                     "\033[1;35m", "\033[1;36m"};
  }

  template<typename T, typename TT> inline std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){

    int percentValue = int(round(double(iCurrent_) / double(iTotal_) * 100.));
    if( percentValue > 100 ){
      percentValue = 100;
    }
    else if( percentValue < 0 ){
      percentValue = 0;
    }

    std::stringstream ssPrefix;
    if( not title_.empty() ) ssPrefix << title_ << " ";

    std::stringstream ssTail;
    ssTail << GenericToolbox::padString(std::to_string(percentValue), 3, ' ') << "%";

    auto newTimePoint = std::chrono::high_resolution_clock::now();
    if (ProgressBar::displaySpeed) {
      ssTail << " (";
      double itPerSec =
        double(iCurrent_) - ProgressBar::lastDisplayedValue; // nb iterations since last print
        double timeInterval;
        if (int(itPerSec) < 0) itPerSec = 0;
        else {
          timeInterval = double(std::chrono::duration_cast<std::chrono::milliseconds>(
            newTimePoint - ProgressBar::lastDisplayedTimePoint
            ).count()) / 1000.;
          if( timeInterval != 0 ){
            itPerSec /= timeInterval; // Count per s
            ProgressBar::lastDisplayedSpeed = itPerSec;
          }
          else itPerSec = ProgressBar::lastDisplayedSpeed;
        }
        ssTail << GenericToolbox::padString(GenericToolbox::parseIntAsString(int(itPerSec)), 5, ' ');
        ssTail << " it/s)";
    }



    // test if the bar is too wide wrt the prompt width
    int displayedBarLength = ProgressBar::maxBarLength;

    auto termWidth = GenericToolbox::getTerminalWidth();
    if( termWidth != 0 ) { // terminal width is measurable

      size_t totalBarLength = GenericToolbox::getPrintSize(ssPrefix.str());
      if(displayedBarLength > 0) {
        totalBarLength += 2; // []
        totalBarLength += displayedBarLength;
        totalBarLength += 1; // space before tail
      }
      totalBarLength += ssTail.str().size();
      totalBarLength += 1; // 1 extra space is necessary to std::endl

      int remainingSpaces = termWidth;
      remainingSpaces -= int(totalBarLength);

      if( remainingSpaces < 0 ){
        if( displayedBarLength >= 0 ){
          // ok, can take some extra space in the bar
          displayedBarLength -= std::abs(remainingSpaces);
          if (displayedBarLength < 12) {
            displayedBarLength = 0;
            remainingSpaces += 2; // get back the [] of the pBar
          }
          remainingSpaces += (ProgressBar::maxBarLength - displayedBarLength );
        }
      }

      // if it's still to big, cut the title
      if ( remainingSpaces < 0) {
        std::string cutPrefix = ssPrefix.str().substr(0, int(ssPrefix.str().size()) - std::abs(remainingSpaces) - 3);
        ssPrefix.str("");
        ssPrefix << cutPrefix;
        ssPrefix << "\033[0m" << "...";
      }
    }
    else {
      displayedBarLength = 0;
    }

    std::stringstream ssProgressBar;
    ssProgressBar << ssPrefix.str();

    if (displayedBarLength > 0) {

      int nbTags = percentValue * displayedBarLength / 100;
      int nbSpaces = displayedBarLength - nbTags;
      ssProgressBar << "[";
      if (not ProgressBar::enableRainbowProgressBar) {
        int iCharTagIndex = 0;
        for (int iTag = 0; iTag < nbTags; iTag++) {
          ssProgressBar << ProgressBar::fillTag[iCharTagIndex];
          iCharTagIndex++;
          if (iCharTagIndex >= ProgressBar::fillTag.size()) iCharTagIndex = 0;
        }
      }
      else {
        int nbTagsCredits = nbTags;
        int nbColors = int(ProgressBar::rainbowColorList.size());
        int nbTagsPerColor = displayedBarLength / nbColors;
        if (displayedBarLength % nbColors != 0) nbTagsPerColor++;
        int iCharTagIndex = 0;
        for (int iColor = 0; iColor < nbColors; iColor++) {
          ssProgressBar << ProgressBar::rainbowColorList[iColor];
          if (nbTagsCredits == 0) break;
          int iSlot = 0;
          while (nbTagsCredits != 0 and iSlot < nbTagsPerColor) {
            ssProgressBar << ProgressBar::fillTag[iCharTagIndex];
            iCharTagIndex++;
            if (iCharTagIndex >= ProgressBar::fillTag.size()) iCharTagIndex = 0;
            nbTagsCredits--;
            iSlot++;
          }
        }
        ssProgressBar << "\033[0m";
      }
      ssProgressBar << repeatString(" ", nbSpaces) << "] ";
    }

    ssProgressBar << ssTail.str();

    ssProgressBar << std::endl; // always jump line to force flush on screen
    if( percentValue != 100 ){
      // those commands won't be flushed until a new print is called:
      // pull back to cursor on the line of the progress bar
      ssProgressBar << static_cast<char>(27) << "[1;1F";
      // Clear the line and add "\r" since a Logger might intercept it to trigger a print of a line header
      ssProgressBar << static_cast<char>(27) << "[1K" << "\r"; // trick to clear
    }

    ProgressBar::lastDisplayedPercentValue = percentValue;
    ProgressBar::lastDisplayedValue = iCurrent_;
    ProgressBar::lastDisplayedTimePoint = newTimePoint;

    if( ProgressBar::debugMode ){
      std::cout << "New timestamp: " << ProgressBar::lastDisplayedTimePoint.time_since_epoch().count() << std::endl;
      std::cout << "ProgressBar::lastDisplayedValue: " << ProgressBar::lastDisplayedValue << std::endl;
      std::cout << "ProgressBar::lastDisplayedPercentValue: " << ProgressBar::lastDisplayedPercentValue << std::endl;
    }

    return ssProgressBar.str();

  }
  template<typename T, typename TT> inline bool showProgressBar(const T& iCurrent_, const TT& iTotal_){

    //    if( // Only the main thread
    //      ProgressBar::_selectedThreadId_ != std::this_thread::get_id()
    //      ){
    //      return false;
    //    }

    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - ProgressBar::lastDisplayedTimePoint
      ).count();
    if( // REQUIRED TO PRINTOUT
    iCurrent_ == 0 // First call
    or ProgressBar::lastDisplayedPercentValue == -1 // never printed before
    or timeDiff >= ProgressBar::refreshRateInMilliSec
    or iCurrent_ + 1 >= iTotal_ // last entry (mandatory to print at least once: need to print endl)
    ){

      int percent = int(round(double(iCurrent_) / double(iTotal_) * 100.));

      if( percent >= 100 ){ percent = 100; }
      else if( percent < 0) percent = 0;

      if( // EXCLUSION CASES
      percent == ProgressBar::lastDisplayedPercentValue // already printed
      ){
        if( ProgressBar::debugMode ){
          std::cout << "Print PBar NOT Ok:" << std::endl;
          std::cout << "percent == ProgressBar::lastDisplayedPercentValue" << std::endl;
        }
        return false;
      }

      if( ProgressBar::debugMode ){
        std::cout << "Print PBar Ok:" << std::endl;
        std::cout << "percent = " << percent << std::endl;
        std::cout << "iCurrent_ = " << iCurrent_ << std::endl;
        std::cout << "iTotal_ = " << iTotal_ << std::endl;
        std::cout << "ProgressBar::lastDisplayedPercentValue = " << ProgressBar::lastDisplayedPercentValue << std::endl;
        std::cout << "ProgressBar::refreshRateInMilliSec = " << ProgressBar::refreshRateInMilliSec << std::endl;
        std::cout << "timeDiff = " << timeDiff << std::endl;
      }

      // OK!
      return true;
    }

    return false;
  }
  template<typename T, typename TT> inline std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    if(forcePrint_ or showProgressBar(iCurrent_, iTotal_) ){
      return generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
    return {};
  }
  template<typename T, typename TT> inline void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    if(forcePrint_ or GenericToolbox::showProgressBar(iCurrent_, iTotal_) ){
      *ProgressBar::outputStreamPtr << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
  }
  inline void resetLastDisplayedValue(){
    std::cout << "resetLastDisplayedValue" << std::endl;
    ProgressBar::lastDisplayedValue = -1;
    ProgressBar::lastDisplayedPercentValue = -1;
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


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
