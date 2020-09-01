//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_H

// STD Headers
#include <map>
#include <string>

// ROOT Headers
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TH1D.h>
#include <TH2D.h>

namespace GenericToolbox{

  TMatrixDSym* convert_to_symmetric_matrix(TMatrixD* matrix_);
  std::map<std::string, TMatrixD*> SVD_matrix_inversion(
    TMatrixD *matrix_,
    std::string output_content_="inverse_covariance_matrix:regularized_eigen_values"
  );

  TH1D* get_TH1D_from_TVectorD(std::string graph_title_, TVectorD *Y_values_, std::string Y_title_ = "", std::string X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  TH2D* get_TH2D_from_TMatrixD(TMatrixD *XY_values_, std::string graph_title_ = "", std::string Z_title_ = "",std::string Y_title_ = "Row #", std::string X_title_ = "Col #");

  TVectorD* get_TVectorD_from_vector(std::vector<double>& vect_);



}

#include "GenericToolboxRootExt.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_H
