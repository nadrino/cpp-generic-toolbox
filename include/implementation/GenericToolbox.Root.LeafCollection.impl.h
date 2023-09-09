//
// Created by Nadrino on 07/09/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFFORM_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFFORM_IMPL_H

#include "GenericToolbox.h"
#include "GenericToolbox.Root.LeafCollection.h"


#include <stdexcept>
#include <iostream>
#include <sstream>

namespace GenericToolbox{

  inline void LeafForm::initialize(){

    // buffer data size
    _dataSize_ = 0; // reset
    if     ( _primaryLeafPtr_ != nullptr ){
      if( _primaryLeafPtr_->GetNdata() != 0 ){
        // primary type leaf (int, double, long, etc...)
        _dataSize_ += _primaryLeafPtr_->GetLenType();
      }
      else{
        // pointer-like obj (TGraph, TClonesArray...)
        _dataSize_ += 2 * _primaryLeafPtr_->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      _dataSize_ = 8; // double
    }
    else{
      throw std::runtime_error(__METHOD_NAME__ + ": no data defined -> " + this->getSummary());
    }


    // buffer data address
    _dataAddress_ = nullptr;
    if     ( _primaryLeafPtr_ != nullptr ){
      _dataAddress_ = _primaryLeafPtr_->GetBranch()->GetAddress() + _primaryLeafPtr_->GetOffset();
      if( not _arrayIndices_.empty() ){
        _dataAddress_ = (char*) _dataAddress_ + _arrayIndices_[0];
      }
//      else if( not _nestedLeafFormPtrList_.empty() ){
//        // will be done while eval, only caching the base address
//      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      _dataAddress_ = (void*) &_localBuffer_;
    }
  }

  inline void LeafForm::fillLocalBuffer() const {
    if( _treeFormulaPtr_ == nullptr ){
      memcpy(&_localBuffer_, this->getDataAddress(), std::min(this->getDataSize(), sizeof(double)));
    }
    else{
      _localBuffer_ = _treeFormulaPtr_->EvalInstance(0);
    }
  }
  template<typename T> const T& LeafForm::eval() const {
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    return *((T*) this->getDataAddress());
  }
  inline void LeafForm::dropToAny(GenericToolbox::AnyType& any_) const{
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    memcpy(any_.getPlaceHolderPtr()->getVariableAddress(), this->getDataAddress(), this->getDataSize());
  }

  inline void* LeafForm::getDataAddress() const{
    if( _primaryLeafPtr_ != nullptr and not _nestedLeafFormPtrList_.empty() ){
      return ((char*) _dataAddress_) + _nestedLeafFormPtrList_[0]->eval<int>(); // offset
    }
    return _dataAddress_;
  }
  inline size_t LeafForm::getDataSize() const{ return _dataSize_; }
  inline std::string LeafForm::getLeafTypeName() const{
    if     ( _primaryLeafPtr_ != nullptr ){ return _primaryLeafPtr_->GetTypeName(); }
    else if( _treeFormulaPtr_ != nullptr ){ return "Double_t"; }
    return {};
  }
  inline std::string LeafForm::getSummary() const{
    std::stringstream ss;
    if     ( _primaryLeafPtr_ != nullptr ){
      ss << _primaryExprStr_ << ": br{ " << _primaryLeafPtr_->GetBranch()->GetFullName() << " }";
      if( not _arrayIndices_.empty() ){
        ss << " indices{ ";
        for( size_t iDim = 0 ; iDim < _arrayIndices_.size() ; iDim++ ){
          if( _arrayIndices_[iDim] != -1 ){ ss << _arrayIndices_[iDim]; }
          else{ ss << _nestedLeafFormPtrList_[iDim]->getPrimaryExprStr(); }
          if( iDim+1 != _arrayIndices_.size() ){ ss << ", "; }
        }
        ss << " }";
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      ss << "formula{ \"" << this->getTreeFormulaPtr()->GetName() << "\" }";
    }
    ss << ", addr{ " << this->getDataAddress() << " }, size{ " << this->getDataSize() << " }";
    if( this->getDataAddress() != nullptr and this->getDataSize() != 0 ){
      if( _treeFormulaPtr_ == nullptr ){
        ss << ", data{ 0x" << GenericToolbox::toHex(this->getDataAddress(), this->getDataSize()) << " }";
      }
      else{
        this->fillLocalBuffer();
        ss << ", eval{ " << _localBuffer_ << " }";
      }

    }
    return ss.str();
  }



  inline LeafCollection::~LeafCollection(){
    if( _treePtr_ != nullptr and _notifyList_ != nullptr ){
      // TTree will conflict the ownership as the notifyList is handled bu
      _treePtr_->SetNotify(nullptr);
    }
  }

  inline int LeafCollection::addLeafExpression(const std::string& leafExpStr_){
    auto iExp{this->getLeafExpIndex(leafExpStr_)};
    if( iExp != -1 ){ return iExp; }
    _leafExpressionList_.emplace_back( leafExpStr_ );
    return int(_leafExpressionList_.size())-1;
  }

  inline void LeafCollection::initialize() {
    if( _treePtr_ == nullptr ){ throw std::logic_error("_treePtr_ not set. Can't" + __METHOD_NAME__); }

    // make sure branch are available for TTreeFormula
    _treePtr_->SetBranchStatus("*", true);

    // read the expressions
    this->parseExpressions();

    // hook now? later?
    for( auto& branchBuf : _branchBufferList_ ){
      branchBuf.buildBuffer();
      branchBuf.hookBuffer();
    }

    // init
    for( auto& leafForm : _leafFormList_ ){ leafForm.initialize(); }

    // only enable selected branches
    _treePtr_->SetBranchStatus("*", false);
    for( auto& leafForm : _leafFormList_ ){
      if( leafForm.getTreeFormulaPtr() != nullptr ){
        GenericToolbox::enableSelectedBranches(_treePtr_, leafForm.getTreeFormulaPtr().get());
      }
      else{
        leafForm.getPrimaryLeafPtr()->GetBranch()->SetStatus(true);
      }
    }

//    GenericToolbox::disableUnhookedBranches( _treePtr_ );

//    auto nBranches = _treePtr_->GetListOfBranches()->GetEntries();
//    for( int iBr = 0 ; iBr < nBranches ; iBr++ ){
//      auto* br = (TBranch*) _treePtr_->GetListOfBranches()->At(iBr);
//    }
  }
  inline std::string LeafCollection::getSummary() const{
    std::stringstream ss;
    ss << "tree{ " << _treePtr_->GetName() << " }";
    ss << std::endl << "leavesExpr" << GenericToolbox::parseVectorAsString(_leafExpressionList_, true);
    ss << std::endl << "branchesBuf" << GenericToolbox::iterableToString(_branchBufferList_, [](const BranchBuffer& b){ return b.getSummary(); });
    ss << std::endl << "leafForm" << GenericToolbox::iterableToString(_leafFormList_, [](const LeafForm& l){ return l.getSummary(); } );
    return ss.str();
  }
  inline int LeafCollection::getLeafExpIndex(const std::string& leafExpression_) const {
    return GenericToolbox::findElementIndex( leafExpression_, _leafExpressionList_ );
  }
  inline const LeafForm* LeafCollection::getLeafFormPtr(const std::string& leafExpression_) const {
    auto idx{getLeafExpIndex(leafExpression_)};
    if( not _leafFormList_.empty() and idx != -1 ){ return &_leafFormList_[idx]; }
    return nullptr;
  }

  inline void LeafCollection::parseExpressions() {

    // avoid moving memory around
    _leafFormList_.reserve(_leafExpressionList_.size());

    // loop over the expressions. _leafExpressionList_ size might change within the loop
    for( size_t iExp = 0 ; iExp < _leafExpressionList_.size() ; iExp++ ){
      _leafFormList_.emplace_back();
      _leafFormList_.back().setPrimaryExprStr( _leafExpressionList_[iExp] );

      std::vector<std::string> argBuffer;
      auto strippedLeafExpr = GenericToolbox::stripBracket(_leafExpressionList_[iExp], '[', ']', false, &argBuffer);

      if( strippedLeafExpr.empty() ){
        throw std::runtime_error("Bad leaf form expression: " + _leafExpressionList_[iExp]);
      }

      // first, check if the remaining expr is a leaf
      auto* leafPtr = _treePtr_->GetLeaf(strippedLeafExpr.c_str());

      if( leafPtr != nullptr ){
        // leaf exists, create the associated branch buffer if not already set
        this->setupBranchBuffer( leafPtr );

        // set the leaf data will be extracted
        _leafFormList_.back().setPrimaryLeafPtr( leafPtr );

        // check if all args in between [] are 'int'. Otherwise, create an extra leaf dependency
        for( auto& arg : argBuffer ){
          try{
            int index = std::stoi(arg);
            _leafFormList_.back().addIndex( index );
            _leafFormList_.back().addNestedLeafFormPtr( nullptr );
          }
          catch(...){
            // nested? -> try
            size_t idx = this->addLeafExpression( arg ); // will be processed later
            _leafFormList_.back().addIndex( -1 );
            _leafFormList_.back().addNestedLeafFormPtr( (LeafForm*) idx ); // tweaking types...
          }
        }
      }
      else{
        // use a complete TTreeFormula to eval the obj
        _leafFormList_.back().setTreeFormulaPtr( std::make_shared<TTreeFormula>(
            _leafExpressionList_[iExp].c_str(),
            _leafExpressionList_[iExp].c_str(),
            _treePtr_
        ) );

        // ROOT Hot fix: https://root-forum.cern.ch/t/ttreeformula-evalinstance-return-0-0/16366/10
        _leafFormList_.back().getTreeFormulaPtr()->GetNdata();

        if( _leafFormList_.back().getTreeFormulaPtr()->GetNdim() == 0 ){
          throw std::runtime_error(__METHOD_NAME__+": \"" + _leafExpressionList_[iExp] + "\" could not be parsed by the TTree");
        }

        if( _notifyList_ == nullptr ){
          _notifyList_ = std::make_shared<TList>();
          _treePtr_->SetNotify(_notifyList_.get());
        }

        _notifyList_->Add( _leafFormList_.back().getTreeFormulaPtr().get() );

      }
    }

    // refill up with the proper ptr addresses
    for( auto& leafForm: _leafFormList_ ){
      for( auto& nestedForm : leafForm.getNestedLeafFormPtrList() ){
        auto iForm = (size_t) nestedForm;
        nestedForm = &(_leafFormList_[iForm]);
      }
    }

  }
  inline void LeafCollection::setupBranchBuffer(TLeaf* leaf_){
    auto* brPtr = leaf_->GetBranch();

    // leave if already set
    for( auto& branchBuffer : _branchBufferList_ ){
      if( branchBuffer.getBranchPtr() == brPtr ){ return; }
    }

    _branchBufferList_.emplace_back();
    _branchBufferList_.back().setBranchPtr( brPtr );
  }

}

#endif // CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFFORM_IMPL_H
