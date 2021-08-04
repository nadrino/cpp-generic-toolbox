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

#include "GenericToolbox.AnyType.h"


namespace GenericToolbox{

  class LeafHolder{

  public:
    inline LeafHolder();
//    inline LeafHolder(const LeafHolder& other_);
    inline virtual ~LeafHolder();

    inline void reset();

    inline void hookToTree(TTree* tree_, const std::string& branchName_);
    template<typename T> inline void defineVariable(T variable_, size_t arraySize_ = 1);
    template<typename T> inline T& getVariable(size_t arrayIndex_ = 0);

    template<typename T> inline const T& getVariable(size_t arrayIndex_ = 0) const;
    inline double getVariableAsDouble(size_t arrayIndex_ = 0) const;

    inline const std::string &getLeafTypeName() const;

    // Stream operator
    inline friend std::ostream& operator <<( std::ostream& o, const LeafHolder& v );

  private:
    std::string _leafTypeName_;
    std::vector<AnyType> _leafDataList_;

  };

}

#include "implementation/GenericToolbox.Root.LeafHolder.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_LEAFHOLDER_H
