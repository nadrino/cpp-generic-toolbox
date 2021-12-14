//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H

#include "implementation/GenericToolbox.param.h"

#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <cstddef>

namespace GenericToolbox{

  class ProgressBar{

  public:
    inline ProgressBar();
    inline virtual ~ProgressBar();

    template<typename T, typename TT> inline std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ );
    template<typename T, typename TT> inline bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
    template<typename T, typename TT> inline std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ );
    template<typename T, typename TT> inline void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_);

    void resetLastDisplayedValue();


  private:
    bool debugMode{false};
    bool enableRainbowProgressBar{PROGRESS_BAR_ENABLE_RAINBOW};
    bool displaySpeed{PROGRESS_BAR_SHOW_SPEED};
    int maxBarLength{PROGRESS_BAR_LENGTH};
    size_t refreshRateInMilliSec{PROGRESS_BAR_REFRESH_DURATION_IN_MS};
    std::string fillTag{PROGRESS_BAR_FILL_TAG};
    std::ostream* outputStreamPtr{&std::cout};

    int lastDisplayedPercentValue{-1};
    int lastDisplayedValue = {-1};
    double lastDisplayedSpeed{0};
    std::chrono::high_resolution_clock::time_point lastDisplayedTimePoint{std::chrono::high_resolution_clock::now()};
    std::thread::id _selectedThreadId_ = std::this_thread::get_id(); // get the main thread id

  };

}

#include "implementation/GenericToolbox.ProgressBar.Impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H
