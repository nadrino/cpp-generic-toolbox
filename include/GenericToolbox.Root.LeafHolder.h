//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H

#include "GenericToolbox.AnyType.h"

#include "TTree.h"
#include "TLeaf.h"

#include <vector>
#include <string>

namespace GenericToolbox{

  class LeafHolder{

  public:
    inline LeafHolder();
    inline virtual ~LeafHolder();

    inline void reset();
    inline void hook(TTree *tree_, const std::string& leafName_);

    // const
    inline size_t getLeafTypeSize() const;
    inline const std::string &getLeafTypeName() const;
    inline const std::string &getLeafFullName() const;
    inline const std::vector<unsigned char> &getByteBuffer() const;

    // non-const
    inline std::vector<unsigned char> &getByteBuffer();
    template<typename T> inline T& getVariable(size_t arrayIndex_ = 0);

    // core
    template<typename T> inline const T& getVariable(size_t arrayIndex_ = 0) const;
    inline void copyToAny(std::vector<AnyType>& anyV_) const;
    inline void copyToAny(AnyType& any_, size_t slot_) const;
    inline size_t getArraySize() const;

    // Stream operator
    inline friend std::ostream& operator <<( std::ostream& o, const LeafHolder& v );

  private:
    size_t _leafTypeSize_{0};
    std::string _leafTypeName_{};
    std::string _leafFullName_{};
    std::vector<unsigned char> _byteBuffer_{};

  };

}

#include "implementation/GenericToolbox.Root.LeafHolder.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
