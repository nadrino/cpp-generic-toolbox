//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_H
#define GENERICTOOLBOX_JSONUTILS_H


#include "nlohmann/json.hpp"

#include <string>
#include <vector>
#include <iostream>


namespace GenericToolbox::Json {

  inline nlohmann::json readConfigJsonStr(const std::string& configJsonStr_);
  inline nlohmann::json readConfigFile(const std::string& configFilePath_);
  inline nlohmann::json getForwardedConfig(const nlohmann::json& config_);
  inline nlohmann::json getForwardedConfig(const nlohmann::json& config_, const std::string& keyName_);
  inline void forwardConfig(nlohmann::json& config_, const std::string& className_ = "");
  inline void unfoldConfig(nlohmann::json& config_);
  inline std::string toReadableString(const nlohmann::json& config_);

  inline std::vector<std::string> ls(const nlohmann::json& jsonConfig_);
  inline bool doKeyExist(const nlohmann::json& jsonConfig_, const std::string& keyName_);
  inline nlohmann::json fetchSubEntry(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyPath_);
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_) -> T;
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyNames_) -> T;
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_, const T& defaultValue_) -> T;
  template<class T> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyName_, const T& defaultValue_) -> T;
  template<class T> inline auto fetchValuePath(const nlohmann::json& jsonConfig_, const std::string& keyNamePath_) -> T;
  template<class T> inline nlohmann::json fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const T& keyValue_);
  template<typename F> inline void deprecatedAction(const nlohmann::json& jsonConfig_, const std::string& keyName_, const F& action_);

  // template specialization when a string literal is passed:
  template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string;
  template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyName_, const char (&defaultValue_)[N]) -> std::string;
  template<std::size_t N> inline nlohmann::json fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&keyValue_)[N]);


};


#include "implementation/GenericToolbox.Json.impl.h"

#endif //GENERICTOOLBOX_JSONUTILS_H
