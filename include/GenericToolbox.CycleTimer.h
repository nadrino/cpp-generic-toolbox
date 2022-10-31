//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H

#include "GenericToolbox.h"

#include <utility>
#include "string"
#include "vector"


// Classes : VariablesMonitor
namespace GenericToolbox{

  struct CycleTimer{
    long long counts{0};
    long long cumulated{0};
    friend std::ostream& operator<< (std::ostream& stream, const CycleTimer& timer_) {
      if(timer_.counts == 0) stream << "0s";
      else stream << GenericToolbox::parseTimeUnit(double(timer_.cumulated) / double(timer_.counts));
      return stream;
    }
  };

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_CYCLETIMER_H
