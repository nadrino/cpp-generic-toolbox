//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_ROOT_H
#define CPP_GENERIC_TOOLBOX_ROOT_H

#include "GenericToolbox.Build.h"

// GenericToolbox
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Utils.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Log.h"

// ROOT Headers
#include "TMatrixDSymEigen.h"
#include "TTreeFormula.h"
#include "TPaletteAxis.h"
#include "TMatrixDSym.h"
#include "TDecompChol.h"
#include "TParameter.h"
#include "TRandom3.h"
#include "TMatrixD.h"
#include "TFormula.h"
#include "TCanvas.h"
#include "TSpline.h"
#include "TGlobal.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TMath.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TAxis.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TEnv.h"

// STD Headers
#include <type_traits>
#include <string>
#include <memory>
#include <map>
#include <utility>
#include <functional>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


//! User Parameters
//! (CAVEAT: only set for a given source file)
namespace GenericToolbox{
  namespace Parameters{
    #ifdef CPP_GENERIC_TOOLBOX_ROOT_COMPILED
extern CPP_GENERIC_TOOLBOX_ROOT_API int _verboseLevel_;
#else
static int _verboseLevel_ = 0;
#endif
  }
}

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

// Function header
namespace GenericToolbox{

  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* toTMatrixD(TH2D* input_);

  //! Conversion Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertToTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertToTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertToTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertToTH2D(const TMatrixDSym *XY_values_, const std::string& graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  template<typename T> inline TVectorT<T>* convertToTVector(const std::vector<T>& vector_);
  template<typename T> inline TMatrixTSym<T>* toTMatrixTSym(const TMatrixT<T>* matrix_){
    // https://root-forum.cern.ch/t/multivariate-gaussian-object-creation/55721/2
    if( matrix_ == nullptr ){ return nullptr; }

    GTLogThrowIf(
        matrix_->GetNcols() != matrix_->GetNrows(),
        "Matrix dimensions aren't symmetric:" << matrix_->GetNcols() << "x" << matrix_->GetNrows()
    );

    // define the output
    auto* out = new TMatrixTSym<T>(matrix_->GetNrows());

    // check if input is exactly symmetric
    auto isSymmetricFct = [&](){
      for( int iCol = 0 ; iCol < matrix_->GetNcols() ; iCol++ ){
        for( int iRow = iCol+1 ; iRow < matrix_->GetNrows() ; iRow++ ){
          if( (*matrix_)[iCol][iRow] != (*matrix_)[iRow][iCol] ){
            GTLogAlert << "Found a non-symmetric element m[" << iCol << "][" << iRow << "]: diff = "
            << (*matrix_)[iCol][iRow] - (*matrix_)[iRow][iCol] << std::endl;
            return false;
          }
        }
      }
      return true;
    };

    if( not isSymmetricFct() ){
      // assuming this is noise
      GTLogAlert << "Conversion to symmetric matrix by averaging opposite elements..." << std::endl;
      std::unique_ptr<TMatrixT<T>> regularisedMatrix( (TMatrixD *) matrix_->Clone() );

      for( int iCol = 0 ; iCol < matrix_->GetNcols() ; iCol++ ){
        for( int iRow = 0 ; iRow < matrix_->GetNrows() ; iRow++ ){
          (*regularisedMatrix)[iCol][iRow] = ( (*matrix_)[iCol][iRow] + (*matrix_)[iRow][iCol] ) / 2.;
        }
      }

      out->SetSub(0, *regularisedMatrix);
    }
    else{
      out->SetSub(0, *matrix_);
    }

    return out;
  }

  // Deprecated calls (kept for compatibility):
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixDSym *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD* convertStdVectorToTVectorD(const std::vector<double> &vect_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym* convertToSymmetricMatrix(TMatrixD* matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym* convertToSymmetricMatrix(const TMatrixD* matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);

  //! Formula Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TFormula* convertToFormula(TTreeFormula* treeFormula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesEntryPassCut(TTreeFormula* treeFormula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void enableSelectedBranches(TTree* tree_, TTreeFormula* formula_);

  //! Files Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TFile* openExistingTFile(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_ = {});
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_ = {});
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_ = false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TFile*> getListOfOpenedTFiles();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* getCurrentTDirectory();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject* objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject& objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject& objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_=false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void triggerTFileWrite(TDirectory* dir_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsTDirectory(TDirectory* directory_, const std::string& className_ = "");
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsSubDirTDirectory(TDirectory* directory_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TObject*> getListOfObjectFromTDirectory(TDirectory* directory_, const std::string &className_ = "", bool cloneObj_=false);

  template<typename T> std::vector<T*> getObjectList(TDirectory* directory_, bool cloneObj_ = false);

  namespace Internal{
    template<typename T> inline bool convertFromTObject(TObject* objectBuffer_, T*& out_);
    template<typename T> inline void fillFromTObject(TObject* objectBuffer_, T*& out_);
  }

  //! Trees Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void disableUnhookedBranches(TTree* tree_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TLeaf*> getEnabledLeavesList(TTree* tree_, bool includeArrayLeaves_ = true);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD* generateMeanVectorOfTree(TTree* tree_, bool showProgressBar_ = false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* generateCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_ = false, TVectorD* meanValueLeafList_ = nullptr);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::string generateCleanBranchName(const std::string& name_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void generateCompareTree(TTree* tree1_, TTree* tree2_, TDirectory* outDir_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<size_t> getMultiDimArraySizeLeaf(TBranch* branch_);

  //! Matrix Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::map<std::string, TMatrixD*> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_= "inverse_covariance_matrix:regularized_eigen_values");
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getEigenValues(TMatrixD *matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_);
//  inline TMatrixD* computeSqrt(TMatrixD* inputMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ = nullptr);
  template<typename T> inline void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_);
  template<typename T> inline TMatrixT<T>* makeIdentityMatrix(int dim_);
  template<typename T> inline TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_);

  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixLine(TMatrixT<T>* m_, int line_);
  template<typename T> inline TVectorT<T>* getMatrixColumn(TMatrixT<T>* m_, int col_);

  //! Histogram Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void drawHistHorizontalBars(TH1D* hist_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void resetHistogram(TH1D* hist_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void rescalePerBinWidth(TH1D* hist_, double globalScaler_ = 1);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void transformBinContent(TH1D* hist_, const std::function<void(TH1D*, int)>& transformFunction_, bool processOverflowBins_ = false);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range fetchYRange(TH1* h_, bool withError_ = true, const Range& caps_ = {0., std::nan("unset")});
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(TH1* h_, const Range& margins_ = {0.1, 0.25});
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(const std::vector<TH1*>& h_, const Range& margins_ = {0.1, 0.25});
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_= "XY");
  template <class T> inline double getHistogramFwhm(TH1* hist_, int binMin_ = 1, int binMax_ = -1);


  //! Spline / Graph tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TGraph* graph_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TSpline3* spline_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool hasUniformlySpacedKnots(const TGraph* graph_, double tolerance_ = 1E-6);

  //! Canvas Tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setDefaultPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setBlueRedPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setT2kPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setOrangePalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void fixTH2display(TH2 *histogram_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_);

  //! Display tools
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TCanvas* canvas_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TH1* hist_);


  //! ROOT Internals
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void muteRoot();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void unmuteRoot();

  CPP_GENERIC_TOOLBOX_ROOT_INLINE char findOriginalVariableType(const AnyType& obj_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const std::string& leafTypeName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const TLeaf* leaf_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const TLeaf* leaf_, AnyType& out_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const std::string& leafTypeName_, AnyType& out_);

  // LinkDef does not enable function declaration by itself.
  // A proxy class can be used to trigger the function declaration on CINT startup
  struct Enabler{};

  class [[maybe_unused]] TFilePath {

  public:
    explicit TFilePath(TDirectory* rootDir_): rootDir(rootDir_) {}
    TFilePath(TDirectory* rootDir_, std::string subDirPath_): rootDir(rootDir_), subDirPath(std::move(subDirPath_)) {}

    [[nodiscard]] TDirectory* getRootDir() const { return rootDir; }
    [[nodiscard]] TFilePath getSubDir( const std::string& subDir_ ) const {
      TFilePath out(*this);
      out.subDirPath = GenericToolbox::joinPath(this->subDirPath, subDir_);
      return out;
    }
    [[nodiscard]] TDirectory* getDir() const { return GenericToolbox::mkdirTFile(rootDir, subDirPath); }

  private:
    TDirectory* rootDir{nullptr};
    std::string subDirPath{};
  };

}


//! Conversion Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const TVectorD* yValuesPtr_, const std::string &histTitle_, const std::string &yTitle_,
                                     const std::string &xTitle_, TVectorD* yErrorsPtr_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_, const std::string &Y_title_, const std::string &X_title_, TVectorD *Y_errors_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixD* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixDSym* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD *convertStdVectorToTVectorD(const std::vector<double> &vect_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym *convertToSymmetricMatrix(const TMatrixD *matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym *convertToSymmetricMatrix(TMatrixD *matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);
  template<typename T> inline T* toCorrelationMatrix(const T* matrix_){
    if( matrix_ == nullptr ){ return nullptr;}
    auto* out = (T*) matrix_->Clone();

    for(int iRow = 0 ; iRow < matrix_->GetNrows() ; iRow++){
      for(int iCol = 0 ; iCol < matrix_->GetNcols() ; iCol++){
        if( (*matrix_)[iRow][iRow] == 0 or (*matrix_)[iCol][iCol] == 0 ){ (*out)[iRow][iCol] = 0; }
        else{ (*out)[iRow][iCol] /= std::sqrt((*matrix_)[iRow][iRow]*(*matrix_)[iCol][iCol]); }
      }
    }

    return out;
  }
  template<typename T> inline std::shared_ptr<T> toCorrelationMatrix(const std::shared_ptr<T>& matrix_){
    return std::shared_ptr<T>(toCorrelationMatrix(matrix_.get()));
  }

}

//! Formula Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE TFormula* convertToFormula(TTreeFormula* treeFormula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesEntryPassCut(TTreeFormula* treeFormula_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void enableSelectedBranches(TTree* tree_, TTreeFormula* formula_);

}

//! Files Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE TFile* openExistingTFile(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TObject *> getListOfObjectFromTDirectory(TDirectory *directory_, const std::string &className_, bool cloneObj_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsTDirectory(TDirectory* directory_, const std::string& className_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsSubDirTDirectory(TDirectory* directory_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TFile *> getListOfOpenedTFiles();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* getCurrentTDirectory();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject* objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject& objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject& objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void triggerTFileWrite(TDirectory* dir_);

  // template
  template<typename T> inline std::vector<T*> getObjectList(TDirectory* directory_, bool cloneObj_){
    if( directory_ == nullptr ) return {};

    TClass* templateClass = TClass::GetClass<T>();
    if( templateClass == nullptr ){
      throw std::runtime_error("invalid template class in getObjectList. Not TObject castable?");
    }

    auto ls = lsTDirectory(directory_, templateClass->GetName());
    std::vector<T*> output; output.reserve(ls.size());
    for( auto& entry : ls ){
      output.emplace_back( directory_->Get<T>(entry.c_str()) );
      if( cloneObj_ ){ output.back() = (T*) output.back()->Clone(); }
    }

    delete templateClass;
    return output;
  }
  template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>> inline void writeInTFileWithObjTypeExt(TDirectory* dir_, const T& objToSave_, std::string saveName_){
    TParameter<T> out(saveName_.c_str(), objToSave_);
    writeInTFileWithObjTypeExt(dir_, (TObject*) &out);
  }
  template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>> inline void writeInTFile(TDirectory* dir_, const T& objToSave_, std::string saveName_){
    TParameter<T> out(saveName_.c_str(), objToSave_);
    writeInTFile(dir_, (TObject*) &out);
  }
  template<typename T> auto fetchObject(const std::string& objectPath_) -> T*{

    GTLogThrowIf( objectPath_.empty(), "No object path provided." );

    // example: objectPath_ = "/path/to/tfile.root:my/dir/object"
    auto pathElements = GenericToolbox::splitString(objectPath_, ":");
    GTLogThrowIf(pathElements.size() < 2, "Could not parse path: " + objectPath_);

    std::string rootFilePath{ GenericToolbox::joinVectorString(pathElements, ":", 0, -1 ) };
    std::string rootObjectPath{ pathElements[int(pathElements.size())-1] };

    rootFilePath = GenericToolbox::expandEnvironmentVariables(rootFilePath);

    auto currentDir = gDirectory->GetDirectory("");

    // the containing TFile will auto destruct once we leave the thread
    std::unique_ptr<TFile> file(TFile::Open(rootFilePath.c_str()));
    GTLogThrowIf(file == nullptr or not file->IsOpen(), "Could not open: " + rootFilePath);

    TObject* objBuffer = file->Get(rootObjectPath.c_str());
    GTLogThrowIf(objBuffer == nullptr, "Could not fine object with name \"" + rootObjectPath + "\" within the opened TFile: " + rootFilePath);

    // make sure the cloned obj doesn't belong to a file
    T* out{nullptr};
    gDirectory = currentDir; // make sure the cloned object is not handled by the tfile
    Internal::fillFromTObject(objBuffer, out);

    GTLogThrowIf(out == nullptr, "While reading TObject from: " << objectPath_ << ", couldn't convert the TObject/" << objBuffer->ClassName() << " to the desired type");

    return out;
  }
  template<typename T> void fetchObject(const std::string& objectPath_, std::shared_ptr<T>& out_){
    T* outPtr = fetchObject<T>( objectPath_ );
    out_ = std::shared_ptr<T>( outPtr );
  }

  namespace Internal{
    // not meant to be used by devs
    // those are tricks to avoid template overrides

    // custom conversions
    template<typename T> inline bool convertFromTObject(TObject* objectBuffer_, T*& out_){ return false; }
    template<typename U> inline bool convertFromTObject(TObject* objectBuffer_, TMatrixTSym<U>*& out_){

      if( objectBuffer_ == nullptr ){ return false; }

      if( GenericToolbox::startsWith(std::string( objectBuffer_->ClassName() ), "TMatrixT") ){
        out_ = GenericToolbox::toTMatrixTSym((TMatrixT<U>*) objectBuffer_->Clone());
        return true;
      }

      return false;
    }

    // assuming T is a ROOT TObject based
    template<typename T> inline void fillFromTObject(TObject* objectBuffer_, T*& out_){

      // this template implementation relies on the fact that T is a ROOT object (TObject based)

      // here handles custom conversions:
      if( convertFromTObject(objectBuffer_, out_) ){
        // if returns true, conversion is considered as successfull
        return;
      }

      // matching type? great! recast to the correct type
      if( std::string(objectBuffer_->ClassName()) == std::string(T::Class_Name()) ){
        out_ = (T*) objectBuffer_->Clone();
        return;
      }

      // inheritance? does the source is higher level?
      if( TClass( objectBuffer_->ClassName() ).InheritsFrom( T::Class_Name() ) ){
        out_ = (T*) objectBuffer_->Clone();
        return;
      }

      // last resort: relying on TObject::Copy()
      // this is dangerous as this could create a seg fault (for instance from TH1F to TVectorD...)
      // Using well known cases where it works
      if( T::Class()->InheritsFrom( "TH1" ) and TClass( objectBuffer_->ClassName() ).InheritsFrom( "TH1" ) ){

        // memory needs to be set
        out_ = new T();

        // use of the Copy() -> TObject based will use the overriden Copy() method
        // with incompatible types, this might end up with a segfault...
        objectBuffer_->Copy(*out_);

        return;
      }

      // reached this point? nothing happened
      GTLogError << "Unhandled conversion from " << objectBuffer_->ClassName() << " to " << T::Class_Name() << std::endl;
    }
  }


}

//! Trees Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void disableUnhookedBranches(TTree* tree_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TLeaf*> getEnabledLeavesList(TTree* tree_, bool includeArrayLeaves_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD* generateMeanVectorOfTree(TTree* tree_, bool showProgressBar_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* generateCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_, TVectorD* meanValueLeafList_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::string generateCleanBranchName(const std::string& name_);

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void generateCompareTree(TTree* tree1_, TTree* tree2_, TDirectory* outDir_){
//    if( tree1_ == nullptr ) throw std::runtime_error("tree1_ is nullptr");
//    if( tree2_ == nullptr ) throw std::runtime_error("tree2_ is nullptr");
//    if( outDir_ == nullptr ) throw std::runtime_error("outDir_ is nullptr");
//
////    TTree* outCompTree{nullptr};
//
//    auto* prevDir = getCurrentTDirectory();
//    outDir_->cd();
//
////    outCompTree = new TTree("outCompTree", "outCompTree");
//
//    ((TBranch*) tree1_->GetListOfBranches()->At(0))->GetAddress();
//
//    prevDir->cd();
  }
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<size_t> getMultiDimArraySizeLeaf(TBranch* branch_){
    if(branch_==nullptr){ return {}; }

    std::string title = branch_->GetTitle(); // "array[4][32]/I"
    std::regex dim_regex(R"(\[(\d+)\])");
    std::smatch match;
    std::vector<size_t> dims;

    for(std::sregex_iterator it(title.begin(), title.end(), dim_regex), end; it != end; ++it){
      dims.emplace_back(std::stoi((*it)[1]));
    }

    return dims;
  }
}

//! Matrix Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::map<std::string, TMatrixD *> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getEigenValues(TMatrixD *matrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ );
  template<typename T> inline void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_){
    if( m_ == nullptr ) return;
    for( int iRow = 0 ; iRow < m_->GetNrows() ; iRow++ ){
      for( int iCol = 0 ; iCol < m_->GetNcols() ; iCol++ ){
        transformFunction_(m_, iRow, iCol);
      }
    }
  }
  template<typename T> inline auto makeIdentityMatrix(int dim_) -> TMatrixT<T>* {
    auto* out = new TMatrixT<T>(dim_, dim_);
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = 1;
    }
    return out;
  }
  template<typename T> inline TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_){
    if( v_ == nullptr ) return nullptr;
    auto* out = new TMatrixT<T>(v_->GetNrows(), v_->GetNrows());
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = (*v_)[iDiag];
    }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_){
    if( m_ == nullptr ) return nullptr;
    auto* out = new TVectorT<T>(std::min(m_->GetNcols(), m_->GetNrows()));
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag] = (*m_)[iDiag][iDiag];
    }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_){
    return getMatrixDiagonal((TMatrixT<T>*) m_);
  }
  template<typename T> inline TVectorT<T>* getMatrixLine(TMatrixT<T>* m_, int line_){
    if( m_ == nullptr ) return nullptr;
    if( line_ < 0 or line_ >= m_->GetNrows() ) throw std::runtime_error("invalid matrix line: " + std::to_string(line_));
    auto* out = new TVectorT<T>(m_->GetNcols());
    for( int iCol = 0 ; iCol < out->GetNrows() ; iCol++ ){ (*out)[iCol] = (*m_)[line_][iCol]; }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixColumn(TMatrixT<T>* m_, int col_){
    if( m_ == nullptr ) return nullptr;
    if( col_ < 0 or col_ >= m_->GetNcols() ) throw std::runtime_error("invalid matrix line: " + std::to_string(col_));
    auto* out = new TVectorT<T>(m_->GetNrows());
    for( int iRow = 0 ; iRow < out->GetNrows() ; iRow++ ){ (*out)[iRow] = (*m_)[iRow][col_]; }
    return out;
  }
}


//! Histogram Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void drawHistHorizontalBars(TH1D* hist_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void resetHistogram(TH1D* hist_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void rescalePerBinWidth(TH1D* hist_, double globalScaler_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void transformBinContent(TH1D* hist_, const std::function<void(TH1D*, int)>& transformFunction_, bool processOverflowBins_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range fetchYRange(TH1* h_, bool withError_, const Range& caps_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(TH1* h_, const Range& margins_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(const std::vector<TH1*>& h_, const Range& margins_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D *getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D *getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_);

  template<class T> inline double getHistogramFwhm(TH1* hist_, int binMin_, int binMax_) {
    /**
     * @param hist_ histogram to extract FWHM
     * @param binMin_ first bin to evaluate FWHM
     * @param binMax_ last bin to evaluate FWHM
     * @return full width half maximum of the histogram
     */

    // Sanity check
    if( hist_ == nullptr ) throw std::logic_error("hist_ is nullptr");

    // TH2 are castable to TH1 but are not allowed arg
    // GetBinLowEdge() for TH2 return NaN. Test it.
    double test = hist_->GetBinLowEdge(1);
    if( test != test ) throw std::logic_error("Argument is not 1D histo");

    if( binMax_ == -1 ) binMax_ = hist_->GetNbinsX();
    double maxValue = hist_->GetMaximum();

    double lowFwhmBound{0};
    // First, start from the lower bound
    int iBin;
    for( iBin = binMin_; iBin <= binMax_; iBin++ ) {
      if( hist_->GetBinContent(iBin) > maxValue/2. ) { lowFwhmBound = hist_->GetBinLowEdge(iBin); break; }
    }

    // Continue until the high bound has been found
    double highFwhmBound = lowFwhmBound;
    for( ; iBin <= binMax_; iBin++ ) {
      if( hist_->GetBinContent(iBin) < maxValue / 2) { highFwhmBound = hist_->GetBinLowEdge(iBin) + hist_->GetBinWidth(iBin); break; }
    }

    return highFwhmBound - lowFwhmBound;
  }
  CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TH2D*> chopTH2D(TH2D* h, int iBranchSeparation);

}

// Misc
namespace GenericToolbox{

  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TGraph* graph_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TSpline3* spline_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE bool hasUniformlySpacedKnots(const TGraph* graph_, double tolerance_);

}

//! Canvas Tools
namespace GenericToolbox {

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setDefaultPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setBlueRedPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setT2kPalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setOrangePalette();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void fixTH2display(TH2 *histogram_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_);

}


//! Display tools
namespace GenericToolbox{

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TCanvas* canvas_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TH1* hist_);

}


//! ROOT Internals
namespace GenericToolbox{

  #ifdef CPP_GENERIC_TOOLBOX_ROOT_COMPILED
extern CPP_GENERIC_TOOLBOX_ROOT_API Int_t oldVerbosity;
#else
static Int_t oldVerbosity = -1;
#endif

  CPP_GENERIC_TOOLBOX_ROOT_INLINE void muteRoot();
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void unmuteRoot();

  CPP_GENERIC_TOOLBOX_ROOT_INLINE char findOriginalVariableType(const AnyType& obj_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const std::string& leafTypeName_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const TLeaf* leaf_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const TLeaf* leaf_, AnyType& out_);
  CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const std::string& leafTypeName_, AnyType& out_);

}


// Classes

// CorrelatedVariablesSampler
namespace GenericToolbox {
  class CorrelatedVariablesSampler : public InitBaseClass {

  public:
    CorrelatedVariablesSampler() = default;
    ~CorrelatedVariablesSampler() override { for( auto& diag : diagonalPerBlock ){ delete diag.getCovarianceMatrixPtr(); } };

    // setters
    void setCovarianceMatrixPtr(const TMatrixDSym *covarianceMatrixPtr_){ _covarianceMatrixPtr_ = covarianceMatrixPtr_; }
    void setPrng(TRandom* prng_){ _prng_ = prng_; }
    void setNbMaxTries(int nMaxTries_){ nMaxTries = nMaxTries_; }

    // const getters
    [[nodiscard]] const TMatrixDSym* getCovarianceMatrixPtr() const { return _covarianceMatrixPtr_; }

    // mutable getters
    std::vector<Range>& getParLimitList(){ return _rangeList_; }

    // main methods
    CPP_GENERIC_TOOLBOX_ROOT_INLINE void throwCorrelatedVariables(TVectorD& output_);
    CPP_GENERIC_TOOLBOX_ROOT_INLINE void extractBlocks();

  protected:
    CPP_GENERIC_TOOLBOX_ROOT_INLINE void initializeImpl() override;

  private:
    // inputs:
    const TMatrixDSym * _covarianceMatrixPtr_{nullptr};
    TRandom* _prng_{gRandom}; // using TRandom3 by default
    int nMaxTries{1};

    // internals
    std::shared_ptr<TDecompChol> _choleskyDecomposer_{nullptr};
    std::shared_ptr<TMatrixD> _sqrtMatrix_{nullptr};
    std::shared_ptr<TVectorD> _throwBuffer_{nullptr};

    std::vector<Range> _rangeList_{};

    // nested
    std::vector<std::vector<int>> blockIndexLists{};
    std::vector<CorrelatedVariablesSampler> diagonalPerBlock{};

  };


}

// TreeBuffer
namespace GenericToolbox{

  class TreeBuffer{
    /*
     * The ChainBuffer helps to handle general TTree/TChain expressions
     * that get automatically parsed and converted to the optimal type
     * by using GenericToolbox::AnyType
     *
     */

  public:
    // forward declaration for getters
    class ExpressionBuffer;

  public:
    TreeBuffer() = default;

    // setters
    void setTree(TTree* chainPtr_){ _treePtr_ = chainPtr_; }

    // const getters
    [[nodiscard]] const std::vector<std::shared_ptr<ExpressionBuffer>>& getExpressionBufferList() const{ return _expressionBufferList_; }

    // pre-initialize
    CPP_GENERIC_TOOLBOX_ROOT_INLINE int addExpression(const std::string& exprStr_);

    // initialize
    CPP_GENERIC_TOOLBOX_ROOT_INLINE void initialize();

    // main methods
    CPP_GENERIC_TOOLBOX_ROOT_INLINE void saveExpressions();
    [[nodiscard]] CPP_GENERIC_TOOLBOX_ROOT_INLINE const ExpressionBuffer* getExpressionBuffer(const std::string& exprStr_) const;
    [[nodiscard]] CPP_GENERIC_TOOLBOX_ROOT_INLINE int getExpressionBufferIndex(const std::string& exprStr_) const;

    // sub-classes
    struct BranchInfo{
      // used for ttree update / notify
      std::string name{};
      size_t byteOffset{};
    };
    class TObjNotifier : public TObject {

    public:
      TObjNotifier() = default;

      // overrides
      ~TObjNotifier() override = default;
      Bool_t Notify() override { if( _onNotifyFct_ ){ _onNotifyFct_(); return true; } return false; }
      static constexpr Version_t Class_Version() { return 1; }

      // setters
      void setOnNotifyFct(const std::function<void()>& onNotifyFct_){ _onNotifyFct_ = onNotifyFct_; }

    private:
      std::function<void()> _onNotifyFct_{};

    };

    // expression and its derivatives
    class ExpressionBuffer{

    public:
      virtual ~ExpressionBuffer() = default;

      virtual void save(const uint8_t* src_);
      virtual void notify(TTree* tree_){}

      // setters
      void setByteOffset(size_t offset_){ _byteOffset_ = offset_;}
      void setExpression(const std::string& expression_){ _expression_ = expression_; }
      void setBuffer(const GenericToolbox::AnyType &buffer_){ _buffer_ = buffer_; }

      // getters
      [[nodiscard]] const GenericToolbox::AnyType& getBuffer() const{ return _buffer_; }
      [[nodiscard]] const std::string& getExpression() const{ return _expression_; }

    protected:
      [[nodiscard]] virtual const uint8_t* getBufferAddress(const uint8_t* src_) const{ return src_ + _byteOffset_; }

      size_t _byteOffset_{0};
      std::string _expression_;
      // this buffers both saves the type, its size and its value
      GenericToolbox::AnyType _buffer_{};
    };
    class NestedArrayExpression : public ExpressionBuffer {
    public:
      void setObjSize(size_t objSize_){ objSize = objSize_;}
      void setExpPtrArrayIndex(ExpressionBuffer* expPtrArrayIndex_){ expPtrArrayIndex = expPtrArrayIndex_;}

    protected:
      [[nodiscard]] const uint8_t* getBufferAddress(const uint8_t* src_) const override;

      size_t objSize{0};
      ExpressionBuffer* expPtrArrayIndex{nullptr};
    };
    class FormulaExpression : public ExpressionBuffer {
    public:
      void parseFormula(TTree* tree_);

    protected:
      std::unique_ptr<TTreeFormula> _formulaPtr_{nullptr};

      void notify(TTree* tree_) override;
      void save(const uint8_t* src_) override;
    };

  protected:
    size_t getBranchByteOffset(const std::string& branchName_);
    void expandBuffer(const std::string& branchName_);
    void notify();

  private:
    // The handled TChained
    TTree* _treePtr_{nullptr};

    // where TChain is writing the data
    std::vector<uint8_t> _chainBuffer_{};

    // holding branch info for re-linking the TChain
    std::vector<BranchInfo> _branchInfoList_{};

    // where the expressions are evaluated
    std::vector<std::shared_ptr<ExpressionBuffer>> _expressionBufferList_{};

    // to change re-hook address on a new TTree
    TObjNotifier _notifier_{};

  };


}


#ifndef CPP_GENERIC_TOOLBOX_ROOT_COMPILED
#include "implementation/GenericToolbox.Root.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_ROOT_H
