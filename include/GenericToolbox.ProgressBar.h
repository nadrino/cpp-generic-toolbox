//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H

#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <cstddef>

#include "implementation/GenericToolbox.param.h"


namespace GenericToolbox{

  class ProgressBar{

  public:
    inline ProgressBar();
    inline virtual ~ProgressBar();

    template<typename T, typename TT> inline std::string generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ );
    template<typename T, typename TT> inline bool showProgressBar(T iCurrent_, TT iTotal_);
    template<typename T, typename TT> inline std::string getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_ );
    template<typename T, typename TT> inline void displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_);

    inline void resetLastDisplayedValue();

    inline void setEnableRainbowProgressBar(bool enableRainbowProgressBar);
    inline void setMaxBarLength(int maxBarLength);

  protected:
    inline std::string _generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_);

  private:
    bool _debugMode_{false};
    bool _enableRainbowProgressBar_{PROGRESS_BAR_ENABLE_RAINBOW};
    bool _displaySpeed_{PROGRESS_BAR_SHOW_SPEED};
    int _maxBarLength_{PROGRESS_BAR_LENGTH};
    size_t _refreshRateInMilliSec_{PROGRESS_BAR_REFRESH_DURATION_IN_MS};
    std::string _fillTag_{PROGRESS_BAR_FILL_TAG};
    std::ostream* _outputStreamPtr_{&std::cout};

    int _lastDisplayedPercentValue_{-1};
    int _lastDisplayedValue_ = {-1};
    double _lastDisplayedSpeed_{0};
    std::chrono::high_resolution_clock::time_point _lastDisplayedTimePoint_{std::chrono::high_resolution_clock::now()};
    std::thread::id _selectedThreadId_ = std::this_thread::get_id(); // get the main thread id

    //buffers
    std::chrono::high_resolution_clock::time_point _timePointBuffer_{};
    long _deltaTimeMilliSec_{};
    double _timeIntervalBuffer_{};

  };

  inline static ProgressBar gProgressBar;

}

#include "implementation/GenericToolbox.ProgressBar.Impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_H
