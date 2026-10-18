#pragma once

namespace GenericToolbox {
namespace Yaml {
CPP_GENERIC_TOOLBOX_YAML_INLINE YAML::Node readConfigFile(const std::string &configFilePath_) {
      if( not GenericToolbox::isFile(configFilePath_) ){
        std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
        throw std::runtime_error("file not found.");
      }
      return YAML::LoadFile(configFilePath_);
    }
}

namespace Yaml {
CPP_GENERIC_TOOLBOX_YAML_INLINE std::string toJsonString(const YAML::Node& yamlConfig_) {
      YAML::Emitter emitter;
      emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << yamlConfig_;
      std::string out{emitter.c_str() + 1};

      // workaround for old version of YAML: !<!> was used to identify strings
      GenericToolbox::replaceSubstringInsideInputString(out, "!<!> ", " ");
      return out;
    }
}
}
