//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H

#include "GenericToolbox.AnyType.h"

#include "TTree.h"
#include "TLeaf.h"
#include "TTreeFormula.h"

#include <vector>
#include <string>

namespace GenericToolbox{

  class LeafHolder{

  public:
    inline LeafHolder() = default;
    inline virtual ~LeafHolder() = default;

    inline void hook(TTree *tree_, TLeaf* leaf_);
    inline void hook(TTree *tree_, const std::string& leafName_);
    inline void hookDummyDouble(const std::string& leafName_);

    // const
    [[nodiscard]] inline size_t getArraySize() const;
    [[nodiscard]] inline size_t getLeafTypeSize() const;
    [[nodiscard]] inline const std::string &getLeafTypeName() const;
    [[nodiscard]] inline const std::string &getLeafFullName() const;
    [[nodiscard]] inline const std::vector<unsigned char> &getByteBuffer() const;

    // non-const
    inline std::vector<unsigned char> &getByteBuffer();

    // core
    [[nodiscard]] inline std::string getSummary() const;
    inline void dropToAny(std::vector<AnyType>& anyV_) const;
    inline void dropToAny(AnyType& any_, size_t slot_) const;

    // template
    template<typename T> inline T& getVariable(size_t arrayIndex_ = 0);
    template<typename T> inline const T& getVariable(size_t arrayIndex_ = 0) const;

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
