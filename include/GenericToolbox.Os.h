//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_OS_H
#define CPP_GENERIC_TOOLBOX_OS_H

#include "GenericToolbox.Build.h"

#include "GenericToolbox.String.h"

#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <array>

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <climits>
#include <unistd.h>
#include <unistd.h>
#include <pwd.h>

#ifdef __SWITCH__
#include <switch.h>
struct utsname {
  char sysname[1];
  char nodename[1];
  char release[1];
  char version[1];
  char machine[1];
};
static int uname(struct utsname *buf) { return 0; }
#else
#include <sys/utsname.h>
#endif


// ***************************
//! Operating System Tools
// ***************************


// Declaration section
namespace GenericToolbox{

  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getHomeDirectory();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getCurrentWorkingDirectory();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string expandEnvironmentVariables(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getExecutableName(); // untested on windows platform
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getUserName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getHostName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsVersion();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsArchitecture();

  // hardware
  CPP_GENERIC_TOOLBOX_OS_STATIC size_t getProcessMemoryUsage();
  CPP_GENERIC_TOOLBOX_OS_STATIC size_t getProcessMaxMemoryUsage();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getCpuUsageByProcess();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getCpuNbAvailableCores();
  CPP_GENERIC_TOOLBOX_OS_STATIC long getProcessMemoryUsageDiffSinceLastCall();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getFreeDiskSpacePercent( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC unsigned long long getFreeDiskSpace( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC unsigned long long getTotalDiskSpace( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC int getTerminalWidth();
  CPP_GENERIC_TOOLBOX_OS_STATIC int getTerminalHeight();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);

}


// Implementation section
#if defined(_WIN32)
// Windows
#include <windows.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <sys/times.h>
#include <mach/mach.h>
#include <unistd.h>
#include <array>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/times.h>
#include <sys/statvfs.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/times.h>

#else
// Unsupported
#endif

extern char* __progname;


namespace GenericToolbox{

  namespace Internals {
    CPP_GENERIC_TOOLBOX_OS_STATIC char * getEnvironmentVariable(char const envVarName_[]);
    CPP_GENERIC_TOOLBOX_OS_STATIC bool expandEnvironmentVariables(const char *fname, char *xname);
    namespace Hardware{
      #ifdef CPP_GENERIC_TOOLBOX_OS_COMPILED
extern size_t lastProcessMemoryUsage;
#else
static size_t lastProcessMemoryUsage = 0;
#endif
    }
  }

  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getHomeDirectory();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getCurrentWorkingDirectory();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string expandEnvironmentVariables(const std::string &filePath_);
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getExecutableName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getUserName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getHostName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsName();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsVersion();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::string getOsArchitecture();

  struct CpuStat{
    inline CpuStat(){ this->getCpuUsageByProcess(); }
    clock_t lastCPU{}, lastSysCPU{}, lastUserCPU{};
    inline double getCpuUsageByProcess(){
      double percent{0};
#if defined(_WIN32)
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) || defined(__APPLE__) && defined(__MACH__)
      struct tms timeSample{};
      clock_t now;

      now = times(&timeSample);
      if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
          timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
      }
      else{
        percent = double(timeSample.tms_stime - lastSysCPU) +
                  double(timeSample.tms_utime - lastUserCPU);
        percent /= double(now - lastCPU);
        percent *= 100;
      }
      lastCPU = now;
      lastSysCPU = timeSample.tms_stime;
      lastUserCPU = timeSample.tms_utime;
#endif
      return percent;
    }
  };
  #ifdef CPP_GENERIC_TOOLBOX_OS_COMPILED
extern CpuStat cs;
#else
static CpuStat cs{};
#endif

  CPP_GENERIC_TOOLBOX_OS_STATIC size_t getProcessMemoryUsage();
  CPP_GENERIC_TOOLBOX_OS_STATIC size_t getProcessMaxMemoryUsage();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getCpuUsageByProcess();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getCpuNbAvailableCores();
  CPP_GENERIC_TOOLBOX_OS_STATIC long getProcessMemoryUsageDiffSinceLastCall();
  CPP_GENERIC_TOOLBOX_OS_STATIC double getFreeDiskSpacePercent( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC unsigned long long getFreeDiskSpace( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC unsigned long long getTotalDiskSpace( const std::string& path_ );
  CPP_GENERIC_TOOLBOX_OS_STATIC int getTerminalWidth();
  CPP_GENERIC_TOOLBOX_OS_STATIC int getTerminalHeight();
  CPP_GENERIC_TOOLBOX_OS_STATIC std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_);


}


#ifndef CPP_GENERIC_TOOLBOX_OS_COMPILED
#include "implementation/GenericToolbox.Os.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_OS_H
