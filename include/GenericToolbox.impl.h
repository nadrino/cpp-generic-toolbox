//
// Created by Nadrino on 28/08/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H

#include <utility>
#include <cmath>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <thread>
#include <numeric>


// Displaying Tools
namespace GenericToolbox {

  void displayProgressBar(int iCurrent_, int iTotal_, std::string title_, bool forcePrint_){
    if(
      std::time(nullptr) - GenericToolbox::ProgressBar::progressLastDisplayedTimestamp >= time_t(0.5) // every 0.5 second (10fps)
      or GenericToolbox::ProgressBar::lastDisplayedValue == -1 // never printed before
      or iCurrent_ == 0 // first call
      or forcePrint_ // display every calls
      or iCurrent_ >= iTotal_-1 // last entry (mandatory to print endl)
      ){

      if(GenericToolbox::ProgressBar::_selectedThreadId_ != std::this_thread::get_id()) return; // While multithreading, this function is muted

      if( iCurrent_ >= iTotal_-1 and GenericToolbox::ProgressBar::lastDisplayedValue == 100 ){ // last has already been printed ?
        return;
      }

      int percentValue = int(round(double(iCurrent_) / iTotal_ * 100.));

      if(not (iCurrent_ >= iTotal_-1)){ // if not last entry
        if(percentValue > 100) percentValue = 100; // sanity check
        if(percentValue < 0) percentValue = 0;
        if(percentValue == GenericToolbox::ProgressBar::lastDisplayedValue) return; // skipping!
      }
      else{
          percentValue = 100; // force to be 100
      }

      std::stringstream ss;
      ss << "\r";

      int displayedBarLength = GenericToolbox::ProgressBar::barLength;
      std::string displayedTitle = title_;

      // test if the bar is too wide wrt the prompt width
      if(GenericToolbox::getTerminalWidth() != 0){ // terminal width is measurable

        int totalLength = 0;
        std::string strippedTitle = GenericToolbox::stripStringUnicode(title_); // remove special chars and colors
        totalLength += strippedTitle.size();
        if(not title_.empty()) totalLength += 1; // after title space
        if(displayedBarLength > 0){
          totalLength += 2; // []
          totalLength += displayedBarLength;
        }
        totalLength += 1 + 3 + 1; // space, 100, %
        totalLength += 1; // one extra free char is needed to flush

        int extraCharsCount = (totalLength - GenericToolbox::getTerminalWidth());
        if(extraCharsCount > 0 and displayedBarLength > 0){
          // first, reduce the bar width
          // 12 arbitrary chosen as the minimal barWidth
          displayedBarLength = std::max(GenericToolbox::ProgressBar::barLength - extraCharsCount, 12);
          extraCharsCount -= GenericToolbox::ProgressBar::barLength - displayedBarLength;
        }
        if(extraCharsCount > 0){
          // if it's still to big, cut the title
          displayedTitle = title_.substr(0, strippedTitle.size() - extraCharsCount - 3);
          displayedTitle += "...";
        }
      }

      if(not displayedTitle.empty()){
        ss << displayedTitle << " ";
      }

      if(displayedBarLength > 0){

        int nbTags   = percentValue * displayedBarLength / 100;
        int nbSpaces = displayedBarLength - nbTags;
        ss << "[";
        if(not GenericToolbox::ProgressBar::enableRainbowProgressBar){
          int iCharTagIndex = 0;
          for(int iTag = 0 ; iTag < nbTags ; iTag++){
            ss << GenericToolbox::ProgressBar::fillTag[iCharTagIndex];
            iCharTagIndex++;
            if(iCharTagIndex >= GenericToolbox::ProgressBar::fillTag.size()) iCharTagIndex = 0;
          }
        }
        else{
          int nbTagsCredits = nbTags;
          int nbColors = GenericToolbox::ProgressBar::rainbowColorList.size();
          int nbTagsPerColor = displayedBarLength/nbColors;
          if( displayedBarLength % nbColors != 0) nbTagsPerColor++;
          int iCharTagIndex = 0;
          for(int iColor = 0 ; iColor < nbColors ; iColor++ ){
            ss << GenericToolbox::ProgressBar::rainbowColorList[iColor];
            if(nbTagsCredits == 0) break;
            int iSlot = 0;
            while( nbTagsCredits != 0 and iSlot < nbTagsPerColor ){
              ss << GenericToolbox::ProgressBar::fillTag[iCharTagIndex];
              iCharTagIndex++;
              if(iCharTagIndex >= GenericToolbox::ProgressBar::fillTag.size()) iCharTagIndex = 0;
              nbTagsCredits--;
              iSlot++;
            }
          }
          ss << "\033[0m";
        }
        ss << repeatString(" ", nbSpaces) << "]";
      }

      ss << " " << percentValue << "%";

      std::cout << ss.str();

      if(percentValue == 100){
        std::cout << std::endl;
      }
      else{
        std::cout << std::flush << "\r";
      }

      GenericToolbox::ProgressBar::lastDisplayedValue = percentValue;
      GenericToolbox::ProgressBar::progressLastDisplayedTimestamp = std::time(nullptr);
    }
  }
  template <typename T> std::string getVectorAsString(const std::vector<T>& vector_){
      std::stringstream ss;
      ss << "{ ";
      bool isFirst = true;
      for(const auto& element: vector_){
          if(not isFirst) ss << ", ";
          else isFirst = false;
          ss << element;
      }
      ss << " }";
      return ss.str();
  }
  template <typename T> void printVector(const std::vector<T>& vector_){
    std::cout << getVectorAsString(vector_) << std::endl;
  }

}


//! Vector management
namespace GenericToolbox {

  template <typename T> bool doesElementIsInVector(T element_, const std::vector<T>& vector_){
    return std::find(vector_.begin(), vector_.end(), element_) != vector_.end();
  }
  bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_){
    return std::find(vector_.begin(), vector_.end(), element_) != vector_.end();
  }
  template <typename T> std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(T&, T&)>& compareLambda_ ){
    std::vector<size_t> p(vectorToSort_.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
              [&](size_t i, size_t j){ return compareLambda_(vectorToSort_[i], vectorToSort_[j]); });
    return p;
  }
  template <typename T> std::vector<T> applyPermutation(const std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_ ){
    std::vector<T> sorted_vec(vectorToPermute_.size());
    std::transform(sortPermutation_.begin(), sortPermutation_.end(), sorted_vec.begin(),
                   [&](std::size_t i){ return vectorToPermute_[i]; });
    return sorted_vec;
  }

}


// String Management Tools
namespace GenericToolbox {

  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
    if (substring_.empty()) return true;
    if (substring_.size() > string_.size()) return false;
    if (ignoreCase_) {
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    if (string_.find(substring_) != std::string::npos) return true;
    else return false;
  }
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
    if (substring_.empty()) return true;
    if (substring_.size() > string_.size()) return false;
    if (ignoreCase_) {
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    return (not string_.compare(0, substring_.size(), substring_));
  }
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
    if (substring_.empty()) return true;
    if (substring_.size() > string_.size()) return false;
    if (ignoreCase_) {
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    return (not string_.compare(string_.size() - substring_.size(), substring_.size(), substring_));
  }
  std::string toLowerCase(const std::string &inputStr_) {
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }
  std::string stripStringUnicode(const std::string &inputStr_){
    std::string outputStr(inputStr_);

    if(GenericToolbox::doesStringContainsSubstring(outputStr, "\033")){
      // remove color
      std::string tempStr;
      auto splitOuputStr = GenericToolbox::splitString(outputStr, "\033");
      for(const auto& sliceStr : splitOuputStr){
        if(sliceStr.empty()) continue;
        if(tempStr.empty()){
          tempStr = sliceStr;
          continue;
        }
        // look for a 'm' char that determines the end of the color code
        bool mCharHasBeenFound = false;
        for(const char& c : sliceStr){
          if(not mCharHasBeenFound){
            if(c == 'm'){
              mCharHasBeenFound = true;
            }
          }
          else{
            tempStr += c;
          }
        }
      }
      outputStr = tempStr;
    }

    outputStr.erase(
      remove_if(
        outputStr.begin(), outputStr.end(),
        [](const char& c){return !isprint( static_cast<unsigned char>( c ) );}
      ),
      outputStr.end()
    );




    return outputStr;
  }
  std::string repeatString(const std::string inputStr_, int amount_){
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }
  std::string removeRepeatedCharacters(const std::string &inputStr_, std::string repeatedChar_) {
    std::string outStr = inputStr_;
    std::string oldStr;
    std::string repeatedCharTwice = repeatedChar_;
    repeatedCharTwice += repeatedChar_;
    while(oldStr != outStr){
      oldStr = outStr;
      outStr = GenericToolbox::replaceSubstringInString(outStr, repeatedCharTwice, repeatedChar_);
    }
    return outStr;
  }
  std::string joinVectorString(const std::vector<std::string> &string_list_, std::string delimiter_, int begin_index_, int end_index_) {

    std::string joined_string;
    if (end_index_ == 0) end_index_ = int(string_list_.size());

    // circular permutation -> python style : tab[-1] = tab[tab.size - 1]
    if (end_index_ < 0 and int(string_list_.size()) > std::fabs(end_index_))
      end_index_ = int(string_list_.size()) + end_index_;

    for (int i_list = begin_index_; i_list < end_index_; i_list++) {
      if (not joined_string.empty()) joined_string += delimiter_;
      joined_string += string_list_[i_list];
    }

    return joined_string;
  }
  std::string replaceSubstringInString(const std::string &input_str_, std::string substr_to_look_for_, std::string substr_to_replace_) {
    std::string stripped_str = input_str_;
    size_t index = 0;
    while ((index = stripped_str.find(substr_to_look_for_, index)) != std::string::npos) {
      stripped_str.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
    return stripped_str;
  }
  std::string parseSizeUnits(unsigned int sizeInBytes_){
    if(sizeInBytes_ / 1024 == 0 ){ // print in bytes
      return std::to_string(sizeInBytes_) + " B";
    }
    sizeInBytes_ = sizeInBytes_ / 1024; // in KB
    if(sizeInBytes_ / 1024 == 0){ // print in KB
      return std::to_string(sizeInBytes_) + " KB";
    }
    sizeInBytes_ = sizeInBytes_ / 1024; // in MB
    if(sizeInBytes_ / 1024 == 0){ // print in MB
      return std::to_string(sizeInBytes_) + " MB";
    }
    sizeInBytes_ = sizeInBytes_ / 1024; // in GB
    return std::to_string(sizeInBytes_) + " GB";
  }
  std::vector<std::string> splitString(const std::string &inputString_, std::string delimiter_) {

    std::vector<std::string> output_splited_string;

    const char *src = inputString_.c_str();
    const char *next = src;

    std::string out_string_piece;

    while ((next = std::strstr(src, delimiter_.c_str())) != nullptr) {
      out_string_piece = "";
      while (src != next) {
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
  template<typename ... Args> std::string formatString(std::string format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }

}


// Conversion Tools
namespace GenericToolbox {

  bool toBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
  }

}


// FS Tools
namespace GenericToolbox{

  // -- without IO dependencies
  bool doesFilePathHasExtension(const std::string &filePath_, std::string ext_){
    return doesStringEndsWithSubstring(filePath_, "."+ext_);
  }
  std::string getFolderPathFromFilePath(const std::string &filePath_){
    std::string folder_path;
    if(filePath_[0] == '/') folder_path += "/";
    auto splitted_path = splitString(filePath_, "/");
    folder_path += joinVectorString(
      splitted_path,
      "/",
      0,
      int(splitted_path.size()) - 1
    );
    return folder_path;
  }

  // -- with direct IO dependencies
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
  bool doFilesAreTheSame(std::string filePath1_, std::string filePath2_){

    if( not doesPathIsFile(filePath1_) ) return false;
    if( not doesPathIsFile(filePath2_) ) return false;

    std::ifstream fileStream1(filePath1_);
    std::ifstream fileStream2(filePath2_);

    // Buffer size 1 Megabyte (or any number you like)
    size_t buffer_size = 1<<20;
    char *buffer1 = new char[buffer_size];
    char *buffer2 = new char[buffer_size];

    std::hash<std::string> hashBuffer1;
    std::hash<std::string> hashBuffer2;

    while (fileStream1 and fileStream2) {
      // Try to read next chunk of data
      fileStream1.read(buffer1, buffer_size);
      fileStream2.read(buffer2, buffer_size);

      // Get the number of bytes actually read
      if(fileStream1.gcount() != fileStream2.gcount()){
        return false;
      }

      size_t count = fileStream1.gcount();
      // If nothing has been read, break
      if( count == 0 ){
        break;
      }

      // Compare hash files
      if(hashBuffer1(buffer1) != hashBuffer2(buffer2))
        return false;

    }

    delete[] buffer1;
    delete[] buffer2;

    return true;
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
      current_level = removeRepeatedCharacters(current_level, "/");
      // create current level
      if(not doesPathIsFolder(current_level)){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;

  }
  bool deleteFile(std::string filePath_){
    if(not doesPathIsFile(filePath_)) return true;
    std::remove(filePath_.c_str());
    return not doesPathIsFile(filePath_);
  }
  bool copyFile(std::string source_file_path_, std::string destination_file_path_, bool force_){

    if( not doesPathIsFile(source_file_path_) ){
      return false;
    }

    if( doesPathIsFile(destination_file_path_) ){
      if( force_ ){
        deleteFile(destination_file_path_);
      }
      else{
        return false;
      }
    }

    std::ifstream  src(source_file_path_, std::ios::binary);
    std::ofstream  dst(destination_file_path_,   std::ios::binary);

    dst << src.rdbuf();

    return true;
  }
  bool mvFile(std::string sourceFilePath_, std::string destinationFilePath_, bool force_) {

    if( not doesPathIsFile(sourceFilePath_) ){
      return false;
    }

    if( doesPathIsFile(destinationFilePath_) ){
      if(force_){
        deleteFile(destinationFilePath_);
      }
      else{
        return false;
      }
    }
    else{
      std::string destination_folder_path = getFolderPathFromFilePath(destinationFilePath_);
      if(not doesPathIsFile(destination_folder_path)){
        mkdirPath(destination_folder_path);
      }
    }

    std::rename(sourceFilePath_.c_str(), destinationFilePath_.c_str());

    if(
      doesPathIsFile(destinationFilePath_)
      and not doesPathIsFile(sourceFilePath_)
      ) return true;
    else return false;
  }
  size_t getHashFile(std::string filePath_) {
    std::hash<std::string> hashString;
    return hashString(dumpFileAsString(filePath_));
  }
  long int getFileSizeInBytes(const std::string &filePath_){
    long int output_size = 0;
    if(doesPathIsFile(filePath_)){
      std::ifstream testFile(filePath_.c_str(), std::ios::binary);
      const auto begin = testFile.tellg();
      testFile.seekg (0, std::ios::end);
      const auto end = testFile.tellg();
      const auto fsize = (end-begin);
      output_size = fsize;
    }
    return output_size;
  }
  std::string getCurrentWorkingDirectory(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string output_cwd(cwd);
    return output_cwd;
  }
  void dumpStringInFile(std::string outFilePath_, std::string stringToWrite_){
    std::ofstream out(outFilePath_.c_str());
    out << stringToWrite_;
    out.close();
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
  std::vector<std::string> getListOfEntriesInFolder(std::string folderPath_, std::string entryNameRegex_) {

    std::vector<std::string> entries_list;
    if(not doesPathIsFolder(folderPath_)) return entries_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return entries_list;
    }
    else {

      std::vector<std::string> nameElements;
      if(not entryNameRegex_.empty()){
        nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
      }
      bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
        isValid = true;

        if(std::string(entry->d_name) == "." or std::string(entry->d_name) == ".."){
          isValid = false;
        }
        else if(not entryNameRegex_.empty()){
          std::string entryCandidate = entry->d_name;

          for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

            if( iElement == 0 ){
              if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                isValid = false;
                break;
              }
            }
            else if( iElement+1 == int(nameElements.size()) ){
              if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                isValid = false;
              }
            }
            else{
              if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                  ){
                isValid = false;
                break;
              }
            }

            if( iElement+1 != int(nameElements.size()) ){
              entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
            }
          }
        }
        if(isValid) entries_list.emplace_back(entry->d_name);
      }
      closedir(directory);
      return entries_list;
    }

  }
  std::vector<std::string> getListOfSubfoldersInFolder(std::string folderPath_, std::string entryNameRegex_) {
    std::vector<std::string> subfolders_list;
    if(not doesPathIsFolder(folderPath_)) return subfolders_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return subfolders_list;
    } else {

        std::vector<std::string> nameElements;
        if(not entryNameRegex_.empty()){
            nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
        }
        bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
          isValid = true;
        std::string folder_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFolder(folder_candidate)){
            if(std::string(entry->d_name) == "." or std::string(entry->d_name) == ".."){
                isValid = false;
            }
            else if(not entryNameRegex_.empty()){
              std::string entryCandidate = entry->d_name;

              for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

                if( iElement == 0 ){
                  if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                    break;
                  }
                }
                else if( iElement+1 == int(nameElements.size()) ){
                  if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                  }
                }
                else{
                  if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                      ){
                    isValid = false;
                    break;
                  }
                }

                if( iElement+1 != int(nameElements.size()) ){
                  entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
                }
              }
            }
            if(isValid) subfolders_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return subfolders_list;
    }

  }
  std::vector<std::string> getListOfFilesInFolder(std::string folderPath_, std::string entryNameRegex_){
    std::vector<std::string> files_list;
    if(not doesPathIsFolder(folderPath_)) return files_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return files_list;
    } else {

        std::vector<std::string> nameElements;
        if(not entryNameRegex_.empty()){
            nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
        }
        bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
          isValid = true;
        std::string file_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFile(file_candidate)){
            if(not entryNameRegex_.empty()){
              std::string entryCandidate = entry->d_name;

              for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

                if( iElement == 0 ){
                  if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                    break;
                  }
                }
                else if( iElement+1 == int(nameElements.size()) ){
                  if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                  }
                }
                else{
                  if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                      ){
                    isValid = false;
                    break;
                  }
                }

                if( iElement+1 != int(nameElements.size()) ){
                  entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
                }
              }
            }
            if(isValid) files_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return files_list;
    }
  }

  // -- with direct IO dependencies
  bool doesFolderIsEmpty(std::string folderPath_){
    if(not doesPathIsFolder(folderPath_)) return false;
    return getListOfEntriesInFolder(folderPath_, std::string()).empty();
  }
  std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_) {

    // WARNING : Recursive function
    std::vector<std::string> output_file_paths = getListOfFilesInFolder(folderPath_);

    auto subfolder_names_list = getListOfSubfoldersInFolder(folderPath_);
    for(auto &subfolder_name : subfolder_names_list){
      std::string subfolder_full_path = folderPath_;
      subfolder_full_path += "/";
      subfolder_full_path += subfolder_name;
      auto subfile_names_list = getListFilesInSubfolders(subfolder_full_path);
      for(auto &subfile_name : subfile_names_list){
        std::string relative_subfile_path;
        relative_subfile_path += subfolder_name;
        relative_subfile_path += "/";
        relative_subfile_path += subfile_name;
        output_file_paths.emplace_back(removeRepeatedCharacters(relative_subfile_path, "/"));
      }
    }

    return output_file_paths;

  }

}



// Hardware Tools
#if defined(_WIN32)
// Windows
#include <windows.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <sys/ioctl.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>
#include <sys/ioctl.h>

#else
// Unsupported
#endif
namespace GenericToolbox{

  size_t getProcessMemoryUsage(){
    /**
     * Returns the current resident set size (physical memory use) measured
     * in bytes, or zero if the value cannot be determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof memCounter))
        return (size_t)memCounter.WorkingSetSize;
    return (size_t)0; /* get process mem info failed */

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size;
    return (size_t)0; /* query failed */

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);

#else
    // AIX, BSD, Solaris, and Unknown OS
    return (size_t)0L;          /* Unsupported. */

#endif
  }
  size_t getProcessMaxMemoryUsage(){
    /**
     * Returns the peak (maximum so far) resident set size (physical
     * memory use) measured in bytes, or zero if the value cannot be
     * determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.PeakWorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size_max;
    return (size_t)0; /* query failed */

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    struct rusage rusage;
    if (!getrusage(RUSAGE_SELF, &rusage))
        return (size_t)rusage.ru_maxrss;
    return (size_t)0; /* query failed */

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    // AIX and Solaris
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);
#else
    // Unknown OS
    return (size_t)0L;          /* Unsupported. */
#endif
  }
  int getTerminalWidth(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    outWith = (int)(csbi.dwSize.X);
//    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    outWith = (int)(w.ws_col);
//    outWith = (int)(w.ws_row);
#endif // Windows/Linux
    return outWith;
  }
  int getTerminalHeight(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//    outWith = (int)(csbi.dwSize.X);
    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
//    outWith = (int)(w.ws_col);
    outWith = (int)(w.ws_row);
#endif // Windows/Linux
    return outWith;
  }

}


// Misc Tools
namespace GenericToolbox{

  std::string getClassName(const std::string& PRETTY_FUNCTION__){
    size_t colons = PRETTY_FUNCTION__.find("::");
    if (colons == std::string::npos)
      return "::";
    size_t begin = PRETTY_FUNCTION__.substr(0,colons).rfind(' ') + 1;
    size_t end = colons - begin;

    return PRETTY_FUNCTION__.substr(begin,end);
  }
  std::string getMethodName(const std::string& PRETTY_FUNCTION__){
    size_t colons = PRETTY_FUNCTION__.find("::");
    size_t begin = PRETTY_FUNCTION__.substr(0,colons).rfind(' ') + 1;
    size_t end = PRETTY_FUNCTION__.rfind('(') - begin;

    return PRETTY_FUNCTION__.substr(begin,end) + "()";
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
