//
// Created by Nadrino on 06/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_H

#include <string>
#include "vector"
#include "chrono"
#include "functional"

namespace GenericToolbox{

  class VariableMonitor{

  public:
    inline explicit VariableMonitor( std::string  name_ );

    inline void reset();

    inline void addQuantity(const double& quantityToAdd_);

    inline const std::string &getName() const;
    inline double getTotalAccumulated() const;
    inline double getLastValue() const;

    inline double evalTotalGrowthRate();
    inline double evalCallGrowthRate() const;
    inline double evalCallGrowthRatePerSecond() const;


  private:
    const std::string _name_;

    double _totalAccumulated_{0};
    double _lastTotalAccumulated_{0};
    std::chrono::high_resolution_clock::time_point _lastTotalRateEval_;

    size_t _currentHistorySize_;
    std::vector<double> _addToAccumulatorHistory_;
    std::vector<std::chrono::high_resolution_clock::time_point> _addToAccumulatorTimeHistory_;
    size_t _currentSlotIndex_{0};

  };

  struct QuantityMonitor{
    QuantityMonitor(std::string  name_, std::string  title_, std::function<std::string(VariableMonitor&)>  evalFunction_, bool isExpandablePadSize_ = true ) :
        name(std::move(name_)), title(std::move(title_)), isExpandablePadSize(isExpandablePadSize_), evalFunction(std::move(evalFunction_)) {};
    std::string name; // kept for reference
    std::string title;
    bool isExpandablePadSize{true};
    std::function<std::string(VariableMonitor&)> evalFunction;
  };

}

#include "implementation/GenericToolbox.VariableMonitor.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLEMONITOR_H
