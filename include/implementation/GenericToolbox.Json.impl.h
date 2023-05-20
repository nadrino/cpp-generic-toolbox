//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_IMPL_H
#define GENERICTOOLBOX_JSONUTILS_IMPL_H

#include "../GenericToolbox.Json.h"
#include "../GenericToolbox.h"

#include "nlohmann/json.hpp"

#include "string"
#include "iostream"

namespace GenericToolbox::Json {

  inline nlohmann::json readConfigJsonStr(const std::string& configJsonStr_){
    std::stringstream ss;
    ss << configJsonStr_;

    nlohmann::json output;
    ss >> output;

    return output;
  }
  inline nlohmann::json readConfigFile(const std::string& configFilePath_){
    if( not GenericToolbox::doesPathIsFile(configFilePath_) ){
      std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
      throw std::runtime_error("file not found.");
    }

    std::fstream fs;
    fs.open(configFilePath_, std::ios::in);

    if( not fs.is_open() ) {
      std::cout << "\"" << configFilePath_ << "\": could not read file." << std::endl;
      throw std::runtime_error("file not readable.");
    }

    nlohmann::json output;
    fs >> output;
    return output;
  }
  inline nlohmann::json getForwardedConfig(const nlohmann::json& config_){
    nlohmann::json out = config_;
    while( out.is_string() ){
      out = GenericToolbox::Json::readConfigFile(out.get<std::string>());
    }
    return out;
  }
  inline nlohmann::json getForwardedConfig(const nlohmann::json& config_, const std::string& keyName_){
    return GenericToolbox::Json::getForwardedConfig(GenericToolbox::Json::fetchValue<nlohmann::json>(config_, keyName_));
  }
  inline void forwardConfig(nlohmann::json& config_, const std::string& className_){
    while( config_.is_string() ){
      std::cout << "Forwarding " << (className_.empty()? "": className_ + " ") << "config: \"" << config_.get<std::string>() << "\"" << std::endl;
      auto name = config_.get<std::string>();
      std::string expand = GenericToolbox::expandEnvironmentVariables(name);
      config_ = GenericToolbox::Json::readConfigFile(expand);
    }
  }
  inline void unfoldConfig(nlohmann::json& config_){
    for( auto& entry : config_ ){
      if( entry.is_string() and (
          GenericToolbox::doesStringEndsWithSubstring(entry.get<std::string>(), ".yaml", true)
          or GenericToolbox::doesStringEndsWithSubstring(entry.get<std::string>(), ".json", true)
      ) ){
        GenericToolbox::Json::forwardConfig(entry);
        GenericToolbox::Json::unfoldConfig(config_); // remake the loop on the unfolder config
        break; // don't touch anymore
      }

      if( entry.is_structured() ){
        GenericToolbox::Json::unfoldConfig(entry);
      }
    }
  }
  inline std::string toReadableString(const nlohmann::json& config_){
    std::stringstream ss;
    ss << config_ << std::endl;

    std::string originalJson = ss.str();
    ss.str(""); ss.clear();
    int indentLevel{0};
    bool inQuote{false};
    for( char c : originalJson ){

      if( c == '"'){ inQuote = not inQuote; }

      if( not inQuote ){
        if( c == '{' or c == '[' ){
          ss << std::endl << GenericToolbox::repeatString("  ", indentLevel) << c;
          indentLevel++;
          ss << std::endl << GenericToolbox::repeatString("  ", indentLevel);
        }
        else if( c == '}' or c == ']' ){
          indentLevel--;
          ss << std::endl << GenericToolbox::repeatString("  ", indentLevel) << c;
        }
        else if( c == ':' ){
          ss << c << " ";
        }
        else if( c == ',' ){
          ss << c << std::endl << GenericToolbox::repeatString("  ", indentLevel);
        }
        else if( c == '\n' ){
          if( ss.str().back() != '\n' ) ss << c;
        }
        else{
          ss << c;
        }
      }
      else{
        ss << c;
      }

    }
    return ss.str();
  }

  inline bool doKeyExist(const nlohmann::json& jsonConfig_, const std::string& keyName_){
    return jsonConfig_.find(keyName_) != jsonConfig_.end();
  }
  inline std::vector<std::string> ls(const nlohmann::json& jsonConfig_){
    std::vector<std::string> out{};
    for( const auto& entry : jsonConfig_.get<nlohmann::json::object_t>() ){ out.emplace_back(entry.first); }
    return out;
  }
  inline nlohmann::json fetchSubEntry(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyPath_){
    nlohmann::json output = jsonConfig_;
    for( const auto& key : keyPath_ ){
      output = GenericToolbox::Json::fetchValue<nlohmann::json>(output, key);
    }
    return output;
  }

  inline std::string buildFormula(const nlohmann::json& jsonConfig_, const std::string& keyName_, const std::string& joinStr_){
    std::string out;

    if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ){
      std::cout << "Could not find key \"" << keyName_ << "\" in " << jsonConfig_ << std::endl;
      throw std::runtime_error("Could not find key");
    }

    try{ return GenericToolbox::Json::fetchValue<std::string>(jsonConfig_, keyName_); }
    catch (...){
      // it's a vector of strings
    }

    std::vector<std::string> conditionsList;
    for( auto& condEntry : GenericToolbox::Json::fetchValue<std::vector<nlohmann::json>>(jsonConfig_, keyName_) ){
      if( condEntry.is_string() ){
        conditionsList.emplace_back(condEntry.get<std::string>());
      }
      else{
        std::cout << "Could not recognise condition entry: " << condEntry << std::endl;
        throw std::runtime_error("Could not recognise condition entry");
      }
    }

    out += "(";
    out += GenericToolbox::joinVectorString(conditionsList, ") " + joinStr_ + " (");
    out += ")";

    return out;
  }
  inline std::string buildFormula(const nlohmann::json& jsonConfig_, const std::string& keyName_, const std::string& joinStr_, const std::string& defaultFormula_){
    if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ) return defaultFormula_;
    else return buildFormula(jsonConfig_, keyName_, joinStr_);
  }

  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_) -> T{
    auto jsonEntry = jsonConfig_.find(keyName_);
    if( jsonEntry == jsonConfig_.end() ){
      throw std::runtime_error("Could not find json entry: " + keyName_ + ":\n" + jsonConfig_.dump());
    }
    return jsonEntry->template get<T>();
  }
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyNames_) -> T{
    for( auto& keyName : keyNames_){
      if( GenericToolbox::Json::doKeyExist(jsonConfig_, keyName) ){
        return GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName);
      }
    }
    throw std::runtime_error("Could not find any json entry: " + GenericToolbox::parseVectorAsString(keyNames_) + ":\n" + jsonConfig_.dump());
  }
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_, const T& defaultValue_) -> T{
    try{
      T value = GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName_);
      return value; // if nothing has gone wrong
    }
    catch (...){
      return defaultValue_;
    }
  }
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyName_, const T& defaultValue_) -> T{
    for( auto& keyName : keyName_ ){
      try{
        T value = GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName);
        return value; // if nothing has gone wrong
      }
      catch (...){
      }
    }
    return defaultValue_;
  }
  template<class T> inline auto fetchValuePath(const nlohmann::json& jsonConfig_, const std::string& keyNamePath_) -> T{
    auto keyPathElements = GenericToolbox::splitString(keyNamePath_, "/", true);
    nlohmann::json elm{jsonConfig_};
    for( auto& keyPathElement : keyPathElements ){
      elm = GenericToolbox::Json::fetchValue<nlohmann::json>(elm, keyPathElement);
    }
    return elm.template get<T>();
  }
  template<class T> inline nlohmann::json fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const T& keyValue_){

    if( not jsonConfig_.is_array() ){
      std::cout << "key: " << keyName_ << std::endl;
      std::cout << "value: " << keyValue_ << std::endl;
      std::cout << "dump: " << jsonConfig_.dump() << std::endl;
      throw std::runtime_error("GenericToolbox::Json::fetchMatchingEntry: jsonConfig_ is not an array.");
    }

    for( const auto& jsonEntry : jsonConfig_ ){
      try{
        if(GenericToolbox::Json::fetchValue<T>(jsonEntry, keyName_) == keyValue_ ){
          return jsonEntry;
        }
      }
      catch (...){
        // key not present, skip
      }

    }
    return {}; // .empty()
  }
  template<typename F> inline void deprecatedAction(const nlohmann::json& jsonConfig_, const std::string& keyName_, const F& action_){
    if( GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ){
      std::cout << "DEPRECATED option: \"" << keyName_ << "\". Running defined action..." << std::endl;
      action_();
    }
  }

  // specialization
  template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string{
    return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_));
  }
  template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyName_, const char (&defaultValue_)[N]) -> std::string{
    return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_));
  }
  template<std::size_t N> inline nlohmann::json fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&keyValue_)[N]){
    return fetchMatchingEntry(jsonConfig_, keyName_, std::string(keyValue_));
  }

  // gundam specific
  inline std::string buildFormula(const nlohmann::json& jsonConfig_, const std::string& keyName_, const std::string& joinStr_);
  inline std::string buildFormula(const nlohmann::json& jsonConfig_, const std::string& keyName_, const std::string& joinStr_, const std::string& defaultFormula_);



};


#endif //GENERICTOOLBOX_JSONUTILS_IMPL_H
