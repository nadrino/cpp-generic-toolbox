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
    inline static bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_){
      bool isSuccess{false};

      if(not doesPathIsFile(srcFilePath_)) return false;

      if( doesPathIsFile(dstFilePath_) ){
        if( not force_ ){ return false; }
        if( not deleteFile(dstFilePath_) ){ return false; }
      }

      auto outDir = GenericToolbox::getFolderPathFromFilePath(dstFilePath_);
      if( not doesPathIsFolder(outDir) ){ mkdirPath(outDir); }

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
//        GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
        Utils::b.progressMap["copyFile"] = double(iChunk+1) / double(nChunk);

        // buffering source file
        GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
        in.read(contentBuffer.data(), bufferSize);
        timeLoad += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);

        out.write(contentBuffer.data(), in.gcount());
        timeDrop += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
      }

//      GenericToolbox::displayProgressBar(nChunk, nChunk, pTitle);
//      std::cout << GET_VAR_NAME_VALUE(timeLoad) << " / " << GET_VAR_NAME_VALUE(timeDrop) << std::endl;

      Utils::b.progressMap["copyFile"] = 1.;
      return isSuccess;
    }
    inline static bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_){
      if( not doesPathIsFile(file1Path_) ) { return false; }
      if( not doesPathIsFile(file2Path_) ) { return false; }

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
    }
    inline static void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_) {
      std::ofstream out(outFilePath_, std::ios::out | std::ios::binary);
      out.write(stringToWrite_.data(), long(stringToWrite_.size()));
    }
  }

  namespace Utils{
    inline static std::string lookForTidInSubFolders(const std::string& folderPath_){
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
    inline static uint8_t* getFolderIconFromTitleId(const std::string& titleId_){
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

  namespace UI{
    inline static std::string openKeyboardUi(const std::string &defaultStr_) {
      SwkbdConfig kbd;
      char tmpoutstr[64];

      if (R_SUCCEEDED(swkbdCreate(&kbd, 0))) {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetInitialText(&kbd, defaultStr_.c_str());
        swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
        swkbdClose(&kbd);
      }

      return {tmpoutstr};
    }
  }

  namespace Terminal {

    inline static void printRight(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{GenericToolbox::Switch::Hardware::getTerminalWidth()};
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));
      if( nbSpaceLeft <= 0 ){
        if( nbSpaceLeft == 0 ) nbSpaceLeft-=1;
        GenericToolbox::Switch::Terminal::printRight(
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
    inline static void printLeft(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{ GenericToolbox::Switch::Hardware::getTerminalWidth() };
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));

      if( flush_ ){
        // can only flush if the terminal is not filled up
        nbSpaceLeft--;
        std::cout << "\r";
      }

      if( nbSpaceLeft < 0 ){
        GenericToolbox::Switch::Terminal::printLeft(
            input_.substr(0, input_.size() + nbSpaceLeft - int(flush_)), // remove extra char if flush
            color_,
            flush_
        );
        return;
      }

      std::cout << color_ << input_ << GenericToolbox::repeatString(" ", nbSpaceLeft) << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
    }
    inline static void printLeftRight(const std::string& inputLeft_, const std::string& inputRight_, const std::string& color_, bool flush_){
      int nbSpaceLeft{ GenericToolbox::Switch::Hardware::getTerminalWidth() };
      nbSpaceLeft -= int( GenericToolbox::getPrintSize(inputLeft_) );
      nbSpaceLeft -= int( GenericToolbox::getPrintSize(inputRight_) );

      std::stringstream ss;

      if(nbSpaceLeft <= 0){ ss << inputLeft_.substr(0, inputLeft_.size() + nbSpaceLeft); }
      else{ ss << inputLeft_; }

      ss << GenericToolbox::repeatString(" ", nbSpaceLeft);
      ss << inputRight_;

      std::cout << color_ << ss.str() << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
      else{
        if( GenericToolbox::Switch::Hardware::getTerminalWidth() < int(inputLeft_.size()) + int(inputRight_.size())) {
          std::cout << std::endl;
        }
      }
    }

    inline static void makePause(const std::string& message_){
      if( not message_.empty() ) std::cout << message_ << std::endl;
      std::cout << "+ to quit now or PRESS any button to continue." << std::endl;
      consoleUpdate(nullptr);

      PadState pad;
      padInitializeAny(&pad);

      // clear buttons?
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      while(appletMainLoop()){

        padUpdate( &pad );
        u64 kDown = padGetButtonsDown( &pad );

        if     ( kDown & HidNpadButton_Plus ) {
          consoleExit(nullptr);
          exit(EXIT_SUCCESS);
        }
        else if( kDown != 0 ) {
          consoleUpdate(nullptr);
          break; // break in order to return to hbmenu
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      consoleUpdate(nullptr);
    }
    template<typename T, typename TT> inline static void displayProgressBar(const T& iCurrent_, const TT& iTotal_,
        const std::string &title_, bool forcePrint_, const std::string& color_){
      if(forcePrint_ or ProgressBar::gProgressBar.template showProgressBar(iCurrent_, iTotal_) ){
        ProgressBar::gProgressBar.setDisableVt100Cmd( true );
        printRight(ProgressBar::gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, GenericToolbox::padString(title_, 40)), color_, true);
        consoleUpdate(nullptr);
      }
    }

  }

  namespace Hardware{
    inline static u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_) {
      u64 out{0};
      svcGetSystemInfo(&out, type_, INVALID_HANDLE, of_);
      return out;
    }
    inline static std::string getMemoryUsageStr(PhysicalMemoryOf of_) {
      std::stringstream ss;
      ss << PhysicalMemoryOfEnumNamespace::toString(of_) << ": ";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(UsedPhysicalMemorySize, of_)));
      ss << "/";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(TotalPhysicalMemorySize, of_)));
      return ss.str();
    }

    inline static int getTerminalWidth(){
      return consoleGetDefault()->consoleWidth;
    }
    inline static int getTerminalHeight(){
      return consoleGetDefault()->consoleHeight;
    }
  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
