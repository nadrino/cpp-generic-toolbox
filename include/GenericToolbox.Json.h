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
  namespace Json {

    // we want to preserve the ordering of the key for the
    // override feature to fully work
    // -> nlohmann::json are faster but move the keys
    typedef nlohmann::ordered_json JsonType;

    // Objects configured by a JSON file can inherit from this
    typedef ConfigClass<JsonType> ConfigBaseClass;

    // IO
    inline JsonType readConfigJsonStr(const std::string& configJsonStr_);
    inline JsonType readConfigFile(const std::string& configFilePath_);
    inline JsonType getForwardedConfig(const JsonType& config_);
    inline void forwardConfig(JsonType& config_, const std::string& className_ = "");
    inline void unfoldConfig(JsonType& config_);
    inline std::string toReadableString(const JsonType& config_);

    // reading
    inline JsonType cd(const JsonType& json_, const std::string& keyPath_);
    inline std::vector<std::string> ls(const JsonType& jsonConfig_);
    inline bool doKeyExist(const JsonType& jsonConfig_, const std::string& keyPath_);
    inline std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_);
    inline std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_);
    inline void fillFormula(const JsonType& jsonConfig_, std::string& formulaToFill_, const std::string& keyPath_, const std::string& joinStr_);

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
    inline std::string applyOverrides(JsonType& outConfig_, const JsonType& overrideConfig_);
    inline void clearEntry(JsonType& jsonConfig_, const std::string& path_);

    namespace Internal{
      // not meant to be used by devs
      // those are tricks to avoid template overrides
      template<typename T> T getImpl(const JsonType& json_, T*);
      inline double getImpl(const JsonType& json_, double*);
      inline Range getImpl(const JsonType& json_, Range*);
      template<typename T> std::vector<T> getImpl(const JsonType& json_, std::vector<T>*);
    }

  }
}


// Implementation
namespace GenericToolbox {
  namespace Json {

    // io
    inline JsonType readConfigJsonStr(const std::string& configJsonStr_){
      std::stringstream ss;
      ss << configJsonStr_;

      JsonType output;
      ss >> output;

      return output;
    }
    inline JsonType readConfigFile(const std::string& configFilePath_){
      if( not isFile(configFilePath_) ){
        std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
        throw std::runtime_error("file not found.");
      }

      std::fstream fs;
      fs.open(configFilePath_, std::ios::in);

      if( not fs.is_open() ) {
        std::cout << "\"" << configFilePath_ << "\": could not read file." << std::endl;
        throw std::runtime_error("file not readable.");
      }

      JsonType output;
      fs >> output;
      return output;
    }
    inline JsonType getForwardedConfig(const JsonType& config_) {
      JsonType out = config_;
      while( out.is_string() ){
        out = readConfigFile(out.template get<std::string>());
      }
      return out;
    }
    inline void forwardConfig(JsonType& config_, const std::string& className_){
      while( config_.is_string() ){
        std::cout << "Forwarding " << (className_.empty()? "": className_ + " ") << "config: \"" << config_.template get<std::string>() << "\"" << std::endl;
        auto name = config_.template get<std::string>();
        std::string expand = expandEnvironmentVariables(name);
        config_ = readConfigFile(expand);
      }
    }
    inline void unfoldConfig(JsonType& config_){
      for( auto& entry : config_ ){
        if( entry.is_string() and (
            endsWith(entry.template get<std::string>(), ".json", true)
        ) ){
          forwardConfig(entry);
          unfoldConfig(config_); // remake the loop on the unfolder config
          break; // don't touch anymore
        }

        if( entry.is_structured() ){
          unfoldConfig(entry);
        }
      }
    }
    inline std::string toReadableString(const JsonType& config_){
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
            ss << std::endl << repeatString("  ", indentLevel) << c;
            indentLevel++;
            ss << std::endl << repeatString("  ", indentLevel);
          }
          else if( c == '}' or c == ']' ){
            indentLevel--;
            ss << std::endl << repeatString("  ", indentLevel) << c;
          }
          else if( c == ':' ){
            ss << c << " ";
          }
          else if( c == ',' ){
            ss << c << std::endl << repeatString("  ", indentLevel);
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

    // read
    inline bool doKeyExist(const JsonType& jsonConfig_, const std::string& keyPath_){
      try{
        fetchValue<JsonType>(jsonConfig_, keyPath_);
        // if successfully fetched -> it exists
        return true;
      }
      catch(...){
        // thrown an error -> does not exist
        return false;
      }
    }
    inline JsonType cd(const JsonType& json_, const std::string& keyPath_){
      JsonType out{};
      fillValue(json_, out, keyPath_);
      return out;
    }
    inline std::vector<std::string> ls(const JsonType& jsonConfig_){
      std::vector<std::string> out{};
      out.reserve( jsonConfig_.size() );
      for (auto& entry : jsonConfig_.items()){ out.emplace_back( entry.key() ); }
      return out;
    }
    inline std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_){
      std::string out;

      if( not doKeyExist(jsonConfig_, keyPath_) ){
        std::cout << "Could not find key \"" << keyPath_ << "\" in " << jsonConfig_ << std::endl;
        throw std::runtime_error("Could not find key");
      }

      try{ return fetchValue<std::string>(jsonConfig_, keyPath_); }
      catch (...){
        // it's a vector of strings
      }

      std::vector<std::string> conditionsList;
      auto jsonList( fetchValue<JsonType>(jsonConfig_, keyPath_) );

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
      out += joinVectorString(conditionsList, ") " + joinStr_ + " (");
      out += ")";

      return out;
    }
    inline std::string buildFormula(const JsonType& jsonConfig_, const std::string& keyPath_, const std::string& joinStr_, const std::string& defaultFormula_){
      if( not doKeyExist(jsonConfig_, keyPath_) ) return defaultFormula_;
      else return buildFormula(jsonConfig_, keyPath_, joinStr_);
    }
    inline void fillFormula(const JsonType& jsonConfig_, std::string& formulaToFill_, const std::string& keyPath_, const std::string& joinStr_){
      formulaToFill_ = buildFormula(jsonConfig_, keyPath_, joinStr_, formulaToFill_);
    }

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
      for(const auto& jsonEntry : jsonConfig_.template get<std::vector<JsonType>>() ){
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
    inline std::string applyOverrides(JsonType& outConfig_, const JsonType& overrideConfig_){

      // dev options
      bool debug{false};
      bool allowAddMissingKey{true};

      // summary
      std::stringstream ss;

      // specific keys like "name" might help reference the lists
      std::vector<std::string> listOfIdentifiers{{"name"}, {"__INDEX__"}};


      std::vector<std::string> jsonPath{};
      std::function<void(JsonType&, const JsonType&)> overrideRecursive =
          [&](JsonType& outEntry_, const JsonType& overrideEntry_){
            GTLogDebugIf(debug) << GET_VAR_NAME_VALUE(joinPath(jsonPath)) << std::endl;

            if( overrideEntry_.is_array() ){
              // entry is list
              if( not outEntry_.is_array() ){
                std::cerr << joinPath( jsonPath ) << " is not an array: " << std::endl << outEntry_ << std::endl << std::endl << overrideEntry_;
                throw std::runtime_error("not outEntry_.is_array()");
              }

              // is it empty? -> erase
              if( overrideEntry_.empty() ){
                ss << "Overriding list: " << joinPath(jsonPath) << std::endl;
                outEntry_ = overrideEntry_;
                return;
              }

              // is it an array of primitive type? like std::vector<std::string>?
              bool isStructured{false};
              for( auto& outListEntry : outEntry_.items() ){ if( outListEntry.value().is_structured() ){ isStructured = true; break; } }
              if( not isStructured ){
                ss << "Overriding list: " << joinPath(jsonPath) << std::endl;
                outEntry_ = overrideEntry_;
                return;
              }

              // loop over to find the right entry
              for( auto& overrideListEntry: overrideEntry_.items() ){

                // fetch identifier if available using override
                std::string identifier{};
                for( auto& identifierCandidate : listOfIdentifiers ){
                  if( doKeyExist( overrideListEntry.value(), identifierCandidate ) ){
                    identifier = identifierCandidate;
                  }
                }

                if( not identifier.empty() ){
                  if     ( identifier == "__INDEX__" ){
                    // make sure we don't write override identifiers
                    JsonType override(overrideListEntry.value());
                    override.erase("__INDEX__");

                    if     ( overrideListEntry.value()[identifier].is_string() and overrideListEntry.value()[identifier].get<std::string>() == "*" ){
                      // applying on every entry
                      int index{0};
                      for( auto& outSubEntry : outEntry_ ){
                        jsonPath.emplace_back(std::to_string(index++));
                        overrideRecursive(outSubEntry, override);
                        jsonPath.pop_back();
                      }
                    }
                    else if( overrideListEntry.value()[identifier].get<int>() == -1 ){
                      // add entry
                      if( allowAddMissingKey ){
                        ss << "Added: " << joinPath(jsonPath, outEntry_.size());
                        if( overrideListEntry.value().is_primitive() ){ ss << " -> " << override; }
                        ss << std::endl;
                        outEntry_.emplace_back(override);
                      }
                    }
                    else if( overrideListEntry.value()[identifier].get<size_t>() < outEntry_.size() ){
                      jsonPath.emplace_back( overrideListEntry.key() );
                      overrideRecursive( outEntry_[overrideListEntry.value()[identifier].get<size_t>()], override );
                      jsonPath.pop_back();
                    }
                    else{
                      std::cerr << "Invalid __INDEX__: " << overrideListEntry.value()[identifier].get<int>() << std::endl;
                      throw std::runtime_error("Invalid index.");
                    }
                  }
                  else{
                    // "name" identifier
                    bool matchingFound{false};

                    std::vector<std::string> availableNameList{};

                    // looping over all list options
                    for( auto& outListEntry : outEntry_ ){
                      // does this entry has a "name"
                      if( not doKeyExist( outListEntry, "name" ) ){ continue; }

                      availableNameList.emplace_back(outListEntry["name"]);

                      if( outListEntry["name"] == overrideListEntry.value()["name"]
                          or isMatching(outListEntry["name"], overrideListEntry.value()["name"]) ) {
                        matchingFound = true;

                        GTLogDebugIf(debug) << "Found matching " << outListEntry["name"] << " with " << overrideListEntry.value()["name"] << std::endl;

                        // make sure we don't write override identifiers
                        JsonType override(overrideListEntry.value());
                        if(not override.is_array()){ override.erase("name"); }

                        jsonPath.emplace_back(joinAsString("",overrideListEntry.key(),"(name:",outListEntry["name"],")"));
                        overrideRecursive(outListEntry, override);
                        jsonPath.pop_back();
                      }
                    }

                    if( not matchingFound and allowAddMissingKey ){
                      GTLogDebugIf(debug) << "Did NOT find any matching name entry matching " << overrideListEntry.value()["name"]
                      // << " among: " << GenericToolbox::toString(availableNameList)
                      << std::endl;

                      ss << "Added: "
                      << joinPath(jsonPath, outEntry_.size())
                      << "(name:"
                      << overrideListEntry.value()["name"] << ")" << std::endl;
                      outEntry_.emplace_back(overrideListEntry.value());
                    }
                  }
                }
                else{
                  ss << "No identifier found for list def in " << joinPath(jsonPath) << std::endl;
                  continue;
                }
              }
            }
            else{
              GTLogDebugIf(debug) << "Not array: " << overrideEntry_.empty() << std::endl;

              if( overrideEntry_.empty() ){
                ss << "Removing entry: " << joinPath(jsonPath) << std::endl;
                outEntry_ = overrideEntry_;
                return;
              }

              // entry is dictionary
              for( auto& overrideEntry : overrideEntry_.items() ){
                GTLogDebugIf(debug) << GET_VAR_NAME_VALUE(overrideEntry.key()) << std::endl;

                // addition mode:
                if( not doKeyExist(outEntry_, overrideEntry.key()) ){
                  if( overrideEntry.key() != "__INDEX__" ){
                    if( allowAddMissingKey ){
                      ss << "Added: " << joinPath(jsonPath, overrideEntry.key());
                      if( overrideEntry.value().is_primitive() ){ ss << " -> " << overrideEntry.value(); }
                      ss << std::endl;
                      outEntry_[overrideEntry.key()] = overrideEntry.value();
                    }
                    else{
                      std::cerr << "Could not edit missing key \""
                      << joinPath(jsonPath, overrideEntry.key()) << "\" ("
                      << GET_VAR_NAME_VALUE(allowAddMissingKey) << ")" << std::endl;
                      throw std::runtime_error("Could not edit missing key.");
                    }
                  }
                  else{
                    GTLogDebugIf(debug) << "skipping __INDEX__ entry" << std::endl;
                  }
                  continue;
                }

                // override
                auto& outSubEntry = outEntry_[overrideEntry.key()];

                if( overrideEntry.value().is_structured() ){
                  GTLogDebugIf(debug) << "Is structured... going recursive..." << std::endl;
                  // recursive candidate
                  jsonPath.emplace_back(overrideEntry.key());
                  overrideRecursive(outSubEntry, overrideEntry.value());
                  jsonPath.pop_back();
                }
                else{
                  // override
                  if( not isIn(overrideEntry.key(), listOfIdentifiers) ){
                    ss << "Override: " << joinPath(jsonPath, overrideEntry.key()) << ": "
                              << outSubEntry << " -> " << overrideEntry.value() << std::endl;
                  }
                  outSubEntry = overrideEntry.value();
                }
              }
            }

          };

      // recursive
      overrideRecursive(outConfig_, overrideConfig_);

      return ss.str();
    }
    inline void clearEntry(JsonType& jsonConfig_, const std::string& path_){

      auto pathEntries{ splitString(path_, "/") };
      auto* configEntry{&jsonConfig_};

      for( auto& pathEntry : pathEntries ){
        if( doKeyExist( *configEntry, pathEntry ) ){
          // next
          configEntry = &( configEntry->find(pathEntry).value() );
        }
        else{
          // no need to override. The key does not exist in the config
          return;
        }
      }

      // clearing up
      configEntry->clear();
    }

    namespace Internal{
      template<typename T> T getImpl(const JsonType& json_, T*){
        return json_.template get<T>();
      }
      inline double getImpl(const JsonType& json_, double*){
        // better handling of nan
        if( json_.is_string() ){
          if( json_.template get<std::string>() == "nan" ){ return std::nan("nan"); }
          if( json_.template get<std::string>() == "null" ){ return std::nan("null"); }
          // otherwise it is a string that should be castable as a double
        }
        if( json_.is_null() ){ return std::nan("null"); } // macOS
        return json_.template get<double>();
      }
      inline Range getImpl(const JsonType& json_, Range*){
        if( not json_.is_array() ){ throw std::runtime_error("provided json is not an array."); }
        if( json_.size() != 2 ){ throw std::runtime_error("Range has " + std::to_string(json_.size()) + " values (2 expected)."); }
        Range out{};
        out.min = get<double>(json_[0]);
        out.max = get<double>(json_[1]);
        if( out.min > out.max ){ std::swap(out.min, out.max); }
        return out;
      }
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

#endif //GENERICTOOLBOX_JSONUTILS_H
