//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H

#ifdef __SWITCH__

#include "GenericToolbox.h"

#include "switch.h"
#include "zlib.h"

#include <map>
#include <string>


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
    inline static bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    inline static bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, std::string> strMap;
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b{};

    inline static bool isTidLike(const std::string& name_);
    inline static std::string lookForTidInSubFolders(const std::string& folderPath_, int maxDepth_ = 5);
    inline static uint8_t* getIconFromTitleId(const std::string& titleId_);
  }

  namespace UI{
    inline static std::string openKeyboardUi(const std::string &defaultStr_ = "");
  }

  // Printout
  namespace Terminal{
    inline static void printRight(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    inline static void printLeft(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    inline static void printLeftRight(const std::string& inputLeft_, const std::string& inputRight_, const std::string& color_ = "", bool flush_ = false);

    inline static void makePause(const std::string& message_ = "");
    template<typename T, typename TT> inline static void displayProgressBar( const T& iCurrent_, const TT& iTotal_,
        const std::string &title_ = "", bool forcePrint_ = false, const std::string& color_="");

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

    inline static u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_);
    inline static std::string getMemoryUsageStr(PhysicalMemoryOf of_);

    inline static int getTerminalWidth();
    inline static int getTerminalHeight();

  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
