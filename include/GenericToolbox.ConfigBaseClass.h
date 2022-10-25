//
// Created by Adrien BLANCHET on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H
#define GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H

namespace GenericToolbox{

  template<class ConfigType>
  class ConfigBaseClass {

  public:
    // Common structure
    inline ConfigBaseClass() = default;
    inline virtual ~ConfigBaseClass() = default;

    inline virtual void setConfig(const ConfigType& config_);

    inline void readConfig();
    inline void readConfig(const ConfigType& config_);

    inline void initialize();

    inline bool isConfigReadDone() const;
    inline bool isInitialized() const;
    inline const ConfigType &getConfig() const;

  protected:
    // where the derivative classes will specify (although override is optional)
    inline virtual void readConfigImpl(){};
    inline virtual void initializeImpl(){};

    // Can be accessed by derivative classes
    ConfigType _config_{};

  private:
    bool _isConfigReadDone_{false};
    bool _isInitialized_{false};

  };


}


#include "implementation/GenericToolbox.ConfigBaseClass.Impl.h"

#endif //GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H
