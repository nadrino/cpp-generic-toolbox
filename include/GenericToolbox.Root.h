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
#include "TRandom3.h"

namespace GenericToolbox{

  const std::vector<Color_t> defaultColorWheel = {
      kGreen-3, kTeal+3, kAzure+7,
      kCyan-2, kBlue-7, kBlue+2,
      kOrange-3, kOrange+9, kRed+2,
      kPink+9, kViolet, kGreen-8,
      kCyan+1, kOrange-4, kOrange+6,
      kMagenta-10, kCyan-9, kGreen-10
  };

}

namespace GenericToolbox{

  //! Conversion Tools
  inline TH1D* convertToTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  inline TH1D* convertToTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertToTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TH2D* convertToTH2D(const TMatrixDSym *XY_values_, const std::string& graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  template<typename T> inline TVectorT<T>* convertToTVector(const std::vector<T>& vector_);

  // Deprecated calls (kept for compatibility):
  inline TH1D* convertTVectorDtoTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  inline TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixDSym *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TVectorD* convertStdVectorToTVectorD(const std::vector<double> &vect_);
  inline TMatrixDSym* convertToSymmetricMatrix(TMatrixD* matrix_);
  inline TMatrixDSym* convertToSymmetricMatrix(const TMatrixD* matrix_);
  inline TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);

  //! Formula Tools
  inline TFormula* convertToFormula(TTreeFormula* treeFormula_);
  inline std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_);
  inline std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_);
  inline TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_);
  inline bool doesLoadedEntryPassCut(TTreeFormula* treeFormula_);

  //! Files Tools
  inline bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_ = {});
  inline bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_ = false);
  inline std::vector<TFile*> getListOfOpenedTFiles();
  inline std::vector<TObject*> getListOfObjectFromTDirectory(TDirectory* directory_, const std::string &class_name_ = "");
  inline TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_);
  inline TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_);
  inline TDirectory* getCurrentTDirectory();
  inline void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_ = "");

  //! Trees Tools
  inline void disableUnhookedBranches(TTree* tree_);
  inline TMatrixD* getCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_ = false);

  //! Matrix Tools
  inline std::map<std::string, TMatrixD*> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_= "inverse_covariance_matrix:regularized_eigen_values");
  inline std::vector<double> getEigenValues(TMatrixD *matrix_);
  inline TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_);
  inline TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_);
  inline std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_);
  inline void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_);
//  inline TMatrixD* computeSqrt(TMatrixD* inputMatrix_);
  inline TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ = nullptr);
  template<typename T> inline void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_);
  template<typename T> inline TMatrixT<T>* makeIdentityMatrix(int dim_);
  template<typename T> inline TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_);

  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixLine(TMatrixT<T>* m_, int line_);
  template<typename T> inline TVectorT<T>* getMatrixColumn(TMatrixT<T>* m_, int col_);

  //! Histogram Tools
  inline void drawHistHorizontalBars(TH1D* hist_);
  inline void resetHistogram(TH1D* hist_);
  inline void rescalePerBinWidth(TH1D* hist_, double globalScaler_ = 1);
  inline void transformBinContent(TH1D* hist_, std::function<void(TH1D*, int)> transformFunction_, bool processOverflowBins_ = false);
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
