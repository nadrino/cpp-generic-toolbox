//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_FS_H
#define CPP_GENERIC_TOOLBOX_FS_H

#include "GenericToolbox.Build.h"

// ***************************
//! Filesystem related tools
// ***************************

#include "GenericToolbox.String.h"

#include <sys/stat.h>
#if defined(_WIN32)
#include "GenericToolbox.Windows.h"
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <memory>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define USE_FILESYSTEM 0 // experimental
#if HAS_CPP_17 && USE_FILESYSTEM
#include <filesystem>
#endif


// Declaration section
namespace GenericToolbox{

#if defined(_WIN32)
  // Keep the existing ls(type) values used by dirent on Unix.
  constexpr int DT_DIR = 4;
  constexpr int DT_REG = 8;
#endif

  // -- no IO dependencies (string parsing)
  CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::string &extension_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::vector<std::string> &extensionList_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getExtension(const std::string& filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFolderPath(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileName(const std::string &filePath_, bool withExtension_ = true);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string generateCleanFileName(const std::string &fileName_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string replaceExtension(const std::string& filePath_, const std::string& newExtension_);
  template<typename T1, typename T2> static std::string joinPath(const T1& str1_, const T2& str2_);
  template<typename T1, typename T2> static std::string joinPath(const std::vector<T1>& vec1_, const std::vector<T2>& vec2_);
  template<typename T, typename T2> static std::string joinPath(const std::vector<T>& vec_, const T2& str_);
  template<typename T1, typename T> static std::string joinPath(const T1& str_, const std::vector<T>& vec_);
  template<typename T> static std::string joinPath(const std::vector<T>& vec_);
  template<typename First, typename Second, typename... Args> static std::string joinPath(const First& first_, const Second& second_, const Args&... args_);

  // -- with direct IO dependencies
  CPP_GENERIC_TOOLBOX_FS_STATIC bool isPathValid(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool isFile(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool isDir(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool areSameFiles(const std::string &filePath1_, const std::string &filePath2_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool isDirEmpty(const std::string &dirPath_);

  // -- unix-like commands
  CPP_GENERIC_TOOLBOX_FS_STATIC bool mkdir(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool rm(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool mv(const std::string &src_, const std::string &destination_, bool force_ = false);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool cp(const std::string &src_, const std::string &destination_, bool force_ = false);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool rmDir(const std::string &dirPath_); // works for empty dirs
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> ls(const std::string &dirPath_, const std::string &entryNameRegex_ = "", int type_=-1, size_t maxEntries_ = 0);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> lsDirs(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> lsFiles(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> lsRecursive(const std::string &dirPath_, int type_ = -1);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> lsDirsRecursive(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> lsFilesRecursive(const std::string &dirPath_);

  // -- read file
  CPP_GENERIC_TOOLBOX_FS_STATIC size_t getFileHash(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC ssize_t getFileSize(const std::string& filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string dumpFileAsString(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_=false);

  // -- write file
  CPP_GENERIC_TOOLBOX_FS_STATIC void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::vector<std::string> &linesToWrite_);

  // -- binary reader
  CPP_GENERIC_TOOLBOX_FS_STATIC void fillData( std::istream& file_, std::string& buffer_, size_t size_ );
  template<typename T> static void fillData( std::istream& file_, T& buffer_ );

  template<typename T> static void writeData( std::ofstream& file_, const T& buffer_ );
  template<> inline void writeData( std::ofstream& file_, const std::string& buffer_ );

}


// Implementation section
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::string &extension_);
  CPP_GENERIC_TOOLBOX_FS_STATIC bool hasExtension(const std::string &filePath_, const std::vector<std::string> &extensionList_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getExtension(const std::string& filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFolderPath(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileName(const std::string &filePath_, bool withExtension_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string generateCleanFileName(const std::string &fileName_);
  CPP_GENERIC_TOOLBOX_FS_STATIC std::string replaceExtension(const std::string& filePath_, const std::string& newExtension_);
  template<typename T1, typename T2> static inline std::string joinPath(const T1& str1_, const T2& str2_){
    std::stringstream ss;
    ss << str1_;
    if( not ss.str().empty() ){ ss << "/"; }
    ss << str2_;
    // explicit type specification as `auto` is not converted into a std::string for GCC 4.8.5
    std::string out{ss.str()};
    GenericToolbox::removeRepeatedCharInsideInputStr( out, "/" );
    GenericToolbox::removeTrailingCharInsideInputStr( out, "/" );
    return out;
  }
  template<typename T1, typename T2> static inline std::string joinPath(const std::vector<T1>& vec1_, const std::vector<T2>& vec2_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec1_), GenericToolbox::joinPath(vec2_));
  }
  template<typename T, typename T2> static inline std::string joinPath(const std::vector<T>& vec_, const T2& str_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec_), str_);
  }
  template<typename T1, typename T> static inline std::string joinPath(const T1& str_, const std::vector<T>& vec_){
    return GenericToolbox::joinPath(str_, GenericToolbox::joinPath(vec_));
  }
  template<typename T> static inline std::string joinPath(const std::vector<T>& vec_){
    std::string out;
    for( auto& elm : vec_ ){ out = GenericToolbox::joinPath(out, elm); }
    return out;
  }
  template<typename First, typename Second, typename... Args> static inline std::string joinPath(const First& first_, const Second& second_, const Args&... args_){
    // unfold to binary expression
    auto out{joinPath(first_, joinPath(second_, args_...))};
    return out;
  }

  // -- with direct IO dependencies
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isPathValid(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isFile(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isDir(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool areSameFiles(const std::string &filePath1_, const std::string &filePath2_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool isDirEmpty(const std::string &dirPath_);

  // -- unix-like commands
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool mkdir(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool rm(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool mv(const std::string &src_, const std::string &destination_, bool force_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool cp(const std::string &src_, const std::string &destination_, bool force_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE bool rmDir(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> ls(const std::string &dirPath_, const std::string &entryNameRegex_, int type_, size_t maxEntries_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsDirs(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsFiles(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsRecursive(const std::string &dirPath_, int type_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsDirsRecursive(const std::string &dirPath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> lsFilesRecursive(const std::string &dirPath_);

  // -- read file
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE size_t getFileHash(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE ssize_t getFileSize(const std::string& filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::string dumpFileAsString(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_);

  // -- write file
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  CPP_GENERIC_TOOLBOX_FS_STATIC void writeToFile(const std::string &outFilePath_, const std::vector<std::string> &linesToWrite_);

  // -- binary reader
  template<typename T> static inline void fillData( std::istream& file_, T& buffer_ ){
    file_.read( reinterpret_cast<char*>(&buffer_), sizeof(T) );
  }
  template<typename T> static inline void fillData( std::istream& file_, T* buffer_, size_t size_ ){
    file_.read( reinterpret_cast<char*>(buffer_), sizeof(T)*size_ );
  }
  template<typename T, size_t N> static inline void fillData( std::istream& file_, std::array<T, N>& buffer_ ){
    file_.read( reinterpret_cast<char*>(&buffer_[0]), sizeof(T)*N );
  }
  template<typename T> static inline void writeData( std::ofstream& file_, const T& buffer_ ){
    file_.write( reinterpret_cast<const char*>(&buffer_), sizeof(T) );
  }
  template<> inline void writeData( std::ofstream& file_, const std::string& buffer_ ){
    file_.write( &buffer_[0], long(buffer_.size()) );
  }
  CPP_GENERIC_TOOLBOX_FS_STATIC CPP_GENERIC_TOOLBOX_FS_INLINE void fillData( std::istream& file_, std::string& buffer_, size_t size_ );

  // Deprecated
  GT_DEPRECATED("renamed: isPathValid") CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsValid(const std::string &filePath_);
  GT_DEPRECATED("renamed: isFile") CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsFile(const std::string &filePath_);
  GT_DEPRECATED("renamed: isDir") CPP_GENERIC_TOOLBOX_FS_STATIC bool doesPathIsFolder(const std::string &dirPath_);
  GT_DEPRECATED("renamed: areSameFiles") CPP_GENERIC_TOOLBOX_FS_STATIC bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_);

  GT_DEPRECATED("renamed: mkdir") CPP_GENERIC_TOOLBOX_FS_STATIC bool mkdirPath(const std::string &dirPath_);
  GT_DEPRECATED("renamed: rm") CPP_GENERIC_TOOLBOX_FS_STATIC bool deleteFile(const std::string &filePath_);
  GT_DEPRECATED("renamed: mv") CPP_GENERIC_TOOLBOX_FS_STATIC bool mvFile(const std::string &src_, const std::string &destination_, bool force_ = false);
  GT_DEPRECATED("renamed: cp") CPP_GENERIC_TOOLBOX_FS_STATIC bool copyFile(const std::string &src_, const std::string &destination_, bool force_ = false);
  GT_DEPRECATED("renamed: rmDir") CPP_GENERIC_TOOLBOX_FS_STATIC bool deleteEmptyDirectory(const std::string &dirPath_);

  GT_DEPRECATED("renamed: isDirEmpty") CPP_GENERIC_TOOLBOX_FS_STATIC bool doesFolderIsEmpty(const std::string &dirPath_);

  GT_DEPRECATED("renamed: hasExtension") CPP_GENERIC_TOOLBOX_FS_STATIC bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_);
  GT_DEPRECATED("renamed: getExtension") CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileExtension(const std::string& filePath_);
  GT_DEPRECATED("renamed: getFolderPath") CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFolderPathFromFilePath(const std::string &filePath_);
  GT_DEPRECATED("renamed: getFileName") CPP_GENERIC_TOOLBOX_FS_STATIC std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ = true);
  GT_DEPRECATED("renamed: replaceExtension") CPP_GENERIC_TOOLBOX_FS_STATIC std::string replaceFileExtension(const std::string& filePath_, const std::string& newExtension_);

  GT_DEPRECATED("renamed: getFileSize") CPP_GENERIC_TOOLBOX_FS_STATIC long int getFileSizeInBytes(const std::string &filePath_);
  GT_DEPRECATED("renamed: ls") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfEntriesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", int type_=-1, size_t maxEntries_ = 0);
  GT_DEPRECATED("renamed: lsDirs") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfSubFoldersInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  GT_DEPRECATED("renamed: lsFiles") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFilesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  GT_DEPRECATED("renamed: lsRecursive") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfEntriesInSubFolders(const std::string &dirPath_, int type_ = -1);
  GT_DEPRECATED("renamed: lsDirsRecursive") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFoldersInSubFolders(const std::string &dirPath_);
  GT_DEPRECATED("renamed: lsFilesRecursive") CPP_GENERIC_TOOLBOX_FS_STATIC std::vector<std::string> getListOfFilesInSubFolders(const std::string &dirPath_);


}


#ifndef CPP_GENERIC_TOOLBOX_FS_COMPILED
#include "implementation/GenericToolbox.Fs.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_FS_H
