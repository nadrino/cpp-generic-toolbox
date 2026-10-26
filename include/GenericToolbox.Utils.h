//
// Created by Nadrino on 08/01/2024.
//

#ifndef CPP_GENERIC_TOOLBOX_UTILS_H
#define CPP_GENERIC_TOOLBOX_UTILS_H

#include "GenericToolbox.Build.h"


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

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


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
  CPP_GENERIC_TOOLBOX_UTILS_STATIC void resetLastDisplayedValue();
  CPP_GENERIC_TOOLBOX_UTILS_STATIC void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");

  namespace ProgressBar{
    class ProgressBar{

    public:
      inline ProgressBar() = default;
      inline virtual ~ProgressBar() = default;

      inline void setMaxBarLength(int maxBarLength){ _maxBarLength_ = maxBarLength; }
      inline void setDisableVt100Cmd(bool disableVt100Cmd_){ _disableVt100Cmd_ = disableVt100Cmd_; }
      inline void setEnableRainbowProgressBar(bool enableRainbowProgressBar){_enableRainbowProgressBar_ = enableRainbowProgressBar;}

      CPP_GENERIC_TOOLBOX_UTILS_INLINE void resetLastDisplayedValue();

      template<typename T, typename TT> inline std::string generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "");
      template<typename T, typename TT> inline bool showProgressBar(T iCurrent_, TT iTotal_);
      template<typename T, typename TT> inline std::string getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
      template<typename T, typename TT> inline void displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);

    protected:
      CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string _generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_);

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

#ifdef CPP_GENERIC_TOOLBOX_UTILS_COMPILED
    extern CPP_GENERIC_TOOLBOX_UTILS_API ProgressBar gProgressBar;
#else
    static ProgressBar gProgressBar;
#endif

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


    // protected

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
  CPP_GENERIC_TOOLBOX_UTILS_STATIC void resetLastDisplayedValue();
  CPP_GENERIC_TOOLBOX_UTILS_STATIC void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_);

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
    [[nodiscard]] bool isBelowMin(double val_) const{ return (hasLowerBound() and val_ < min); }
    [[nodiscard]] GT_DEPRECATED("replace with isBelowMin()") bool isBellowMin(double val_) const { return isBelowMin(val_); }
    [[nodiscard]] bool isAboveMax(double val_) const{ return (hasUpperBound() and val_ > max); }
    [[nodiscard]] bool isInBounds(double val_) const{
      // both bounds are inclusive [min, max]
      if( isBelowMin(val_) ){ return false; }
      if( isAboveMax(val_) ){ return false; }
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
    Range& operator-=(double shift_){ *this += -shift_; return *this; }
    bool operator==(const Range& other_) const { return min == other_.min && max == other_.max; }
    bool operator!=(const Range& other_) const { return !(*this == other_); }

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

    virtual CPP_GENERIC_TOOLBOX_UTILS_INLINE void initialize();
    void unInitialize(){ _isInitialized_ = false; }

    [[nodiscard]] auto isInitialized() const{ return _isInitialized_; }

    CPP_GENERIC_TOOLBOX_UTILS_INLINE void throwIfInitialized(const std::string& functionName_ = {}) const;
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void throwIfNotInitialized(const std::string& functionName_ = {}) const;

  protected:
    // where the derivative classes will specify (although override is optional)
    virtual void initializeImpl(){};

  private:
    bool _isInitialized_{false};

  };


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

    CPP_GENERIC_TOOLBOX_UTILS_INLINE void reset();

    // const getters
    [[nodiscard]] auto& getRawDataArray() const{ return rawData; }

    // mutable getters
    auto& getRawDataArray(){ return rawData; }

    // main methods
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void writeMemoryContent( const void *address_, size_t dataSize_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void writeMemoryContent( const void *address_, size_t dataSize_, size_t byteOffset_ );

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


}

// TablePrinter
namespace GenericToolbox{
  class TablePrinter{

  public:
    TablePrinter() = default;

    // setters
    void setColorBuffer( const std::string &colorBuffer_ ){ cursor.color = colorBuffer_; }

    // main methods
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void reset();
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void fillTable( const std::vector<std::vector<std::string>> &tableLines_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void setColTitles( const std::vector<std::string> &colTitles_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void addTableLine( const std::vector<std::string> &colValues_ = std::vector<std::string>(), const std::string &colorCode_ = "" );
    void addSeparatorLine(){ tableContent.addSeparator(); }

    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string generateTableString() const;

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
          for( int iCol = 0; iCol < nCols; ++iCol ) {
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
    CPP_GENERIC_TOOLBOX_UTILS_INLINE TablePrinter &operator<<( Action action_ );

  };

}

// VariableMonitor
namespace GenericToolbox{
  class VariableMonitor{

  public:
    inline VariableMonitor() = default; // for vector
    inline explicit VariableMonitor( std::string name_ ) : _name_{std::move(name_)}{ this->reset(); }

    [[nodiscard]] inline const std::string &getName() const { return _name_; }
    [[nodiscard]] inline double getTotalAccumulated() const { return _totalAccumulated_; }
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE double getLastValue() const;

    CPP_GENERIC_TOOLBOX_UTILS_INLINE void reset();
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void addQuantity( const double &quantityToAdd_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE double evalTotalGrowthRate();

    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE double evalCallGrowthRate() const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE double evalCallGrowthRatePerSecond() const;


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
    CPP_GENERIC_TOOLBOX_UTILS_INLINE VariablesMonitor();
    inline virtual ~VariablesMonitor() = default;

    inline void setFooterString( const std::string &footerString ){ _footerString_ = footerString; }
    inline void setHeaderString( const std::string &headerString ){ _headerString_ = headerString; }
    inline void setMaxRefreshRateInMs( long long int maxRefreshRateInMs ){ _maxRefreshRateInMs_ = maxRefreshRateInMs; }

    CPP_GENERIC_TOOLBOX_UTILS_INLINE long long int getMaxRefreshRateInMs() const;

    CPP_GENERIC_TOOLBOX_UTILS_INLINE void addDisplayedQuantity( const std::string &quantityName_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE void defineNewQuantity( const QuantityMonitor &quantity_ );

    CPP_GENERIC_TOOLBOX_UTILS_INLINE VariableMonitor &getVariable( const std::string &name_ );
    CPP_GENERIC_TOOLBOX_UTILS_INLINE QuantityMonitor &getQuantity( const std::string &quantityName_ );

    CPP_GENERIC_TOOLBOX_UTILS_INLINE bool isGenerateMonitorStringOk() const;

    CPP_GENERIC_TOOLBOX_UTILS_INLINE void addVariable( const std::string &name_ );
    inline void clearDisplayQuantityList(){ _displayQuantityIndexList_.clear(); }

    CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string generateMonitorString( bool trailBackCursor_ = false, bool forceGenerate_ = false );

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
    CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType(const AnyType& other_);
    template<typename ValueType> inline explicit AnyType(const ValueType& value_);
    inline virtual ~AnyType() = default;

    // Operators
    template<typename ValueType> inline AnyType& operator=(const ValueType & rhs);
    CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType& operator=(const AnyType& rhs);

    CPP_GENERIC_TOOLBOX_UTILS_INLINE bool empty();
    CPP_GENERIC_TOOLBOX_UTILS_INLINE PlaceHolder* getPlaceHolderPtr();
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE size_t getStoredSize() const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE const std::type_info& getType() const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE const PlaceHolder* getPlaceHolderPtr() const;

    template<typename ValueType> inline void setValue(const ValueType& value_);
    template<typename ValueType> inline ValueType& getValue();
    template<typename ValueType> inline const ValueType& getValue() const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE double getValueAsDouble() const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_UTILS_INLINE long getValueAsLong() const;

    friend CPP_GENERIC_TOOLBOX_UTILS_INLINE std::ostream& operator <<( std::ostream& o, const AnyType& v );


  protected:
    CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType& swap(AnyType& rhs) noexcept;


  private:
    std::unique_ptr<PlaceHolder> _varPtr_{};

  };


  template<typename ValueType> inline AnyType::AnyType(const ValueType& value_){
    this->setValue(value_);
  }

  template<typename ValueType> inline AnyType& AnyType::operator=(const ValueType & rhs) {
    AnyType(rhs).swap(*this);
    return *this;
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
    return static_cast<const VariableHolder<RequestedType> *>(_varPtr_.get())->_variable_;
  }


  // Protected

}


#ifndef CPP_GENERIC_TOOLBOX_UTILS_COMPILED
#include "implementation/GenericToolbox.Utils.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_UTILS_H
