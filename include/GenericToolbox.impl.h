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


// Displaying Tools
namespace GenericToolbox {

  static std::time_t _progressLastDisplayedTimestamp_ = std::time(nullptr);
  static int _lastDisplayedValue_ = -1;
  static std::thread::id _selectedThreadId_ = std::this_thread::get_id(); // get the main thread id

  void displayProgress(int iCurrent_, int iTotal_, std::string title_, bool forcePrint_){
    if(
      std::time(nullptr) - GenericToolbox::_progressLastDisplayedTimestamp_ >= time_t(0.5) // every 0.5 second (10fps)
      or _lastDisplayedValue_ == -1 // never printed before
      or iCurrent_ == 0 // first call
      or forcePrint_ // display every calls
      or iCurrent_ >= iTotal_-1 // last entry
      ){

      if(_selectedThreadId_ != std::this_thread::get_id()) return; // While multithreading, this function is muted

      int percentValue = int(round(double(iCurrent_) / iTotal_ * 100.));
      if(percentValue > 100) percentValue = 100; // sanity check
      if(percentValue < 0) percentValue = 0;
      if(percentValue == GenericToolbox::_lastDisplayedValue_) return; // skipping!

      std::cout << "\r";
      if(not title_.empty()) std::cout << title_ << ": ";
      std::cout << "[" << repeatString("#", percentValue/10) << repeatString(" ", (100-percentValue)/10) << "]" << std::endl;
      std::cout << percentValue << "%";
      if(iCurrent_ < iTotal_-1){
        std::cout << std::flush << "\r";
      }
      else{
        std::cout << std::endl;
      }

      GenericToolbox::_lastDisplayedValue_ = percentValue;
      GenericToolbox::_progressLastDisplayedTimestamp_ = std::time(nullptr);
    }
  }
  template <typename T> void printVector(const std::vector<T>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }

}


//! Vector management
namespace GenericToolbox {

  template <typename T>
  struct identity { typedef T type; };

  template <typename T> bool doesElementIsInVector(typename identity<T>::type element_, const std::vector<T>& vector_){
    for(const auto& element: vector_){
      if(element_ == element) return true;
    }
    return false;
//    return std::any_of(vector_.begin(), vector_.end(), element_);
  }

}


// String Management Tools
namespace GenericToolbox {

  bool doesStringContainsSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
    if (substring_.size() > string_.size()) return false;
    if (ignoreCase_) {
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    if (string_.find(substring_) != std::string::npos) return true;
    else return false;
  }
  bool doesStringStartsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
    if (substring_.size() > string_.size()) return false;
    if (ignoreCase_) {
      string_ = toLowerCase(string_);
      substring_ = toLowerCase(substring_);
    }
    return (not string_.compare(0, substring_.size(), substring_));
  }
  bool doesStringEndsWithSubstring(std::string string_, std::string substring_, bool ignoreCase_) {
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
  std::string repeatString(const std::string inputStr_, int amount_){
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }
  std::string removeExtraDoubledCharacters(const std::string &inputStr_, std::string doubledChar_) {
    std::vector<std::string> substr_list = splitString(inputStr_, doubledChar_);
    std::vector<std::string> cleaned_substr_list;
    for (int i_substr = 0; i_substr < int(substr_list.size()); i_substr++) {
      if (not substr_list[i_substr].empty())
        cleaned_substr_list.emplace_back(substr_list[i_substr]);
    }
    std::string cleaned_input_str;
    if (doesStringStartsWithSubstring(inputStr_, doubledChar_)) cleaned_input_str += doubledChar_;
    cleaned_input_str += joinVectorString(cleaned_substr_list, doubledChar_);
    if (doesStringEndsWithSubstring(inputStr_, doubledChar_)) cleaned_input_str += doubledChar_;
    return cleaned_input_str;
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
  std::vector<std::string> getListOfEntriesInFolder(std::string folderPath_) {

    std::vector<std::string> entries_list;
    if(not doesPathIsFolder(folderPath_)) return entries_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return entries_list;
    }
    else {
      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
        entries_list.emplace_back(entry->d_name);
      }
      closedir(directory);
      return entries_list;
    }

  }
  std::vector<std::string> getListOfSubfoldersInFolder(std::string folderPath_) {
    std::vector<std::string> subfolders_list;
    if(not doesPathIsFolder(folderPath_)) return subfolders_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return subfolders_list;
    } else {
      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
        std::string folder_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFolder(folder_candidate)){
          subfolders_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return subfolders_list;
    }

  }
  std::vector<std::string> getListOfFilesInFolder(std::string folderPath_){
    std::vector<std::string> files_list;
    if(not doesPathIsFolder(folderPath_)) return files_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return files_list;
    } else {
      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
        std::string file_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFile(file_candidate)){
          files_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return files_list;
    }
  }

  // -- with direct IO dependencies
  bool doesFolderIsEmpty(std::string folderPath_){
    if(not doesPathIsFolder(folderPath_)) return false;
    return getListOfEntriesInFolder(folderPath_).empty();
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
        output_file_paths.emplace_back(removeExtraDoubledCharacters(relative_subfile_path, "/"));
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
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>

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

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
