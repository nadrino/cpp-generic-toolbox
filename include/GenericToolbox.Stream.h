//
// Created by Nadrino on 22/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_STREAM_H
#define CPP_GENERIC_TOOLBOX_STREAM_H

#include "GenericToolbox.Build.h"


#include <cstring>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <istream>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


// Declaration
namespace GenericToolbox{
  class DataStream{

  public:
    DataStream() = default;

    // setters
    inline void setPos(size_t pos_) const { _pos_ = pos_; }

    // const-getters
    inline size_t getPos() const { return _pos_; }
    [[nodiscard]] inline const std::vector<uint8_t>& getBuffer() const { return _buffer_; }

    // core
    CPP_GENERIC_TOOLBOX_STREAM_INLINE void load(std::istream& stream_, long long offset_=0, size_t size_=0);

    // templates
    template<typename T> inline auto readAt(size_t pos_) const -> T;
    template<typename T> inline void readAt(size_t pos_, T& out_) const;

    template<typename T> inline auto read() const -> T;
    template<typename T> inline void read(T& out_) const;

    CPP_GENERIC_TOOLBOX_STREAM_INLINE void readStringAt(size_t pos_, std::string& out_, size_t length_) const;
    CPP_GENERIC_TOOLBOX_STREAM_INLINE void readString(std::string& out_, size_t length_) const;
    CPP_GENERIC_TOOLBOX_STREAM_INLINE std::string readString(size_t length_) const;

  private:
    mutable size_t _pos_{0};
    std::vector<uint8_t> _buffer_{};

  };


  template<typename T> inline void DataStream::readAt(size_t pos_, T& out_) const{
    if( pos_ + sizeof(T) > _buffer_.size() ){
      throw std::runtime_error(
          "buffer overflow: buffer.size = "
          + std::to_string(_buffer_.size())
          + " / pos + data: " + std::to_string(pos_ + sizeof(T))
      );
    }
    out_ = *(reinterpret_cast<const T*>(&this->_buffer_[pos_]));
  }
  template<typename T> inline auto DataStream::readAt(size_t pos_) const -> T {
    T out{};
    this->readAt(pos_, out);
    return out;
  }

  template<typename T> inline void DataStream::read(T& out_) const {
    this->readAt(_pos_, out_);
    _pos_ += sizeof(T);
  }
  template<typename T> inline auto DataStream::read() const -> T {
    T out{};
    read(out);
    return out;
  }


}

#ifndef CPP_GENERIC_TOOLBOX_STREAM_COMPILED
#include "implementation/GenericToolbox.Stream.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_STREAM_H
