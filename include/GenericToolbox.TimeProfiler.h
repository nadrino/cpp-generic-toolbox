//
// Created by Nadrino on 31/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_TIMEPROFILER_H
#define GUNDAM_GENERICTOOLBOX_TIMEPROFILER_H

#include "GenericToolbox.h"

#include <chrono>

namespace GenericToolbox{

  class TimeProfileMaster{
  public:
    TimeProfileMaster() = default;

    void addDuration(const std::chrono::duration<std::chrono::steady_clock>& deltaT_){
      totalDuration += deltaT_;
    }

  private:
    std::chrono::duration<std::chrono::steady_clock> totalDuration;
  };

  class TimeProfileSlave{
  public:
    explicit TimeProfileSlave(TimeProfileMaster& master_): _master_(&master_), _start_(std::chrono::high_resolution_clock::now()) {}
    ~TimeProfileSlave(){
      _master_->addDuration(std::chrono::high_resolution_clock::now() - _start_);
    }


  private:
    TimeProfileMaster* _master_;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start_;
  };

}



#endif //GUNDAM_GENERICTOOLBOX_TIMEPROFILER_H
