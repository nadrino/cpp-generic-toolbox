//
// Created by Nadrino on 22/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_STREAM_IMPL_H
#define CPP_GENERIC_TOOLBOX_STREAM_IMPL_H

#include <stdexcept>


namespace GenericToolbox{


  inline void DataStream::load(std::istream& stream_, long long offset_, size_t size_){
    if( size_ == 0 ){ stream_.seekg(0, std::ios::end); size_ = stream_.tellg(); }
    _buffer_.clear();
    _buffer_.resize(size_);

    stream_.seekg( offset_ );
    stream_.read( reinterpret_cast<char *>(&_buffer_[0]), long(size_) );
  }
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

  inline void DataStream::readStringAt(size_t pos_, std::string& out_, size_t length_) const {
    out_.clear();
    out_.resize(length_);
    std::memcpy(&out_[0], &this->_buffer_[pos_], length_);
  }
  inline void DataStream::readString(std::string& out_, size_t length_) const{
    this->readStringAt(_pos_, out_, length_);
    _pos_ += length_;
  }
  inline std::string DataStream::readString(size_t length_) const{
    std::string out{};
    this->readString(out, length_);
    return out;
  }

}


#endif // CPP_GENERIC_TOOLBOX_STREAM_IMPL_H
