//
// Created by Nadrino on 07/09/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H

#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TTreeFormula.h"
#include "TTreeFormulaManager.h"

#include <sstream>
#include <string>
#include <vector>

namespace GenericToolbox{

  class TObjNotifier : public TObject {

  public:
    inline TObjNotifier() = default;
    inline virtual ~TObjNotifier() = default;

    inline void setOnNotifyFct(const std::function<void()>& onNotifyFct_){ _onNotifyFct_ = onNotifyFct_; }

    inline Bool_t Notify() override { if( _onNotifyFct_ ){ _onNotifyFct_(); return true; } return false; }

  private:
    std::function<void()> _onNotifyFct_;

  };


  class BranchBuffer{

  public:
    inline BranchBuffer() = default;
    inline virtual ~BranchBuffer() = default;

    // setters
    inline void setBranchPtr(TBranch* branchPtr_){ _branchPtr_ = branchPtr_; _branchName_ = _branchPtr_->GetFullName(); }

    // getters
    [[nodiscard]] inline const std::string& getBranchName() const { return _branchName_; }
    [[nodiscard]] inline const TBranch* getBranchPtr() const { return _branchPtr_; }
    [[nodiscard]] inline const std::vector<unsigned char>& getByteBuffer() const { return _byteBuffer_; }

    inline void buildBuffer();
    inline void hookBuffer();

    [[nodiscard]] inline std::string getSummary() const;

  private:
    TBranch* _branchPtr_{nullptr};
    std::string _branchName_{}; // used for ptr update
    std::vector<unsigned char> _byteBuffer_{};

  };

  class LeafForm{

  public:
    inline LeafForm() = default;
    inline virtual ~LeafForm() = default;

    inline void setIndex(int index_){ _index_ = index_; }
    inline void setNestedFormPtr(LeafForm* nestedLeafFormPtr_){ _nestedLeafFormPtr_ = nestedLeafFormPtr_; }
    inline void setPrimaryExprStr(const std::string &primaryExprStr) { _primaryExprStr_ = primaryExprStr; }
    inline void setPrimaryLeafPtr(TLeaf* primaryLeafPtr_) { _primaryLeafPtr_ = primaryLeafPtr_; _primaryLeafFullName_ = _primaryLeafPtr_->GetFullName(); }
    inline void setTreeFormulaPtr(const std::shared_ptr<TTreeFormula>& treeFormulaPtr) { _treeFormulaPtr_ = treeFormulaPtr; }

    inline const LeafForm* getNestedFormPtr() const { return _nestedLeafFormPtr_; }

    inline TLeaf *getPrimaryLeafPtr() const{ return _primaryLeafPtr_; }
    [[nodiscard]] inline const std::string &getPrimaryExprStr() const { return _primaryExprStr_; }
    [[nodiscard]] inline const std::string &getPrimaryLeafFullName() const { return _primaryLeafFullName_; }
    [[nodiscard]] inline const std::shared_ptr<TTreeFormula> &getTreeFormulaPtr() const { return _treeFormulaPtr_; }

    inline void initialize();

    [[nodiscard]] inline void* getDataAddress() const;
    [[nodiscard]] inline size_t getDataSize() const;
    [[nodiscard]] inline std::string getLeafTypeName() const;

    inline double evalAsDouble() const;
    inline void fillLocalBuffer() const;
    inline void dropToAny(GenericToolbox::AnyType& any_) const;
    [[nodiscard]] inline std::string getSummary() const;

    inline void cacheDataSize();
    inline void cacheDataAddr();

  protected:
    template<typename T> inline const T& eval() const; // Use ONLY if the type is known

  private:
    TLeaf* _primaryLeafPtr_{nullptr};     // volatile ptr for TChains
    std::string _primaryExprStr_{};       // keep track of the expression it handles
    std::string _primaryLeafFullName_{};  // used to keep track of the leaf ptr

    size_t _index_{0};
    LeafForm* _nestedLeafFormPtr_{nullptr};
    std::shared_ptr<TTreeFormula> _treeFormulaPtr_{nullptr};

    // buffers
    size_t _dataSize_{0};
    void* _dataAddress_{nullptr};
    mutable double _localBuffer_{}; // for TTreeFormula
    mutable std::shared_ptr<GenericToolbox::AnyType> _anyTypeContainer_{nullptr};

  };

  class LeafCollection{

  public:
    inline LeafCollection() = default;
    inline virtual ~LeafCollection();

    // setters
    inline void setTreePtr(TTree* treePtr_){ _treePtr_ = treePtr_; }
    inline int addLeafExpression(const std::string& leafExpStr_);

    inline void initialize();

    // getters
    [[nodiscard]] inline const std::vector<LeafForm>& getLeafFormList() const{ return _leafFormList_; }

    // core
    inline void doNotify();
    [[nodiscard]] inline std::string getSummary() const;
    [[nodiscard]] inline int getLeafExpIndex(const std::string& leafExpression_) const;
    [[nodiscard]] inline const LeafForm* getLeafFormPtr(const std::string& leafExpression_) const;

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
    TObjNotifier _objNotifier_{};

  };

}

#include "implementation/GenericToolbox.Root.LeafCollection.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFCOLLECTION_H
