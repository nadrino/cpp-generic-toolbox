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
    static bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    static bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, std::string> strMap;
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b{};

    static bool isTidLike(const std::string& name_);
    static std::string lookForTidInSubFolders(const std::string& folderPath_, int maxDepth_ = 5);
    static uint8_t* getIconFromTitleId(const std::string& titleId_);
  }

  namespace UI{
    static std::string openKeyboardUi(const std::string &defaultStr_ = "");
  }

  // Printout
  namespace Terminal{
    static void printRight(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static void printLeft(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static void printLeftRight(const std::string& inputLeft_, const std::string& inputRight_, const std::string& color_ = "", bool flush_ = false);

    static void makePause(const std::string& message_ = "");
    template<typename T, typename TT> static void displayProgressBar( const T& iCurrent_, const TT& iTotal_,
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

    static u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_);
    static std::string getMemoryUsageStr(PhysicalMemoryOf of_);

    static int getTerminalWidth();
    static int getTerminalHeight();

  }

}

#endif

#include "implementation/GenericToolbox.Switch.impl.h"


#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_SWITCH_H
