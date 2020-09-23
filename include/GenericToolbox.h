//
// Created by Nadrino on 28/08/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

#include <string>
#include <vector>
#include <thread>

// Index
namespace GenericToolbox{

  namespace Parameters{
    int _verboseLevel_ = 0;
  }

  // Parameters for the progress bar
  namespace ProgressBar{

#ifndef PROGRESS_BAR_ENABLE_RAINBOW
#define PROGRESS_BAR_ENABLE_RAINBOW 0
#endif

#ifndef PROGRESS_BAR_LENGTH
#define PROGRESS_BAR_LENGTH 48
#endif

#ifndef PROGRESS_BAR_FILL_TAG
#define PROGRESS_BAR_FILL_TAG "#"
#endif

    static bool enableRainbowProgressBar = PROGRESS_BAR_ENABLE_RAINBOW;
    static int barLength = PROGRESS_BAR_LENGTH;
    static std::string fillTag = PROGRESS_BAR_FILL_TAG; // multi-char is possible

    static int lastDisplayedValue = -1;
    static std::time_t progressLastDisplayedTimestamp = std::time(nullptr);
    static std::thread::id _selectedThreadId_ = std::this_thread::get_id(); // get the main thread id
    static std::vector<std::string> rainbowColorList = {"\033[1;31m", "\033[1;32m", "\033[1;33m", "\033[1;34m", "\033[1;35m", "\033[1;36m"};
  }

  //! Displaying Tools
  void displayProgressBar(int iCurrent_, int iTotal_, std::string title_ = "", bool forcePrint_ = false);
  template <typename T> std::string getVectorAsString(const std::vector<T>& vector_);
  template <typename T> void printVector(const std::vector<T>& vector_);


  //! Vector management
  template <typename T> bool doesElementIsInVector(T element_, const std::vector<T>& vector_);
  bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_);
  template <typename T, typename Compare> std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, Compare& compareLambda_ );
  template <typename T> std::vector<T> applyPermutation(const std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_ );


  //! String Management Tools
  // -- Transformations
  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  std::string toLowerCase(const std::string &inputStr_);
  std::string stripStringUnicode(const std::string &inputStr_);
  std::string repeatString(const std::string inputStr_, int amount_);
  std::string removeRepeatedCharacters(const std::string& inputStr_, std::string repeatedChar_);
  std::string joinVectorString(const std::vector<std::string> &string_list_, std::string delimiter_, int begin_index_ = 0, int end_index_ = 0);
  std::string replaceSubstringInString(const std::string &input_str_, std::string substr_to_look_for_, std::string substr_to_replace_);
  std::vector<std::string> splitString(const std::string& inputString_, std::string delimiter_);
  // -- Parsing
  std::string parseSizeUnits(unsigned int sizeInBytes_);
  template<typename ... Args> std::string formatString( std::string format, Args ... args );


  //! Conversion Tools
  bool toBool(std::string str);


  //! FS Tools
  // -- without IO dependencies
  bool doesFilePathHasExtension(const std::string &filePath_, std::string ext_);
  std::string getFolderPathFromFilePath(const std::string &filePath_);
  // -- with direct IO dependencies
  bool doesPathIsFile(std::string filePath_);
  bool doesPathIsFolder(std::string folderPath_);
  bool doFilesAreTheSame(std::string filePath1_, std::string filePath2_);
  bool mkdirPath(std::string newFolderPath_);
  bool deleteFile(std::string filePath_);
  bool copyFile(std::string source_file_path_, std::string destination_file_path_, bool force_ = false);
  bool mvFile(std::string sourceFilePath_, std::string destinationFilePath_, bool force_ = false);
  size_t getHashFile(std::string filePath_);
  long int getFileSizeInBytes(const std::string &filePath_);
  std::string getCurrentWorkingDirectory();
  void dumpStringInFile(std::string outFilePath_, std::string stringToWrite_);
  std::string dumpFileAsString(std::string filePath_);
  std::vector<std::string> dumpFileAsVectorString(std::string filePath_);
  std::vector<std::string> getListOfEntriesInFolder(std::string folderPath_, std::string entryNameRegex_ = "");
  std::vector<std::string> getListOfSubfoldersInFolder(std::string folderPath_, std::string entryNameRegex_ = "");
  std::vector<std::string> getListOfFilesInFolder(std::string folderPath_, std::string entryNameRegex_ = "");
  // -- with indirect IO dependencies
  std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_);


  //! Hardware Tools
  size_t getProcessMemoryUsage();
  size_t getProcessMaxMemoryUsage();
  int getTerminalWidth();
  int getTerminalHeight();


  //! Misc Tools
  std::string getClassName(const std::string& PRETTY_FUNCTION__); // When calling this functions, provide __PRETTY_FUNCTION__ macro
#define __CLASS_NAME__ GenericToolbox::getClassName(__PRETTY_FUNCTION__)
  std::string getMethodName(const std::string& PRETTY_FUNCTION__);
#define __METHOD_NAME__ GenericToolbox::getMethodName(__PRETTY_FUNCTION__)

  std::stringstream ssBuffer;
#define GET_VAR_NAME_VALUE(var) ( ( GenericToolbox::ssBuffer << #var" = " << (var) ).str() )
#define GET_VAR_NAME(var) std::string(#var)



}

#include "GenericToolbox.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
