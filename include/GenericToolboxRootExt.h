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
#include <TFile.h>

//#include "GenericToolbox.h"

namespace GenericToolbox{

  //! Conversion Tools
  inline TH1D* convertTVectorDtoTH1D(TVectorD *Y_values_, std::string histTitle_ = "", std::string Y_title_ = "", std::string X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH1D* convertTVectorDtoTH1D(std::vector<double> Y_values_, std::string histTitle_ = "", std::string Y_title_ = "", std::string X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertTMatrixDtoTH2D(TMatrixD *XY_values_, std::string graph_title_ = "", std::string Z_title_ = "", std::string Y_title_ = "Row #", std::string X_title_ = "Col #");
  inline TVectorD* convertStdVectorToTVectorD(std::vector<double>& vect_);
  inline TMatrixDSym* convertToSymmetricMatrix(TMatrixD* matrix_);
  inline TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);


  //! Files Tools
  inline bool doesTFileIsValid(std::string input_file_path_);
  inline bool doesTFileIsValid(TFile* input_tfile_, bool check_if_writable_ = false);
  inline std::vector<TFile*> getListOfOpenedTFiles();
  inline std::vector<TObject*> getListOfObjectFromTDirectory(TDirectory* directory_, std::string class_name_ = "");

  //! Matrix Tools
  inline std::map<std::string, TMatrixD*> invertMatrixSVD(TMatrixD *matrix_, std::string outputContent_="inverse_covariance_matrix:regularized_eigen_values");
  inline std::vector<double> getEigenValues(TMatrixD *matrix_);
//  inline TMatrixD* computeSqrt(TMatrixD* inputMatrix_);

  //! Histogram Tools
  inline std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_);
  inline std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_);
  inline TH1D* getTH1DlogBinning(std::string name_, std::string title_, int n_bins_, double X_min_, double X_max_);
  inline TH2D* getTH2DlogBinning(std::string name_, std::string title_, int nb_X_bins_, double X_min_, double X_max_,
                          int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_= "XY");


  //! Canvas Tools
  inline void setDefaultPalette();
  inline void setBlueRedPalette();
  inline void setOrangePalette();
  inline void fixTH2display(TH2 *histogram_);


  //! ROOT Internals
  inline void muteRoot();
inline void unmuteRoot();


}

#include "GenericToolboxRootExt.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_H
