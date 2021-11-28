//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H

#include <utility>
#include "string"
#include "vector"

#include "GenericToolbox.h"


// Classes : VariablesMonitor
namespace GenericToolbox{

  struct CycleTimer{
    long long counts{0};
    long long cumulated{0};
    friend std::ostream& operator<< (std::ostream& stream, const CycleTimer& timer_) {
      stream << GenericToolbox::parseTimeUnit(timer_.cumulated / timer_.counts);
      return stream;
    }
  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
