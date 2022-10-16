//
// Created by Adrien BLANCHET on 22/05/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_ROOT_TREEENTRYBUFFER_H
#define GUNDAM_GENERICTOOLBOX_ROOT_TREEENTRYBUFFER_H

#include "GenericToolbox.Root.LeafHolder.h"

#include "TTree.h"

#include "vector"
#include "string"

namespace GenericToolbox{
  class TreeEntryBuffer {

  public:
    inline TreeEntryBuffer();
    inline virtual ~TreeEntryBuffer();

    inline void setLeafNameList(const std::vector<std::string> &leafNameList_);
    inline void setIsDummyLeaf(const std::string& leafName_, bool isDummy_);
    inline void hook(TTree* tree_);

    inline const std::vector<GenericToolbox::LeafHolder> &getLeafContentList() const;

    inline int fetchLeafIndex(const std::string& leafName_) const;
    inline const GenericToolbox::LeafHolder& getLeafContent(const std::string& leafName_) const;
    inline std::string getSummary() const;

  private:
    std::vector<std::string> _leafNameList_;
    std::vector<bool> _dummyLeafStateLeaf_;
    std::vector<GenericToolbox::LeafHolder> _leafContentList_;

  };
}

#include "implementation/GenericToolbox.Root.TreeEventBuffer.impl.h"

#endif //GUNDAM_GENERICTOOLBOX_ROOT_TREEENTRYBUFFER_H
