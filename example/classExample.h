//
// Created by Adrien BLANCHET on 08/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
#define CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H

#include <iostream>
#include <GenericToolbox.h>

class ClassExample{

public:
  ClassExample(){ std::cout << "The name of this class is: " << __CLASS_NAME__ << std::endl; }
  void MyGreatMethod(){ std::cout << "The name of this method is: " << __METHOD_NAME__ << std::endl; }

};

#endif //CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
