//
// Created by Nadrino on 23/02/2023.
//

#ifndef GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_IMPL_H
#define GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_IMPL_H


#include "../GenericToolbox.CorrelatedVariablesSampler.h"

#include "GenericToolbox.Root.h"

#include <sstream>
#include <stdexcept>


namespace GenericToolbox {

  inline void CorrelatedVariablesSampler::setCovarianceMatrixPtr(TMatrixDSym *covarianceMatrixPtr_) {
    _covarianceMatrixPtr_ = covarianceMatrixPtr_;
  }

  inline void CorrelatedVariablesSampler::initializeImpl() {
    if( _covarianceMatrixPtr_ == nullptr ){
      throw std::runtime_error("Can't init while _covarianceMatrixPtr_ is not set");
    }


    // https://root.cern.ch/doc/master/classTDecompChol.html
    _choleskyDecomposer_ = std::make_shared<TDecompChol>(*_covarianceMatrixPtr_ );
    if( not _choleskyDecomposer_->Decompose() ){
      throw std::runtime_error("Can't decompose covariance matrix.");
    }

    // Decompose a symmetric, positive definite matrix: A = U^T * U
    _sqrtMatrix_ = std::make_shared<TMatrixD>(_choleskyDecomposer_->GetU());

    _sqrtMatrix_->T(); // Transpose it to get the left side one
  }

  inline void CorrelatedVariablesSampler::throwCorrelatedVariables(TVectorD& output_){
    // https://math.stackexchange.com/questions/446093/generate-correlated-normal-random-variables
    this->throwIfNotInitialized(__METHOD_NAME__);
    if(output_.GetNrows() != _covarianceMatrixPtr_->GetNrows()){
      std::stringstream ss;
      ss << __METHOD_NAME__ << ": ";
      ss << "Provided output TVector does not have the same dimension as the cov matrix";
      ss << " -> cov=" << _covarianceMatrixPtr_->GetNrows() << " != vector=" << output_.GetNrows();
      throw std::runtime_error( ss.str() );
    }

    for( int iVar = 0 ; iVar < output_.GetNrows() ; iVar++ ){
      output_[iVar] = _prng_->Gaus(0, 1);
    }

    output_ *= (*_sqrtMatrix_);
  }

}

#endif //GUNDAM_GENERICTOOLBOX_CORRELATEDVARIABLESSAMPLER_IMPL_H