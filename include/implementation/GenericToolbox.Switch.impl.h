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
      fsFsDeleteFile(p.fsBuffer, filePath_.c_str());
      return not doesPathIsFile(filePath_);
    }
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_){
      bool isSuccess{false};

      if(doesPathIsFile(dstFilePath_) and not deleteFile(dstFilePath_)){ return false; }

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

              s64 bufferSize = std::min(GenericToolbox::Switch::IO::ParametersHolder::copyBufferSize, srcFileSize);
              std::cout << GET_VAR_NAME_VALUE(bufferSize) << std::endl;
              u8 contentBuffer[bufferSize];

              s64 iChunk = 0;
              s64 nChunk = (srcFileSize/bufferSize) + 1;

              std::string pTitle = GenericToolbox::getFileNameFromFilePath(srcFilePath_) + " -> " + outDir;
              do {
                GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
                Utils::b.progressMap["copyFile"] = double(iChunk++) / double(nChunk);

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
              while(readOffset < srcFileSize);

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
        FsFile fs_file2;
        snprintf(path_buffer_file2, FS_MAX_PATH, "%s", file2Path_.c_str());
        if(R_SUCCEEDED(fsFsOpenFile(p.fsBuffer, path_buffer_file2, FsOpenMode_Read, &fs_file2))){

          // get size of file1
          s64 file1Size = 0;
          if(R_SUCCEEDED(fsFileGetSize(&file1, &file1Size))){
            // get size of file2
            s64 file2Size = 0;
            if(R_SUCCEEDED(fsFileGetSize(&fs_file2, &file2Size))){
              if(file1Size == file2Size){
                areIdentical = true;
                if(p.useCrcCheck){

//                  size_t copy_buffer_size = 0x10000; // 65 kB (65536 B) // too much for 2 files...
//                  size_t copy_buffer_size = 0x1000; // 4,096 B // on the safe side
                  size_t bufferSize{0xD000}; // 53,248 B
                  s64 readOffset{0};
                  s64 counts = 0;
                  s64 expected_total_count = file1Size / s64(bufferSize);

                  u8 file1ContentBuffer[bufferSize];
                  u64 file1CounterBytesRed{0};
                  auto file1Crc = crc32(0L, Z_NULL, 0);

                  u8 file2ContentBuffer[bufferSize];
                  u64 file2CounterBytesRed{0};
                  auto file2Crc = crc32(0L, Z_NULL, 0);

                  do {
                    GenericToolbox::Switch::Utils::b.progressMap["doFilesAreIdentical"] = double(counts++)/double(expected_total_count);

                    // buffering file1
                    if( R_FAILED(
                          fsFileRead(
                              &file1, readOffset, &file1ContentBuffer[0],
                              bufferSize, FsReadOption_None, &file1CounterBytesRed
                          )
                        )
                      ){
                      areIdentical = false;
                      break;
                    }

                    // buffering file2
                    if(R_FAILED(fsFileRead(&fs_file2, readOffset, &file2ContentBuffer[0], bufferSize, FsReadOption_None, &file2CounterBytesRed))){
                      areIdentical = false; break;
                    }

                    // check read size
                    if(file1CounterBytesRed != file2CounterBytesRed){
                      areIdentical = false;
                      break;
                    }

                    // check crc
                    file1Crc = crc32(file1Crc, file1ContentBuffer, file1CounterBytesRed);
                    file2Crc = crc32(file2Crc, file2ContentBuffer, file2CounterBytesRed);
                    if(file1Crc != file2Crc){ areIdentical = false; break; }

                    // preparing next loop
                    readOffset += s64(file1CounterBytesRed);

                  }
                  while(readOffset < file1Size);

                } // CRC ? yes
                else {
                  // if CRC is disable and we reached this point, then we consider files are the same
                  areIdentical = true;
                } // CRC ? no

              } // size match ?
            } // size file 2
          } // size file 1
        } // open file 2
        fsFileClose(&fs_file2);
      } // open file 1
      fsFileClose(&file1);

      return areIdentical;
    }
  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
