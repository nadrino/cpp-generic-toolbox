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

  inline void BranchBuffer::buildBuffer(){
    if( not _byteBuffer_.empty() ){ throw std::logic_error(__METHOD_NAME__ + ": buffer already set."); }
    if( _branchPtr_ == nullptr ){ throw std::runtime_error(__METHOD_NAME__ + ": branch not set."); }

    // Calculating the requested buffer size
    size_t bufferSize{0};
    auto* leavesList = _branchPtr_->GetListOfLeaves();
    int nLeaves = leavesList->GetEntries();
    for( int iLeaf = 0 ; iLeaf < nLeaves ; iLeaf++ ){
      auto* l = (TLeaf*) leavesList->At(iLeaf);
      if( l->GetNdata() != 0 ){
        // primary type leaf (int, double, long, etc...)
        bufferSize += l->GetNdata() * l->GetLenType();
      }
      else{
        // pointer-like obj (TGraph, TClonesArray...)
        bufferSize += 2 * l->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      }
    }

    if( bufferSize == 0 ){
      throw std::runtime_error(__METHOD_NAME__ + ": empty buffer size for branch: " + _branchPtr_->GetName());
    }

    _byteBuffer_.resize( bufferSize, 0 );
    if( _byteBuffer_.empty() ){ throw std::runtime_error(__METHOD_NAME__ + ": empty byte buffer"); }
  }
  inline void BranchBuffer::hookBuffer(){
    if( _byteBuffer_.empty() ){ throw std::runtime_error(__METHOD_NAME__ + ": empty byte buffer"); }
    _branchPtr_->SetStatus( true ); // on notify, ttree might have started back to all disabled state
    _branchPtr_->SetAddress( (void*) &_byteBuffer_[0] );
  }
  inline std::string BranchBuffer::getSummary() const {
    std::stringstream ss;
    if( _branchPtr_ != nullptr ){
      ss << _branchPtr_->GetName() << ": addr{" << (void*) _branchPtr_->GetAddress() << "}, size{" << _byteBuffer_.size() << "}";
    }
    else{
      ss << "branch not set";
    }
    return ss.str();
  }


  inline void LeafForm::initialize(){
    this->cacheDataSize();
    this->cacheDataAddr();
  }

  inline void* LeafForm::getDataAddress() const{
    if( _nestedLeafFormPtr_ != nullptr ){
      return ((char*) _dataAddress_) + int(_nestedLeafFormPtr_->evalAsDouble()) * this->getDataSize(); // offset
    }
    return _dataAddress_;
  }
  inline size_t LeafForm::getDataSize() const{ return _dataSize_; }
  inline std::string LeafForm::getLeafTypeName() const{
    if     ( _primaryLeafPtr_ != nullptr ){ return _primaryLeafPtr_->GetTypeName(); }
    else if( _treeFormulaPtr_ != nullptr ){ return "Double_t"; }
    return {};
  }

  inline void LeafForm::fillLocalBuffer() const {
    if( _treeFormulaPtr_ == nullptr ){
      memcpy(&_localBuffer_, this->getDataAddress(), std::min(this->getDataSize(), sizeof(double)));
    }
    else{
      _localBuffer_ = _treeFormulaPtr_->EvalInstance(0);
    }
  }
  inline double LeafForm::evalAsDouble() const {
    if( _treeFormulaPtr_ != nullptr ){
      this->fillLocalBuffer();
      return _localBuffer_; // double by default
    }
    else if( this->getLeafTypeName() == "Double_t" ){
      return this->eval<double>();
    }
    else{
      if( _anyTypeContainer_ == nullptr ){
        _anyTypeContainer_ = std::make_shared<AnyType>( GenericToolbox::leafToAnyType( this->getLeafTypeName() ) );
      }
      this->dropToAny(*_anyTypeContainer_);
      return _anyTypeContainer_->getValueAsDouble();
    }
  }
  inline void LeafForm::dropToAny(GenericToolbox::AnyType& any_) const{
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    memcpy(any_.getPlaceHolderPtr()->getVariableAddress(), this->getDataAddress(), this->getDataSize());
  }
  inline std::string LeafForm::getSummary() const{
    std::stringstream ss;
    if     ( _primaryLeafPtr_ != nullptr ){
      ss << _primaryExprStr_ << ": br{ " << _primaryLeafFullName_ << " }";
      if     ( _index_ != 0 )                  { ss << "[" << _index_ << "]"; }
      else if( _nestedLeafFormPtr_ != nullptr ){
        ss << "[" << _nestedLeafFormPtr_->getPrimaryExprStr() << " -> " << int(_nestedLeafFormPtr_->evalAsDouble()) << "]";
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

  inline void LeafForm::cacheDataSize(){
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
  }
  inline void LeafForm::cacheDataAddr(){
    // buffer data address
    _dataAddress_ = nullptr;
    if     ( _primaryLeafPtr_ != nullptr ){
      _dataAddress_ = _primaryLeafPtr_->GetBranch()->GetAddress() + _primaryLeafPtr_->GetOffset();
      if( _index_ != 0 ){
        _dataAddress_ = (char*) _dataAddress_ + _index_ * this->getDataSize();
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      _dataAddress_ = (void*) &_localBuffer_;
    }
  }

  template<typename T> inline const T& LeafForm::eval() const {
    // Use ONLY if the type is known
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    auto* addr{(T*) this->getDataAddress()};
    if( addr == nullptr ){ throw std::runtime_error(__METHOD_NAME__ + ": invalid address: " + this->getSummary()); }
    return *addr;
  }

  inline LeafCollection::~LeafCollection(){
    if( _treePtr_ != nullptr and _treePtr_->GetNotify() == &_objNotifier_ ){
      // TTree will conflict the ownership as the _objNotifier_ is handled by us
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

    // make sure the branch & leaf addr get updated
    _objNotifier_.setOnNotifyFct( [this](){ this->doNotify(); } );
    _treePtr_->SetNotify( &_objNotifier_ );
    this->doNotify();
  }
  void LeafCollection::doNotify(){
    // use for updating branches and leaves addresses
    // should be auto triggered by the TTree

    // reset all branch status to 0
    _treePtr_->SetBranchStatus("*", false);

    for( auto& br : _branchBufferList_ ){
      br.setBranchPtr( _treePtr_->GetBranch( br.getBranchName().c_str() ) );
      br.hookBuffer();
    }

    for( auto& lf : _leafFormList_ ){
      if( lf.getPrimaryLeafPtr() != nullptr ){
        lf.setPrimaryLeafPtr( _treePtr_->GetLeaf( lf.getPrimaryLeafFullName().c_str() ) );
      }
      if( lf.getTreeFormulaPtr() != nullptr ){
        lf.getTreeFormulaPtr()->Notify();
        GenericToolbox::enableSelectedBranches(_treePtr_, lf.getTreeFormulaPtr().get());
      }
      lf.cacheDataAddr();
    }
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

    // loop over the expressions. _leafExpressionList_ size might change within the loop -> USE INDICES (iExp) loop based
    for( size_t iExp = 0 ; iExp < _leafExpressionList_.size() ; iExp++ ){
      _leafFormList_.emplace_back();
      _leafFormList_.back().setPrimaryExprStr( _leafExpressionList_[iExp] );

      std::vector<std::string> argBuffer;
      auto strippedLeafExpr = GenericToolbox::stripBracket(_leafExpressionList_[iExp], '[', ']', false, &argBuffer);
      if( strippedLeafExpr.empty() ){ throw std::runtime_error(__METHOD_NAME__ + " Bad leaf form expression: " + _leafExpressionList_[iExp]); }

      // first, check if the remaining expr is a leaf
      auto* leafPtr = _treePtr_->GetLeaf(strippedLeafExpr.c_str());
      if( leafPtr == nullptr or argBuffer.size() > 1 ){
        // no leaf or multi-dim array -> use a complete TTreeFormula to eval the obj

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
      }
      else{
        // leaf exists, create the associated branch buffer if not already set
        this->setupBranchBuffer( leafPtr );

        // set the leaf data will be extracted
        _leafFormList_.back().setPrimaryLeafPtr( leafPtr );

        // array-like
        if( not argBuffer.empty() ){
          try{
            int index = std::stoi(argBuffer[0]);
            _leafFormList_.back().setIndex( index );
          }
          catch(...){
            // nested? -> try
            size_t idx = this->addLeafExpression( argBuffer[0] ); // will be processed later
            _leafFormList_.back().setNestedFormPtr( (LeafForm*) idx ); // tweaking types while not all ptr are settled
          }
        }
      }
    }

    // refill up with the proper ptr addresses now _leafFormList_ size won't change
    for( auto& leafForm: _leafFormList_ ){
      if( leafForm.getNestedFormPtr() != nullptr ){
        leafForm.setNestedFormPtr( &_leafFormList_[(size_t) leafForm.getNestedFormPtr()] );
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
