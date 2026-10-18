#pragma once

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_MACRO_STATIC std::string getClassName(const std::string& PRETTY_FUNCTION_) {
    size_t colons = PRETTY_FUNCTION_.find("::");
    if (colons == std::string::npos)
      return "::";
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = colons - begin;

    return PRETTY_FUNCTION_.substr(begin, end);
  }

CPP_GENERIC_TOOLBOX_MACRO_STATIC std::string getMethodName(const std::string& PRETTY_FUNCTION_) {
    size_t colons = PRETTY_FUNCTION_.find("::");
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = PRETTY_FUNCTION_.rfind('(') - begin;

    return PRETTY_FUNCTION_.substr(begin, end) + "()";
  }
}
