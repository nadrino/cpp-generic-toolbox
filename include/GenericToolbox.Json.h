//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_H
#define GENERICTOOLBOX_JSONUTILS_H

#include "GenericToolbox.Build.h"


#include "GenericToolbox.Vector.h"
#include "GenericToolbox.String.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Os.h"
#include "GenericToolbox.Utils.h"
#include "GenericToolbox.Log.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <vector>

// extension for GenericToolbox::MakeEnum
#define MAKE_ENUM_JSON_INTERFACE(EnumName_) \
  friend void from_json(const GenericToolbox::Json::JsonType& j, EnumName_& obj){ \
    obj = EnumName_::toEnum( j.get<std::string>() ); \
    if( obj == EnumName_::overflowValue ){ \
      GTLogError << j.get<std::string>() << " isn't a valid enum. Available: " << EnumName_::generateEnumStrList() << std::endl; \
      std::exit(1); \
    } \
  }


// Declaration
namespace GenericToolbox {

  namespace Json{
    // we want to preserve the ordering of the key for the
    // override feature to fully work
    // -> nlohmann::json are faster but move the keys
    typedef nlohmann::ordered_json JsonType;

    // Objects configured by a JSON file can inherit from this
    typedef ConfigClass<JsonType> ConfigBaseClass;
  }

  // generic calls, overload
  CPP_GENERIC_TOOLBOX_JSON_INLINE std::string toString(const Json::JsonType& config_);

  namespace Json {

    // IO
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType readConfigJsonStr(const std::string& configJsonStr_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType readConfigFile(const std::string& configFilePath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType getForwardedConfig(const JsonType& config_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void forwardConfig(JsonType& config_, const std::string& className_ = "");
    CPP_GENERIC_TOOLBOX_JSON_INLINE void unfoldConfig(JsonType& config_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string toReadableString(const JsonType& config_, bool shallow_=false);

    // reading
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType cd(const JsonType& json_, const std::string& keyPath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::vector<std::string> ls(const JsonType& jsonConfig_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE bool doKeyExist(const JsonType& jsonConfig_, const std::string& keyPath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void fillFormula(const JsonType& jsonConfig_, std::string& formulaToFill_, const std::string& keyPath_, const std::string& joinStr_);

    // reading -- templates
    template<typename T> auto get(const JsonType& json_) -> T;
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::string& keyPath_) -> T;
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_) -> T;
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::string& keyPath_, const T& defaultValue_) -> T;
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_, const T& defaultValue_) -> T;
    template<typename T> void fillValue(const JsonType& jsonConfig_, T& varToFill_, const std::string& keyPath_);
    template<typename T> void fillValue(const JsonType& jsonConfig_, T&varToFill_, const std::vector<std::string>& keyPathList_);
    template<typename T> void fillEnum(const JsonType& jsonConfig_, T &enumToFill_, const std::string &keyPath_ );
    template<typename T> void fillEnum(const JsonType& jsonConfig_, T &enumToFill_, const std::vector<std::string> &keyPathList_ );
    template<typename T> JsonType fetchMatchingEntry(const JsonType& jsonConfig_, const std::string& keyPath_, const T& keyValue_);
    template<typename F> bool deprecatedAction(const JsonType& jsonConfig_, const std::string& keyPath_, const F& action_);
    template<typename F> bool deprecatedAction(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_, const F& action_);

    // template overrides (not specialization)
    template<std::size_t N> auto fetchValue(const JsonType& jsonConfig_, const std::string& keyPath_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyPath_, std::string(defaultValue_)); }
    template<std::size_t N> auto fetchValue(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyPathList_, std::string(defaultValue_)); }
    template<std::size_t N> JsonType fetchMatchingEntry(const JsonType& jsonConfig_, const std::string& keyPath_, const char (&keyValue_)[N]){ return fetchMatchingEntry(jsonConfig_, keyPath_, std::string(keyValue_)); }

    // writing
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string applyOverrides(JsonType& outConfig_, const JsonType& overrideConfig_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void clearEntry(JsonType& jsonConfig_, const std::string& path_);

    namespace Internal{
      // not meant to be used by devs
      // those are tricks to avoid template overrides
      template<typename T> T getImpl(const JsonType& json_, T*);
      CPP_GENERIC_TOOLBOX_JSON_INLINE double getImpl(const JsonType& json_, double*);
      CPP_GENERIC_TOOLBOX_JSON_INLINE std::string getImpl(const JsonType& json_, std::string*);
      CPP_GENERIC_TOOLBOX_JSON_INLINE Range getImpl(const JsonType& json_, Range*);
      template<typename T> std::vector<T> getImpl(const JsonType& json_, std::vector<T>*);
    }

  }
}


// Implementation
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_JSON_INLINE std::string toString(const Json::JsonType& config_);

  namespace Json {

    // io
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType readConfigJsonStr(const std::string& configJsonStr_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType readConfigFile(const std::string& configFilePath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType getForwardedConfig(const JsonType& config_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void forwardConfig(JsonType& config_, const std::string& className_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void unfoldConfig(JsonType& config_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string toReadableString(const JsonType& config_, bool shallow_);

    // read
    CPP_GENERIC_TOOLBOX_JSON_INLINE bool doKeyExist(const JsonType& jsonConfig_, const std::string& keyPath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE JsonType cd(const JsonType& json_, const std::string& keyPath_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::vector<std::string> ls(const JsonType& jsonConfig_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void fillFormula(const JsonType& jsonConfig_, std::string& formulaToFill_, const std::string& keyPath_, const std::string& joinStr_);

    // templates
    template<typename T> auto get(const JsonType& json_) -> T {
      // the dummy argument leverage the ambiguity.
      // This is a trick to bypass template specializations, and do proper overrides
      try{
        return Internal::getImpl(json_, static_cast<T*>(nullptr));
      }
      catch(const std::exception& e_){
        // don't print anything with std::cout as this exception could be caught from outside

        std::stringstream ss;
        ss << "json_= " << toReadableString(json_);
        ss << "template type: " << typeid(T).name() << std::endl;
        ss << "error: " << e_.what() << std::endl;
        throw std::runtime_error(GTLogBase + "Could not read json value: " + ss.str());
      }
    }
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::string& keyPath_) -> T{
      // always treats as a key path
      std::vector<std::string> keyPathElements{splitString(keyPath_, "/")};
      JsonType walkConfig(jsonConfig_);
      for( auto& keyName : keyPathElements ){
        auto entry = walkConfig.find(keyName);
        if( entry == walkConfig.end() ){
          throw std::runtime_error(
              "Could not find json entry: " + keyPath_ + ":\n"
              + toReadableString(jsonConfig_)
          );
        }
        walkConfig = get<JsonType>(*entry);
      }
      return get<T>(walkConfig);
    }
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_) -> T{
      for( auto& keyPath : keyPathList_){
        try{ return fetchValue<T>(jsonConfig_, keyPath); }
        catch(...){} // try the next ones
      }
      throw std::runtime_error("Could not find any json entry: " + toString(keyPathList_) + ":\n" + toReadableString(jsonConfig_));
    }
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::string& keyPath_, const T& defaultValue_) -> T{
      try{ return fetchValue<T>(jsonConfig_, keyPath_); }
      catch(...){ return defaultValue_; }
    }
    template<typename T> auto fetchValue(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_, const T& defaultValue_) -> T{
      try{ return fetchValue<T>(jsonConfig_, keyPathList_); }
      catch(...){}
      return defaultValue_;
    }
    template<typename T> void fillValue(const JsonType& jsonConfig_, T &varToFill_, const std::string &keyPath_ ){
      varToFill_ = fetchValue(jsonConfig_, keyPath_, varToFill_);
    }
    template<typename T> void fillValue(const JsonType& jsonConfig_, T &varToFill_, const std::vector<std::string> &keyPathList_ ){
      varToFill_ = fetchValue(jsonConfig_, keyPathList_, varToFill_);
    }
    template<typename T> void fillEnum(const JsonType& jsonConfig_, T& enumToFill_, const std::string& keyPath_ ){
      enumToFill_ = enumToFill_.toEnum( fetchValue(jsonConfig_, keyPath_, enumToFill_.toString()), true );
    }
    template<typename T> void fillEnum(const JsonType& jsonConfig_, T& enumToFill_, const std::vector<std::string>& keyPathList_ ){
      for( auto& keyPath : keyPathList_ ){
        if( not doKeyExist(jsonConfig_, keyPath)){ continue; }
        fillEnum(jsonConfig_, enumToFill_, keyPathList_);
        break;
      }
    }
    template<typename T> JsonType fetchMatchingEntry(const JsonType& jsonConfig_, const std::string& keyPath_, const T& keyValue_){
      if( jsonConfig_.empty() or not jsonConfig_.is_array() ){ return {}; }
      for(const auto& jsonEntry : jsonConfig_.get<std::vector<JsonType>>() ){
        try{ if( fetchValue<T>(jsonEntry, keyPath_) == keyValue_ ){ return jsonEntry; } }
        catch (...){} // next
      }
      return {}; // .empty()
    }
    template<typename F> bool deprecatedAction(const JsonType& jsonConfig_, const std::string& keyPath_, const F& action_){
      if( not doKeyExist(jsonConfig_, keyPath_) ){ return false; }
      action_();
      return true;
    }
    template<typename F> bool deprecatedAction(const JsonType& jsonConfig_, const std::vector<std::string>& keyPathList_, const F& action_){
      for( auto& keyPath : keyPathList_ ){
        if( doKeyExist(jsonConfig_, keyPath) ){
          action_( keyPath );
          return true;
        }
      }
      return false;
    }

    // write
    CPP_GENERIC_TOOLBOX_JSON_INLINE std::string applyOverrides(JsonType& outConfig_, const JsonType& overrideConfig_);
    CPP_GENERIC_TOOLBOX_JSON_INLINE void clearEntry(JsonType& jsonConfig_, const std::string& path_);

    namespace Internal{
      template<typename T> T getImpl(const JsonType& json_, T*){
        return json_.get<T>();
      }
      CPP_GENERIC_TOOLBOX_JSON_INLINE double getImpl(const JsonType& json_, double*);
      CPP_GENERIC_TOOLBOX_JSON_INLINE std::string getImpl(const JsonType& json_, std::string*);
      CPP_GENERIC_TOOLBOX_JSON_INLINE Range getImpl(const JsonType& json_, Range*);
      template<typename T> std::vector<T> getImpl(const JsonType& json_, std::vector<T>*){
        std::vector<T> out; out.reserve(json_.size());
        for( auto& entry : json_ ){
          out.emplace_back( getImpl(entry, static_cast<T*>(nullptr) ) );
        }
        return out;
      }

    }

  }
}

#ifndef CPP_GENERIC_TOOLBOX_JSON_COMPILED
#include "implementation/GenericToolbox.Json.impl.h"
#endif

#endif //GENERICTOOLBOX_JSONUTILS_H
