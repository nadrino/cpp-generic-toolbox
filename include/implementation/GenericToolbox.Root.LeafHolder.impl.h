//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H

#include "TGraph.h"
#include "TLeaf.h"
#include "TClonesArray.h"

#include "GenericToolbox.h"
#include "GenericToolbox.Root.h"

namespace GenericToolbox{

  inline LeafHolder::LeafHolder() { this->reset(); }
  inline LeafHolder::LeafHolder(const LeafHolder& other_){
    this->reset();
    _leafTypeName_ = other_._leafTypeName_;
    _leafDataList_ = other_._leafDataList_;
    if( _leafTypeName_ == "TClonesArray" ){
      for( size_t iLeaf = 0 ; iLeaf < _leafDataList_.size() ; iLeaf++ ){
        _leafDataList_[iLeaf] =
            std::shared_ptr<TClonesArray>(
                (TClonesArray*) (other_._leafDataList_[iLeaf].getValue<std::shared_ptr<TClonesArray>>())->Clone()
                );
//        std::cout << _leafDataList_[iLeaf].getValue<std::shared_ptr<TClonesArray>>() << " -> "
//        << ((TGraph*)(_leafDataList_[iLeaf].getValue<std::shared_ptr<TClonesArray>>())->At(0))->GetN() << std::endl;
      }
    }
//    _leafDataList_.resize(other_._leafDataList_.size());
//    for( size_t iLeaf = 0 ; iLeaf < _leafDataList_.size() ; iLeaf++ ){
//      _leafDataList_[iLeaf] = AnyType(other_._leafDataList_[iLeaf]);
//    }
  }
  inline LeafHolder::~LeafHolder() { this->reset(); }

  inline void LeafHolder::reset(){
    _leafTypeName_ = "";
    _leafDataList_.clear();
  }

  inline void LeafHolder::hookToTree(TTree* tree_, const std::string& branchName_){
    if(branchName_.empty()) throw std::logic_error("Can't hook empty branch name.");
    auto* treeLeafPtr = tree_->GetLeaf(branchName_.c_str());
    if( treeLeafPtr == nullptr ){
      throw std::runtime_error("Can't find branch \""+branchName_+"\" in tree "+tree_->GetName());
    }
    _leafTypeName_ = treeLeafPtr->GetTypeName();

    if( _leafTypeName_ == "Bool_t" ){
      this->defineVariable(Bool_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Bool_t>());
    }
    else if( _leafTypeName_ == "Char_t" ){
      this->defineVariable(Char_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Char_t>());
    }
    else if( _leafTypeName_ == "UChar_t" ){
      this->defineVariable(UChar_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UChar_t>());
    }
    else if( _leafTypeName_ == "Short_t" ){
      this->defineVariable(Short_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Short_t>());
    }
    else if( _leafTypeName_ == "UShort_t" ){
      this->defineVariable(UShort_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UShort_t>());
    }
    else if( _leafTypeName_ == "Int_t" ){
      this->defineVariable(Int_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Int_t>());
    }
    else if( _leafTypeName_ == "UInt_t" ){
      this->defineVariable(UInt_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<UInt_t>());
    }
    else if( _leafTypeName_ == "Long_t" ){
      this->defineVariable(Long_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Long_t>());
    }
    else if( _leafTypeName_ == "ULong_t" ){
      this->defineVariable(ULong_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<ULong_t>());
    }
    else if( _leafTypeName_ == "Long64_t" ){
      this->defineVariable(Long64_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Long64_t>());
    }
    else if( _leafTypeName_ == "ULong64_t" ){
      this->defineVariable(ULong64_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<ULong64_t>());
    }

    // Floating Variables
    else if( _leafTypeName_ == "Float16_t" ){
      this->defineVariable(Float16_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Float16_t>());
    }
    else if( _leafTypeName_ == "Float_t" ){
      this->defineVariable(Float_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Float_t>());
    }
    else if( _leafTypeName_ == "Double32_t" ){
      this->defineVariable(Double32_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Double32_t>());
    }
    else if( _leafTypeName_ == "Double_t" ){
      this->defineVariable(Double_t(), treeLeafPtr->GetNdata());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<Double_t>());
    }

    // TObjects (can't be loaded as objects)
    else if( _leafTypeName_ == "TGraph" ){
      this->defineVariable(TGraph(), treeLeafPtr->GetLen());
      tree_->SetBranchAddress(branchName_.c_str(), &this->getVariable<TGraph>());
    }
    else if( _leafTypeName_ == "TClonesArray" ){
//      this->defineVariable(std::nullptr_t(), treeLeafPtr->GetLen());
      this->defineVariable(std::shared_ptr<TClonesArray>(), treeLeafPtr->GetLen());
      GenericToolbox::muteRoot();
      // ROOT will complain about the wrong type of pointer
      tree_->SetBranchAddress(branchName_.c_str(), &(this->getVariable<std::shared_ptr<TClonesArray>>()));
      GenericToolbox::unmuteRoot();
    }

    // Others
    else{
      throw std::runtime_error(_leafTypeName_+" is not implemented.");
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

  inline const std::string &LeafHolder::getLeafTypeName() const {
    return _leafTypeName_;
  }

  inline std::ostream& operator <<( std::ostream& o, const LeafHolder& v ){
    if( v._leafDataList_.size() == 1 ){
      o << v._leafTypeName_ << ": " << v._leafDataList_.at(0);
    }
    else if( v._leafDataList_.size() > 1 ){
      o << v._leafTypeName_ << ": " << GenericToolbox::parseVectorAsString(v._leafDataList_);
    }
    return o;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_IMPL_H
