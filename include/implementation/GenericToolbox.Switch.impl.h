//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H

#ifdef __SWITCH__

#include "GenericToolbox.h"

#include <sys/stat.h>

namespace GenericToolbox::Switch {

  namespace IO{


    static inline bool doesPathIsValue(const std::string& path_){
      struct stat buffer{};
      return ( stat(path_.c_str(), &buffer) == 0 );
    }
    static inline bool doesPathIsFile(const std::string& path_){
      struct stat path_stat{};
      stat(path_.c_str(), &path_stat);
      return S_ISREG(path_stat.st_mode); // is regular file?
    }
    static inline bool doesPathIsFolder(const std::string& path_){
      struct stat path_stat{};
      stat(path_.c_str(), &path_stat);
      return S_ISDIR(path_stat.st_mode);
    }


    static inline bool mkdirPath(const std::string& dirPath_){
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
    }
    static inline bool deleteFile(const std::string& filePath_){
      return (::remove(filePath_.c_str()) == 0);
    }
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_){
      bool isSuccess{false};

      if(not force_ and doesPathIsFile(dstFilePath_)){ return false; }

      auto outDir = GenericToolbox::getFolderPathFromFilePath(dstFilePath_);
      if( not doesPathIsFolder(outDir) ){ mkdirPath(outDir); }

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

              size_t bufferSize = std::min(size_t(srcFileSize/100), GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize);
              bufferSize = std::min(bufferSize, size_t(srcFileSize));
              std::vector<u8> contentBuffer(bufferSize, 0);
              size_t nChunk = (size_t(srcFileSize)/bufferSize) + 1;
              std::string pTitle = GenericToolbox::getFileNameFromFilePath(srcFilePath_) + " -> " + outDir;

              for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ){
                GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
                Utils::b.progressMap["copyFile"] = double(iChunk) / double(nChunk);

                // buffering source file
                if(R_FAILED(fsFileRead(&srcFile, readOffset, &contentBuffer[0], bufferSize, FsReadOption_None, &bytesRedCounter))){
                  isSuccess = false;
                  break;
                }

                // dumping data in destination file
                if(R_FAILED(fsFileWrite(&dstFile, readOffset, &contentBuffer[0], bytesRedCounter, FsWriteOption_Flush))){
                  isSuccess = false;
                  break;
                }

                // preparing next loop
                readOffset += s64(bytesRedCounter);
              }

            }
            fsFileClose(&dstFile);
          }

        }
      }
      fsFileClose(&srcFile);

      Utils::b.progressMap["copyFile"] = 1.;
      return isSuccess;
    }
    static inline bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_){
      bool areIdentical{false};

      if( not doesPathIsFile(file1Path_) ) return false;
      if( not doesPathIsFile(file2Path_) ) return false;

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

//                  size_t copy_buffer_size = 0x10000; // 65 kB (65536 B) // too much for 2 files...
//                  size_t copy_buffer_size = 0x1000; // 4,096 B // on the safe side
//                  size_t bufferSize{0xD000}; // 53,248 B
                  s64 readOffset{0};

                  size_t bufferSize = std::min(GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize, size_t(file1Size));
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
    }
  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
