//
// Created by Adrien BLANCHET on 06/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_IMPL_H

#include "GenericToolbox.h"

namespace GenericToolbox{

  inline VariableMonitor::VariableMonitor( std::string  name_ ) : _name_{std::move(name_)} {
    this->reset();
  }

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

  inline const std::string &VariableMonitor::getName() const {
    return _name_;
  }
  inline double VariableMonitor::getTotalAccumulated() const {
    return _totalAccumulated_;
  }
  inline double VariableMonitor::getLastValue() const{
    if( _currentSlotIndex_ == 0 ) return _addToAccumulatorHistory_.back();
    else return _addToAccumulatorHistory_.at(_currentSlotIndex_-1);
  }
  inline double VariableMonitor::evalTotalGrowthRate(){
    double output = (_totalAccumulated_ - _lastTotalAccumulated_);
    output /= std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - _lastTotalRateEval_
    ).count();
    output *= 1000.;

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
    return GenericToolbox::getAveragedSlope(orderedAddToAccumulatorHistory);
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
    return GenericToolbox::getAveragedSlope(orderedAddToAccumulatorHistory, deltaTimes);
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_IMPL_H
