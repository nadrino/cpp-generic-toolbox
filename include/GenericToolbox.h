//
// Created by Adrien BLANCHET on 28/08/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H

#include <string>
#include <vector>
#include <cmath>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <fstream>

// Index
namespace GenericToolbox{

  // string management
  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_ = false);
  std::string toLowerCase(std::string& inputStr_);
  std::string removeExtraDoubledCharacters(std::string inputStr_, std::string doubledChar_);
  std::string joinVectorString(std::vector<std::string> string_list_, std::string delimiter_, int begin_index_ = 0, int end_index_ = 0);
  std::string replaceSubstringInString(std::string input_str_, std::string substr_to_look_for_, std::string substr_to_replace_);
  std::vector<std::string> splitString(std::string inputString_, std::string delimiter_);
  template<typename ... Args> std::string formatString( std::string format, Args ... args );

  // IO tools
  bool doesPathIsFile(std::string filePath_);
  bool doesPathIsFolder(std::string folderPath_);
  bool doesFilePathHasExtension(std::string filePath_, std::string ext_);
  bool mkdirPath(std::string newFolderPath_);
  std::string getCurrentWorkingDirectory();
  std::string dumpFileAsString(std::string filePath_);
  std::vector<std::string> dumpFileAsVectorString(std::string filePath_);

}


// Implementation
namespace GenericToolbox{

  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_){
    if(substring_.size() > string_.size()) return false;
    if(ignoreCase_){
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    if(string_.find(substring_) != std::string::npos) return true;
    else return false;
  }
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_){
    if(substring_.size() > string_.size()) return false;
    if(ignoreCase_){
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    return (not string_.compare(0, substring_.size(), substring_));
  }
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_){
    if(substring_.size() > string_.size()) return false;
    if(ignoreCase_){
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    return (not string_.compare(string_.size() - substring_.size(), substring_.size(), substring_));
  }
  std::string toLowerCase(std::string& inputStr_){
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }
  std::string removeExtraDoubledCharacters(std::string inputStr_, std::string doubledChar_){
    std::vector<std::string> substr_list = splitString(inputStr_, doubledChar_);
    std::vector<std::string> cleaned_substr_list;
    for(int i_substr = 0 ; i_substr < int(substr_list.size()) ; i_substr++){
      if(not substr_list[i_substr].empty())
        cleaned_substr_list.emplace_back(substr_list[i_substr]);
    }
    std::string cleaned_input_str;
    if(doesStringStartsWithSubstring(inputStr_, doubledChar_)) cleaned_input_str += doubledChar_;
    cleaned_input_str += joinVectorString(cleaned_substr_list, doubledChar_);
    if(doesStringEndsWithSubstring(inputStr_, doubledChar_)) cleaned_input_str += doubledChar_;
    return cleaned_input_str;
  }
  std::string joinVectorString(std::vector<std::string> string_list_, std::string delimiter_, int begin_index_, int end_index_) {

    std::string joined_string;
    if(end_index_ == 0) end_index_ = int(string_list_.size());

    // circular permutation -> python style : tab[-1] = tab[tab.size - 1]
    if(end_index_ < 0 and int(string_list_.size()) > std::fabs(end_index_))
      end_index_ = int(string_list_.size()) + end_index_;

    for(int i_list = begin_index_ ; i_list < end_index_ ; i_list++){
      if(not joined_string.empty()) joined_string += delimiter_;
      joined_string += string_list_[i_list];
    }

    return joined_string;
  }
  std::string replaceSubstringInString(std::string input_str_, std::string substr_to_look_for_, std::string substr_to_replace_){
    std::string stripped_str = input_str_;
    size_t index = 0;
    while ((index = stripped_str.find(substr_to_look_for_, index)) != std::string::npos) {
      stripped_str.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
    return stripped_str;
  }
  std::vector<std::string> splitString(std::string inputString_, std::string delimiter_){

    std::vector<std::string> output_splited_string;

    const char *src = inputString_.c_str();
    const char *next = src;

    std::string out_string_piece;

    while ((next = std::strstr(src, delimiter_.c_str())) != nullptr) {
      out_string_piece = "";
      while (src != next){
        out_string_piece += *src++;
      }
      output_splited_string.emplace_back(out_string_piece);
      /* Skip the delimiter_ */
      src += delimiter_.size();
    }

    /* Handle the last token */
    out_string_piece = "";
    while (*src != '\0')
      out_string_piece += *src++;

    output_splited_string.emplace_back(out_string_piece);

    return output_splited_string;

  }

  bool doesPathIsFile(std::string filePath_){
    struct stat info{};
    stat(filePath_.c_str(), &info);
    return S_ISREG(info.st_mode);
  }
  bool doesPathIsFolder(std::string folderPath_){
    struct stat info{};
    stat( folderPath_.c_str(), &info );
    return bool(S_ISDIR(info.st_mode));
  }
  bool doesFilePathHasExtension(std::string filePath_, std::string ext_){
    auto splited = splitString(filePath_, ".");
    if(splited[splited.size()-1] == ext_) return true;
    return false;
  }
  bool mkdirPath(std::string newFolderPath_){
    bool result = false;
    if(doesPathIsFolder(newFolderPath_)) return true;

    std::string current_level;
    std::string level;
    std::stringstream ss(newFolderPath_);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')){
      current_level += level; // append folder to the current level
      if(current_level.empty()) current_level = "/";
      current_level = removeExtraDoubledCharacters(current_level, "/");
      // create current level
      if(not doesPathIsFolder(current_level)){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;

  }
  std::string getCurrentWorkingDirectory(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string output_cwd(cwd);
    return output_cwd;
  }
  std::string dumpFileAsString(std::string filePath_){
    std::string data;
    if(doesPathIsFile(filePath_)){
      std::ifstream input_file(filePath_.c_str(), std::ios::binary | std::ios::in );
      std::ostringstream ss;
      ss << input_file.rdbuf();
      data = ss.str();
      input_file.close();
    }
    return data;
  }
  std::vector<std::string> dumpFileAsVectorString(std::string filePath_){
    std::vector<std::string> lines;
    if(doesPathIsFile(filePath_)){
      std::string data = GenericToolbox::dumpFileAsString(filePath_);
      lines = GenericToolbox::splitString(data, "\n");
    }
    for(auto& line: lines){
      if(GenericToolbox::doesStringEndsWithSubstring(line, "\r")){
        line = line.substr(0, line.size()-1);
      }
    }

    return lines;
  }


}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_H
