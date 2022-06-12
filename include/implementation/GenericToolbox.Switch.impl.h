//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H

#ifdef __SWITCH__

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
    static inline void deleteFile(const std::string& filePath_){
      fsFsDeleteFile(p.fsBuffer, filePath_.c_str());
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
                  u64 readOffset{0};
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
                    readOffset += file1CounterBytesRed;

                  }
                  while(s64(readOffset) < file1Size);

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
