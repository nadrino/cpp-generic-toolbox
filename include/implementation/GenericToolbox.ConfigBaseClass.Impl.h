//
// Created by Adrien BLANCHET on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H
#define GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H

#include "GenericToolbox.ConfigBaseClass.h"

#include <stdexcept>


namespace GenericToolbox{


  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::setConfig(const ConfigType &config_) {
    if( _isInitialized_ ) throw std::logic_error("Can't read the config while already initialized.");
    _config_ = config_;
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig() {
    if( _isInitialized_ ) throw std::logic_error("Can't read the config while already initialized.");
    _isConfigReadDone_ = true;
    this->readConfigImpl();
  }
  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig(const ConfigType& config_){
    this->setConfig(config_);
    this->readConfig();
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::initialize() {
    if( _isInitialized_ ) throw std::logic_error("Can't re-initialize while already done. Call unInitialize() before.");
    if( not _isConfigReadDone_ ) this->readConfig();
    this->initializeImpl();
    _isInitialized_ = true;
  }
  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::unInitialize(){
    _isInitialized_ = false;
  }

  template<class ConfigType> inline bool ConfigBaseClass<ConfigType>::isConfigReadDone() const { return _isConfigReadDone_; }
  template<class ConfigType> inline bool ConfigBaseClass<ConfigType>::isInitialized() const { return _isInitialized_; }
  template<class ConfigType> const ConfigType &ConfigBaseClass<ConfigType>::getConfig() const { return _config_; }


}


#endif //GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_IMPL_H
