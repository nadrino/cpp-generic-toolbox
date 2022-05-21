//
// Created by Nadrino on 16/11/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_H

#include <utility>
#include "string"
#include "vector"

#include "GenericToolbox.h"


namespace GenericToolbox{

  class RawDataArray{

  public:
    inline RawDataArray() = default;
    inline virtual ~RawDataArray() = default;

    inline void reset();

    inline std::vector<unsigned char>& getRawDataArray();
    inline const std::vector<unsigned char>& getRawDataArray() const;

    inline void writeMemoryContent(const void* address_, size_t dataSize_);
    inline void writeMemoryContent(const void* address_, size_t dataSize_, size_t byteOffset_);
    template<typename T> inline void writeRawData(const T& data); // auto incrementing "_currentOffset_"
    template<typename T> inline void writeRawData(const T& data, size_t byteOffset_);

    void resetCurrentByteOffset();
    void lockArraySize();
    void unlockArraySize();

  private:
    bool _lockArraySize_{false};
    size_t _currentByteOffset_{0};
    std::vector<unsigned char> rawData{};

  };

}

#include "implementation/GenericToolbox.RawDataArray.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_RAWDATAARRAY_H
