//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H

#ifdef __SWITCH__

#include "switch.h"
#include "zlib.h"

#include "map"
#include "string"

namespace GenericToolbox::Switch{

  // IO
  namespace IO{

    struct ParametersHolder{
      inline void initFileSystemBuffer(){
        if( this->fsBuffer != nullptr ){ return; }
        fsdevMountSdmc();
        this->fsBuffer = fsdevGetDeviceFileSystem("sdmc");
      }

//      ParametersHolder() = default;
      ParametersHolder(){
        this->initFileSystemBuffer();
      };

      FsFileSystem* fsBuffer{nullptr};
      bool useCrcCheck{true};

      static const size_t maxBufferSize{0x100000}; // 1MB
    };
    static ParametersHolder p{};


    // Poll
    static inline bool doesPathIsValue(const std::string& path_);
    static inline bool doesPathIsFile(const std::string& path_);
    static inline bool doesPathIsFolder(const std::string& path_);

    // Read/Write
    static inline bool mkdirPath(const std::string& dirPath_);
    static inline bool deleteFile(const std::string& filePath_);
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    static inline bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b;


  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
