//
// Created by Nadrino on 22/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_STREAM_H
#define CPP_GENERIC_TOOLBOX_STREAM_H


#include <cstdint>
#include <istream>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


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
    inline void load(std::istream& stream_, long long offset_=0, size_t size_=0);

    // templates
    template<typename T> inline auto readAt(size_t pos_) const -> T;
    template<typename T> inline void readAt(size_t pos_, T& out_) const;

    template<typename T> inline auto read() const -> T;
    template<typename T> inline void read(T& out_) const;

    inline void readStringAt(size_t pos_, std::string& out_, size_t length_) const;
    inline void readString(std::string& out_, size_t length_) const;
    inline std::string readString(size_t length_) const;

  private:
    mutable size_t _pos_{0};
    std::vector<uint8_t> _buffer_{};

  };

}


#include "implementation/GenericToolbox.Stream.Impl.h"

#endif // CPP_GENERIC_TOOLBOX_STREAM_H
