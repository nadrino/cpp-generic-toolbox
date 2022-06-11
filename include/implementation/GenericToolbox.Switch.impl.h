//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H

#ifdef __SWITCH__

namespace GenericToolbox {
  namespace Switch{

    namespace IO{

      static inline bool doPathIsFile(const std::string& path_){
        bool result{false};
        FsFile fs_FileBuffer;
        char fs_pathBuffer[FS_MAX_PATH];

        snprintf(fs_pathBuffer, FS_MAX_PATH, "%s", path_.c_str());
        if(R_SUCCEEDED(fsFsOpenFile(c.fsBuffer, &fs_pathBuffer[0], FsOpenMode_Read, &fs_FileBuffer))) {
          result = true;
        }
        fsFileClose(&fs_FileBuffer);
        return result;
      }
      static inline void deleteFile(const std::string& filePath_){
        fsFsDeleteFile(c.fsBuffer, filePath_.c_str());
      }

    }

  }
}

#endif


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_IMPL_H
