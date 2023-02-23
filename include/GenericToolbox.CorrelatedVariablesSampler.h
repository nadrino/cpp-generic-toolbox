//
// Created by Adrien Blanchet on 23/02/2023.
//

#ifndef GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_H
#define GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_H

#include "GenericToolbox.InitBaseClass.h"

#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TDecompChol.h"
#include "TRandom3.h"

#include "vector"

namespace GenericToolbox {

  class CorrelatedVariablesSampler : InitBaseClass {

  public:
    CorrelatedVariablesSampler() = default;
    ~CorrelatedVariablesSampler() override = default;

    inline void setCovarianceMatrixPtr(TMatrixDSym *covarianceMatrixPtr_);

    inline void initializeImpl() override;

    inline void throwCorrelatedVariables(TVectorD& output_);

  private:
    // inputs:
    TMatrixDSym * _covarianceMatrixPtr_{nullptr};

    // optionals
    TRandom* _prng_{gRandom}; // using TRandom3 by default

    // internals
    std::shared_ptr<TDecompChol> _choleskyDecomposer_{nullptr};
    std::shared_ptr<TMatrixD> _sqrtMatrix_{nullptr};
    std::shared_ptr<TVectorD> _throwBuffer_{nullptr};

  };

}

#include "implementation/GenericToolbox.CorrelatedVariablesSampler.Impl.h"


#endif //GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_H
