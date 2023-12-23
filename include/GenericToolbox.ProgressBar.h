//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_PROGRESS_BAR_H
#define CPP_GENERIC_TOOLBOX_PROGRESS_BAR_H

#include "GenericToolbox.String.h"
#include "GenericToolbox.Os.h"

#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <cstddef>


#ifndef PROGRESS_BAR_ENABLE_RAINBOW
#define PROGRESS_BAR_ENABLE_RAINBOW 0
#endif

#ifndef PROGRESS_BAR_LENGTH
#define PROGRESS_BAR_LENGTH 36
#endif

#ifndef PROGRESS_BAR_SHOW_SPEED
#define PROGRESS_BAR_SHOW_SPEED 1
#endif

#ifndef PROGRESS_BAR_REFRESH_DURATION_IN_MS
// 33 ms per frame = 0.033 seconds per frame = 1/30 sec per frame = 30 fps
#define PROGRESS_BAR_REFRESH_DURATION_IN_MS 33
#endif

#ifndef PROGRESS_BAR_FILL_TAG
// multi-char is possible
#define PROGRESS_BAR_FILL_TAG "#"
#endif



namespace GenericToolbox{

  template<typename T, typename TT> inline static void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> inline static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> inline static std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> inline static bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  inline static void resetLastDisplayedValue();
  inline static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");

  namespace ProgressBar{
    class ProgressBar{

    public:
      inline ProgressBar();
      inline virtual ~ProgressBar();

      template<typename T, typename TT> inline std::string generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "");
      template<typename T, typename TT> inline bool showProgressBar(T iCurrent_, TT iTotal_);
      template<typename T, typename TT> inline std::string getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
      template<typename T, typename TT> inline void displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);

      inline void resetLastDisplayedValue();

      inline void setEnableRainbowProgressBar(bool enableRainbowProgressBar);
      inline void setMaxBarLength(int maxBarLength);
      inline void setDisableVt100Cmd(bool disableVt100Cmd_);

    protected:
      inline std::string _generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_);

    private:
      bool _debugMode_{false};
      bool _disableVt100Cmd_{false};
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

    static ProgressBar gProgressBar;
  }

}


namespace GenericToolbox {

  template<typename T, typename TT> inline static std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){
    return ProgressBar::gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, title_);
  }
  template<typename T, typename TT> inline static bool showProgressBar(const T& iCurrent_, const TT& iTotal_){
    return ProgressBar::gProgressBar.template showProgressBar(iCurrent_, iTotal_);
  }
  template<typename T, typename TT> inline static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    return ProgressBar::gProgressBar.template getProgressBarStr(iCurrent_, iTotal_, title_, forcePrint_);
  }
  template<typename T, typename TT> inline static void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    return ProgressBar::gProgressBar.template displayProgressBar(iCurrent_, iTotal_, title_, forcePrint_);
  }
  inline static void resetLastDisplayedValue(){
    ProgressBar::gProgressBar.resetLastDisplayedValue();
  }

  namespace ProgressBar{

    ProgressBar::ProgressBar() = default;

    ProgressBar::~ProgressBar() = default;

    template<typename T, typename TT>
    std::string ProgressBar::generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_) {
      return this->_generateProgressBarStr(double(iCurrent_), double(iTotal_), title_);
    }

    template<typename T, typename TT>
    bool ProgressBar::showProgressBar(T iCurrent_, TT iTotal_) {

      //    if( // Only the main thread
      //      this->_selectedThreadId_ != std::this_thread::get_id()
      //      ){
      //      return false;
      //    }

      _deltaTimeMilliSec_ = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - this->_lastDisplayedTimePoint_
      ).count();
      if ( // REQUIRED TO PRINTOUT
          iCurrent_ == 0 // First call
          or this->_lastDisplayedPercentValue_ == -1 // never printed before
          or _deltaTimeMilliSec_ >= long(this->_refreshRateInMilliSec_)
          or iCurrent_ + 1 >= T(iTotal_) // last entry (mandatory to print at least once: need to print endl)
          ) {

        int percent = int(std::round(double(iCurrent_) / double(iTotal_) * 100.));

        if (percent >= 100) { percent = 100; }
        else if (percent < 0) percent = 0;

        if ( // EXCLUSION CASES
            percent == this->_lastDisplayedPercentValue_ // already printed
            ) {
          if (this->_debugMode_) {
            std::cout << "Print PBar NOT Ok:" << std::endl;
            std::cout << "percent == this->lastDisplayedPercentValue" << std::endl;
          }
          return false;
        }

        if (this->_debugMode_) {
          std::cout << "Print PBar Ok:" << std::endl;
          std::cout << "percent = " << percent << std::endl;
          std::cout << "iCurrent_ = " << iCurrent_ << std::endl;
          std::cout << "iTotal_ = " << iTotal_ << std::endl;
          std::cout << "this->lastDisplayedPercentValue = " << this->_lastDisplayedPercentValue_ << std::endl;
          std::cout << "this->refreshRateInMilliSec = " << this->_refreshRateInMilliSec_ << std::endl;
        }

        // OK!
        return true;
      }

      return false;
    }

    template<typename T, typename TT>
    std::string ProgressBar::getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        return this->generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
      return {};
    }

    template<typename T, typename TT>
    void ProgressBar::displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        (*this->_outputStreamPtr_) << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
    }

    void ProgressBar::resetLastDisplayedValue() {
      this->_lastDisplayedValue_ = -1;
      this->_lastDisplayedPercentValue_ = -1;
    }

    inline std::string ProgressBar::_generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_) {

      _lastDisplayedPercentValue_ = int(std::round(iCurrent_ / iTotal_ * 100.));
      if (_lastDisplayedPercentValue_ > 100) { _lastDisplayedPercentValue_ = 100; }
      else if (_lastDisplayedPercentValue_ < 0) { _lastDisplayedPercentValue_ = 0; }

      int termWidth{0};
#ifndef CPP_GENERIC_TOOLBOX_BATCH
      // this returns non-zero value if it measurable
      termWidth = GenericToolbox::getTerminalWidth();
#endif

      std::stringstream ssPrefix;

      if (not title_.empty()){
        auto titleLines = GenericToolbox::splitString(title_, "\n");

        for( size_t iLine = 0 ; iLine < titleLines.size()-1 ; iLine++ ){
          if( termWidth == 0 ){
            // print the whole line
            ssPrefix << titleLines[iLine] << std::endl;
          }
          else{
            // print the line in the available space
            std::string buffer{titleLines[iLine]};
            while( int(GenericToolbox::getPrintSize(buffer)) >= termWidth ){
              // removing the last character and test if the printed size fit the terminal window.
              buffer = buffer.substr(0, buffer.size()-1);
            }
            ssPrefix << buffer << std::endl;
          }
        }

        ssPrefix << titleLines.back() << " ";
      }

      std::stringstream ssTail;
      ssTail << GenericToolbox::padString(std::to_string(_lastDisplayedPercentValue_), 3, ' ') << "%";

      _timePointBuffer_ = std::chrono::high_resolution_clock::now();
      if (_displaySpeed_) {

        // How much time since last call?
        _timeIntervalBuffer_ = double(std::chrono::duration_cast<std::chrono::milliseconds>(
            _timePointBuffer_ - _lastDisplayedTimePoint_).count()) / 1000.;
        if (_timeIntervalBuffer_ != 0) {
          // How much iterations since last call?
          _lastDisplayedSpeed_ = iCurrent_ - _lastDisplayedValue_;
          // Count per second
          _lastDisplayedSpeed_ /= _timeIntervalBuffer_;
        }

        // When iCurrent go back to zero -> _lastDisplayedValue_ makes no sense
        if (_lastDisplayedSpeed_ < 0) _lastDisplayedSpeed_ = 0;

        ssTail << " (";
        ssTail << GenericToolbox::padString(GenericToolbox::parseIntAsString(int(_lastDisplayedSpeed_)), 5, ' ');
        ssTail << " it/s)";
      }
      _lastDisplayedValue_ = int( iCurrent_ );
      _lastDisplayedTimePoint_ = _timePointBuffer_;


      // test if the bar is too wide wrt the prompt width
      int displayedBarLength = _maxBarLength_;
      if (termWidth > 0) { // terminal width is measurable

        size_t lastLinePos = ssPrefix.str().find_last_of('\n');
        if (lastLinePos == size_t(-1)) lastLinePos = 0;

        size_t lastLineLength = GenericToolbox::getPrintSize(ssPrefix.str().substr(lastLinePos));
        if (displayedBarLength > 0) {
          lastLineLength += 2; // []
          lastLineLength += displayedBarLength;
          lastLineLength += 1; // space before tail
        }
        lastLineLength += ssTail.str().size();
        lastLineLength += 1; // 1 extra space is necessary to std::endl

        int remainingSpaces = termWidth;
        remainingSpaces -= int(lastLineLength);

        if (remainingSpaces < 0) {
          if (displayedBarLength >= 0) {
            // ok, can take some extra space in the bar
            displayedBarLength -= std::abs(remainingSpaces);
            if (displayedBarLength < 12) {
              displayedBarLength = 0;
              remainingSpaces += 2; // get back the [] of the pBar
            }
            remainingSpaces += (this->_maxBarLength_ - displayedBarLength);
          }
        }

        // if it's still to big, cut the title
        if (remainingSpaces < 0) {
          std::string cutPrefix = ssPrefix.str().substr(0, int(ssPrefix.str().size()) - std::abs(remainingSpaces) - 3);
          ssPrefix.str("");
          ssPrefix << cutPrefix;
          ssPrefix << static_cast<char>(27) << "[0m" << "...";
        }
      } else {
        displayedBarLength = 0;
      }

      std::stringstream ssProgressBar;
      ssProgressBar << ssPrefix.str();

      if (displayedBarLength > 0) {
        int nbTags = _lastDisplayedPercentValue_ * displayedBarLength / 100;
        int nbSpaces = displayedBarLength - nbTags;

        std::string padProgressBar;
        for (int iTag = 0; iTag < nbTags; iTag++) {
          padProgressBar += this->_fillTag_[iTag % this->_fillTag_.size()];
        }
        padProgressBar += GenericToolbox::repeatString(" ", nbSpaces);

        if (_enableRainbowProgressBar_) {
          padProgressBar = GenericToolbox::makeRainbowString(padProgressBar, false);
        }

        ssProgressBar << "[" << padProgressBar << "] ";
      }

      ssProgressBar << ssTail.str();

      if( not _disableVt100Cmd_ ){
        ssProgressBar << std::endl; // always jump line to force flush on screen
#ifndef CPP_GENERIC_TOOLBOX_BATCH
        auto nLines = GenericToolbox::getNLines(ssProgressBar.str());
        if (_lastDisplayedPercentValue_ != 100) {
          // those commands won't be flushed until a new print is called:
          // pull back to cursor on the line of the progress bar
          ssProgressBar << static_cast<char>(27) << "[" << nLines - 1 << "F";
          // Clear the line and add "\r" since a logger (in charge of printing this string)
          // might intercept it to trigger a print of a line header
          ssProgressBar << static_cast<char>(27) << "[1K" << "\r"; // trick to clear
        }
#endif
      }


      if (this->_debugMode_) {
        std::cout << "New timestamp: " << this->_lastDisplayedTimePoint_.time_since_epoch().count() << std::endl;
        std::cout << "this->lastDisplayedValue: " << this->_lastDisplayedValue_ << std::endl;
        std::cout << "this->lastDisplayedPercentValue: " << this->_lastDisplayedPercentValue_ << std::endl;
      }

      return ssProgressBar.str();
    }

    void ProgressBar::setEnableRainbowProgressBar(bool enableRainbowProgressBar) {
      _enableRainbowProgressBar_ = enableRainbowProgressBar;
    }
    void ProgressBar::setMaxBarLength(int maxBarLength) {
      _maxBarLength_ = maxBarLength;
    }
    void ProgressBar::setDisableVt100Cmd(bool disableVt100Cmd_){
      _disableVt100Cmd_ = disableVt100Cmd_;
    }

  }

  inline static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

    auto anchorTimePoint = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds totalDurationToWait(nbMilliSecToWait_*1000);
    std::chrono::microseconds cumulatedDuration(0);
    std::chrono::microseconds loopUpdateMaxFrequency(nbMilliSecToWait_); // 1000x faster than the whole time

    GenericToolbox::displayProgressBar( 0, totalDurationToWait.count(), progressTitle_);
    while( true ){
      std::this_thread::sleep_for( loopUpdateMaxFrequency );
      cumulatedDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - anchorTimePoint);
      if( cumulatedDuration >= totalDurationToWait ){
        return;
      }
      else{
        GenericToolbox::displayProgressBar( cumulatedDuration.count(), totalDurationToWait.count(), progressTitle_);
      }
    }
    GenericToolbox::displayProgressBar( totalDurationToWait.count(), totalDurationToWait.count(), progressTitle_);

  }

}


#endif // CPP_GENERIC_TOOLBOX_PROGRESS_BAR_H
