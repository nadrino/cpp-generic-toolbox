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

      static const size_t maxBufferSize{0x200000}; // 2 MB
      static const size_t minBufferSize{0x10000}; // 65 KB
    };
    static ParametersHolder p{};

    // Read/Write
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    static inline bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b{};

    static inline std::string lookForTidInSubFolders(const std::string& folderPath_);
    static inline uint8_t* getFolderIconFromTitleId(const std::string& titleId_);
  }

  // Printout
  namespace Printout{

  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
