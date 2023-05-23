//
// Created by Nadrino on 16/11/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_IMPL_H

#include "GenericToolbox.h"

#include <string>
#include <vector>
#include <utility>


namespace GenericToolbox{

  inline void RawDataArray::reset(){
    rawData = std::vector<unsigned char>();
    resetCurrentByteOffset();
    unlockArraySize();
  }

  inline std::vector<unsigned char>& RawDataArray::getRawDataArray(){
    return rawData;
  }
  inline const std::vector<unsigned char>& RawDataArray::getRawDataArray() const{
    return rawData;
  }

  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_){
    this->writeMemoryContent(address_, dataSize_, _currentByteOffset_);
    _currentByteOffset_+=dataSize_;
  }
  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_, size_t byteOffset_){
    if(rawData.size() < byteOffset_ + dataSize_ ){
      if( _lockArraySize_ ) throw std::runtime_error("Can't resize raw array since _lockArraySize_ is true.");
      rawData.resize(byteOffset_ + dataSize_);
    }
    memcpy(&rawData[byteOffset_], address_, dataSize_);
  }
  template<typename T> inline void RawDataArray::writeRawData(const T& data){
    this->writeMemoryContent(&data, sizeof(data));
  }
  template<typename T> inline void RawDataArray::writeRawData(const T& data, size_t byteOffset_){
    this->writeMemoryContent(&data, sizeof(data), byteOffset_);
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
