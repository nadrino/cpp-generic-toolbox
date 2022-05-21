//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H

#include "GenericToolbox.h"
#include "GenericToolbox.Root.h"

#include "TGraph.h"
#include "TSpline.h"
#include "TLeaf.h"
#include "TClonesArray.h"

#include <typeinfo>

namespace GenericToolbox{

  inline LeafHolder::LeafHolder() { this->reset(); }
  inline LeafHolder::~LeafHolder() { this->reset(); }

  inline void LeafHolder::reset(){
    _leafTypeSize_=0;
    _leafTypeName_="";
    _byteBuffer_.clear();
  }
  inline void LeafHolder::hook(TTree *tree_, const std::string& leafName_){
    TLeaf* leaf = tree_->GetLeaf(leafName_.c_str());
    if(leaf == nullptr) throw std::runtime_error("_leaf_ is nullptr.");

    _leafTypeName_ = leaf->GetTypeName();
    _leafFullName_ = leaf->GetFullName();

    // Setup buffer
    _byteBuffer_.clear();
    if(leaf->GetNdata() != 0){
      _leafTypeSize_ = leaf->GetLenType();
      _byteBuffer_.resize(leaf->GetNdata() * _leafTypeSize_, 0);
    }
    else{
      // pointer-like obj
      _leafTypeSize_ = 2 * leaf->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      _byteBuffer_.resize(_leafTypeSize_, 0);
    }
    if( _byteBuffer_.empty() ){ throw std::runtime_error("empty byte buffer"); }

    tree_->SetBranchStatus(leaf->GetBranch()->GetName(), true);
    tree_->SetBranchAddress(leaf->GetBranch()->GetName(), (void*) &_byteBuffer_[0]);

    // THIS IS NOT WORKING WITH TCHAINS!!
//    _leaf_->GetBranch()->SetAddress(&_byteBuffer_[0]);
//    _leaf_->SetAddress(&_byteBuffer_[0]);
  }

  inline size_t LeafHolder::getLeafTypeSize() const {
    return _leafTypeSize_;
  }
  inline const std::string &LeafHolder::getLeafTypeName() const {
    return _leafTypeName_;
  }
  inline const std::string &LeafHolder::getLeafFullName() const {
    return _leafFullName_;
  }
  inline const std::vector<unsigned char> &LeafHolder::getByteBuffer() const {
    return _byteBuffer_;
  }

  inline std::vector<unsigned char> &LeafHolder::getByteBuffer(){
    return _byteBuffer_;
  }
  template<typename T> inline T& LeafHolder::getVariable(size_t arrayIndex_){
    return *((T*) &_byteBuffer_[arrayIndex_ * sizeof(T)]);
  }

  template<typename T> inline const T& LeafHolder::getVariable(size_t arrayIndex_) const{
    return *((T*) &_byteBuffer_[arrayIndex_ * sizeof(T)]);
  }

  inline void LeafHolder::copyToAny(std::vector<AnyType>& anyV_) const{
    if(anyV_.empty()){ anyV_.resize(getArraySize(), GenericToolbox::leafToAnyType(_leafTypeName_)); }
    for( size_t iSlot = 0 ; iSlot < anyV_.size() ; iSlot++ ){ this->copyToAny(anyV_[iSlot], iSlot); }
  }
  inline void LeafHolder::copyToAny(AnyType& any_, size_t slot_) const{
    memcpy(any_.getPlaceHolderPtr()->getVariableAddress(), &_byteBuffer_[slot_ * _leafTypeSize_], _leafTypeSize_);
  }
  inline size_t LeafHolder::getArraySize() const{
    return _byteBuffer_.size()/_leafTypeSize_;
  }

  inline std::ostream& operator <<( std::ostream& o, const LeafHolder& v ){
    if( not v._byteBuffer_.empty() ){
      o << v._leafFullName_ << "/" << v._leafTypeName_ << " = { ";
      auto aBuf = GenericToolbox::leafToAnyType(v._leafTypeName_);
      for(int iSlot=0 ; iSlot < v.getArraySize() ; iSlot++ ){
        if( iSlot != 0 ) o << ", ";
        v.copyToAny(aBuf, iSlot);
        o << aBuf;
      }
      o << " } | ";
      o << GenericToolbox::stackToHex(v._byteBuffer_, v._leafTypeSize_);
    }
    return o;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
