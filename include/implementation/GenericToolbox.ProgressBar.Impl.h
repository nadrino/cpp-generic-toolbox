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

  template<typename T, typename TT> std::string ProgressBar::generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){

    int percentValue = int(std::round(double(iCurrent_) / double(iTotal_) * 100.));
    if( percentValue > 100 ){
      percentValue = 100;
    }
    else if( percentValue < 0 ){
      percentValue = 0;
    }

    std::stringstream ssPrefix;
    if( not title_.empty() ) ssPrefix << title_ << " ";

    std::stringstream ssTail;
    ssTail << GenericToolbox::padString(std::to_string(percentValue), 3, ' ') << "%";

    std::chrono::high_resolution_clock::time_point newTimePoint = std::chrono::high_resolution_clock::now();
    if (this->displaySpeed) {
      ssTail << " (";
      double itPerSec =
          double(iCurrent_) - this->lastDisplayedValue; // nb iterations since last print
      double timeInterval;
      if (int(itPerSec) < 0) itPerSec = 0;
      else {
        timeInterval = double(std::chrono::duration_cast<std::chrono::milliseconds>(
            newTimePoint - this->lastDisplayedTimePoint
        ).count()) / 1000.;
        if( timeInterval != 0 ){
          itPerSec /= timeInterval; // Count per s
          this->lastDisplayedSpeed = itPerSec;
        }
        else itPerSec = this->lastDisplayedSpeed;
      }
      ssTail << GenericToolbox::padString(GenericToolbox::parseIntAsString(int(itPerSec)), 5, ' ');
      ssTail << " it/s)";
    }



    // test if the bar is too wide wrt the prompt width
    int displayedBarLength = this->maxBarLength;

    auto termWidth = GenericToolbox::getTerminalWidth();
    if( termWidth != 0 ) { // terminal width is measurable

      size_t totalBarLength = GenericToolbox::getPrintSize(ssPrefix.str());
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
          remainingSpaces += (this->maxBarLength - displayedBarLength );
        }
      }

      // if it's still to big, cut the title
      if ( remainingSpaces < 0) {
        std::string cutPrefix = ssPrefix.str().substr(0, int(ssPrefix.str().size()) - std::abs(remainingSpaces) - 3);
        ssPrefix.str("");
        ssPrefix << cutPrefix;
        ssPrefix << "\033[0m" << "...";
      }
    }
    else {
      displayedBarLength = 0;
    }

    std::stringstream ssProgressBar;
    ssProgressBar << ssPrefix.str();

    if (displayedBarLength > 0) {
      int nbTags = percentValue * displayedBarLength / 100;
      int nbSpaces = displayedBarLength - nbTags;

      std::string padProgressBar;
      for (int iTag = 0; iTag < nbTags; iTag++) {
        padProgressBar += this->fillTag[iTag%this->fillTag.size()];
      }
      padProgressBar += repeatString(" ", nbSpaces);

      if( this->enableRainbowProgressBar ){
        padProgressBar = GenericToolbox::makeRainbowString(padProgressBar, false);
      }

      ssProgressBar << "[" << padProgressBar << "] ";
    }

    ssProgressBar << ssTail.str();

    ssProgressBar << std::endl; // always jump line to force flush on screen
    if( percentValue != 100 ){
      // those commands won't be flushed until a new print is called:
      // pull back to cursor on the line of the progress bar
      ssProgressBar << static_cast<char>(27) << "[1;1F";
      // Clear the line and add "\r" since a Logger might intercept it to trigger a print of a line header
      ssProgressBar << static_cast<char>(27) << "[1K" << "\r"; // trick to clear
    }

    this->lastDisplayedPercentValue = percentValue;
    this->lastDisplayedValue = iCurrent_;
    this->lastDisplayedTimePoint = newTimePoint;

    if( this->debugMode ){
      std::cout << "New timestamp: " << this->lastDisplayedTimePoint.time_since_epoch().count() << std::endl;
      std::cout << "this->lastDisplayedValue: " << this->lastDisplayedValue << std::endl;
      std::cout << "this->lastDisplayedPercentValue: " << this->lastDisplayedPercentValue << std::endl;
    }

    return ssProgressBar.str();

  }
  template<typename T, typename TT> bool ProgressBar::showProgressBar(const T& iCurrent_, const TT& iTotal_){

    //    if( // Only the main thread
    //      this->_selectedThreadId_ != std::this_thread::get_id()
    //      ){
    //      return false;
    //    }

    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - this->lastDisplayedTimePoint
    ).count();
    if( // REQUIRED TO PRINTOUT
        iCurrent_ == 0 // First call
        or this->lastDisplayedPercentValue == -1 // never printed before
        or timeDiff >= this->refreshRateInMilliSec
        or iCurrent_ + 1 >= iTotal_ // last entry (mandatory to print at least once: need to print endl)
        ){

      int percent = int(std::round(double(iCurrent_) / double(iTotal_) * 100.));

      if( percent >= 100 ){ percent = 100; }
      else if( percent < 0) percent = 0;

      if( // EXCLUSION CASES
          percent == this->lastDisplayedPercentValue // already printed
          ){
        if( this->debugMode ){
          std::cout << "Print PBar NOT Ok:" << std::endl;
          std::cout << "percent == this->lastDisplayedPercentValue" << std::endl;
        }
        return false;
      }

      if( this->debugMode ){
        std::cout << "Print PBar Ok:" << std::endl;
        std::cout << "percent = " << percent << std::endl;
        std::cout << "iCurrent_ = " << iCurrent_ << std::endl;
        std::cout << "iTotal_ = " << iTotal_ << std::endl;
        std::cout << "this->lastDisplayedPercentValue = " << this->lastDisplayedPercentValue << std::endl;
        std::cout << "this->refreshRateInMilliSec = " << this->refreshRateInMilliSec << std::endl;
        std::cout << "timeDiff = " << timeDiff << std::endl;
      }

      // OK!
      return true;
    }

    return false;
  }
  template<typename T, typename TT> std::string ProgressBar::getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    if(forcePrint_ or this->showProgressBar(iCurrent_, iTotal_) ){
      return this->generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
    return {};
  }
  template<typename T, typename TT> void ProgressBar::displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    if(forcePrint_ or this->showProgressBar(iCurrent_, iTotal_) ){
      (*this->outputStreamPtr) << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
    }
  }
  void ProgressBar::resetLastDisplayedValue(){
    this->lastDisplayedValue = -1;
    this->lastDisplayedPercentValue = -1;
  }


}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_PROGRESS_BAR_IMPL_H
