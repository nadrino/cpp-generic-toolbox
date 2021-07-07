//
// Created by Adrien BLANCHET on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H

#include <utility>

#include "string"
#include "vector"

#include "implementation/GenericToolbox.VariableMonitor.h"

// Classes : VariablesMonitor
namespace GenericToolbox{

  class VariablesMonitor {

  public:
    inline VariablesMonitor();
    inline virtual ~VariablesMonitor();

    inline void clearDisplayQuantityList();

    inline void reset();

    inline void addVariable(const std::string& name_);
    inline void addDisplayedQuantity(const std::string& quantityName_);
    inline void defineNewQuantity(const QuantityMonitor& quantity_);
    inline void setHeaderString(const std::string &headerString);
    inline void setFooterString(const std::string &footerString);
    inline void setMaxRefreshRateInMs(long long int maxRefreshRateInMs);

    inline VariableMonitor& getVariable(const std::string& name_);

    inline std::string generateMonitorString(bool trailBackCursor_ = false);

  private:
    long long _maxRefreshRateInMs_{500}; // 1/0.033 = 30 fps, 500 = 1/0.500 = 2 fps
    std::chrono::high_resolution_clock::time_point _lastGeneratedMonitorStringTime_;
    std::string _headerString_;
    std::string _footerString_;
    std::vector<VariableMonitor> _varMonitorList_;
    std::vector<QuantityMonitor> _quantityMonitorList_;
    std::vector<size_t> _displayQuantityIndexList_;
    std::vector<size_t> _basedPaddingList_;
  };

}

#include "implementation/GenericToolbox.VariablesMonitor.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H
