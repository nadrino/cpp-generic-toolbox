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

  namespace Utils{
    static inline std::string lookForTidInSubFolders(const std::string& folderPath_){
      // WARNING : Recursive function
      std::string tidExample = "0100626011656000";
      std::vector<std::string> subFolderList = GenericToolbox::getListOfSubFoldersInFolder(folderPath_);

      for(auto &subFolder : subFolderList){
        if(subFolder.size() == tidExample.size() and subFolder[0] == tidExample[0] ){ return subFolder; }
      }

      // if not found
      std::string tidCandidate;
      std::string path;
      for(auto &subFolder : subFolderList){
        tidCandidate = GenericToolbox::Switch::Utils::lookForTidInSubFolders(folderPath_ + "/" + subFolder);
        if(not tidCandidate.empty()){ return tidCandidate; }
      }

      return "";
    }
    static inline uint8_t* getFolderIconFromTitleId(const std::string& titleId_){
      if( titleId_.empty() ) return nullptr;

      uint8_t* icon = nullptr;
      NsApplicationControlData controlData;
      size_t controlSize  = 0;
      uint64_t tid;

      std::istringstream buffer(titleId_);
      buffer >> std::hex >> tid;

      if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize))){ return nullptr; }

      icon = new uint8_t[0x20000];
      memcpy(icon, controlData.icon, 0x20000);
      return icon;
    }
  }

  namespace Printout {

    static inline void printRight(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{GenericToolbox::Switch::Hardware::getTerminalWidth()};
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));
      if( nbSpaceLeft <= 0 ){
        GenericToolbox::Switch::Printout::printRight(
            input_.substr(0, input_.size() + nbSpaceLeft - int(flush_)), // remove extra char if flush
            color_,
            flush_
            );
        return;
      }

      if(flush_){ nbSpaceLeft-=1; std::cout << "\r"; }
      std::cout << color_ << GenericToolbox::repeatString(" ", nbSpaceLeft) << input_ << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
      else if(int(input_.size()) > GenericToolbox::Switch::Hardware::getTerminalWidth()) std::cout << std::endl;
    }
    static inline void printLeft(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{GenericToolbox::Switch::Hardware::getTerminalWidth()};
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));
      if( nbSpaceLeft <= 0 ){
        GenericToolbox::Switch::Printout::printLeft(
            input_.substr(0, input_.size() + nbSpaceLeft - int(flush_)), // remove extra char if flush
            color_,
            flush_
        );
        return;
      }
      if(flush_){ nbSpaceLeft-=1; std::cout << "\r"; }
      std::cout << color_ << input_ << GenericToolbox::repeatString(" ", nbSpaceLeft) << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
      else if(int(input_.size()) > GenericToolbox::Switch::Hardware::getTerminalWidth()) std::cout << std::endl;
    }
    static inline void printLeftRight(const std::string& input_left_, const std::string& input_right_, const std::string& color_){
      int nbSpaceLeft{GenericToolbox::Switch::Hardware::getTerminalWidth()};
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_left_));
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_right_));

      if(nbSpaceLeft <= 0){
        GenericToolbox::Switch::Printout::printLeftRight(
            input_left_.substr(0, input_left_.size() + nbSpaceLeft),
            input_right_,
            color_
        );
        return;
      }

      std::cout << color_ << input_left_;
      std::cout << GenericToolbox::repeatString(" ", nbSpaceLeft);
      std::cout << input_right_;
      std::cout << GenericToolbox::ColorCodes::resetColor;
      if(GenericToolbox::Switch::Hardware::getTerminalWidth() < int(input_left_.size()) + int(input_right_.size())) std::cout << std::endl;
    }

  }

  namespace Hardware{
    static inline u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_) {
      u64 out{0};
      svcGetSystemInfo(&out, type_, INVALID_HANDLE, of_);
      return out;
    }
    static inline std::string getMemoryUsageStr(PhysicalMemoryOf of_) {
      std::stringstream ss;
      ss << PhysicalMemoryOfEnumNamespace::toString(of_) << ": ";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(UsedPhysicalMemorySize, of_)));
      ss << "/";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(TotalPhysicalMemorySize, of_)));
      return ss.str();
    }

    static inline int getTerminalWidth(){
      return consoleGetDefault()->consoleWidth;
    }
    static inline int getTerminalHeight(){
      return consoleGetDefault()->consoleHeight;
    }
  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
