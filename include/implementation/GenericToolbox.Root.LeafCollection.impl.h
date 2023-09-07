//
// Created by Nadrino on 07/09/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFFORM_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFFORM_IMPL_H

#include "GenericToolbox.h"

#include "exception"
#include "GenericToolbox.Root.LeafCollection.h"


namespace GenericToolbox{

  inline size_t LeafCollection::addLeafExpression(const std::string& leafExpStr_){
    auto iExp{GenericToolbox::findElementIndex( leafExpStr_, _leafExpressionList_ )};
    if( iExp  != -1 ){ return iExp; }
    _leafExpressionList_.emplace_back( leafExpStr_ );
    return _leafExpressionList_.size()-1;
  }

  inline void LeafCollection::initialize() {
    std::cout << __METHOD_NAME__ << std::endl;
    if( _treePtr_ == nullptr ){ throw std::logic_error("_treePtr_ not set. Can't" + __METHOD_NAME__); }

    _treePtr_->SetBranchStatus("*", true);
    this->parseExpressions();

    for( auto& branchBuf : _branchBufferList_ ){
      branchBuf.buildBuffer();
      branchBuf.hookBuffer();
    }

    _treePtr_->SetBranchStatus("*", false);
    // re-enable...

    std::cout << getSummary() << std::endl;
  }
  inline std::string LeafCollection::getSummary() const{
    std::stringstream ss;
    ss << "tree{ " << _treePtr_->GetName() << " }";
    ss << std::endl << "leavesExpr" << GenericToolbox::parseVectorAsString(_leafExpressionList_);
    ss << std::endl << "branchesBuf" << GenericToolbox::iterableToString(_branchBufferList_, [](const BranchBuffer& b){ return b.getSummary(); });
    ss << std::endl << "leafForm" << GenericToolbox::iterableToString(_leafFormList_, [](const LeafForm& l){ return l.getSummary(); } );
    return ss.str();
  }

  inline void LeafCollection::parseExpressions() {

    // avoid moving memory around
    _leafFormList_.reserve(_leafExpressionList_.size());

    // loop over the expressions. _leafExpressionList_ size might change within the loop
    for( size_t iExp = 0 ; iExp < _leafExpressionList_.size() ; iExp++ ){
      std::cout << GET_VAR_NAME_VALUE(iExp) << std::endl;

      _leafFormList_.emplace_back();
      _leafFormList_.back().setPrimaryExprStr( _leafExpressionList_[iExp] );

      std::vector<std::string> argBuffer;
      auto strippedLeafExpr = GenericToolbox::stripBracket(_leafExpressionList_[iExp], '[', ']', false, &argBuffer);

      if( strippedLeafExpr.empty() ){
        throw std::runtime_error("Bad leaf form expression: " + _leafExpressionList_[iExp]);
      }

      // first, check if the remaining expr is a leaf
      auto* leafPtr = _treePtr_->GetLeaf(strippedLeafExpr.c_str());

      std::cout << GET_VAR_NAME_VALUE(leafPtr) << std::endl;

      if( leafPtr != nullptr ){
        std::cout << GET_VAR_NAME_VALUE(_leafExpressionList_[iExp]) << std::endl;
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
            auto idx = this->addLeafExpression( arg ); // will be processed later
            _leafFormList_.back().addIndex( -1 );
            _leafFormList_.back().addNestedLeafFormPtr( (LeafForm*) idx ); // tweaking types...
          }
        }
      }
      else{
        std::cout << "FORMULA?" << std::endl;
        // use a complete TTreeFormula to eval the obj
        _leafFormList_.back().setTreeFormulaPtr( std::make_shared<TTreeFormula>(
            _leafExpressionList_[iExp].c_str(),
            _leafExpressionList_[iExp].c_str(),
            _treePtr_
            ) );
      }
      std::cout << GET_VAR_NAME_VALUE(_leafExpressionList_[iExp]) << std::endl;

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
    std::cout << "look for branch: " << leaf_->GetFullName() << std::endl;
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
