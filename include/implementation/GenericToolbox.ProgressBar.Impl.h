//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_IMPL_H

#include <chrono>
#include <sstream>
#include <string>
#include <cmath>

namespace GenericToolbox{

  ProgressBar::ProgressBar() = default;
  ProgressBar::~ProgressBar() = default;

  template<typename T, typename TT> std::string ProgressBar::generateProgressBarStr( T iCurrent_, TT iTotal_, const std::string &title_ ){
    return this->_generateProgressBarStr(double(iCurrent_), double(iTotal_), title_);
  }
  template<typename T, typename TT> bool ProgressBar::showProgressBar(T iCurrent_, TT iTotal_){

    //    if( // Only the main thread
    //      this->_selectedThreadId_ != std::this_thread::get_id()
    //      ){
    //      return false;
    //    }

    _deltaTimeMilliSec_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - this->_lastDisplayedTimePoint_
    ).count();
    if( // REQUIRED TO PRINTOUT
        iCurrent_ == 0 // First call
        or this->_lastDisplayedPercentValue_ == -1 // never printed before
        or _deltaTimeMilliSec_ >= long(this->_refreshRateInMilliSec_)
        or iCurrent_ + 1 >= T(iTotal_) // last entry (mandatory to print at least once: need to print endl)
        ){

      int percent = int(std::round(double(iCurrent_) / double(iTotal_) * 100.));

      if( percent >= 100 ){ percent = 100; }
      else if( percent < 0) percent = 0;

      if( // EXCLUSION CASES
          percent == this->_lastDisplayedPercentValue_ // already printed
          ){
        if( this->_debugMode_ ){
          std::cout << "Print PBar NOT Ok:" << std::endl;
          std::cout << "percent == this->lastDisplayedPercentValue" << std::endl;
        }
        return false;
      }

      if( this->_debugMode_ ){
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
  template<typename T, typename TT> std::string ProgressBar::getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_ ){
    if(forcePrint_ or this->showProgressBar(iCurrent_, iTotal_) ){
      return this->generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
    return {};
  }
  template<typename T, typename TT> void ProgressBar::displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
    if(forcePrint_ or this->showProgressBar(iCurrent_, iTotal_) ){
      (*this->_outputStreamPtr_) << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
  }
  
  void ProgressBar::resetLastDisplayedValue(){
    this->_lastDisplayedValue_ = -1;
    this->_lastDisplayedPercentValue_ = -1;
  }

  inline std::string ProgressBar::_generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_){

    _lastDisplayedPercentValue_ = int(std::round(iCurrent_ / iTotal_ * 100.));
    if     (_lastDisplayedPercentValue_ > 100 ){ _lastDisplayedPercentValue_ = 100; }
    else if(_lastDisplayedPercentValue_ < 0   ){ _lastDisplayedPercentValue_ = 0;   }

    std::stringstream ssPrefix;
    if( not title_.empty() ) ssPrefix << title_ << " ";

    std::stringstream ssTail;
    ssTail << GenericToolbox::padString(std::to_string(_lastDisplayedPercentValue_), 3, ' ') << "%";

    _timePointBuffer_ = std::chrono::high_resolution_clock::now();
    if(_displaySpeed_){

      // How much time since last call?
      _timeIntervalBuffer_ = double(std::chrono::duration_cast<std::chrono::milliseconds>(_timePointBuffer_ - _lastDisplayedTimePoint_).count()) / 1000.;
      if( _timeIntervalBuffer_ != 0 ){
        // How much iterations since last call?
        _lastDisplayedSpeed_ = iCurrent_ - _lastDisplayedValue_;
        // Count per second
        _lastDisplayedSpeed_ /= _timeIntervalBuffer_;
      }

      // When iCurrent go back to zero -> _lastDisplayedValue_ makes no sense
      if( _lastDisplayedSpeed_ < 0 ) _lastDisplayedSpeed_ = 0;

      ssTail << " (";
      ssTail << GenericToolbox::padString(GenericToolbox::parseIntAsString(int(_lastDisplayedSpeed_)), 5, ' ');
      ssTail << " it/s)";
    }
    _lastDisplayedValue_ = iCurrent_;
    _lastDisplayedTimePoint_ = _timePointBuffer_;


    // test if the bar is too wide wrt the prompt width
    int displayedBarLength = _maxBarLength_;
    int termWidth = 0;
#ifndef CPP_GENERIC_TOOLBOX_BATCH
    termWidth = GenericToolbox::getTerminalWidth();
#endif
    if( termWidth > 0 ) { // terminal width is measurable

      size_t lastLinePos = ssPrefix.str().find_last_of('\n');
      if( lastLinePos == -1 ) lastLinePos = 0;
      size_t totalBarLength = GenericToolbox::getPrintSize(ssPrefix.str().substr(lastLinePos));
      if(displayedBarLength > 0) {
        totalBarLength += 2; // []
        totalBarLength += displayedBarLength;
        totalBarLength += 1; // space before tail
      }
      totalBarLength += ssTail.str().size();
      totalBarLength += 1; // 1 extra space is necessary to std::endl

      int remainingSpaces = termWidth;
      remainingSpaces -= int(totalBarLength);

      if( remainingSpaces < 0 ){
        if( displayedBarLength >= 0 ){
          // ok, can take some extra space in the bar
          displayedBarLength -= std::abs(remainingSpaces);
          if (displayedBarLength < 12) {
            displayedBarLength = 0;
            remainingSpaces += 2; // get back the [] of the pBar
          }
          remainingSpaces += (this->_maxBarLength_ - displayedBarLength );
        }
      }

      // if it's still to big, cut the title
      if ( remainingSpaces < 0) {
        std::string cutPrefix = ssPrefix.str().substr(0, int(ssPrefix.str().size()) - std::abs(remainingSpaces) - 3);
        ssPrefix.str("");
        ssPrefix << cutPrefix;
        ssPrefix << static_cast<char>(27) << "[0m" << "...";
      }
    }
    else {
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
      padProgressBar += repeatString(" ", nbSpaces);

      if( _enableRainbowProgressBar_ ){
        padProgressBar = GenericToolbox::makeRainbowString(padProgressBar, false);
      }

      ssProgressBar << "[" << padProgressBar << "] ";
    }

    ssProgressBar << ssTail.str();

    ssProgressBar << std::endl; // always jump line to force flush on screen
    auto nLines = GenericToolbox::getNLines(ssProgressBar.str());
#ifndef CPP_GENERIC_TOOLBOX_BATCH
    if(_lastDisplayedPercentValue_ != 100 ){
      // those commands won't be flushed until a new print is called:
      // pull back to cursor on the line of the progress bar
      ssProgressBar << static_cast<char>(27) << "[" << nLines-1 << "F";
      // Clear the line and add "\r" since a logger (in charge of printing this string)
      // might intercept it to trigger a print of a line header
      ssProgressBar << static_cast<char>(27) << "[1K" << "\r"; // trick to clear
    }
#endif

    if( this->_debugMode_ ){
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


}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_IMPL_H
