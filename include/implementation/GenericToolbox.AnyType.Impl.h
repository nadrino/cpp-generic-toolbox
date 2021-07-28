//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_IMPL_H

namespace GenericToolbox{

  inline AnyType::AnyType(){ this->reset(); }
  inline AnyType::AnyType(const AnyType& other_){
    this->reset();
    this->_varPtr_ = std::shared_ptr<PlaceHolder>(other_._varPtr_->clone());
  }
  template<typename ValueType> inline AnyType::AnyType(const ValueType& value_){
    this->reset();
    this->template setValue(value_);
  }
  inline AnyType::~AnyType() { this->reset(); }

  template<typename ValueType> inline AnyType& AnyType::operator=(const ValueType & rhs) {
    AnyType(rhs).swap(*this);
    return *this;
  }
  inline AnyType& AnyType::operator=(const AnyType& rhs){
    AnyType(rhs).swap(*this);
    return *this;
  }

  inline void AnyType::reset(){
    _varPtr_.reset();
    _varPtr_ = nullptr;
  }
  inline bool AnyType::empty(){
    return (_varPtr_ == nullptr);
  }
  inline const std::type_info& AnyType::getType() const{
    return _varPtr_ != nullptr ? _varPtr_->getType() : typeid(void);
  }
  inline const std::shared_ptr<PlaceHolder> &AnyType::getVarPtr() const {
    return _varPtr_;
  }

  template<typename ValueType> inline void AnyType::setValue(const ValueType& value_){
    _varPtr_ = std::shared_ptr<VariableHolder<ValueType>>(new VariableHolder<ValueType>(value_));
  }
  template<typename ValueType> inline ValueType& AnyType::getValue() {
    if ( _varPtr_ == nullptr ){ throw std::runtime_error("AnyType value not set."); }
    if ( getType() != typeid(ValueType) ) { throw std::runtime_error("AnyType value type mismatch."); }
    return static_cast<VariableHolder<ValueType> *>(_varPtr_.get())->_variable_;
  }
  template<typename ValueType> inline ValueType& AnyType::getValue() const{
    if ( _varPtr_ == nullptr ){ throw std::runtime_error("AnyType value not set."); }
    if ( getType() != typeid(ValueType) ) { throw std::runtime_error("AnyType value type mismatch."); }
    return static_cast<const VariableHolder<ValueType> *>(_varPtr_.get())->_variable_;
  }
  inline double AnyType::getValueAsDouble() const{
    return _varPtr_->getVariableAsDouble();
  }

  inline std::ostream& operator<<( std::ostream& o, const AnyType& v ) {
    if( v._varPtr_ != nullptr ) v._varPtr_->writeToStream(o);
    return o;
  }

  // Protected
  inline AnyType& AnyType::swap(AnyType& rhs) {
    std::swap(_varPtr_, rhs._varPtr_);
    return *this;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_IMPL_H
