#pragma once

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_VECTOR_STATIC bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_) {
    return std::find(vector_.cbegin(), vector_.cend(), element_) != vector_.cend();
  }

CPP_GENERIC_TOOLBOX_VECTOR_INLINE CPP_GENERIC_TOOLBOX_VECTOR_STATIC int findElementIndex(const char* element_, const std::vector<std::string>& vector_ ) {
    auto it = std::find(vector_.cbegin(), vector_.cend(), element_);
    if( it == vector_.cend() ){ return -1; }
    return static_cast<int>( std::distance(vector_.cbegin(), it) );
  }

CPP_GENERIC_TOOLBOX_VECTOR_STATIC void addIfNotInVector(const char* element_, std::vector<std::string> &vector_) {
    if( not GenericToolbox::doesElementIsInVector(element_, vector_) ){
      vector_.emplace_back(element_);
    }
  }
}
