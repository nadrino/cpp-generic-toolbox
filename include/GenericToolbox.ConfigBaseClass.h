//
// Created by Nadrino on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H
#define GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H

#include "GenericToolbox.InitBaseClass.h"

namespace GenericToolbox{

  template<class ConfigType>
  class ConfigBaseClass : public InitBaseClass {

  public:
    // Common structure
    inline ConfigBaseClass() = default;
    inline ~ConfigBaseClass() override = default;

    inline virtual void setConfig(const ConfigType& config_);

    inline void readConfig();
    inline void readConfig(const ConfigType& config_);

    inline void initialize() override;

    [[nodiscard]] inline bool isConfigReadDone() const;
    inline const ConfigType &getConfig() const;

  protected:
    // where the derivative classes will specify (although override is optional)
    inline virtual void readConfigImpl(){};

    // Can be accessed by derivative classes
    ConfigType _config_{};

  private:
    bool _isConfigReadDone_{false};

  };


}


#include "implementation/GenericToolbox.ConfigBaseClass.Impl.h"

#endif //GUNDAM_GENERICTOOLBOX_CONFIGBASECLASS_H
