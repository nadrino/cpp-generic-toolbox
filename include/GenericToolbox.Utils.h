//
// Created by Nadrino on 08/01/2024.
//

#ifndef CPP_GENERIC_TOOLBOX_UTILS_H
#define CPP_GENERIC_TOOLBOX_UTILS_H


#include "GenericToolbox.Wrappers.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.String.h"
#include "GenericToolbox.Os.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


#ifndef CPP_GENERIC_TOOLBOX_BATCH
#define GT_CHAR_LTCORN "┌"
#define GT_CHAR_LBCORN "└"
#define GT_CHAR_RTCORN "┐"
#define GT_CHAR_RBCORN "┘"
#define GT_CHAR_TRIGHT "├"
#define GT_CHAR_TLEFT "┤"
#define GT_CHAR_TTOP "┬"
#define GT_CHAR_TBOT "┴"
#define GT_CHAR_CROSS "┼"
#define GT_CHAR_HLINE "─"
#define GT_CHAR_VLINE "│"
#else
#define GT_CHAR_LTCORN "#"
#define GT_CHAR_LBCORN "#"
#define GT_CHAR_RTCORN "#"
#define GT_CHAR_RBCORN "#"
#define GT_CHAR_TRIGHT "|"
#define GT_CHAR_TLEFT "|"
#define GT_CHAR_TTOP "-"
#define GT_CHAR_TBOT "-"
#define GT_CHAR_CROSS "|"
#define GT_CHAR_HLINE "-"
#define GT_CHAR_VLINE "|"
#endif

// Declarations
namespace GenericToolbox{
  struct Range;
  class InitBaseClass;
  class ConfigBaseClass;
  template<class ConfigType> class ConfigClass;
  class ScopedGuard;
  class RawDataArray;
  class TablePrinter;
  class VariableMonitor;
  struct QuantityMonitor;
  class VariablesMonitor;
  class AnyType;
  namespace ProgressBar{ class ProgressBar; }
}


// Implementations

// ProgressBar
namespace GenericToolbox{

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

  template<typename T, typename TT> static void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> static std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> static bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  static void resetLastDisplayedValue();
  static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");

  namespace ProgressBar{
    class ProgressBar{

    public:
      inline ProgressBar() = default;
      inline virtual ~ProgressBar() = default;

      inline void setMaxBarLength(int maxBarLength){ _maxBarLength_ = maxBarLength; }
      inline void setDisableVt100Cmd(bool disableVt100Cmd_){ _disableVt100Cmd_ = disableVt100Cmd_; }
      inline void setEnableRainbowProgressBar(bool enableRainbowProgressBar){_enableRainbowProgressBar_ = enableRainbowProgressBar;}

      inline void resetLastDisplayedValue();

      template<typename T, typename TT> inline std::string generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "");
      template<typename T, typename TT> inline bool showProgressBar(T iCurrent_, TT iTotal_);
      template<typename T, typename TT> inline std::string getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
      template<typename T, typename TT> inline void displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);

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
      std::thread::id _selectedThreadId_{std::this_thread::get_id()}; // get the main thread id

      //buffers
      std::chrono::high_resolution_clock::time_point _timePointBuffer_{};
      long _deltaTimeMilliSec_{};
      double _timeIntervalBuffer_{};

    };

    static ProgressBar gProgressBar;

    template<typename T, typename TT> inline std::string ProgressBar::generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_) {
      return this->_generateProgressBarStr(double(iCurrent_), double(iTotal_), title_);
    }
    template<typename T, typename TT> inline bool ProgressBar::showProgressBar(T iCurrent_, TT iTotal_) {

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
    template<typename T, typename TT> inline std::string ProgressBar::getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        return this->generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
      return {};
    }
    template<typename T, typename TT> inline void ProgressBar::displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        (*this->_outputStreamPtr_) << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
    }
    inline void ProgressBar::resetLastDisplayedValue() {
      this->_lastDisplayedValue_ = -1;
      this->_lastDisplayedPercentValue_ = -1;
    }

    // protected
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
  }

  template<typename T, typename TT> static std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){
    return ProgressBar::gProgressBar.generateProgressBarStr(iCurrent_, iTotal_, title_);
  }
  template<typename T, typename TT> static bool showProgressBar(const T& iCurrent_, const TT& iTotal_){
    return ProgressBar::gProgressBar.showProgressBar(iCurrent_, iTotal_);
  }
  template<typename T, typename TT> static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    return ProgressBar::gProgressBar.getProgressBarStr(iCurrent_, iTotal_, title_, forcePrint_);
  }
  template<typename T, typename TT> static void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    return ProgressBar::gProgressBar.displayProgressBar(iCurrent_, iTotal_, title_, forcePrint_);
  }
  static void resetLastDisplayedValue(){
    ProgressBar::gProgressBar.resetLastDisplayedValue();
  }
  static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

    auto anchorTimePoint = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds totalDurationToWait(nbMilliSecToWait_*1000);
    std::chrono::microseconds cumulatedDuration(0);
    std::chrono::microseconds loopUpdateMaxFrequency(nbMilliSecToWait_); // 1000x faster than the whole time

    GenericToolbox::displayProgressBar( 0, totalDurationToWait.count(), progressTitle_);
    while( true ){
      std::this_thread::sleep_for( loopUpdateMaxFrequency );
      cumulatedDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - anchorTimePoint);
      if( cumulatedDuration >= totalDurationToWait ){ break; }
      else{ GenericToolbox::displayProgressBar( cumulatedDuration.count(), totalDurationToWait.count(), progressTitle_); }
    }
    GenericToolbox::displayProgressBar( totalDurationToWait.count(), totalDurationToWait.count(), progressTitle_);

  }

}


namespace GenericToolbox{
  struct Range{
    double min{std::nan("unset")};
    double max{std::nan("unset")};

    Range() = default;
    Range(double min_, double max_) : min(min_), max(max_){}
    void fillMostConstrainingBounds(const Range& other_){
      // std::min(std::nan(""),1.) -> nan
      // std::min(1.,std::nan("")) -> 1
      // so we check the bounds first
      if(other_.hasLowerBound()){ min = std::max(other_.min, min); }
      if(other_.hasUpperBound()){ max = std::min(other_.max, max); }
    }
    [[nodiscard]] bool hasLowerBound() const{ return not std::isnan(min); }
    [[nodiscard]] bool hasUpperBound() const{ return not std::isnan(max); }
    [[nodiscard]] bool hasBound() const{ return hasLowerBound() or hasUpperBound(); }
    [[nodiscard]] bool hasBothBounds() const{ return hasLowerBound() and hasUpperBound(); }
    [[nodiscard]] bool isUnbounded() const{ return not hasBound(); }
    [[nodiscard]] bool isInBounds(double val_) const{
      // both bounds are inclusive [min, max]
      if( hasLowerBound() and val_ < min ){ return false; }
      if( hasUpperBound() and val_ > max ){ return false; }
      return true;
    }
    [[nodiscard]] std::string toString() const{
      std::stringstream ss;
      ss << "[";
      std::isnan(min) ? ss << "-inf" : ss << min;
      ss << ", ";
      std::isnan(max) ? ss << "+inf" : ss << max;
      ss << "]";
      return ss.str();
    }

    Range& operator+=(double shift_){ min += shift_; max += shift_; return *this; }
    Range& operator-=(double shift_){ min -= shift_; max -= shift_; return *this; }
    friend std::ostream& operator <<( std::ostream& o, const Range& this_ ){ o << this_.toString(); return o; }
  };
}

// InitBaseClass
namespace GenericToolbox{
  class InitBaseClass{

  public:
    // Common structure
    InitBaseClass() = default;
    virtual ~InitBaseClass() = default;

    virtual inline void initialize();
    void unInitialize(){ _isInitialized_ = false; }

    [[nodiscard]] auto isInitialized() const{ return _isInitialized_; }

    inline void throwIfInitialized(const std::string& functionName_ = {}) const;
    inline void throwIfNotInitialized(const std::string& functionName_ = {}) const;

  protected:
    // where the derivative classes will specify (although override is optional)
    virtual void initializeImpl(){};

  private:
    bool _isInitialized_{false};

  };

  inline void InitBaseClass::initialize() {
    this->initializeImpl();
    _isInitialized_ = true;
  }

  inline void InitBaseClass::throwIfInitialized(const std::string& functionName_) const {
    if( _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object already initialized.");
      }
      throw std::runtime_error(__METHOD_NAME__ + "Can't \""+functionName_+"\" while already initialized.");
    }
  }
  inline void InitBaseClass::throwIfNotInitialized( const std::string& functionName_ ) const{
    if( not _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object not initialized.");
      }
      throw std::runtime_error(__METHOD_NAME__ + ": Can't \""+functionName_+"\" while not initialized.");
    }
  }
}

// ConfigBaseClass
namespace GenericToolbox{

  class ConfigBaseClass : public InitBaseClass {

  public:
    // C-tor and D-tor
    ConfigBaseClass() = default; // purely virtual
    ~ConfigBaseClass() override = default;

    // const getters
    [[nodiscard]] auto isConfigured() const { return _isConfigured_; }

    // main methods
    virtual void configure(){ _isConfigured_ = true; configureImpl(); }

  protected:
    // where the derivative classes will specify (although override is optional)
    virtual void configureImpl(){};

  private:
    bool _isConfigured_{false};

  };

  template<class ConfigType> class ConfigClass : public ConfigBaseClass {

  public:
    // C-tor and D-tor
    ConfigClass() = default;
    ~ConfigClass() override = default;

    // setters
    virtual void setConfig(const ConfigType& config_){ _config_ = config_; }

    // const getters
    auto& getConfig() const { return _config_; }

    // mutable getters
    auto& getConfig(){ return _config_; }

    // main methods
    void configure() override { this->ConfigBaseClass::configure(); }
    void configure(const ConfigType& config_){ setConfig(config_); this->ConfigBaseClass::configure(); }

  protected:
    // Can be accessed by derivative classes
    ConfigType _config_{};

  };

}

// ScopedGuard
namespace GenericToolbox{
  class ScopedGuard{

  public:
    using Action = std::function<void()>;

    explicit ScopedGuard(const Action& onCreate_, Action onDelete_) : _onDelete_{std::move(onDelete_)} { if( onCreate_ ){ onCreate_(); } }
    ~ScopedGuard() { fireOnDelete(); }

    void fireOnDelete(){ if( _onDelete_ ){ _onDelete_(); } this->dismiss(); }
    void dismiss(){ _onDelete_ = []{}; }

    // non-copiable
    ScopedGuard() = default;
    ScopedGuard( ScopedGuard const& ) = delete;
    ScopedGuard( ScopedGuard&& ) = default;
    auto operator=( ScopedGuard&& ) -> ScopedGuard& = default;
    auto operator=( ScopedGuard const& ) -> ScopedGuard& = delete;

  private:
    Action _onDelete_{};

  };
}

// RawDataArray
namespace GenericToolbox{
  class RawDataArray{

  public:
    RawDataArray() = default;
    virtual ~RawDataArray() = default;

    inline void reset();

    // const getters
    [[nodiscard]] auto& getRawDataArray() const{ return rawData; }

    // mutable getters
    auto& getRawDataArray(){ return rawData; }

    // main methods
    inline void writeMemoryContent( const void *address_, size_t dataSize_ );
    inline void writeMemoryContent( const void *address_, size_t dataSize_, size_t byteOffset_ );

    template<typename T> void writeRawData( const T &data ){ this->writeMemoryContent(&data, sizeof(data)); }
    template<typename T> void writeRawData( const T &data, size_t byteOffset_ ){ this->writeMemoryContent(&data, sizeof(data), byteOffset_); }

    void resetCursor(){ _cursor_ = 0; }
    void lock(){ _lock_ = true; }
    void unlock(){ _lock_ = false; }

  private:
    bool _lock_{false};
    size_t _cursor_{0};
    std::vector<uint8_t> rawData{};

  };

  inline void RawDataArray::reset(){
    rawData = {};
    resetCursor();
    unlock();
  }
  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_){
    this->writeMemoryContent(address_, dataSize_, _cursor_);
    _cursor_+=dataSize_;
  }
  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_, size_t byteOffset_){
    if(rawData.size() < byteOffset_ + dataSize_ ){
      if( _lock_ ){ throw std::runtime_error("Can't resize raw array since _lockArraySize_ is true."); }
      rawData.resize(byteOffset_ + dataSize_);
    }
    memcpy(&rawData[byteOffset_], address_, dataSize_);
  }
}

// TablePrinter
namespace GenericToolbox{
  class TablePrinter{

  public:
    TablePrinter() = default;

    // setters
    void setColorBuffer( const std::string &colorBuffer_ ){ cursor.color = colorBuffer_; }

    // main methods
    inline void reset();
    inline void fillTable( const std::vector<std::vector<std::string>> &tableLines_ );
    inline void setColTitles( const std::vector<std::string> &colTitles_ );
    inline void addTableLine( const std::vector<std::string> &colValues_ = std::vector<std::string>(), const std::string &colorCode_ = "" );
    void addSeparatorLine(){ tableContent.addSeparator(); }

    [[nodiscard]] inline std::string generateTableString() const;

    void printTable() const { std::cout << generateTableString() << std::endl; }

    // utils
    struct TableContent{
      struct LineContent{
        // keeping in a struct so it's explicit what it contains

        std::vector<std::string> columnList{};
      };
      int nCols{-1};
      // int colCursorPosition{0}; // for the streamer
      std::vector<LineContent> tableLineList{};
      std::vector<size_t> separatorPositionList{};

      void addLine(const std::vector<std::string>& colList_, const std::string& colorCode_={}){
        if( nCols == -1 ){ resetHeader(colList_); return; }

        // adding line content
        tableLineList.emplace_back();
        auto& line = tableLineList.back();
        line.columnList.resize(nCols, {""});

        // fill
        size_t nLoop{std::min(static_cast<size_t>(nCols), colList_.size())};
        for( size_t iCol = 0; iCol < nLoop; ++iCol ){
          line.columnList[iCol] = colList_[iCol];
          if( not colorCode_.empty() ) {
            line.columnList[iCol].insert(0, colorCode_);
            line.columnList[iCol] += ColorCodes::resetColor;
          }
        }
      }
      void resetHeader(const std::vector<std::string>& colList_){
        if( tableLineList.empty() ){ tableLineList.emplace_back(); }
        nCols = static_cast<int>(colList_.size());
        // in case the table is already filled up, resize:
        for( auto& line : tableLineList ){ line.columnList.resize(nCols); }
        // now set the header
        auto& headerLine = tableLineList[0];
        headerLine.columnList = colList_;
        addSeparator();
      }
      void addSeparator(){ separatorPositionList.emplace_back(tableLineList.size()); }
      [[nodiscard]] std::vector<size_t> getPadding() const{
        std::vector<size_t> output(nCols, 0);
        for( auto& line : tableLineList ) {
          for( size_t iCol = 0; iCol < nCols; ++iCol ) {
            output[iCol] = std::max(output[iCol], getPrintSize(line.columnList[iCol]));
          }
        }
        return output;
      }
    };
    struct StreamBuffer{
      std::stringstream ss{};
      std::vector<std::string> lineBuffer{};
      std::string color{};
    };

  private:
    TableContent tableContent{};
    StreamBuffer cursor{};

  public:
    typedef enum{ None = 0, Reset, NextColumn, NextLine } Action;
    template<typename T> TablePrinter &operator<<( const T &data ){ cursor.ss << data; return *this; }
    inline TablePrinter &operator<<( Action action_ );

  };

  void TablePrinter::reset(){ tableContent = {}; cursor = {}; }
  void TablePrinter::fillTable(const std::vector<std::vector<std::string>> &tableLines_){
    reset();
    for( auto& line: tableLines_ ){ tableContent.addLine(line); }
  }
  void TablePrinter::setColTitles(const std::vector<std::string>& colTitles_){
    if( colTitles_.empty() ){ throw std::runtime_error("colTitles_ is empty."); }
    tableContent.resetHeader(colTitles_);
  }
  void TablePrinter::addTableLine(const std::vector<std::string>& colValues_, const std::string&  colorCode_){
    tableContent.addLine(colValues_, colorCode_);
  }
  std::string TablePrinter::generateTableString() const {
    std::stringstream ss;

    // paving
    std::vector<size_t> paddingList = tableContent.getPadding();

    struct Separators{ std::string left{}, mid{}, right{}; };
    auto generateLine = [&](const Separators& sep_, const std::vector<std::string>& colList_={}){
      std::stringstream out;
      out << sep_.left;
      if( colList_.empty() ){
        for( int iCol = 0 ; iCol < tableContent.nCols-1 ; iCol++ ) {
          out << repeatString(GT_CHAR_HLINE, static_cast<int>(paddingList[iCol]+2)) << sep_.mid;
        }
        out << repeatString(GT_CHAR_HLINE, static_cast<int>(paddingList.back()+2)) << sep_.right;
      }
      else{
        for( int iCol = 0 ; iCol < tableContent.nCols ; iCol++ ){
          out << " " << padString(colList_[iCol], paddingList[iCol]) << " " << sep_.mid;
        }
      }
      return out.str();
    };

    // ┌───────────┬───────────────┬──────────────────┐
    // or
    // #----------------------------------------------#
    ss << generateLine({GT_CHAR_LTCORN, GT_CHAR_TTOP, GT_CHAR_RTCORN}) << std::endl;

    // content
    for( size_t iLine = 0 ; iLine < tableContent.tableLineList.size() ; iLine++ ){
      if( isIn(iLine, tableContent.separatorPositionList) ){
        ss << generateLine({GT_CHAR_TRIGHT, GT_CHAR_CROSS, GT_CHAR_TLEFT}) << std::endl;
      }
      ss << generateLine({GT_CHAR_VLINE, GT_CHAR_VLINE, GT_CHAR_VLINE}, tableContent.tableLineList[iLine].columnList) << std::endl;
    }


    // └───────────┴───────────────┴──────────────────┘
    // or
    // #----------------------------------------------#
    ss << generateLine({GT_CHAR_LBCORN, GT_CHAR_TBOT, GT_CHAR_RBCORN});

    return ss.str();
  }
  inline TablePrinter &TablePrinter::operator<<(const Action action_){

    if( action_ == None ){ return *this; }
    if( action_ == Reset ){ reset(); return *this; }

    // Either NextColumn or NextLine
    cursor.lineBuffer.emplace_back(cursor.ss.str());
    if( not cursor.color.empty() ){
      cursor.lineBuffer.back().insert(0, cursor.color);
      cursor.lineBuffer.back() += ColorCodes::resetColor;
    }
    cursor.ss.str(""); // reset

    // Drop in the table?
    if( action_ == NextLine or cursor.lineBuffer.size() == tableContent.nCols ){
      tableContent.addLine(cursor.lineBuffer);
      cursor.lineBuffer.clear();
    }

    return *this;
  }
}

// VariableMonitor
namespace GenericToolbox{
  class VariableMonitor{

  public:
    inline VariableMonitor() = default; // for vector
    inline explicit VariableMonitor( std::string name_ ) : _name_{std::move(name_)}{ this->reset(); }

    [[nodiscard]] inline const std::string &getName() const { return _name_; }
    [[nodiscard]] inline double getTotalAccumulated() const { return _totalAccumulated_; }
    [[nodiscard]] inline double getLastValue() const;

    inline void reset();
    inline void addQuantity( const double &quantityToAdd_ );
    inline double evalTotalGrowthRate();

    [[nodiscard]] inline double evalCallGrowthRate() const;
    [[nodiscard]] inline double evalCallGrowthRatePerSecond() const;


  private:
    std::string _name_{};

    double _totalAccumulated_{0};
    double _lastTotalAccumulated_{0};
    std::chrono::high_resolution_clock::time_point _lastTotalRateEval_;

    size_t _currentHistorySize_{0};
    std::vector<double> _addToAccumulatorHistory_{};
    std::vector<std::chrono::high_resolution_clock::time_point> _addToAccumulatorTimeHistory_{};
    size_t _currentSlotIndex_{0};

  };

  inline void VariableMonitor::reset(){
    _totalAccumulated_ = 0;
    _currentHistorySize_ = 0;
    _addToAccumulatorHistory_.clear();
    _addToAccumulatorHistory_.resize(20, 0); // 20 slots
    _addToAccumulatorTimeHistory_.resize(20); // 20 slots
    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
  }
  inline void VariableMonitor::addQuantity(const double& quantityToAdd_){
    _addToAccumulatorHistory_[_currentSlotIndex_] = quantityToAdd_;
    _addToAccumulatorTimeHistory_[_currentSlotIndex_] = std::chrono::high_resolution_clock::now();
    if( _currentHistorySize_ < _addToAccumulatorHistory_.size() ) _currentHistorySize_++;
    _currentSlotIndex_++;
    if( _currentSlotIndex_ == _addToAccumulatorHistory_.size() ) _currentSlotIndex_ = 0; // did a cycle
    _totalAccumulated_ += quantityToAdd_;
  }
  inline double VariableMonitor::getLastValue() const{
    if( _currentSlotIndex_ == 0 ) return _addToAccumulatorHistory_.back();
    else return _addToAccumulatorHistory_.at(_currentSlotIndex_-1);
  }
  inline double VariableMonitor::evalTotalGrowthRate(){
    double output = (_totalAccumulated_ - _lastTotalAccumulated_);
    output /= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - _lastTotalRateEval_
    ).count());
    output *= 1000.;

    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
    _lastTotalAccumulated_ = _totalAccumulated_;
    return output;
  }
  inline double VariableMonitor::evalCallGrowthRate() const{
    if( _currentHistorySize_ == 0 ) return 0;
    std::vector<double> orderedAddToAccumulatorHistory(_currentHistorySize_, 0);
    size_t latestIndex = _currentSlotIndex_;
    if( _currentHistorySize_ != _addToAccumulatorHistory_.size() ){
      latestIndex = 0;
    }
    size_t slotIndex = latestIndex;
    size_t orderedSlotIndex = 0;
    do{
      orderedAddToAccumulatorHistory.at(orderedSlotIndex) = _addToAccumulatorHistory_.at(slotIndex);
      slotIndex++;
      orderedSlotIndex++;
      if( slotIndex == _addToAccumulatorHistory_.size() ) slotIndex = 0;
    }
    while( orderedSlotIndex < orderedAddToAccumulatorHistory.size() );
    return getAveragedSlope(orderedAddToAccumulatorHistory);
  }
  inline double VariableMonitor::evalCallGrowthRatePerSecond() const{
    if( _currentHistorySize_ == 0 ) return 0;
    std::vector<double> orderedAddToAccumulatorHistory(_currentHistorySize_, 0);
    std::vector<size_t> deltaTimes(_currentHistorySize_, 0);
    size_t latestIndex = _currentSlotIndex_;
    if( _currentHistorySize_ != _addToAccumulatorHistory_.size() ){
      latestIndex = 0;
    }
    auto latestTp = _addToAccumulatorTimeHistory_.at(latestIndex);
    size_t slotIndex = latestIndex;
    size_t orderedSlotIndex = 0;
    do{
      orderedAddToAccumulatorHistory.at(orderedSlotIndex) = _addToAccumulatorHistory_.at(slotIndex);
      deltaTimes.at(orderedSlotIndex) = std::chrono::duration_cast<std::chrono::microseconds>(
          _addToAccumulatorTimeHistory_.at(slotIndex) - latestTp
      ).count();
      slotIndex++;
      orderedSlotIndex++;
      if( slotIndex == _addToAccumulatorHistory_.size() ) slotIndex = 0;
    }
    while( orderedSlotIndex < orderedAddToAccumulatorHistory.size() );
    return getAveragedSlope(orderedAddToAccumulatorHistory, deltaTimes);
  }
}

// QuantityMonitor
namespace GenericToolbox{
  struct QuantityMonitor{
    QuantityMonitor() = default;
    QuantityMonitor( std::string name_, std::string title_,
                     std::function<std::string( VariableMonitor & )> evalFunction_, bool isExpandablePadSize_ = true ) :
        name(std::move(name_)), title(std::move(title_)), isExpandablePadSize(isExpandablePadSize_),
        evalFunction(std::move(evalFunction_)){};
    std::string name{}; // kept for reference
    std::string title{};
    bool isExpandablePadSize{true};
    std::function<std::string( VariableMonitor & )> evalFunction{};
  };
}

// VariablesMonitor
namespace GenericToolbox{
  class VariablesMonitor{

  public:
    inline VariablesMonitor();
    inline virtual ~VariablesMonitor() = default;

    inline void setFooterString( const std::string &footerString ){ _footerString_ = footerString; }
    inline void setHeaderString( const std::string &headerString ){ _headerString_ = headerString; }
    inline void setMaxRefreshRateInMs( long long int maxRefreshRateInMs ){ _maxRefreshRateInMs_ = maxRefreshRateInMs; }

    inline long long int getMaxRefreshRateInMs() const;

    inline void addDisplayedQuantity( const std::string &quantityName_ );
    inline void defineNewQuantity( const QuantityMonitor &quantity_ );

    inline VariableMonitor &getVariable( const std::string &name_ );
    inline QuantityMonitor &getQuantity( const std::string &quantityName_ );

    inline bool isGenerateMonitorStringOk() const;

    inline void addVariable( const std::string &name_ );
    inline void clearDisplayQuantityList(){ _displayQuantityIndexList_.clear(); }

    inline std::string generateMonitorString( bool trailBackCursor_ = false, bool forceGenerate_ = false );

  private:
    long long int _maxRefreshRateInMs_{500}; // 1/0.033 = 30 fps, 500 = 1/0.500 = 2 fps
    std::chrono::high_resolution_clock::time_point _lastGeneratedMonitorStringTime_{};
    std::string _headerString_{};
    std::string _footerString_{};
    std::vector<VariableMonitor> _varMonitorList_{};
    std::vector<QuantityMonitor> _quantityMonitorList_{};
    std::vector<size_t> _displayQuantityIndexList_{};
    std::vector<size_t> _basedPaddingList_{};

    TablePrinter _tablePrinter_{};
  };

  inline VariablesMonitor::VariablesMonitor(){
    this->defineNewQuantity({ "VarName", "Variable",  [](VariableMonitor& v){ return v.getName(); } });
    this->defineNewQuantity({ "LastAddedValue", "Last Value", [](VariableMonitor& v){ return parseUnitPrefix(v.getLastValue(), 8); } });
    this->defineNewQuantity({ "Accumulated", "Accumulated", [](VariableMonitor& v){ return parseUnitPrefix(v.getTotalAccumulated(), 8); } });
    this->defineNewQuantity({ "AccumulationRate", "Acc. Rate /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalTotalGrowthRate(), 8); } });
    this->defineNewQuantity({ "SlopePerSecond", "Slope /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRatePerSecond(),5); } });
    this->defineNewQuantity({ "SlopePerCall", "Slope /call", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRate(), 5); } });
  }
  inline void VariablesMonitor::addVariable(const std::string& name_){
    for( const auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        throw std::logic_error("Variable name already added to the monitor");
      }
    }
    _varMonitorList_.emplace_back(name_);
  }
  inline void VariablesMonitor::addDisplayedQuantity(const std::string& quantityName_){
    int index = -1;
    for( size_t iQuantity = 0 ; iQuantity < _quantityMonitorList_.size() ; iQuantity++ ){
      if( _quantityMonitorList_.at(iQuantity).name == quantityName_ ){
        index = int(iQuantity);
        break;
      }
    }

    if( index == -1 ){
      throw std::logic_error("quantityName_ = " + quantityName_ + " not found.");
    }
    else{
      _displayQuantityIndexList_.emplace_back(index);
    }
  }
  inline void VariablesMonitor::defineNewQuantity(const QuantityMonitor& quantity_){
    int index = -1;
    for( size_t iQuantity = 0 ; iQuantity < _quantityMonitorList_.size() ; iQuantity++ ){
      if( _quantityMonitorList_.at(iQuantity).name == quantity_.name ){
        index = int(iQuantity);
        break;
      }
    }
    if( index == -1 ){
      _quantityMonitorList_.emplace_back(quantity_);
    }
    else{
      throw std::logic_error("quantity_: " + quantity_.name + " already exists.");
    }
  }
  inline long long int VariablesMonitor::getMaxRefreshRateInMs() const {
    return _maxRefreshRateInMs_;
  }
  inline VariableMonitor& VariablesMonitor::getVariable(const std::string& name_){
    for( auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        return v;
      }
    }
    throw std::logic_error("Variable with name " + name_ + " is not monitored");
  }
  inline QuantityMonitor& VariablesMonitor::getQuantity(const std::string& quantityName_){
    for( auto& q : _quantityMonitorList_ ){
      if( q.name == quantityName_ ){
        return q;
      }
    }
    throw std::logic_error("Quantity with name " + quantityName_ + " is not monitored");
  }
  inline bool VariablesMonitor::isGenerateMonitorStringOk() const {
    if( _maxRefreshRateInMs_ != -1 ){
      if( _lastGeneratedMonitorStringTime_.time_since_epoch().count() != 0
          and std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - _lastGeneratedMonitorStringTime_
      ).count() < _maxRefreshRateInMs_
          ){
        return false;
      }
    }
    return true;
  }
  inline std::string VariablesMonitor::generateMonitorString(bool trailBackCursor_, bool forceGenerate_) {

    if( not forceGenerate_ and trailBackCursor_ and not this->isGenerateMonitorStringOk() ) return {};
    _lastGeneratedMonitorStringTime_ = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<std::string>> varElementsList(_varMonitorList_.size()+1, std::vector<std::string>(_displayQuantityIndexList_.size()));

    int iQuantity = 0;
    for( const auto& quantityIndex : _displayQuantityIndexList_ ){
      varElementsList[0][iQuantity++] = _quantityMonitorList_.at(quantityIndex).title;
    }

    for( size_t iVar = 0 ; iVar < _varMonitorList_.size() ; iVar++ ){ // line
      iQuantity = 0;
      for( const auto& quantityIndex : _displayQuantityIndexList_ ){ // col
        if( _quantityMonitorList_.at(quantityIndex).evalFunction ){
          varElementsList[iVar+1][iQuantity++] = _quantityMonitorList_.at(quantityIndex).evalFunction(_varMonitorList_.at(iVar));
        }
      }
    }
    _tablePrinter_.fillTable(varElementsList);

    std::stringstream ss;

    // Optional Header
    if( not _headerString_.empty() ){
      ss << _headerString_ << std::endl;
    }

    ss << _tablePrinter_.generateTableString() << std::endl;

    // Optional Footer
    if( not _footerString_.empty() ) ss << _footerString_ << std::endl;


    std::stringstream ssLineCleaner;
#ifdef CPP_GENERIC_TOOLBOX_BATCH
    #warning Variables Monitor in batch mode
    return ss.str();
#endif

    auto nLines = splitString(ss.str(), "\n").size();
    for( size_t iLine = 1 ; iLine < nLines ; iLine++ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << std::endl;
    }
    ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines-1 << "F";
    ssLineCleaner << "\r" << ss.str(); // "\r" can be intercepted by loggers to know if a new line header can be printed

    if( trailBackCursor_ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines << "F" << std::endl;
      ssLineCleaner << static_cast<char>(27) << "[2K"; // un-flushed part: this clear line will only be displayed once a new line will try to override it
    }

    ssLineCleaner << std::flush;
    return ssLineCleaner.str();
  }
}

// AnyType
namespace GenericToolbox{  // Structs to decide if a stream function can be implemented
  template<typename S, typename T, typename = void> struct is_streamable : std::false_type {};
  template<typename S, typename T> struct is_streamable<S, T, decltype(std::declval<S&>() << std::declval<T&>(), void())> : std::true_type {};

  // Required by C++11 and C++14
  template <typename T, bool> class StreamerImpl {};
  template <typename T> class StreamerImpl<T,true> { public: static void implement(std::ostream& os, T const& t) { os << t; } };
  template <typename T> class StreamerImpl<T,false>{ public: static void implement(std::ostream& os, T const& t) { os << typeid(t).name(); } };
  template <typename T, bool> class DoubleCastImpl {};
  template <typename T> class DoubleCastImpl<T,true> { public: static double implement(T const& t) { return static_cast<double>(t); } };
  template <typename T> class DoubleCastImpl<T,false>{ public: static double implement(T const& t) { return std::nan(typeid(t).name()); } };
  template <typename T, bool> class LongCastImpl {};
  template <typename T> class LongCastImpl<T,true> { public: static long implement(T const& t) { return static_cast<long>(t); } };
  template <typename T> class LongCastImpl<T,false>{ public: static long implement(T const& t) { return std::nan(typeid(t).name()); } };

  // PlaceHolder is used in AnyType as a pointer member
  struct PlaceHolder{
    virtual ~PlaceHolder() = default;
    [[nodiscard]] virtual const std::type_info& getType() const = 0;
    [[nodiscard]] virtual bool isPointerType() const = 0;
    [[nodiscard]] virtual PlaceHolder* clone() const = 0;
    virtual void writeToStream(std::ostream& o) const = 0;
    [[nodiscard]] virtual double getVariableAsDouble() const = 0;
    [[nodiscard]] virtual long getValueAsLong() const = 0;
    [[nodiscard]] virtual size_t getVariableSize() const = 0;
    [[nodiscard]] virtual const void* getVariableAddress() const = 0;
    virtual void* getVariableAddress() = 0;
  };

  // VariableHolder is the specialized PlaceHolder containing the _variable_ of interest
  template<typename VariableType> struct VariableHolder: public PlaceHolder{
    explicit VariableHolder(VariableType value_) : _nBytes_(sizeof(value_)), _variable_(std::move(value_)){  }
    [[nodiscard]] const std::type_info & getType() const override { return typeid(VariableType); }
#if HAS_CPP_17
    [[nodiscard]] bool isPointerType() const override { return std::is_pointer_v<VariableType>; }
#else
    [[nodiscard]] bool isPointerType() const override { return std::is_pointer<VariableType>::value; }
#endif
    [[nodiscard]] PlaceHolder* clone() const override { return new VariableHolder(_variable_); }
    void writeToStream(std::ostream& o) const override {
#if HAS_CPP_17
      if constexpr (is_streamable<std::ostream,VariableType>::value) { o << _variable_; }
#else
      StreamerImpl<VariableType, is_streamable<std::ostream,VariableType>::value>::implement(o, _variable_);
#endif
    }
    [[nodiscard]] double getVariableAsDouble() const override {
      return DoubleCastImpl<VariableType, std::is_convertible<VariableType, double>::value>::implement(_variable_);
    }
    [[nodiscard]] long getValueAsLong() const override {
      return LongCastImpl<VariableType, std::is_convertible<VariableType, long>::value>::implement(_variable_);
    }
    [[nodiscard]] size_t getVariableSize() const override{
      return _nBytes_;
    }
    [[nodiscard]] const void* getVariableAddress() const override{
      return static_cast<const void*>(&_variable_);
    }
    void* getVariableAddress() override{
      return static_cast<void*>(&_variable_);
    }

    size_t _nBytes_;
    VariableType _variable_;
  };


  // The Class:
  class AnyType{

  public:

    inline AnyType() = default;
    inline AnyType(const AnyType& other_);
    template<typename ValueType> inline explicit AnyType(const ValueType& value_);
    inline virtual ~AnyType() = default;

    // Operators
    template<typename ValueType> inline AnyType& operator=(const ValueType & rhs);
    inline AnyType& operator=(const AnyType& rhs);

    inline bool empty();
    inline PlaceHolder* getPlaceHolderPtr();
    [[nodiscard]] inline size_t getStoredSize() const;
    [[nodiscard]] inline const std::type_info& getType() const;
    [[nodiscard]] inline const PlaceHolder* getPlaceHolderPtr() const;

    template<typename ValueType> inline void setValue(const ValueType& value_);
    template<typename ValueType> inline ValueType& getValue();
    template<typename ValueType> inline const ValueType& getValue() const;
    [[nodiscard]] inline double getValueAsDouble() const;
    [[nodiscard]] inline long getValueAsLong() const;

    inline friend std::ostream& operator <<( std::ostream& o, const AnyType& v );


  protected:
    inline AnyType& swap(AnyType& rhs) noexcept;


  private:
    std::unique_ptr<PlaceHolder> _varPtr_{};

  };

  inline AnyType::AnyType(const AnyType& other_){
    if( other_._varPtr_ != nullptr ){ this->_varPtr_ = std::unique_ptr<PlaceHolder>(other_._varPtr_->clone()); }
  }
  template<typename ValueType> inline AnyType::AnyType(const ValueType& value_){
    this->setValue(value_);
  }

  template<typename ValueType> inline AnyType& AnyType::operator=(const ValueType & rhs) {
    AnyType(rhs).swap(*this);
    return *this;
  }
  inline AnyType& AnyType::operator=(const AnyType& rhs){
    AnyType(rhs).swap(*this);
    return *this;
  }

  inline bool AnyType::empty(){
    return (_varPtr_ == nullptr);
  }
  inline const std::type_info& AnyType::getType() const{
    return _varPtr_ != nullptr ? _varPtr_->getType() : typeid(void);
  }
  inline const PlaceHolder* AnyType::getPlaceHolderPtr() const {
    return _varPtr_.get();
  }
  inline PlaceHolder* AnyType::getPlaceHolderPtr() {
    return _varPtr_.get();
  }
  inline size_t AnyType::getStoredSize() const{
    return _varPtr_->getVariableSize();
  }

  template<typename ValueType> void AnyType::setValue(const ValueType& value_){
    _varPtr_ = std::unique_ptr<VariableHolder<ValueType>>(new VariableHolder<ValueType>(value_));
  }
  template<typename RequestedType> RequestedType& AnyType::getValue() {
    return const_cast<RequestedType&>( const_cast<const AnyType*>(this)->getValue<RequestedType>() );
  }
  template<typename RequestedType> const RequestedType& AnyType::getValue() const{
    if( _varPtr_ == nullptr ){ throw std::runtime_error("AnyType value not set."); }
#if HAS_CPP_17
    if( std::is_pointer_v<RequestedType> and _varPtr_->isPointerType() ){
#else
    if( std::is_pointer<RequestedType>::value and _varPtr_->isPointerType() ){
#endif
      // allow the cast of different type pointer
    }
    else if( getType() != typeid(RequestedType) ) {
      // otherwise it's a problem
      throw std::runtime_error("AnyType value type mismatch: stored: " + std::string(getType().name()) + ", requested: " + typeid(RequestedType).name());
    }
    return static_cast<const VariableHolder<const RequestedType> *>(_varPtr_.get())->_variable_;
  }
  inline double AnyType::getValueAsDouble() const{
    return _varPtr_->getVariableAsDouble();
  }
  inline long AnyType::getValueAsLong() const{
    return _varPtr_->getValueAsLong();
  }

  inline std::ostream& operator<<( std::ostream& o, const AnyType& v ) {
    if( v._varPtr_ != nullptr ) v._varPtr_->writeToStream(o);
    return o;
  }

  // Protected
  inline AnyType& AnyType::swap(AnyType& rhs) noexcept {
    std::swap(_varPtr_, rhs._varPtr_);
    return *this;
  }
}


#endif // CPP_GENERIC_TOOLBOX_UTILS_H
