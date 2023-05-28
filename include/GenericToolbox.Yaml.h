//
// Created by Nadrino on 17/06/2021.
//

#ifndef GENERICTOOLBOX_YAMLUTILS_H
#define GENERICTOOLBOX_YAMLUTILS_H

// dependency
#include "yaml-cpp/yaml.h"

#include <string>
#include <iostream>


namespace GenericToolbox { namespace Yaml {

  inline YAML::Node readConfigFile(const std::string& configFilePath_);
  inline std::string toJsonString(const YAML::Node& yamlConfig_);

  template<class T> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_) -> T;
  template<class T> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_, const T& defaultValue_) -> T;
  template<class T> inline YAML::Node fetchMatchingEntry(const YAML::Node& yamlConfig_, const std::string& keyName_, const T& keyValue_);

  // template specialization when a string literal is passed:
  template<std::size_t N> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string;
  template<std::size_t N> inline YAML::Node fetchMatchingEntry(const YAML::Node& yamlConfig_, const std::string& keyName_, const char (&keyValue_)[N]);

}};

#include "implementation/GenericToolbox.Yaml.impl.h"

#endif //GENERICTOOLBOX_YAMLUTILS_H
