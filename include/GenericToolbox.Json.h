//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_H
#define GENERICTOOLBOX_JSONUTILS_H


#include "nlohmann/json.hpp"

#include <string>
#include <vector>
#include <iostream>


namespace GenericToolbox {
  namespace Json {

    template<typename J> inline auto readConfigJsonStr(const std::string& configJsonStr_) -> J;
    template<typename J> inline auto readConfigFile(const std::string& configFilePath_) -> J;
    template<typename J> inline auto getForwardedConfig(const J& config_);
    template<typename J> inline auto getForwardedConfig(const J& config_, const std::string& keyName_);
    template<typename J> inline void forwardConfig(J& config_, const std::string& className_ = "");
    template<typename J> inline void unfoldConfig(J& config_);
    template<typename J> inline std::string toReadableString(const J& config_);

    template<typename J> inline std::vector<std::string> ls(const J& jsonConfig_);
    template<typename J> inline bool doKeyExist(const J& jsonConfig_, const std::string& keyName_);
    template<typename J> inline auto fetchSubEntry(const J& jsonConfig_, const std::vector<std::string>& keyPath_) -> J;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyNames_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyName_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline auto fetchValuePath(const J& jsonConfig_, const std::string& keyNamePath_) -> T;
    template<typename J, typename T> inline auto fetchMatchingEntry(const J& jsonConfig_, const std::string& keyName_, const T& keyValue_) -> J;
    template<typename J, typename F> inline void deprecatedAction(const J& jsonConfig_, const std::string& keyName_, const F& action_);

    // template specialization when a string literal is passed:
    template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_)); }
    template<std::size_t N> inline auto fetchValue(const nlohmann::json& jsonConfig_, const std::vector<std::string>& keyName_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_)); }
    template<std::size_t N> inline auto fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&keyValue_)[N]) -> nlohmann::json{ return fetchMatchingEntry(jsonConfig_, keyName_, std::string(keyValue_)); }

    // GUNDAM/ROOT specific
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_);
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_, const std::string& defaultFormula_);

    // defaults
    inline nlohmann::json readConfigJsonStr(const std::string& configJsonStr_) { return readConfigJsonStr<nlohmann::json>(configJsonStr_); }
    inline nlohmann::json readConfigFile(const std::string& configJsonStr_) { return readConfigFile<nlohmann::json>(configJsonStr_); }

  }
};


#include "implementation/GenericToolbox.Json.impl.h"

#endif //GENERICTOOLBOX_JSONUTILS_H
