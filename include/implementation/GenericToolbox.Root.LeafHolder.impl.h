//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H

#include <typeinfo>

#include "TGraph.h"
#include "TSpline.h"
#include "TLeaf.h"
#include "TClonesArray.h"

#include "GenericToolbox.h"
#include "GenericToolbox.Root.h"

namespace GenericToolbox{

  inline LeafHolder::LeafHolder() { this->reset(); }
  inline LeafHolder::~LeafHolder() { this->reset(); }

  inline void LeafHolder::reset(){
    _leafDataList_.clear();
  }

  inline void LeafHolder::hookToTree(TTree* tree_, const std::string& branchName_){
    if(branchName_.empty()) throw std::logic_error("Can't hook empty branch name.");
    auto* treeLeafPtr = tree_->GetLeaf(branchName_.c_str());
    if( treeLeafPtr == nullptr ){
      std::stringstream ss;
      ss << "Can't find leaf " << branchName_ << " in tree " << tree_->GetName();
      std::cout << ss.str() << std::endl;
      throw std::runtime_error(ss.str());
    }
    std::string leafTypeName = treeLeafPtr->GetTypeName();
    if(leafTypeName == "Bool_t" ){
      this->defineVariable(Bool_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Bool_t>());
    }
    else if(leafTypeName == "Char_t" ){
      this->defineVariable(Char_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Char_t>());
    }
    else if(leafTypeName == "UChar_t" ){
      this->defineVariable(UChar_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UChar_t>());
    }
    else if(leafTypeName == "Short_t" ){
      this->defineVariable(Short_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Short_t>());
    }
    else if(leafTypeName == "UShort_t" ){
      this->defineVariable(UShort_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UShort_t>());
    }
    else if(leafTypeName == "Int_t" ){
      this->defineVariable(Int_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Int_t>());
    }
    else if(leafTypeName == "UInt_t" ){
      this->defineVariable(UInt_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UInt_t>());
    }
    else if(leafTypeName == "Long_t" ){
      this->defineVariable(Long_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Long_t>());
    }
    else if(leafTypeName == "ULong_t" ){
      this->defineVariable(ULong_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<ULong_t>());
    }
    else if(leafTypeName == "Long64_t" ){
      this->defineVariable(Long64_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Long64_t>());
    }
    else if(leafTypeName == "ULong64_t" ){
      this->defineVariable(ULong64_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<ULong64_t>());
    }

    // Floating Variables
    else if(leafTypeName == "Float16_t" ){
      this->defineVariable(Float16_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Float16_t>());
    }
    else if(leafTypeName == "Float_t" ){
      this->defineVariable(Float_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Float_t>());
    }
    else if(leafTypeName == "Double32_t" ){
      this->defineVariable(Double32_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Double32_t>());
    }
    else if(leafTypeName == "Double_t" ){
      this->defineVariable(Double_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Double_t>());
    }

    // TObjects (can't be loaded as objects)
    else if(leafTypeName == "TGraph" ){
      TGraph* bufGrPtr{nullptr};
      this->defineVariable(bufGrPtr, treeLeafPtr->GetLen());
      tree_->SetBranchAddress(branchName_.c_str(), &(this->getVariable<TGraph*>()));
    }
    else if(leafTypeName == "TSpline3" ){
      TSpline3* bufGrPtr{nullptr};
      this->defineVariable(bufGrPtr, treeLeafPtr->GetLen());
      tree_->SetBranchAddress(branchName_.c_str(), &(this->getVariable<TSpline3*>()));
    }
    else if(leafTypeName == "TClonesArray" ){
      TClonesArray* bufPtr{nullptr};
      this->defineVariable(bufPtr, treeLeafPtr->GetLen());
      // ROOT will complain about the wrong type of pointer
      tree_->SetBranchAddress(branchName_.c_str(), &(this->getVariable<TClonesArray*>()));
    }

    // Others
    else{
      throw std::runtime_error(leafTypeName + " is not implemented.");
    }
  }
  template<typename T> inline void LeafHolder::defineVariable(T* variable_, size_t arraySize_){
    if( not _leafDataList_.empty() ) throw std::logic_error("LeafHolder not empty: can't defineVariable");
    if(arraySize_ == 0) throw std::runtime_error("Invalid arraySize_");
    _leafDataList_.resize(arraySize_);
    for( auto& leafData: _leafDataList_ ){
      leafData = variable_; // create the variable with the right type
    }
  }
  template<typename T> inline void LeafHolder::defineVariable(T variable_, size_t arraySize_){
    if( not _leafDataList_.empty() ) throw std::logic_error("LeafHolder not empty: can't defineVariable");
    if(arraySize_ == 0) throw std::runtime_error("Invalid arraySize_");
    _leafDataList_.resize(arraySize_);
    for( auto& leafData: _leafDataList_ ){
      leafData = variable_; // create the variable with the right type
    }
  }
  template<typename T> inline T& LeafHolder::getVariable(size_t arrayIndex_){
    return _leafDataList_.at(arrayIndex_).template getValue<T>();
  }
  template<typename T> inline const T& LeafHolder::getVariable(size_t arrayIndex_) const{
    return _leafDataList_.at(arrayIndex_).template getValue<T>();
  }
  inline double LeafHolder::getVariableAsDouble(size_t arrayIndex_) const{
    return _leafDataList_.at(arrayIndex_).getValueAsDouble();
  }
  inline size_t LeafHolder::getArraySize() const{
    return _leafDataList_.size();
  }
  inline void* LeafHolder::getVariableAddress(size_t arrayIndex_){
    return _leafDataList_.at(arrayIndex_).getPlaceHolderPtr().get();
  }
  inline const GenericToolbox::AnyType& LeafHolder::getLeafDataAddress(size_t arrayIndex_) const{
    return _leafDataList_.at(arrayIndex_);
  }
  inline size_t LeafHolder::getVariableSize(size_t arrayIndex_) const{
    return _leafDataList_.at(arrayIndex_).getPlaceHolderPtr()->getVariableSize();
  }
  inline char LeafHolder::findOriginalVariableType() const{

//    auto* classInfo = (TClass*) TDictionary::GetDictionary(_leafDataList_[0].getType());
//    classInfo->Sizeof();

    if( _leafDataList_.empty() ) return 0;
    else if( _leafDataList_[0].getType() == typeid(Bool_t) ){ return 'O'; }
    else if( _leafDataList_[0].getType() == typeid(Char_t) ){ return 'B'; }
    else if( _leafDataList_[0].getType() == typeid(UChar_t) ){ return 'b'; }
    else if( _leafDataList_[0].getType() == typeid(Short_t) ){ return 'S'; }
    else if( _leafDataList_[0].getType() == typeid(UShort_t) ){ return 's'; }
    else if( _leafDataList_[0].getType() == typeid(Int_t) ){ return 'I'; }
    else if( _leafDataList_[0].getType() == typeid(UInt_t) ){ return 'i'; }
    else if( _leafDataList_[0].getType() == typeid(Float_t) ){ return 'F'; }    // `F` : a 32 bit floating point (`Float_t`)
    else if( _leafDataList_[0].getType() == typeid(Float16_t) ){ return 'f'; }  // `f` : a 24 bit floating point with truncated mantissa
    else if( _leafDataList_[0].getType() == typeid(Double_t) ){ return 'D'; }   // `D` : a 64 bit floating point (`Double_t`)
    else if( _leafDataList_[0].getType() == typeid(Double32_t) ){ return 'd'; } // `d` : a 24 bit truncated floating point (`Double32_t`)
    else if( _leafDataList_[0].getType() == typeid(Long64_t) ){ return 'L'; }
    else if( _leafDataList_[0].getType() == typeid(ULong64_t) ){ return 'l'; }
    else if( _leafDataList_[0].getType() == typeid(Long_t) ){ return 'G'; } // `G` : a long signed integer, stored as 64 bit (`Long_t`)
    else if( _leafDataList_[0].getType() == typeid(ULong_t) ){ return 'g'; } // `g` : a long unsigned integer, stored as 64 bit (`ULong_t`)
    return char(0xFF); // OTHER??
  }

  inline void LeafHolder::clonePointerLeaves(){
    for(auto & leafData : _leafDataList_){
      if( leafData.getType() == typeid(std::shared_ptr<TClonesArray>) ){
        leafData = std::shared_ptr<TClonesArray>(
            (TClonesArray*) (leafData.getValue<std::shared_ptr<TClonesArray>>())->Clone()
            );
      }
//      else if(){
//        // ...
//      }
    }
  }

  inline std::ostream& operator <<( std::ostream& o, const LeafHolder& v ){
    if( v._leafDataList_.empty() ) return o;
    o << v._leafDataList_.at(0).getType().name();
    if( v._leafDataList_.size() == 1 ){
      o << ": " << v._leafDataList_.at(0);
    }
    else if( v._leafDataList_.size() > 1 ){
      o << ": " << GenericToolbox::parseVectorAsString(v._leafDataList_);
    }
    return o;
  }

  inline const std::vector<AnyType> &LeafHolder::getLeafDataList() const {
    return _leafDataList_;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
