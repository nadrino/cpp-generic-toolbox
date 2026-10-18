#pragma once

namespace GenericToolbox {
namespace ProgressBar {
#ifdef CPP_GENERIC_TOOLBOX_UTILS_COMPILED
ProgressBar gProgressBar;
#endif
CPP_GENERIC_TOOLBOX_UTILS_INLINE void ProgressBar::resetLastDisplayedValue() {
      this->_lastDisplayedValue_ = -1;
      this->_lastDisplayedPercentValue_ = -1;
    }
}

namespace ProgressBar {
CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string ProgressBar::_generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_) {

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

CPP_GENERIC_TOOLBOX_UTILS_STATIC void resetLastDisplayedValue() {
    ProgressBar::gProgressBar.resetLastDisplayedValue();
  }

CPP_GENERIC_TOOLBOX_UTILS_STATIC void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

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

CPP_GENERIC_TOOLBOX_UTILS_INLINE void InitBaseClass::initialize() {
    this->initializeImpl();
    _isInitialized_ = true;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void InitBaseClass::throwIfInitialized(const std::string& functionName_) const {
    if( _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object already initialized.");
      }
      throw std::runtime_error(__METHOD_NAME__ + "Can't \""+functionName_+"\" while already initialized.");
    }
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void InitBaseClass::throwIfNotInitialized( const std::string& functionName_ ) const {
    if( not _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object not initialized.");
      }
      throw std::runtime_error(__METHOD_NAME__ + ": Can't \""+functionName_+"\" while not initialized.");
    }
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void RawDataArray::reset() {
    rawData = {};
    resetCursor();
    unlock();
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_) {
    this->writeMemoryContent(address_, dataSize_, _cursor_);
    _cursor_+=dataSize_;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_, size_t byteOffset_) {
    if(rawData.size() < byteOffset_ + dataSize_ ){
      if( _lock_ ){ throw std::runtime_error("Can't resize raw array since _lockArraySize_ is true."); }
      rawData.resize(byteOffset_ + dataSize_);
    }
    memcpy(&rawData[byteOffset_], address_, dataSize_);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void TablePrinter::reset() { tableContent = {}; cursor = {}; }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void TablePrinter::fillTable(const std::vector<std::vector<std::string>> &tableLines_) {
    reset();
    for( auto& line: tableLines_ ){ tableContent.addLine(line); }
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void TablePrinter::setColTitles(const std::vector<std::string>& colTitles_) {
    if( colTitles_.empty() ){ throw std::runtime_error("colTitles_ is empty."); }
    tableContent.resetHeader(colTitles_);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void TablePrinter::addTableLine(const std::vector<std::string>& colValues_, const std::string&  colorCode_) {
    tableContent.addLine(colValues_, colorCode_);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string TablePrinter::generateTableString() const {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE TablePrinter &TablePrinter::operator<<(const Action action_) {

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

CPP_GENERIC_TOOLBOX_UTILS_INLINE void VariableMonitor::reset() {
    _totalAccumulated_ = 0;
    _currentHistorySize_ = 0;
    _addToAccumulatorHistory_.clear();
    _addToAccumulatorHistory_.resize(20, 0); // 20 slots
    _addToAccumulatorTimeHistory_.resize(20); // 20 slots
    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void VariableMonitor::addQuantity(const double& quantityToAdd_) {
    _addToAccumulatorHistory_[_currentSlotIndex_] = quantityToAdd_;
    _addToAccumulatorTimeHistory_[_currentSlotIndex_] = std::chrono::high_resolution_clock::now();
    if( _currentHistorySize_ < _addToAccumulatorHistory_.size() ) _currentHistorySize_++;
    _currentSlotIndex_++;
    if( _currentSlotIndex_ == _addToAccumulatorHistory_.size() ) _currentSlotIndex_ = 0; // did a cycle
    _totalAccumulated_ += quantityToAdd_;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE double VariableMonitor::getLastValue() const {
    if( _currentSlotIndex_ == 0 ) return _addToAccumulatorHistory_.back();
    else return _addToAccumulatorHistory_.at(_currentSlotIndex_-1);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE double VariableMonitor::evalTotalGrowthRate() {
    double output = (_totalAccumulated_ - _lastTotalAccumulated_);
    output /= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - _lastTotalRateEval_
    ).count());
    output *= 1000.;

    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
    _lastTotalAccumulated_ = _totalAccumulated_;
    return output;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE double VariableMonitor::evalCallGrowthRate() const {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE double VariableMonitor::evalCallGrowthRatePerSecond() const {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE VariablesMonitor::VariablesMonitor() {
    this->defineNewQuantity({ "VarName", "Variable",  [](VariableMonitor& v){ return v.getName(); } });
    this->defineNewQuantity({ "LastAddedValue", "Last Value", [](VariableMonitor& v){ return parseUnitPrefix(v.getLastValue(), 8); } });
    this->defineNewQuantity({ "Accumulated", "Accumulated", [](VariableMonitor& v){ return parseUnitPrefix(v.getTotalAccumulated(), 8); } });
    this->defineNewQuantity({ "AccumulationRate", "Acc. Rate /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalTotalGrowthRate(), 8); } });
    this->defineNewQuantity({ "SlopePerSecond", "Slope /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRatePerSecond(),5); } });
    this->defineNewQuantity({ "SlopePerCall", "Slope /call", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRate(), 5); } });
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void VariablesMonitor::addVariable(const std::string& name_) {
    for( const auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        throw std::logic_error("Variable name already added to the monitor");
      }
    }
    _varMonitorList_.emplace_back(name_);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE void VariablesMonitor::addDisplayedQuantity(const std::string& quantityName_) {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE void VariablesMonitor::defineNewQuantity(const QuantityMonitor& quantity_) {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE long long int VariablesMonitor::getMaxRefreshRateInMs() const {
    return _maxRefreshRateInMs_;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE VariableMonitor& VariablesMonitor::getVariable(const std::string& name_) {
    for( auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        return v;
      }
    }
    throw std::logic_error("Variable with name " + name_ + " is not monitored");
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE QuantityMonitor& VariablesMonitor::getQuantity(const std::string& quantityName_) {
    for( auto& q : _quantityMonitorList_ ){
      if( q.name == quantityName_ ){
        return q;
      }
    }
    throw std::logic_error("Quantity with name " + quantityName_ + " is not monitored");
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE bool VariablesMonitor::isGenerateMonitorStringOk() const {
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

CPP_GENERIC_TOOLBOX_UTILS_INLINE std::string VariablesMonitor::generateMonitorString(bool trailBackCursor_, bool forceGenerate_) {

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

CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType::AnyType(const AnyType& other_) {
    if( other_._varPtr_ != nullptr ){ this->_varPtr_ = std::unique_ptr<PlaceHolder>(other_._varPtr_->clone()); }
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType& AnyType::operator=(const AnyType& rhs) {
    AnyType(rhs).swap(*this);
    return *this;
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE bool AnyType::empty() {
    return (_varPtr_ == nullptr);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE const std::type_info& AnyType::getType() const {
    return _varPtr_ != nullptr ? _varPtr_->getType() : typeid(void);
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE const PlaceHolder* AnyType::getPlaceHolderPtr() const {
    return _varPtr_.get();
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE PlaceHolder* AnyType::getPlaceHolderPtr() {
    return _varPtr_.get();
  }

CPP_GENERIC_TOOLBOX_UTILS_INLINE size_t AnyType::getStoredSize() const {
    return _varPtr_->getVariableSize();
  }


  CPP_GENERIC_TOOLBOX_UTILS_INLINE double AnyType::getValueAsDouble() const{
    return _varPtr_->getVariableAsDouble();
  }

  CPP_GENERIC_TOOLBOX_UTILS_INLINE long AnyType::getValueAsLong() const{
    return _varPtr_->getValueAsLong();
  }

  CPP_GENERIC_TOOLBOX_UTILS_INLINE std::ostream& operator<<( std::ostream& o, const AnyType& v ) {
    if( v._varPtr_ != nullptr ) v._varPtr_->writeToStream(o);
    return o;
  }

  CPP_GENERIC_TOOLBOX_UTILS_INLINE AnyType& AnyType::swap(AnyType& rhs) noexcept {
    std::swap(_varPtr_, rhs._varPtr_);
    return *this;
  }
}
