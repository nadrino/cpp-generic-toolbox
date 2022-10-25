//
// Created by Adrien BLANCHET on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H
#define GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H

#include "GenericToolbox.ConfigBaseClass.h"

namespace GenericToolbox{


  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::setConfig(const ConfigType &config_) {
    _config_ = config_;
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig() {
    _isConfigReadDone_ = true;
    this->readConfigImpl();
  }
  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig(const ConfigType& config_){
    this->setConfig(config_);
    this->readConfig();
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::initialize() {
    if( not _isConfigReadDone_ ) this->readConfig();
    this->initializeImpl();
    _isInitialized_ = true;
  }

  template<class ConfigType> inline bool ConfigBaseClass<ConfigType>::isConfigReadDone() const { return _isConfigReadDone_; }
  template<class ConfigType> inline bool ConfigBaseClass<ConfigType>::isInitialized() const { return _isInitialized_; }
  template<class ConfigType> const ConfigType &ConfigBaseClass<ConfigType>::getConfig() const { return _config_; }


}


#endif //GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H
