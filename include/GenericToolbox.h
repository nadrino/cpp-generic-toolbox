//
// Created by Nadrino on 28/08/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

#include <string>
#include <vector>

// Index
namespace GenericToolbox{


  //! String Management Tools
  // -- Transformations
  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  std::string toLowerCase(const std::string &inputStr_);
  std::string removeExtraDoubledCharacters(const std::string& inputStr_, std::string doubledChar_);
  std::string joinVectorString(const std::vector<std::string> &string_list_, std::string delimiter_, int begin_index_ = 0, int end_index_ = 0);
  std::string replaceSubstringInString(const std::string &input_str_, std::string substr_to_look_for_, std::string substr_to_replace_);
  std::vector<std::string> splitString(const std::string& inputString_, std::string delimiter_);
  // -- Parsing
  std::string parseSizeUnits(unsigned int sizeInBytes_);
  template<typename ... Args> std::string formatString( std::string format, Args ... args );


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
  long int getFileSize(const std::string &filePath_);
  std::string getCurrentWorkingDirectory();
  std::string dumpFileAsString(std::string filePath_);
  std::vector<std::string> dumpFileAsVectorString(std::string filePath_);
  std::vector<std::string> getListOfEntriesInFolder(std::string folderPath_);
  std::vector<std::string> getListOfSubfoldersInFolder(std::string folderPath_);
  std::vector<std::string> getListOfFilesInFolder(std::string folderPath_);
  // -- with indirect IO dependencies
  std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_);


  //! Hardware Tools
  size_t getProcessMemoryUsage();
  size_t getProcessMaxMemoryUsage();


}

#include <GenericToolbox_impl.h>

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
