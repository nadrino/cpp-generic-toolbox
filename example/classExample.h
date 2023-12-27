//
// Created by Nadrino on 08/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
#define CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H

#include <iostream>
#include <GenericToolbox.h>

#define MAKE_ENUM \
  ENUM_NAME(MyEnum) \
  ENUM_TYPE(unsigned int) \
  ENUM_ENTRY(Case0, 0) \
  ENUM_ENTRY(Case1)    \
  ENUM_ENTRY(Case2)    \
  ENUM_ENTRY(Case33, 33)    \
  ENUM_ENTRY(Case34)    \
  ENUM_ENTRY(Case35)    \
  ENUM_OVERFLOW(0)
#include "GenericToolbox.MakeEnum.h"
#undef MAKE_ENUM

class ClassExample{

public:
  ClassExample(){ std::cout << "The name of this class is: " << __CLASS_NAME__ << std::endl; }
  static void MyGreatMethod(){ std::cout << "The name of this method is: " << __METHOD_NAME__ << std::endl; }

};

#endif //CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
