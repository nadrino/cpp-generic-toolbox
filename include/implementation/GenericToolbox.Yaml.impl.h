//
// Created by Nadrino on 17/06/2021.
//

#ifndef GENERICTOOLBOX_YAMLUTILS_IMPL_H
#define GENERICTOOLBOX_YAMLUTILS_IMPL_H


#include "../GenericToolbox.Yaml.h"
#include "../GenericToolbox.h"

#include <string>
#include <iostream>
#include "exception"

// https://github.com/jbeder/yaml-cpp/wiki/Tutorial

namespace GenericToolbox { namespace Yaml {

  inline YAML::Node readConfigFile(const std::string &configFilePath_) {
    if( not GenericToolbox::doesPathIsFile(configFilePath_) ){
      std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
      throw std::runtime_error("file not found.");
    }
    return YAML::LoadFile(configFilePath_);
  }
  inline std::string toJsonString(const YAML::Node& yamlConfig_){
    YAML::Emitter emitter;
    emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << yamlConfig_;
    return std::string(emitter.c_str() + 1);
  }

  template<class T> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_) -> T{

    if( keyName_.empty() ){
      throw std::runtime_error("Could not fetch in YAML node: keyName_.empty()");
    }
    else if( yamlConfig_.IsNull() ){
      throw std::runtime_error("Could not fetch in YAML node: IsNull()");
    }

    if( not yamlConfig_[keyName_] ){
      throw std::runtime_error(keyName_ + " does not exist");
    }

    return yamlConfig_[keyName_].template as<T>();
  }
  template<class T> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_, const T& defaultValue_) -> T{
    try{
      T value = fetchValue<T>(yamlConfig_, keyName_);
      return value; // if nothing has gone wrong
    }
    catch (...){
      return defaultValue_;
    }
  }
  template<class T> inline YAML::Node fetchMatchingEntry(const YAML::Node& yamlConfig_, const std::string& keyName_, const T& keyValue_){

    if( not yamlConfig_.IsSequence() ){
      throw std::runtime_error("Could not fetchMatchingEntry in YAML node: IsSequence() == false");
    }

    for( const auto& yamlEntry : yamlConfig_ ){
      if(yamlEntry[keyName_] and yamlEntry[keyName_].template as<T>() == keyValue_ ){
        return YAML::Node(yamlEntry);
      }
    }

    return YAML::Node(); // .empty()
  }

  template<std::size_t N> inline auto fetchValue(const YAML::Node& yamlConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string{
    return fetchValue(yamlConfig_, keyName_, std::string(defaultValue_));
  }
  template<std::size_t N> inline YAML::Node fetchMatchingEntry(const YAML::Node& yamlConfig_, const std::string& keyName_, const char (&keyValue_)[N]){
    return fetchMatchingEntry(yamlConfig_, keyName_, std::string(keyValue_));
  }

}};

#endif //GENERICTOOLBOX_YAMLUTILS_IMPL_H
