//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H

#ifdef __SWITCH__

#include "GenericToolbox.h"

#include <sys/stat.h>
#include "fstream"

namespace GenericToolbox::Switch {

  namespace IO{


    static inline bool doesPathIsValue(const std::string& path_){
      struct stat buffer{};
      return ( stat(path_.c_str(), &buffer) == 0 );
    }
    static inline bool doesPathIsFolder(const std::string& path_){
      struct stat path_stat{};
      stat(path_.c_str(), &path_stat);
      return S_ISDIR(path_stat.st_mode);
    }
    static inline bool doesPathIsFile(const std::string& path_){
      struct stat path_stat{};
      stat(path_.c_str(), &path_stat);
      return S_ISREG(path_stat.st_mode); // is regular file?
    }
    static inline ssize_t getFileSize(const std::string& path_){
      struct stat st{};
      stat(path_.c_str(), &st);
      return ssize_t(st.st_size);
    }
    static inline std::vector<std::string> getListOfEntriesInFolder(const std::string& folder_path_){
      std::vector<std::string> out;
      FsDir fs_DirBuffer;
      if(R_FAILED(fsFsOpenDirectory(p.fsBuffer, folder_path_.c_str(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &fs_DirBuffer))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      s64 entry_count;
      if(R_FAILED(fsDirGetEntryCount(&fs_DirBuffer, &entry_count))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      size_t entry_count_size_t(entry_count);
      s64 total_entries;
      std::vector<FsDirectoryEntry> fs_directory_entries(entry_count_size_t);
      if(R_FAILED(fsDirRead(&fs_DirBuffer, &total_entries, entry_count_size_t, &fs_directory_entries[0]))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      for(u32 i_entry = 0 ; i_entry < entry_count_size_t ; i_entry++){
        std::string entry_name = fs_directory_entries[i_entry].name;
        if(entry_name == "." or entry_name == ".."){
          continue;
        }
        out.emplace_back(fs_directory_entries[i_entry].name);
      }
      fsDirClose(&fs_DirBuffer);
      return out;
    }
    static inline std::vector<std::string> getListOfSubFoldersInFolder(const std::string& folder_path_){
      std::vector<std::string> out;
      FsDir fs_DirBuffer;
      if(R_FAILED(fsFsOpenDirectory(p.fsBuffer, folder_path_.c_str(), FsDirOpenMode_ReadDirs, &fs_DirBuffer))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      s64 entry_count;
      if(R_FAILED(fsDirGetEntryCount(&fs_DirBuffer, &entry_count))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      size_t entry_count_size_t(entry_count);
      s64 total_entries;
      std::vector<FsDirectoryEntry> fs_directory_entries(entry_count_size_t);
      if(R_FAILED(fsDirRead(&fs_DirBuffer, &total_entries, entry_count_size_t, &fs_directory_entries[0]))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      for(u32 i_entry = 0 ; i_entry < entry_count_size_t ; i_entry++){
        if(fs_directory_entries[i_entry].type != FsDirEntryType_Dir) // should not be necessary
          continue;
        std::string entry_name = fs_directory_entries[i_entry].name;
        if(entry_name == "." or entry_name == ".."){
          continue;
        }
        out.emplace_back(fs_directory_entries[i_entry].name);
      }
      fsDirClose(&fs_DirBuffer);
      return out;
    }
    static inline std::vector<std::string> getListOfFilesInFolder(const std::string& folderPath_){
      std::vector<std::string> out;
      FsDir fs_DirBuffer;
      if(R_FAILED(fsFsOpenDirectory(p.fsBuffer, folderPath_.c_str(), FsDirOpenMode_ReadFiles, &fs_DirBuffer))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      s64 entry_count;
      if(R_FAILED(fsDirGetEntryCount(&fs_DirBuffer, &entry_count))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      size_t entry_count_size_t(entry_count);
      s64 total_entries;
      std::vector<FsDirectoryEntry> fs_directory_entries(entry_count_size_t);
      if(R_FAILED(fsDirRead(&fs_DirBuffer, &total_entries, entry_count_size_t, &fs_directory_entries[0]))){
        fsDirClose(&fs_DirBuffer);
        return out;
      }
      for(u32 i_entry = 0 ; i_entry < entry_count_size_t ; i_entry++){
        if(fs_directory_entries[i_entry].type != FsDirEntryType_File)
          continue;
        std::string entry_name = fs_directory_entries[i_entry].name;
        if(entry_name == "." or entry_name == ".."){
          continue;
        }
        out.emplace_back(fs_directory_entries[i_entry].name);
      }
      fsDirClose(&fs_DirBuffer);
      return out;
    }
    static inline std::vector<std::string> getListOfFilesInSubFolders(const std::string &folderPath_){
      // WARNING : Recursive function
      std::vector<std::string> out(GenericToolbox::Switch::IO::getListOfFilesInFolder(folderPath_));
      for(auto &subFolder : GenericToolbox::Switch::IO::getListOfSubFoldersInFolder(folderPath_)){
        std::string subfolder_full_path = folderPath_;
        subfolder_full_path += "/";
        subfolder_full_path += subFolder;
        auto subfile_names_list = GenericToolbox::Switch::IO::getListOfFilesInSubFolders(subfolder_full_path);
        for(auto &subfile_name : subfile_names_list){
          std::string relative_subfile_path;
          relative_subfile_path += subFolder;
          relative_subfile_path += "/";
          relative_subfile_path += subfile_name;
          out.emplace_back(GenericToolbox::removeRepeatedCharacters(relative_subfile_path, "/"));
        }
      }
      return out;
    }


    static inline bool mkdirPath(const std::string& dirPath_){
#if 0
      bool isSuccess{true};

      if(doesPathIsFolder(dirPath_)) return isSuccess;

      std::string stagedPath;
      std::string folderLevel;
      std::stringstream ss(dirPath_);

      // split path using slash as a separator
      while (std::getline(ss, folderLevel, '/')) {
        stagedPath += folderLevel; // append folder to the current level
        if(stagedPath.empty()) stagedPath = "/";
        GenericToolbox::removeRepeatedCharInsideInputStr(stagedPath, "/");
        // create current level
        if(not doesPathIsFolder(stagedPath)){
          if(R_FAILED(fsFsCreateDirectory(p.fsBuffer, stagedPath.c_str()))){ isSuccess = false; break; }
        }
        stagedPath += "/"; // don't forget to append a slash
      }

      return isSuccess;
#else
      GenericToolbox::mkdirPath(dirPath_);
      return true;
#endif
    }
    static inline bool deleteFile(const std::string& filePath_){
      return (::remove(filePath_.c_str()) == 0);
    }
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_){
      bool isSuccess{false};

      if(not doesPathIsFile(srcFilePath_)) return false;

      if( doesPathIsFile(dstFilePath_) ){
        if( not force_ ){ return false; }
        if( not deleteFile(dstFilePath_) ){ return false; }
      }

      auto outDir = GenericToolbox::getFolderPathFromFilePath(dstFilePath_);
      if( not doesPathIsFolder(outDir) ){ mkdirPath(outDir); }

#if 0
      GenericToolbox::Switch::Utils::b.progressMap["copyFile"] = 0;
      // opening source file
      FsFile srcFile;
      if(R_SUCCEEDED(fsFsOpenFile(p.fsBuffer, srcFilePath_.c_str(), FsOpenMode_Read, &srcFile))){
        // get size of source file
        s64 srcFileSize{0};
        if(R_SUCCEEDED(fsFileGetSize(&srcFile, &srcFileSize))){

          // create destination file
          if(R_SUCCEEDED(fsFsCreateFile(p.fsBuffer, dstFilePath_.c_str(), srcFileSize, 0))){

            // open destination file
            FsFile dstFile;
            if(R_SUCCEEDED(fsFsOpenFile(p.fsBuffer, dstFilePath_.c_str(), FsOpenMode_Write, &dstFile))){

              u64 bytesRedCounter{0};
              s64 readOffset{0};
              isSuccess = true; // consider it worked by default -> will change if not

              auto bufferSize = size_t(srcFileSize/100); // 1 chunk per %
              bufferSize = std::min(bufferSize, GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize); // cap the buffer size -> not too big
              bufferSize = std::max(bufferSize, GenericToolbox::Switch::IO::ParametersHolder::minBufferSize); // cap the buffer size -> not too small
              std::vector<u8> contentBuffer(bufferSize, 0);
              size_t nChunk = (size_t(srcFileSize)/bufferSize) + 1;
              std::string pTitle = GenericToolbox::getFileNameFromFilePath(srcFilePath_) + " -> " + outDir;

              size_t timeLoad{0};
              size_t timeDrop{0};

              for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ){
                GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
                Utils::b.progressMap["copyFile"] = double(iChunk) / double(nChunk);

                // buffering source file
                GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
                if(R_FAILED(fsFileRead(&srcFile, readOffset, &contentBuffer[0], bufferSize, FsReadOption_None, &bytesRedCounter))){
                  isSuccess = false;
                  break;
                }
                timeLoad += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);

                // dumping data in destination file
                if(R_FAILED(fsFileWrite(&dstFile, readOffset, &contentBuffer[0], bytesRedCounter, FsWriteOption_Flush))){
                  isSuccess = false;
                  break;
                }
                timeDrop += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);

                // preparing next loop
                readOffset += s64(bytesRedCounter);
                if( readOffset == srcFileSize ) break;
              }
              GenericToolbox::displayProgressBar(nChunk, nChunk, pTitle);

              if( not isSuccess ){
                std::cout << "NOT SUCCESS" << std::endl;
              }
              if( readOffset != srcFileSize ){
                std::cout << GET_VAR_NAME_VALUE(readOffset) << " -> " << GET_VAR_NAME_VALUE(srcFileSize) << std::endl;
              }

              std::cout << GET_VAR_NAME_VALUE(timeLoad) << " / " << GET_VAR_NAME_VALUE(timeDrop) << std::endl;

            }
            fsFileClose(&dstFile);
          }

        }
      }
      fsFileClose(&srcFile);
#else
      ssize_t srcFileSize = getFileSize(srcFilePath_);
      std::ifstream in(srcFilePath_, std::ios::in | std::ios::binary);
      std::ofstream out(dstFilePath_, std::ios::out | std::ios::binary);

      auto bufferSize = ssize_t(srcFileSize/500); // 0.2 chunk per % (can see on pixels)
      bufferSize = std::min(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize)); // cap the buffer size -> not too big
      bufferSize = std::max(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::minBufferSize)); // cap the buffer size -> not too small
      std::vector<char> contentBuffer(bufferSize, 0);
      size_t nChunk = (size_t(srcFileSize)/bufferSize) + 1;
      Utils::b.progressMap["copyFile"] = double(1) / double(nChunk);
      std::string pTitle = GenericToolbox::getFileNameFromFilePath(srcFilePath_) + " -> " + outDir;

      size_t timeLoad{0};
      size_t timeDrop{0};

      for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ) {
        GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
        Utils::b.progressMap["copyFile"] = double(iChunk+1) / double(nChunk);

        // buffering source file
        GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
        in.read(contentBuffer.data(), bufferSize);
        timeLoad += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);

        out.write(contentBuffer.data(), in.gcount());
        timeDrop += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
      }

      GenericToolbox::displayProgressBar(nChunk, nChunk, pTitle);
      std::cout << GET_VAR_NAME_VALUE(timeLoad) << " / " << GET_VAR_NAME_VALUE(timeDrop) << std::endl;
#endif

      Utils::b.progressMap["copyFile"] = 1.;
      return isSuccess;
    }
    static inline bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_){
      if( not doesPathIsFile(file2Path_) ) { return false; }
      if( not doesPathIsFile(file1Path_) ) { return false; }

#if 0
      GenericToolbox::Switch::Utils::b.progressMap["doFilesAreIdentical"] = 0.;
      bool areIdentical{false};
      // opening file1
      char path_buffer_file1[FS_MAX_PATH];
      FsFile file1;
      snprintf(path_buffer_file1, FS_MAX_PATH, "%s", file1Path_.c_str());
      if(R_SUCCEEDED(fsFsOpenFile(p.fsBuffer, path_buffer_file1, FsOpenMode_Read, &file1))){
        // opening file2
        char path_buffer_file2[FS_MAX_PATH];
        FsFile file2;
        snprintf(path_buffer_file2, FS_MAX_PATH, "%s", file2Path_.c_str());
        if(R_SUCCEEDED(fsFsOpenFile(p.fsBuffer, path_buffer_file2, FsOpenMode_Read, &file2))){

          // get size of file1
          s64 file1Size = 0;
          if(R_SUCCEEDED(fsFileGetSize(&file1, &file1Size))){
            // get size of file2
            s64 file2Size = 0;
            if(R_SUCCEEDED(fsFileGetSize(&file2, &file2Size))){
              if(file1Size == file2Size){
                areIdentical = true;
                if(p.useCrcCheck){

                  s64 readOffset{0};

                  auto bufferSize = size_t(file1Size/100); // 1 chunk per %
                  bufferSize = std::min(bufferSize, GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize); // cap the buffer size -> not too big
                  bufferSize = std::max(bufferSize, GenericToolbox::Switch::IO::ParametersHolder::minBufferSize); // cap the buffer size -> not too small

                  std::vector<u8> file1ContentBuffer(bufferSize,0);
                  u64 file1CounterBytesRed{0};
                  auto file1Crc = crc32(0L, Z_NULL, 0);

                  std::vector<u8> file2ContentBuffer(bufferSize,0);
                  u64 file2CounterBytesRed{0};
                  auto file2Crc = crc32(0L, Z_NULL, 0);

                  size_t nChunk = (size_t(file1Size)/bufferSize) + 1;
                  for(size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ){
                    GenericToolbox::Switch::Utils::b.progressMap["doFilesAreIdentical"] = double(iChunk)/double(nChunk);

                    // buffering file1
                    if( R_FAILED(fsFileRead(&file1, readOffset, &file1ContentBuffer[0],bufferSize, FsReadOption_None, &file1CounterBytesRed))){
                      areIdentical = false;
                      break;
                    }

                    // buffering file2
                    if(R_FAILED(fsFileRead(&file2, readOffset, &file2ContentBuffer[0], bufferSize, FsReadOption_None, &file2CounterBytesRed))){
                      areIdentical = false; break;
                    }

                    // check read size
                    if(file1CounterBytesRed != file2CounterBytesRed){
                      areIdentical = false;
                      break;
                    }

                    // check crc
                    file1Crc = crc32(file1Crc, &file1ContentBuffer[0], file1CounterBytesRed);
                    file2Crc = crc32(file2Crc, &file2ContentBuffer[0], file2CounterBytesRed);
                    if(file1Crc != file2Crc){ areIdentical = false; break; }

                    // preparing next loop
                    readOffset += s64(file1CounterBytesRed);
                  }

                } // CRC ? yes
                else {
                  // if CRC is disable and we reached this point, then we consider files are the same
                  areIdentical = true;
                } // CRC ? no

              } // size match ?
            } // size file 2
          } // size file 1
        } // open file 2
        fsFileClose(&file2);
      } // open file 1
      fsFileClose(&file1);
      return areIdentical;
#else
      ssize_t file1Size = getFileSize(file1Path_);
      if( file1Size != getFileSize(file2Path_) ) return false;
      if(p.useCrcCheck){
        std::ifstream file1(file1Path_, std::ios::in | std::ios::binary);
        std::ifstream file2(file2Path_, std::ios::in | std::ios::binary);

        auto bufferSize = ssize_t(file1Size/200); // 1 chunk per %
        bufferSize = std::min(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize)); // cap the buffer size -> not too big
        bufferSize = std::max(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::minBufferSize)); // cap the buffer size -> not too small
        std::vector<u8> file1Buffer(bufferSize, 0);
        std::vector<u8> file2Buffer(bufferSize, 0);
        size_t nChunk = (size_t(file1Size)/bufferSize) + 1;
        Utils::b.progressMap["doFilesAreIdentical"] = double(1) / double(nChunk);

        auto file1Crc = crc32(0L, Z_NULL, 0);
        auto file2Crc = crc32(0L, Z_NULL, 0);

        for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ) {
          Utils::b.progressMap["doFilesAreIdentical"] = double(iChunk+1) / double(nChunk);

          // buffering source file
          file1.read(reinterpret_cast<char *>(file1Buffer.data()), bufferSize);
          file2.read(reinterpret_cast<char *>(file2Buffer.data()), bufferSize);

          // check read size
          if( file1.gcount() != file2.gcount() ) return false;

          // check crc
          file1Crc = crc32(file1Crc, file1Buffer.data(), file1.gcount());
          file2Crc = crc32(file2Crc, file2Buffer.data(), file2.gcount());
          if(file1Crc != file2Crc){ return false; }
        }
      }
      return true;
#endif
    }
  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
