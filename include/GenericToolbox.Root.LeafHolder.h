//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H

// STD Headers
#include <vector>
#include <string>
#include <any>

#include "TTree.h"
#include "TLeaf.h"

#include "GenericToolbox.AnyType.h"


namespace GenericToolbox{

  class LeafHolder{

  public:
    inline LeafHolder();
    inline virtual ~LeafHolder();

    inline void reset();

    inline void hookToTree(TTree* tree_, const std::string& branchName_);
    template<typename T> inline void defineVariable(T* variable_, size_t arraySize_ = 1);
    template<typename T> inline void defineVariable(T variable_, size_t arraySize_ = 1);
    template<typename T> inline T& getVariable(size_t arrayIndex_ = 0);

    inline const std::vector<AnyType> &getLeafDataList() const;

    template<typename T> inline const T& getVariable(size_t arrayIndex_ = 0) const;
    inline double getVariableAsDouble(size_t arrayIndex_ = 0) const;
    inline size_t getArraySize() const;

    inline void* getVariableAddress(size_t arrayIndex_ = 0);
    inline const AnyType& getLeafDataAddress(size_t arrayIndex_ = 0) const;
    inline size_t getVariableSize(size_t arrayIndex_ = 0) const;
    inline char findOriginalVariableType() const;

    inline void clonePointerLeaves();

    // Stream operator
    inline friend std::ostream& operator <<( std::ostream& o, const LeafHolder& v );

  private:
    std::vector<AnyType> _leafDataList_;

  };

}

#include "implementation/GenericToolbox.Root.LeafHolder.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
