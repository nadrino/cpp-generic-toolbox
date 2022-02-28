//
// Created by Nadrino on 06/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H

#include "GenericToolbox.VariablesMonitor.h"

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

    std::vector<std::vector<std::string>> varElementsList(_varMonitorList_.size(), std::vector<std::string>(_displayQuantityIndexList_.size()));

    if(_basedPaddingList_.size() != _displayQuantityIndexList_.size() ){
      _basedPaddingList_.clear();
      for( const auto& quantityIndex : _displayQuantityIndexList_ ){
        _basedPaddingList_.emplace_back( std::max(size_t(6), _quantityMonitorList_.at(quantityIndex).title.size()) );
      }
    }
    auto paddingList = _basedPaddingList_;

    for( size_t iVar = 0 ; iVar < _varMonitorList_.size() ; iVar++ ){
      int iQuantity = -1;
      for( const auto& quantityIndex : _displayQuantityIndexList_ ){
        iQuantity++;
        if( _quantityMonitorList_.at(quantityIndex).evalFunction ){
          varElementsList.at(iVar).at(iQuantity) = _quantityMonitorList_.at(quantityIndex).evalFunction(_varMonitorList_.at(iVar));
        }
        if( _quantityMonitorList_.at(quantityIndex).isExpandablePadSize ){
          paddingList.at(iQuantity) = std::max(paddingList.at(iQuantity), varElementsList.at(iVar).at(iQuantity).size());
        }
      }
    }

    std::stringstream ss;
    int barWidth = -1;
    for( const auto& padding : paddingList ){ barWidth += int(padding) + 3; }

    // Optional Header
    if( not _headerString_.empty() ){
      ss << _headerString_ << std::endl;
    }

    int iQuantity = -1;

    // https://en.wikipedia.org/wiki/Box-drawing_character
    ss << "┌";
    for( iQuantity = 0 ; iQuantity < int(_displayQuantityIndexList_.size()) ; iQuantity++ ){
      if( iQuantity != 0 ){ ss << "┬─"; }
      ss << GenericToolbox::repeatString("─", int(paddingList.at(iQuantity)+1));
    }
    ss << "┐" << std::endl;

    // Legend
    std::stringstream sss;
    iQuantity = -1;
    ss << "│";
    for( const auto& quantityIndex : _displayQuantityIndexList_ ){
      iQuantity++;
      if(not sss.str().empty()) sss << " ";
      sss << GenericToolbox::padString(_quantityMonitorList_.at(quantityIndex).title, paddingList.at(iQuantity)) << " │";
    }
    ss << sss.str() << std::endl;

    ss << "├";
    for( iQuantity = 0 ; iQuantity < int(_displayQuantityIndexList_.size()) ; iQuantity++ ){
      if( iQuantity != 0 ){ ss << "┼─"; }
      ss << GenericToolbox::repeatString("─", int(paddingList.at(iQuantity)+1));
    }
    ss << "┤" << std::endl;

    // Content
    for( size_t iVar = 0 ; iVar < _varMonitorList_.size() ; iVar++ ){
      sss.str("");
      iQuantity = -1;
      ss << "│";
      for( const auto& quantityIndex : _displayQuantityIndexList_ ){
        iQuantity++;
        if(not sss.str().empty()) sss << " ";
        sss << GenericToolbox::padString(varElementsList.at(iVar).at(iQuantity), paddingList.at(iQuantity)) << " │";
      }
      ss << sss.str() << std::endl;
    }

    ss << "└";
    for( iQuantity = 0 ; iQuantity < int(_displayQuantityIndexList_.size()) ; iQuantity++ ){
      if( iQuantity != 0 ){ ss << "┴─"; }
      ss << GenericToolbox::repeatString("─", int(paddingList.at(iQuantity)+1));
    }
    ss << "┘" << std::endl;


    // Optional Footer
    if( not _footerString_.empty() ) ss << _footerString_ << std::endl;

    auto nLines = GenericToolbox::splitString(ss.str(), "\n").size();

    std::stringstream ssLineCleaner;
    for( size_t iLine = 1 ; iLine < nLines ; iLine++ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << std::endl;
    }
    ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines-1 << "F";
    ssLineCleaner << "\r" << ss.str(); // "\r" can be intercepted by loggers to know if a new line header can be printed

    if( trailBackCursor_ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines << "F" << std::endl;
      ssLineCleaner << static_cast<char>(27) << "[2K"; // un-flushed part: this clear line will only be displayed once a new line will try to override it
    }

    return ssLineCleaner.str();
  }




}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_IMPL_H
