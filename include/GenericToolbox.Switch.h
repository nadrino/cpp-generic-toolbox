//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H

#ifdef __SWITCH__

#include "GenericToolbox.h"

#include "switch.h"
#include "zlib.h"

#include "map"
#include "string"

namespace GenericToolbox::Switch{

  // IO
  namespace IO{

    struct ParametersHolder{
      bool useCrcCheck{true};
      static const size_t maxBufferSize{0x200000}; // 2 MB
      static const size_t minBufferSize{0x10000}; // 65 KB
    };
    static ParametersHolder p{};

    // Read/Write
    static inline bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    static inline bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
    static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, std::string> strMap;
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b{};

    static inline std::string lookForTidInSubFolders(const std::string& folderPath_);
    static inline uint8_t* getFolderIconFromTitleId(const std::string& titleId_);
  }

  namespace UI{
    static inline std::string openKeyboardUi(const std::string &defaultStr_ = "");
  }

  // Printout
  namespace Terminal{
    static inline void printRight(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static inline void printLeft(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static inline void printLeftRight(const std::string& input_left_, const std::string& input_right_, const std::string& color_ = "");

    static inline void makePause();
    template<typename T, typename TT> static inline void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);

  }

  // Hardware
  namespace Hardware{
    ENUM_EXPANDER(
        PhysicalMemoryType, 0,
        TotalPhysicalMemorySize,
        UsedPhysicalMemorySize
    )

    ENUM_EXPANDER(
        PhysicalMemoryOf, 0,
        Application,
        Applet,
        System,
        SystemUnsafe
        )

    static inline u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_);
    static inline std::string getMemoryUsageStr(PhysicalMemoryOf of_);

    static inline int getTerminalWidth();
    static inline int getTerminalHeight();

  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
