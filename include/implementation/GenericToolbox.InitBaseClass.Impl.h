//
// Created by Nadrino on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_INITBASECLASS_IMPL_H
#define GUNDAM_GENERICTOOLBOX_INITBASECLASS_IMPL_H

#include "../GenericToolbox.InitBaseClass.h"
#include "../GenericToolbox.h"

#include <stdexcept>


namespace GenericToolbox{

  inline void InitBaseClass::initialize() {
    if( _isInitialized_ ) throw std::logic_error("Can't re-initialize while already done. Call unInitialize() before.");
    this->initializeImpl();
    _isInitialized_ = true;
  }
  inline void InitBaseClass::unInitialize(){
    _isInitialized_ = false;
  }

  inline bool InitBaseClass::isInitialized() const { return _isInitialized_; }

  inline void InitBaseClass::throwIfInitialized(const std::string& functionName_) const {
    if( _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object already initialized.");
      }
      else{
        throw std::runtime_error(__METHOD_NAME__ + "Can't \""+functionName_+"\" while already initialized.");
      }
    }
  }
  inline void InitBaseClass::throwIfNotInitialized( const std::string& functionName_ ) const{
    if( not _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object not initialized.");
      }
      else{
        throw std::runtime_error(__METHOD_NAME__ + ": Can't \""+functionName_+"\" while not initialized.");
      }
    }
  }

}


#endif //GUNDAM_GENERICTOOLBOX_INITBASECLASS_IMPL_H
