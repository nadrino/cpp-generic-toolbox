//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H

#include "GenericToolbox.TablePrinter.h"
#include "implementation/GenericToolbox.VariableMonitor.h" // tools

#include <string>
#include <vector>
#include <utility>


// Classes : VariablesMonitor
namespace GenericToolbox{

  class VariablesMonitor {

  public:
    inline VariablesMonitor();
    inline virtual ~VariablesMonitor();

    inline void clearDisplayQuantityList();

    inline void addVariable(const std::string& name_);
    inline void addDisplayedQuantity(const std::string& quantityName_);
    inline void defineNewQuantity(const QuantityMonitor& quantity_);
    inline void setHeaderString(const std::string &headerString);
    inline void setFooterString(const std::string &footerString);
    inline void setMaxRefreshRateInMs(long long int maxRefreshRateInMs);

    inline long long int getMaxRefreshRateInMs() const;
    inline VariableMonitor& getVariable(const std::string& name_);
    inline QuantityMonitor& getQuantity(const std::string& quantityName_);

    inline bool isGenerateMonitorStringOk();
    inline std::string generateMonitorString(bool trailBackCursor_ = false, bool forceGenerate_ = false);

  private:
    long long int _maxRefreshRateInMs_{500}; // 1/0.033 = 30 fps, 500 = 1/0.500 = 2 fps
    std::chrono::high_resolution_clock::time_point _lastGeneratedMonitorStringTime_;
    std::string _headerString_;
    std::string _footerString_;
    std::vector<VariableMonitor> _varMonitorList_;
    std::vector<QuantityMonitor> _quantityMonitorList_;
    std::vector<size_t> _displayQuantityIndexList_;
    std::vector<size_t> _basedPaddingList_;

    TablePrinter _tablePrinter_;
  };

}

#include "implementation/GenericToolbox.VariablesMonitor.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_VARIABLESMONITOR_H
