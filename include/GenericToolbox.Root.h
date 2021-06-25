//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_H

// STD Headers
#include <map>
#include <string>

// ROOT Headers
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TFormula.h>
#include "TTreeFormula.h"


namespace GenericToolbox{

  //! Conversion Tools
  inline TH1D* convertTVectorDtoTH1D(TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  inline TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertTMatrixDtoTH2D(TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TVectorD* convertStdVectorToTVectorD(const std::vector<double> &vect_);
  inline TMatrixDSym* convertToSymmetricMatrix(TMatrixD* matrix_);
  inline TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);

  //! Formula Tools
  inline TFormula* convertToFormula(TTreeFormula* treeFormula_);
  inline std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_);
  inline std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_);
  inline TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_);
  inline bool doesLoadedEntryPassCut(TTreeFormula* treeFormula_);

  //! Files Tools
  inline bool doesTFileIsValid(const std::string &input_file_path_);
  inline bool doesTFileIsValid(TFile* input_tfile_, bool check_if_writable_ = false);
  inline std::vector<TFile*> getListOfOpenedTFiles();
  inline std::vector<TObject*> getListOfObjectFromTDirectory(TDirectory* directory_, const std::string &class_name_ = "");
  inline TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_);
  inline TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_);

  //! Trees Tools
  inline void disableUnhookedBranches(TTree* tree_);
  inline TMatrixD* getCovarianceMatrixOfTree(TTree* tree_);

  //! Matrix Tools
  inline std::map<std::string, TMatrixD*> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_= "inverse_covariance_matrix:regularized_eigen_values");
  inline std::vector<double> getEigenValues(TMatrixD *matrix_);
//  inline TMatrixD* computeSqrt(TMatrixD* inputMatrix_);

  //! Histogram Tools
  inline void resetHistogram(TH1D* hist_);
  inline std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_);
  inline std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_);
  inline TH1D* getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_);
  inline TH2D* getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_= "XY");


  //! Canvas Tools
  inline void setDefaultPalette();
  inline void setBlueRedPalette();
  inline void setOrangePalette();
  inline void fixTH2display(TH2 *histogram_);
  inline void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_);


  //! ROOT Internals
  inline void muteRoot();
  inline void unmuteRoot();


}

#include "implementation/GenericToolbox.Root.impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_H
