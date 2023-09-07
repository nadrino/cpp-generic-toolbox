//
// Created by Nadrino on 07/09/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H

#include "GenericToolbox.Root.LeafHolder.h"

#include "TTree.h"
#include "TBranch.h"

#include <string>
#include <vector>

namespace GenericToolbox{

  class BranchBuffer{

  public:
    inline BranchBuffer() = default;
    inline virtual ~BranchBuffer() = default;

    // setters
    inline void setBranchPtr(TBranch* branchPtr_){ _branchPtr_ = branchPtr_; }

    // getters
    inline TBranch* getBranchPtr(){ return _branchPtr_; }

    inline void buildBuffer(){
      if( not _byteBuffer_.empty() ){ throw std::logic_error("buffer already set."); }
      if( _branchPtr_ == nullptr ){ throw std::runtime_error("branch not set."); }
      if( _branchPtr_->GetAddress() != nullptr ){ throw std::runtime_error(_branchPtr_->GetName() + std::string(": branch address already set.")); }

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
        throw std::runtime_error(std::string("empty buffer size for branch: ") + _branchPtr_->GetName());
      }

      _byteBuffer_.resize( bufferSize, 0 );
      if( _byteBuffer_.empty() ){ throw std::runtime_error("empty byte buffer"); }
    }
    inline void hookBuffer(){
      if( _byteBuffer_.empty() ){ throw std::runtime_error("empty byte buffer, can't " + __METHOD_NAME__); }
      std::cout << _branchPtr_->GetFullName() << " -> ptr = " << GenericToolbox::toHex(_byteBuffer_.data()) << " / " << &_byteBuffer_ << " / " << this << std::endl;
      _branchPtr_->SetAddress( (void*) &_byteBuffer_[0] );
    }

    [[nodiscard]] inline std::string getSummary() const {
      std::stringstream ss;
      if( _branchPtr_ != nullptr ){
        ss << _branchPtr_->GetName() << ": addr{0x" << GenericToolbox::toHex(_branchPtr_->GetAddress()) << "}, size{" << _byteBuffer_.size() << "}";
      }
      return ss.str();
    }

  private:
    TBranch* _branchPtr_{nullptr};
    std::vector<unsigned char> _byteBuffer_{};

  };

  class LeafForm{

  public:
    inline LeafForm() = default;
    inline virtual ~LeafForm() = default;

    inline void addIndex(int index_){ _arrayIndices_.emplace_back( index_ ); }
    inline void addNestedLeafFormPtr(LeafForm* nestedLeafForm_){ _nestedLeafFormPtrList_.emplace_back( nestedLeafForm_ ); }
    inline void setPrimaryExprStr(const std::string &primaryExprStr) { _primaryExprStr_ = primaryExprStr; }
    inline void setPrimaryLeafPtr(TLeaf* primaryLeafPtr_) { _primaryLeafPtr_ = primaryLeafPtr_; }
    inline void setTreeFormulaPtr(const std::shared_ptr<TTreeFormula> &treeFormulaPtr) { _treeFormulaPtr_ = treeFormulaPtr; }

    inline std::vector<LeafForm*>& getNestedLeafFormPtrList(){ return _nestedLeafFormPtrList_; }

    [[nodiscard]] inline const std::string &getPrimaryExprStr() const { return _primaryExprStr_; }

    [[nodiscard]] inline std::string getSummary() const{
      std::stringstream ss;
      if( _primaryLeafPtr_ != nullptr ){
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
        ss << _treeFormulaPtr_->GetExpFormula();
      }
      return ss.str();
    }

  private:
    std::string _primaryExprStr_{}; // keep track of the expression it handles
    TLeaf* _primaryLeafPtr_{nullptr}; //
    std::vector<int> _arrayIndices_{}; // index of the buffer or instance of the formula
    std::vector<LeafForm*> _nestedLeafFormPtrList_{}; // replacing indices
    std::shared_ptr<TTreeFormula> _treeFormulaPtr_{nullptr};

  };

  class LeafCollection{

  public:
    inline LeafCollection() = default;
    inline virtual ~LeafCollection() = default;

    inline void setTreePtr(TTree* treePtr_){ _treePtr_ = treePtr_; }
    inline size_t addLeafExpression(const std::string& leafExpStr_);

    inline void initialize();

    [[nodiscard]] inline std::string getSummary() const;

  protected:
    inline void parseExpressions();
    inline void setupBranchBuffer(TLeaf* leaf_);

  private:
    // user
    TTree* _treePtr_{nullptr};
    std::vector<std::string> _leafExpressionList_{};

    // internals
    std::vector<BranchBuffer> _branchBufferList_{};
    std::vector<LeafForm> _leafFormList_{}; // handle the evaluation of each expression using the shared leaf buffers


  };

}

#include "implementation/GenericToolbox.Root.LeafCollection.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H
