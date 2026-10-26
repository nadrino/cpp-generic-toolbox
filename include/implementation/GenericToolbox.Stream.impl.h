#pragma once

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_STREAM_INLINE void DataStream::load(std::istream& stream_, long long offset_, size_t size_) {
    if( size_ == 0 ){ stream_.seekg(0, std::ios::end); size_ = stream_.tellg(); }
    _buffer_.clear();
    _buffer_.resize(size_);

    stream_.seekg( offset_ );
    stream_.read( reinterpret_cast<char *>(&_buffer_[0]), long(size_) );
  }

CPP_GENERIC_TOOLBOX_STREAM_INLINE void DataStream::readStringAt(size_t pos_, std::string& out_, size_t length_) const {
    out_.clear();
    out_.resize(length_);
    std::memcpy(&out_[0], &this->_buffer_[pos_], length_);
  }

CPP_GENERIC_TOOLBOX_STREAM_INLINE void DataStream::readString(std::string& out_, size_t length_) const {
    this->readStringAt(_pos_, out_, length_);
    _pos_ += length_;
  }

CPP_GENERIC_TOOLBOX_STREAM_INLINE std::string DataStream::readString(size_t length_) const {
    std::string out{};
    this->readString(out, length_);
    return out;
  }
}
