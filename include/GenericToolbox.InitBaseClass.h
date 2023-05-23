//
// Created by Adrien BLANCHET on 25/10/2022.
//

#ifndef GUNDAM_GENERICTOOLBOX_INITBASECLASS_H
#define GUNDAM_GENERICTOOLBOX_INITBASECLASS_H

#include <string>


namespace GenericToolbox{

  class InitBaseClass{

  public:
    // Common structure
    inline InitBaseClass() = default;
    inline virtual ~InitBaseClass() = default;

    virtual inline void initialize();
    inline void unInitialize();

    [[nodiscard]] inline bool isInitialized() const;

    inline void throwIfInitialized(const std::string& functionName_ = {}) const;
    inline void throwIfNotInitialized(const std::string& functionName_ = {}) const;

  protected:
    // where the derivative classes will specify (although override is optional)
    inline virtual void initializeImpl(){};

  private:
    bool _isInitialized_{false};

  };

}


#include "implementation/GenericToolbox.InitBaseClass.Impl.h"

#endif //GUNDAM_GENERICTOOLBOX_INITBASECLASS_H
