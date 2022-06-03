//
// Created by Nadrino on 06/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H

#include "GenericToolbox.VariablesMonitor.h"
#include "GenericToolbox.TablePrinter.h"

namespace GenericToolbox{

  inline VariablesMonitor::VariablesMonitor(){ this->reset(); }
  inline VariablesMonitor::~VariablesMonitor() { this->reset(); }

  inline void VariablesMonitor::clearDisplayQuantityList(){
    _displayQuantityIndexList_.clear();
  }

  inline void VariablesMonitor::reset() {
    _varMonitorList_.clear();
    _quantityMonitorList_.clear();
    _displayQuantityIndexList_.clear();
    _basedPaddingList_.clear();

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
  inline void VariablesMonitor::setHeaderString(const std::string &headerString) {
    _headerString_ = headerString;
  }
  inline void VariablesMonitor::setFooterString(const std::string &footerString) {
    _footerString_ = footerString;
  }
  inline void VariablesMonitor::setMaxRefreshRateInMs(long long int maxRefreshRateInMs) {
    _maxRefreshRateInMs_ = maxRefreshRateInMs;
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

  inline bool VariablesMonitor::isGenerateMonitorStringOk(){
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
  inline std::string VariablesMonitor::generateMonitorString(bool trailBackCursor_) {

    if( trailBackCursor_ and not this->isGenerateMonitorStringOk() ) return {};
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

    auto nLines = GenericToolbox::splitString(ss.str(), "\n").size();
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

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H
