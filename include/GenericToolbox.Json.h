//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_H
#define GENERICTOOLBOX_JSONUTILS_H


#include "GenericToolbox.Vector.h"
#include "GenericToolbox.String.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Os.h"
#include "GenericToolbox.Utils.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <vector>


// Declaration
namespace GenericToolbox {
  namespace Json {

    template<typename J> inline auto readConfigJsonStr(const std::string& configJsonStr_) -> J;
    template<typename J> inline auto readConfigFile(const std::string& configFilePath_) -> J;
    template<typename J> inline auto getForwardedConfig(const J& config_);
    template<typename J> inline auto getForwardedConfig(const J& config_, const std::string& keyPath_);
    template<typename J> inline void forwardConfig(J& config_, const std::string& className_ = "");
    template<typename J> inline void unfoldConfig(J& config_);
    template<typename J> inline std::string toReadableString(const J& config_);

    template<typename J> inline std::vector<std::string> ls(const J& jsonConfig_);
    template<typename J> inline bool doKeyExist(const J& jsonConfig_, const std::string& keyPath_);
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyPath_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyPath_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline void fillValue(const J& jsonConfig_, const std::string& keyPath_, T& varToFill_);
    template<typename T, typename J> inline void fillValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, T& varToFill_);
    template<typename T, typename J> inline void fillEnum(const J& jsonConfig_, const std::string& keyPath_, T& enumToFill_);
    template<typename T, typename J> inline void fillEnum(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, T& enumToFill_);
    template<typename J, typename T> inline auto fetchMatchingEntry(const J& jsonConfig_, const std::string& keyPath_, const T& keyValue_) -> J;
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::string& keyPath_, const F& action_);
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, const F& action_);

    // template specialization when a string literal is passed:
    template<typename J> inline void fillValue(const J& jsonConfig_, const std::string& keyPath_, Range& varToFill_);
    template<std::size_t N, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyPath_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyPath_, std::string(defaultValue_)); }
    template<std::size_t N, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyPathList_, std::string(defaultValue_)); }
    template<std::size_t N> inline auto fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyPath_, const char (&keyValue_)[N]) -> nlohmann::json{ return fetchMatchingEntry(jsonConfig_, keyPath_, std::string(keyValue_)); }

    // GUNDAM/ROOT specific
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_);
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_);

    // defaults
    inline nlohmann::json readConfigJsonStr(const std::string& configJsonStr_) { return readConfigJsonStr<nlohmann::json>(configJsonStr_); }
    inline nlohmann::json readConfigFile(const std::string& configJsonStr_) { return readConfigFile<nlohmann::json>(configJsonStr_); }

  }
}


// Implementation
namespace GenericToolbox {
  namespace Json {

    template<typename J> inline auto readConfigJsonStr(const std::string& configJsonStr_) -> J{
      std::stringstream ss;
      ss << configJsonStr_;

      J output;
      ss >> output;

      return output;
    }
    template<typename J> inline auto readConfigFile(const std::string& configFilePath_) -> J{
      if( not GenericToolbox::isFile(configFilePath_) ){
        std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
        throw std::runtime_error("file not found.");
      }

      std::fstream fs;
      fs.open(configFilePath_, std::ios::in);

      if( not fs.is_open() ) {
        std::cout << "\"" << configFilePath_ << "\": could not read file." << std::endl;
        throw std::runtime_error("file not readable.");
      }

      J output;
      fs >> output;
      return output;
    }
    template<typename J> inline auto getForwardedConfig(const J& config_) -> J {
      J out = config_;
      while( out.is_string() ){
        out = GenericToolbox::Json::readConfigFile<J>(out.template get<std::string>());
      }
      return out;
    }
    template<typename J> inline auto getForwardedConfig(const J& config_, const std::string& keyPath_) -> J{
      return GenericToolbox::Json::getForwardedConfig<J>(GenericToolbox::Json::fetchValue<J>(config_, keyPath_));
    }
    template<typename J> inline void forwardConfig(J& config_, const std::string& className_){
      while( config_.is_string() ){
        std::cout << "Forwarding " << (className_.empty()? "": className_ + " ") << "config: \"" << config_.template get<std::string>() << "\"" << std::endl;
        auto name = config_.template get<std::string>();
        std::string expand = GenericToolbox::expandEnvironmentVariables(name);
        config_ = GenericToolbox::Json::readConfigFile<J>(expand);
      }
    }
    template<typename J> inline void unfoldConfig(J& config_){
      for( auto& entry : config_ ){
        if( entry.is_string() and (
            GenericToolbox::endsWith(entry.template get<std::string>(), ".yaml", true)
            or GenericToolbox::endsWith(entry.template get<std::string>(), ".json", true)
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
    template<typename J> inline std::string toReadableString(const J& config_){
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

    template<typename J> inline bool doKeyExist(const J& jsonConfig_, const std::string& keyPath_){
      try{
        fetchValue<J>(jsonConfig_, keyPath_);
        // if successfully fetched -> it exists
        return true;
      }
      catch(...){
        // thrown an error -> does not exist
        return false;
      }
    }
    template<typename J> inline std::vector<std::string> ls(const J& jsonConfig_){
      std::vector<std::string> out{};
      out.reserve( jsonConfig_.size() );
      for (auto& entry : jsonConfig_.items()){ out.emplace_back( entry.key() ); }
      return out;
    }

    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_){
      std::string out;

      if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyPath_) ){
        std::cout << "Could not find key \"" << keyPath_ << "\" in " << jsonConfig_ << std::endl;
        throw std::runtime_error("Could not find key");
      }

      try{ return GenericToolbox::Json::fetchValue<std::string>(jsonConfig_, keyPath_); }
      catch (...){
        // it's a vector of strings
      }

      std::vector<std::string> conditionsList;
      auto jsonList( GenericToolbox::Json::fetchValue<J>(jsonConfig_, keyPath_) );

      if( jsonList.size() == 1 and not jsonList[0].is_string() and jsonList[0].is_array() ){
        // hot fix for broken json versions
        jsonList = jsonList[0];
      }

      for( auto& condEntry : jsonList ){
        if( condEntry.is_string() ){
          conditionsList.emplace_back( condEntry.template get<std::string>() );
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
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_){
      if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyPath_) ) return defaultFormula_;
      else return buildFormula(jsonConfig_, keyPath_, joinStr_);
    }

    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyPath_) -> T{
      // always treats as a key path
      std::vector<std::string> keyPathElements{GenericToolbox::splitString(keyPath_, "/")};
      J walkConfig(jsonConfig_);
      for( auto& keyName : keyPathElements ){
        auto entry = walkConfig.find(keyName);
        if( entry == walkConfig.end() ){
          throw std::runtime_error(
              "Could not find json entry: " + keyPath_ + ":\n"
              + GenericToolbox::Json::toReadableString(jsonConfig_)
          );
        }
        walkConfig = entry->template get<J>();
      }
      return walkConfig.template get<T>();
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_) -> T{
      for( auto& keyPath : keyPathList_){
        try{ return GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyPath); }
        catch(...){} // try the next ones
      }
      throw std::runtime_error("Could not find any json entry: " + GenericToolbox::toString(keyPathList_) + ":\n" + GenericToolbox::Json::toReadableString(jsonConfig_));
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyPath_, const T& defaultValue_) -> T{
      try{ return GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyPath_); }
      catch(...){ return defaultValue_; }
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, const T& defaultValue_) -> T{
      try{ return GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyPathList_); }
      catch(...){}
      return defaultValue_;
    }
    template<typename T, typename J> inline void fillValue(const J& jsonConfig_, const std::string& keyPath_, T& varToFill_){
      varToFill_ = GenericToolbox::Json::fetchValue(jsonConfig_, keyPath_, varToFill_);
    }
    template<typename T, typename J> inline void fillValue(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, T& varToFill_){
      varToFill_ = GenericToolbox::Json::fetchValue(jsonConfig_, keyPathList_, varToFill_);
    }
    template<typename T, typename J> inline void fillEnum(const J& jsonConfig_, const std::string& keyPath_, T& enumToFill_){
      enumToFill_ = enumToFill_.toEnum( GenericToolbox::Json::fetchValue(jsonConfig_, "pcaMethod", enumToFill_.toString()), true );
    }
    template<typename T, typename J> inline void fillEnum(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, T& enumToFill_){
      for( auto& keyPath : keyPathList_ ){
        if( not doKeyExist(jsonConfig_, keyPath)){ continue; }
        fillEnum(jsonConfig_, keyPathList_, enumToFill_);
        break;
      }
    }
    template<typename J, typename T> inline auto fetchMatchingEntry(const J& jsonConfig_, const std::string& keyPath_, const T& keyValue_) -> J{
      if( jsonConfig_.empty() or not jsonConfig_.is_array() ){ return {}; }
      for( const auto& jsonEntry : jsonConfig_.template get<std::vector<J>>() ){
        try{ if( GenericToolbox::Json::fetchValue<T>(jsonEntry, keyPath_) == keyValue_ ){ return jsonEntry; } }
        catch (...){} // next
      }
      return {}; // .empty()
    }
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::string& keyPath_, const F& action_){
      if( not doKeyExist(jsonConfig_, keyPath_) ){ return false; }
      action_();
      return true;
    }
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::vector<std::string>& keyPathList_, const F& action_){
      for( auto& keyPath : keyPathList_ ){
        if( doKeyExist(jsonConfig_, keyPath) ){
          action_( keyPath );
          return true;
        }
      }
      return false;
    }
    template<typename J> inline void fillValue(const J& jsonConfig_, const std::string& keyPath_, Range& varToFill_){
      std::vector<double> buffer;
      fetchValue(jsonConfig_, keyPath_, buffer);
      if( buffer.empty() ){ return; }
      if( buffer.size() != 2 ){ throw std::runtime_error(keyPath_ + " has " + std::to_string(buffer.size()) + " values (2 expected)."); }
      varToFill_.min = buffer[0];
      varToFill_.max = buffer[1];
    }

  }
}

#endif //GENERICTOOLBOX_JSONUTILS_H
