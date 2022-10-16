//
// Created by Adrien BLANCHET on 22/05/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_ROOT_TREEEVENTBUFFER_IMPL_H
#define GUNDAM_GENERICTOOLBOX_ROOT_TREEEVENTBUFFER_IMPL_H

#include "sstream"

namespace GenericToolbox{

  inline TreeEntryBuffer::TreeEntryBuffer() = default;
  inline TreeEntryBuffer::~TreeEntryBuffer() = default;

  inline void TreeEntryBuffer::hook(TTree* tree_){
    _leafContentList_.clear();
    _leafContentList_.resize(_leafNameList_.size());
    for( size_t iLeaf{0} ; iLeaf < _leafNameList_.size() ; iLeaf++ ){
      if( _dummyLeafStateLeaf_[iLeaf] ){ _leafContentList_[iLeaf].hookDummyDouble( _leafNameList_[iLeaf] ); }
      else{ _leafContentList_[iLeaf].hook(tree_, _leafNameList_[iLeaf]); }
    }
  }

  inline void TreeEntryBuffer::setLeafNameList(const std::vector<std::string> &leafNameList_) {
    _leafNameList_ = leafNameList_;
    _dummyLeafStateLeaf_.resize(_leafNameList_.size(), false);
  }
  inline void TreeEntryBuffer::setIsDummyLeaf(const std::string& leafName_, bool isDummy_){
    int index = GenericToolbox::findElementIndex( leafName_, _leafNameList_ );
    if( index == -1 ) {
      throw std::runtime_error(
          "TreeEntryBuffer::setIsDummyLeaf: \"" + leafName_ + "\" not found in leaf list: "
          + GenericToolbox::parseVectorAsString(_leafNameList_)
          );
    }
    _dummyLeafStateLeaf_[index] = isDummy_;
  }

  inline const std::vector<GenericToolbox::LeafHolder> &TreeEntryBuffer::getLeafContentList() const {
    return _leafContentList_;
  }

  inline int TreeEntryBuffer::fetchLeafIndex(const std::string& leafName_) const{
    return GenericToolbox::findElementIndex(leafName_, _leafNameList_);
  }
  inline const GenericToolbox::LeafHolder& TreeEntryBuffer::getLeafContent(const std::string& leafName_) const{
    int i = this->fetchLeafIndex(leafName_);
    if(i==-1){ throw std::runtime_error(leafName_ + ": not found."); }
    return _leafContentList_[i];
  }
  inline std::string TreeEntryBuffer::getSummary() const{
    std::stringstream ss;
    ss << "TreeEventBuffer:" << std::endl << "_leafContentList_ = {";
    if( not _leafNameList_.empty() ){
      for( size_t iLeaf = 0 ; iLeaf < _leafNameList_.size() ; iLeaf++ ){
        ss<< std::endl << "  " << _leafContentList_[iLeaf];
      }
      ss << std::endl << "}";
    }
    else{
      ss << "}";
    }
    return ss.str();
  }

}

#endif //GUNDAM_GENERICTOOLBOX_ROOT_TREEEVENTBUFFER_IMPL_H
