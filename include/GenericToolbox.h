//
// Created by Adrien BLANCHET on 28/08/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

#include <string>
#include <vector>

// Index
namespace GenericToolbox{

  // string management
  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  std::string toLowerCase(const std::string &inputStr_);
  std::string removeExtraDoubledCharacters(const std::string& inputStr_, std::string doubledChar_);
  std::string joinVectorString(const std::vector<std::string> &string_list_, std::string delimiter_, int begin_index_ = 0, int end_index_ = 0);
  std::string replaceSubstringInString(const std::string &input_str_, std::string substr_to_look_for_, std::string substr_to_replace_);
  std::vector<std::string> splitString(const std::string& inputString_, std::string delimiter_);
  template<typename ... Args> std::string formatString( std::string format, Args ... args );

  // IO tools
  bool doesPathIsFile(std::string filePath_);
  bool doesPathIsFolder(std::string folderPath_);
  bool doesFilePathHasExtension(const std::string &filePath_, std::string ext_);
  bool mkdirPath(std::string newFolderPath_);
  bool deleteFile(std::string file_path_);
  bool copyFile(std::string source_file_path_, std::string destination_file_path_, bool force_ = false);
  bool mvFile(std::string source_file_path_, std::string destination_file_path_, bool force_ = false);
  std::string getFolderPathFromFilePath(std::string file_path_);
  std::string getCurrentWorkingDirectory();
  std::string dumpFileAsString(std::string filePath_);
  std::vector<std::string> dumpFileAsVectorString(std::string filePath_);

  // Hardware Tools
  void process_mem_usage(double& vm_usage, double& resident_set);


}

#include <GenericToolbox_impl.h>

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
