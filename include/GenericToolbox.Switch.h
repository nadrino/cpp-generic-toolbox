//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H

#ifdef __SWITCH__

#include "switch.h"

namespace GenericToolbox::Switch{

  // IO
  namespace IO{

    struct Cache{
      Cache() = default;

      inline void initFileSystemBuffer(){
        if( this->fsBuffer != nullptr ){ return; }
        fsdevMountSdmc();
        this->fsBuffer = fsdevGetDeviceFileSystem("sdmc");
      }

      FsFileSystem* fsBuffer{nullptr};


    };
    static Cache c;

    static inline void initFileSystemBuffer();

    static inline void doPathIsFile(const std::string& path_);
    static inline void deleteFile(const std::string& filePath_);
  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
