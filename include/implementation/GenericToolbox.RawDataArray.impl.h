//
// Created by Nadrino on 16/11/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_IMPL_H

#include <utility>
#include "string"
#include "vector"

#include "GenericToolbox.h"


namespace GenericToolbox{

  inline void RawDataArray::reset(){
    rawData = std::vector<char>();
    resetCurrentByteOffset();
    unlockArraySize();
  }

  inline std::vector<char>& RawDataArray::getRawDataArray(){
    return rawData;
  }

  template<typename T> inline void RawDataArray::writeRawData(const T& data){
    this->writeRawData(data, _currentByteOffset_);
    _currentByteOffset_+=sizeof(data);
  }
  template<typename T> inline void RawDataArray::writeRawData(const T& data, size_t byteOffset_){
    if(rawData.size() < byteOffset_ + sizeof(data) ){
      if( _lockArraySize_ ) throw std::runtime_error("Can't resize raw array since _lockArraySize_ is true.");
      rawData.resize(byteOffset_ + sizeof(data));
    }
    memcpy(&rawData[byteOffset_], &data, sizeof(data));
  }

  inline void RawDataArray::resetCurrentByteOffset(){
    _currentByteOffset_=0;
  }
  inline void RawDataArray::lockArraySize(){
    _lockArraySize_=true;
  }
  inline void RawDataArray::unlockArraySize(){
    _lockArraySize_=false;
  }

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_IMPL_H
