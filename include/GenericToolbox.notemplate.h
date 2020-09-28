//
// Created by Adrien BLANCHET on 02/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_NOTEMPLATE_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_NOTEMPLATE_H

#include <vector>
#include <string>
#include <iostream>

namespace GenericToolBox{

  // template <typename T> void printVector(const std::vector<T>& vector_);
  template <typename T> inline void printVector(const std::vector<std::string>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }
  template <typename T> inline void printVector(const std::vector<int>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }
  template <typename T> inline void printVector(const std::vector<double>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }
  template <typename T> inline void printVector(const std::vector<float>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }
  template <typename T> inline void printVector(const std::vector<char>& vector_){
    std::cout << "{ ";
    bool isFirst = true;
    for(const auto& element: vector_){
      if(not isFirst) std::cout << ", ";
      else isFirst = false;
      std::cout << element;
    }
    std::cout << " }" << std::endl;
  }


}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_NOTEMPLATE_H
