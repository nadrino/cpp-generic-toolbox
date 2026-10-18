#pragma once

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::string &extension_) {
    return endsWith(filePath_, "." + extension_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::vector<std::string> &extensionList_) {
    return std::any_of(extensionList_.begin(), extensionList_.end(), [&](const std::string& extension){ return GenericToolbox::hasExtension(filePath_, extension); });
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getExtension(const std::string& filePath_) {
    if( filePath_.find_last_of('.') == size_t(-1) ) return {};
    return filePath_.substr(filePath_.find_last_of('.') + 1);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFolderPath(const std::string &filePath_) {
    if( filePath_.find_last_of("/\\") == size_t(-1) ) return {};
    return filePath_.substr(0,filePath_.find_last_of("/\\"));
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileName(const std::string &filePath_, bool withExtension_) {
#if HAS_CPP_17 && USE_FILESYSTEM
    std::filesystem::path pathObj(filePath_);
    return ( keepExtension_ ? pathObj.filename().string() : pathObj.stem().string());
#else
    const size_t pos = filePath_.find_last_of("/\\");
    const std::string filename = ( pos != std::string::npos ) ? filePath_.substr(pos + 1) : filePath_;
    return ( withExtension_ or filename.find('.') == std::string::npos ) ? filename : filename.substr(0, filename.find_last_of('.'));
#endif
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string generateCleanFileName(const std::string &fileName_) {
    std::string out{fileName_};

    replaceSubstringInsideInputString(out, " ", "_");
    replaceSubstringInsideInputString(out, "-", "_");
    replaceSubstringInsideInputString(out, "/", "_");
    replaceSubstringInsideInputString(out, "<", "_");

    replaceSubstringInsideInputString(out, ">", "");
    replaceSubstringInsideInputString(out, "(", "");
    replaceSubstringInsideInputString(out, ")", "");
    replaceSubstringInsideInputString(out, "{", "");
    replaceSubstringInsideInputString(out, "}", "");
    replaceSubstringInsideInputString(out, "[", "");
    replaceSubstringInsideInputString(out, "]", "");
    replaceSubstringInsideInputString(out, "#", "");

    return out;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string replaceExtension(const std::string& filePath_, const std::string& newExtension_) {
    if( newExtension_.empty() ) return filePath_.substr(0, filePath_.find_last_of('.'));
    return filePath_.substr(0, filePath_.find_last_of('.')) + "." + newExtension_;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isPathValid(const std::string &filePath_) {
    return ( access( filePath_.c_str(), F_OK ) == 0 );
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isFile(const std::string &filePath_) {
    struct stat info{};
    if( lstat(filePath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISREG(info.st_mode);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isDir(const std::string &dirPath_) {
    struct stat info{};
    if( lstat(dirPath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISDIR(info.st_mode);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool areSameFiles(const std::string &filePath1_, const std::string &filePath2_) {

    if( not isFile(filePath1_) ) return false;
    if( not isFile(filePath2_) ) return false;

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
      fileStream1.read(buffer1, long(buffer_size));
      fileStream2.read(buffer2, long(buffer_size));

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

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isDirEmpty(const std::string &dirPath_) {
    if( not isDir(dirPath_) ){ return false; }
    return lsRecursive(dirPath_).empty();
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool mkdir(const std::string &dirPath_) {
    bool result = false;
    if( isDir(dirPath_) ){ return true; }

    std::string current_level;
    std::string level;
    std::stringstream ss(dirPath_);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')){
      current_level += level; // append folder to the current level
      if(current_level.empty()) current_level = "/";
      current_level = removeRepeatedCharacters(current_level, "/");
      // create current level
      if( not isDir(current_level) ){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool rm(const std::string &filePath_) {
    // not using ::delete as if the specified file path is a symbolic link,
    // it deletes the link and the file or directory that the link refers to

    // ::unlink: If the specified file path is a symbolic link,
    // only the link itself is deleted, not the file or directory that the link refers to
    return ( ::unlink(filePath_.c_str()) == 0 );
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool mv(const std::string &src_, const std::string &destination_, bool force_) {
    if( not isPathValid(src_) ){ return false; }

    if( isFile(destination_) ){
      if(force_){ rm(destination_); }
      else{ return false; }
    }
    else{
      std::string destination_folder_path = getFolderPath(destination_);
      if(not isFile(destination_folder_path)){ mkdir(destination_folder_path); }
    }

    return (std::rename(src_.c_str(), destination_.c_str()) == 0);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool cp(const std::string &src_, const std::string &destination_, bool force_) {
    if( not isFile(src_) ){ return false; }
    if( isFile(destination_) ){
      if( force_ ){ rm(destination_); }
      else{ return false; }
    }

    std::ifstream  src(src_, std::ios::binary);
    std::ofstream  dst(destination_, std::ios::binary);

    dst << src.rdbuf();

    return true;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool rmDir(const std::string &dirPath_) {
    return (::rmdir(dirPath_.c_str()) == 0);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> ls(const std::string &dirPath_, const std::string &entryNameRegex_, int type_, size_t maxEntries_) {
    if( not isDir( dirPath_ ) ) return {};

    DIR* directory;
    directory = opendir(dirPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ){ return {}; }

    std::vector<std::string> nameElements;
    if(not entryNameRegex_.empty()){ nameElements = GenericToolbox::splitString(entryNameRegex_, "*"); }

    struct dirent* entry;
    std::vector<std::string> subFoldersList;
    while ( ( entry = readdir(directory) ) ) {
      if( type_ != -1 and entry->d_type != type_ ){ continue; }
      if( strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0 ){ continue; }

      if(not entryNameRegex_.empty()){
        std::string entryCandidate = entry->d_name;

        bool isValid{true};
        for( size_t iElement = 0 ; iElement < nameElements.size() ; iElement++ ){
          if(nameElements[iElement].empty()) continue;

          if( iElement == 0 ){
            if( not GenericToolbox::startsWith(entryCandidate, nameElements[iElement]) ){
              isValid = false;
              break;
            }
          }
          else if( iElement+1 == nameElements.size() ){
            if(not GenericToolbox::endsWith(entryCandidate, nameElements[iElement]) ){
              isValid = false;
            }
          }
          else{
            if( not GenericToolbox::hasSubStr(entryCandidate, nameElements[iElement])
                ){
              isValid = false;
              break;
            }
          }

          if( iElement+1 != nameElements.size() ){
            entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
          }
        }
        if( not isValid ) continue;
      }
      subFoldersList.emplace_back(entry->d_name);
      if( maxEntries_ != 0 and subFoldersList.size() >= maxEntries_ ){ break; }
    }
    closedir(directory);
    return subFoldersList;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsDirs(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_) {
    return GenericToolbox::ls(dirPath_, entryNameRegex_, DT_DIR, maxEntries_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsFiles(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_) {
    return GenericToolbox::ls(dirPath_, entryNameRegex_, DT_REG, maxEntries_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsRecursive(const std::string &dirPath_, int type_) {
    // WARNING : Recursive function

    // first, get the files in this folder
    std::vector<std::string> out;

    // then walk in sub-folders
    auto subFolderList = GenericToolbox::lsDirs( dirPath_ );
    for(auto &subFolder : subFolderList ){

      // recursive ////////
      auto subFileList = GenericToolbox::lsRecursive( GenericToolbox::joinPath(dirPath_, subFolder), type_ );
      /////////////////////

      out.reserve( out.size() + subFileList.size() );
      for(auto &subFile : subFileList ){
        out.emplace_back( GenericToolbox::joinPath(subFolder, subFile) );
      }
    }

    auto entries = GenericToolbox::ls(dirPath_, "", type_);
    out.reserve( out.size() + entries.size() );
    for( auto& entry : entries ){ out.emplace_back( entry ); }

    return out;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsDirsRecursive(const std::string &dirPath_) { return lsRecursive(dirPath_, DT_DIR); }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsFilesRecursive(const std::string &dirPath_) { return lsRecursive(dirPath_, DT_REG); }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE size_t getFileHash(const std::string &filePath_) {
    std::hash<std::string> hashString;
    return hashString(dumpFileAsString(filePath_));
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE ssize_t getFileSize(const std::string& filePath_) {
    struct stat st{};
    stat(filePath_.c_str(), &st);
    return ssize_t(st.st_size);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::string dumpFileAsString(const std::string &filePath_) {
    if( not isFile(filePath_) ){ return {}; }
    std::string data;
    std::ifstream input_file(filePath_.c_str(), std::ios::binary | std::ios::in );
    std::ostringstream ss;
    ss << input_file.rdbuf();
    data = ss.str();
    input_file.close();
    return data;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_) {

    if( not isFile( filePath_ ) ){ return {}; }

    std::ifstream file( filePath_ );
    if( not file.is_open() ){ return {}; }

    std::vector<std::string> lines;

    std::string lineBuffer{};
    while( std::getline(file, lineBuffer) ){
      lines.emplace_back( lineBuffer );
    }

    return lines;
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_) {
    writeToFile(outFilePath_, stringToWrite_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::string &stringToWrite_) {
    std::ofstream out(outFilePath_.c_str());
    out << stringToWrite_;
    out.close();
  }

CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::vector<std::string> &linesToWrite_) {
    std::ofstream out(outFilePath_.c_str());
    for( auto& line : linesToWrite_ ){
      out << line << std::endl;
    }
    out.close();
  }

CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE void fillData( std::istream& file_, std::string& buffer_, size_t size_ ) {
    buffer_.clear();
    buffer_.resize(size_);
    file_.read( &buffer_[0], long(size_) );
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsValid(const std::string &filePath_) {
    return isPathValid(filePath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsFile(const std::string &filePath_) {
    return isFile(filePath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsFolder(const std::string &dirPath_) {
    return isDir(dirPath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_) {
    return areSameFiles(filePath1_, filePath2_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool mkdirPath(const std::string &dirPath_) {
    return mkdir(dirPath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool deleteFile(const std::string &filePath_) {
    return rm(filePath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool mvFile(const std::string &src_, const std::string &destination_, bool force_ ) {
    return mv(src_, destination_, force_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool copyFile(const std::string &src_, const std::string &destination_, bool force_ ) {
    return cp(src_, destination_, force_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool deleteEmptyDirectory(const std::string &dirPath_) {
    return rmDir(dirPath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doesFolderIsEmpty(const std::string &dirPath_) {
    return isDirEmpty(dirPath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_) {
    return hasExtension(filePath_, extension_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileExtension(const std::string& filePath_) {
    return getExtension(filePath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFolderPathFromFilePath(const std::string &filePath_) { return getFolderPath(filePath_); }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ ) {
    return getFileName(filePath_, keepExtension_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::string replaceFileExtension(const std::string& filePath_, const std::string& newExtension_) {
    return replaceExtension(filePath_, newExtension_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC long int getFileSizeInBytes(const std::string &filePath_) {
    return getFileSize(filePath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfEntriesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ , int type_, size_t maxEntries_ ) {
    return ls(dirPath_, entryNameRegex_, type_, maxEntries_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfSubFoldersInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ , size_t maxEntries_ ) {
    return lsDirs(dirPath_, entryNameRegex_, maxEntries_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFilesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ , size_t maxEntries_ ) {
    return lsFiles(dirPath_, entryNameRegex_, maxEntries_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfEntriesInSubFolders(const std::string &dirPath_, int type_ ) {
    return lsRecursive(dirPath_, type_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFoldersInSubFolders(const std::string &dirPath_) {
    return lsDirsRecursive(dirPath_);
  }

CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFilesInSubFolders(const std::string &dirPath_) {
    return lsFilesRecursive(dirPath_);
  }
}
