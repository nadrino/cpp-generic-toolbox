//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_IMPL_H

// STD Headers

// ROOT Headers

// This Project
#include <GenericToolbox.h>
#include <TMatrixDSymEigen.h>
#include <TPaletteAxis.h>
#include <TPad.h>
#include <TMath.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGlobal.h>
#include <TROOT.h>
#include <TFrame.h>


//! Conversion Tools
namespace GenericToolbox {

  TH1D* convertTVectorDtoTH1D(TVectorD* Y_values_, std::string histTitle_, std::string Y_title_,
                              std::string X_title_, TVectorD* Y_errors_){

    auto* th1_histogram = new TH1D(histTitle_.c_str(), histTitle_.c_str(),
                                   Y_values_->GetNrows(), -0.5, Y_values_->GetNrows() - 0.5);

    for(int i_row = 0; i_row < Y_values_->GetNrows(); i_row++)
    {
      th1_histogram->SetBinContent(i_row + 1, (*Y_values_)[i_row]);
      if(Y_errors_ != nullptr)
        th1_histogram->SetBinError(i_row + 1, (*Y_errors_)[i_row]);
    }

    th1_histogram->SetLineWidth(2);
    th1_histogram->SetLineColor(kBlue);
    th1_histogram->GetXaxis()->SetTitle(X_title_.c_str());
    th1_histogram->GetYaxis()->SetTitle(Y_title_.c_str());

    return th1_histogram;
  }
  TH1D* convertTVectorDtoTH1D(std::vector<double> Y_values_, std::string histTitle_, std::string Y_title_, std::string X_title_, TVectorD *Y_errors_){
    TH1D* out = nullptr;
    auto* tVectorHandler = new TVectorD(Y_values_.size(), &Y_values_[0]);
    out = convertTVectorDtoTH1D(tVectorHandler, histTitle_, Y_title_, X_title_, Y_errors_);
    delete tVectorHandler;
    return out;
  }
  TH2D* convertTMatrixDtoTH2D(TMatrixD* XY_values_, std::string graph_title_, std::string Z_title_,
                              std::string Y_title_, std::string X_title_){

    if(graph_title_.empty())
      graph_title_ = XY_values_->GetTitle();

    auto* th2_histogram = new TH2D(graph_title_.c_str(), graph_title_.c_str(),
                                   XY_values_->GetNrows(), -0.5, XY_values_->GetNrows() - 0.5,
                                   XY_values_->GetNcols(), -0.5, XY_values_->GetNcols() - 0.5);

    for(int i_col = 0; i_col < XY_values_->GetNcols(); i_col++)
    {
      for(int j_row = 0; j_row < XY_values_->GetNrows(); j_row++)
      {
        th2_histogram->SetBinContent(i_col + 1, j_row + 1, (*XY_values_)[i_col][j_row]);
      }
    }

    th2_histogram->GetXaxis()->SetTitle(X_title_.c_str());
    th2_histogram->GetYaxis()->SetTitle(Y_title_.c_str());
    th2_histogram->GetZaxis()->SetTitle(Z_title_.c_str());

    return th2_histogram;
  }
  TVectorD *convertStdVectorToTVectorD(std::vector<double>& vect_){

    auto *output = new TVectorD(vect_.size());
    for(int i = 0 ; i < int(vect_.size()) ; i++){
      (*output)[i] = vect_[i];
    }
    return output;

  }
  TMatrixDSym *convertToSymmetricMatrix(TMatrixD *matrix_) {

    auto *symmetric_matrix = (TMatrixD *) matrix_->Clone();
    auto *transposed_symmetric_matrix = new TMatrixD(*matrix_);

    transposed_symmetric_matrix->Transpose(*matrix_);
    *symmetric_matrix += *transposed_symmetric_matrix;
    for (int i_col = 0; i_col < matrix_->GetNcols(); i_col++) {
      for (int i_row = 0; i_row < matrix_->GetNrows(); i_row++) {
        (*symmetric_matrix)[i_row][i_col] /= 2.;
      }
    }

    auto *result = (TMatrixDSym *) symmetric_matrix->Clone(); // Convert to TMatrixDSym

    delete transposed_symmetric_matrix;
    delete symmetric_matrix;

    return result;
  }
    TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_){
        if(covarianceMatrix_ == nullptr) return nullptr;
        if(covarianceMatrix_->GetNrows() != covarianceMatrix_->GetNcols()) return nullptr;

        auto* correlationMatrix = (TMatrixD*) covarianceMatrix_->Clone();

        for(int iRow = 0 ; iRow < covarianceMatrix_->GetNrows() ; iRow++){
            for(int iCol = 0 ; iCol < covarianceMatrix_->GetNcols() ; iCol++){

                if(   (*covarianceMatrix_)[iRow][iRow] == 0
                   or (*covarianceMatrix_)[iCol][iCol] == 0 ){
                    (*correlationMatrix)[iRow][iCol] = 0;
                }
                else{
                    (*correlationMatrix)[iRow][iCol] /=
                        TMath::Sqrt((*covarianceMatrix_)[iRow][iRow]*(*covarianceMatrix_)[iCol][iCol]);
                }

            }
        }

        return correlationMatrix;
    }


}


//! Files Tools
namespace GenericToolbox {

  bool doesTFileIsValid(std::string input_file_path_){
    bool file_is_valid = false;
    if(GenericToolbox::doesPathIsFile(input_file_path_))
    {
      auto old_verbosity = gErrorIgnoreLevel;
      gErrorIgnoreLevel  = kFatal;
      auto* input_tfile  = TFile::Open(input_file_path_.c_str(), "READ");
      if(doesTFileIsValid(input_tfile))
      {
        file_is_valid = true;
        input_tfile->Close();
      }
      delete input_tfile;
      gErrorIgnoreLevel = old_verbosity;
    }
    return file_is_valid;
  }
  bool doesTFileIsValid(TFile* input_tfile_, bool check_if_writable_){

    if(input_tfile_ == nullptr){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "input_tfile_ is a nullptr" << std::endl;
      return false;
    }

    if(not input_tfile_->IsOpen()){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "input_tfile_ = " << input_tfile_->GetName() << " is not opened."
                  << std::endl;
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "input_tfile_->IsOpen() = " << input_tfile_->IsOpen()
                  << std::endl;
      return false;
    }

    if(check_if_writable_ and not input_tfile_->IsWritable()){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "input_tfile_ = " << input_tfile_->GetName()
                  << " is not writable." << std::endl;
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "input_tfile_->IsWritable() = " << input_tfile_->IsWritable()
                  << std::endl;
      return false;
    }

    return true;
  }
  std::vector<TObject *> getListOfObjectFromTDirectory(TDirectory *directory_, std::string class_name_) {
    std::vector<TObject *> output;

    for (int i_entry = 0; i_entry < directory_->GetListOfKeys()->GetSize(); i_entry++) {
      std::string object_name = directory_->GetListOfKeys()->At(i_entry)->GetName();
      TObject *obj = directory_->Get(object_name.c_str());
      if (class_name_.empty() or obj->ClassName() == class_name_) {
        output.emplace_back((TObject *) obj->Clone(object_name.c_str()));
      }
    }

    return output;
  }
  inline TDirectory* mkdirTFile(TDirectory* baseDir_, std::string dirName_){
      if(baseDir_->GetDirectory(dirName_.c_str()) == nullptr){
          baseDir_->mkdir(dirName_.c_str());
      }
      return baseDir_->GetDirectory(dirName_.c_str());
  }
  inline TDirectory* mkdirTFile(TFile* outputFile_, std::string dirName_){
    return mkdirTFile(outputFile_->GetDirectory(""), dirName_);
  }
  std::vector<TFile *> getListOfOpenedTFiles() {
    std::vector<TFile *> output;
    // TIter next_iter(gROOT->GetListOfGlobals());
    auto *global_obj_list = (TList *) gROOT->GetListOfGlobals();
    TGlobal *global;
    for (int i_obj = 0; i_obj < global_obj_list->GetEntries(); i_obj++) {
      global = (TGlobal *) global_obj_list->At(i_obj);
      TString type = global->GetTypeName();
      if (type == "TFile") {
        auto *file = (TFile *) gInterpreter->Calc(global->GetName());
        if (file && file->IsOpen()) {
          // printf("%s: %s\n", global->GetName(),file->GetName());
          output.emplace_back(file);
        }
      }
    }
    // while ((global=(TGlobal*)next_iter())) {

    // }
    return output;
  }


}


//! Matrix Tools
namespace GenericToolbox {

  std::map<std::string, TMatrixD *> invertMatrixSVD(TMatrixD *matrix_, std::string outputContent_) {
    std::map<std::string, TMatrixD *> results_handler;

    auto content_names = GenericToolbox::splitString(outputContent_, ":");

    if (std::find(content_names.begin(), content_names.end(), "inverse_covariance_matrix") != content_names.end()) {
      results_handler["inverse_covariance_matrix"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "regularized_covariance_matrix") != content_names.end()) {
      results_handler["regularized_covariance_matrix"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "projector") != content_names.end()) {
      results_handler["projector"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "regularized_eigen_values") != content_names.end()) {
      results_handler["regularized_eigen_values"]
        = new TMatrixD(matrix_->GetNrows(), 1);
    }

    // make sure all are 0
    for (const auto &matrix_handler : results_handler) {
      for (int i_dof = 0; i_dof < matrix_handler.second->GetNrows(); i_dof++) {
        for (int j_dof = 0; j_dof < matrix_handler.second->GetNcols(); j_dof++) {
          (*matrix_handler.second)[i_dof][j_dof] = 0.;
        }
      }
    }


    // Covariance matrices are symetric :
    auto *symmetric_matrix = convertToSymmetricMatrix(matrix_);
    auto *Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto *Eigen_values = &(Eigen_matrix_decomposer->GetEigenValues());
    auto *Eigen_vectors = &(Eigen_matrix_decomposer->GetEigenVectors());

    double max_eigen_value = (*Eigen_values)[0];
    for (int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++) {
      if (max_eigen_value < (*Eigen_values)[i_eigen_value]) {
        max_eigen_value = (*Eigen_values)[i_eigen_value];
      }
    }

    for (int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++) {
      if ((*Eigen_values)[i_eigen_value] > max_eigen_value * 1E-5) {
        if (results_handler.find("regularized_eigen_values") != results_handler.end()) {
          (*results_handler["regularized_eigen_values"])[i_eigen_value][0]
            = (*Eigen_values)[i_eigen_value];
        }
        for (int i_dof = 0; i_dof < matrix_->GetNrows(); i_dof++) {
          for (int j_dof = 0; j_dof < matrix_->GetNrows(); j_dof++) {
            if (results_handler.find("inverse_covariance_matrix") != results_handler.end()) {
              (*results_handler["inverse_covariance_matrix"])[i_dof][j_dof]
                += (1. / (*Eigen_values)[i_eigen_value])
                   * (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if (results_handler.find("projector") != results_handler.end()) {
              (*results_handler["projector"])[i_dof][j_dof]
                += (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if (results_handler.find("regularized_covariance_matrix") != results_handler.end()) {
              (*results_handler["regularized_covariance_matrix"])[i_dof][j_dof]
                += (*Eigen_values)[i_eigen_value]
                   * (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }

          }
        }
      } else {
//            std::cout << ALERT << "Skipping i_eigen_value = " << (*Eigen_values)[i_eigen_value]
//                      << std::endl;
      }
    }

    // No memory leak ? : CHECKED
    delete Eigen_matrix_decomposer;
    delete symmetric_matrix;

    return results_handler;
  }
  std::vector<double> getEigenValues(TMatrixD *matrix_) {
    auto *symmetric_matrix = GenericToolbox::convertToSymmetricMatrix(matrix_);
    auto *Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto *Eigen_values = &(Eigen_matrix_decomposer->GetEigenValues());

    std::vector<double> output;
    for (int i_dim = 0; i_dim < matrix_->GetNcols(); i_dim++) {
      output.emplace_back((*Eigen_values)[i_dim]);
    }
    std::sort(output.begin(), output.end(), std::greater<double>());
    return output;
  }
//  TMatrixD* computeSqrt(TMatrixD* inputMatrix_){
//    TMatrixD* sqrtMatrix = (TMatrixD*) inputMatrix_->Clone();
//    // calculate sqrt(V) as lower diagonal matrix
//    for( int iRow = 0; iRow < sqrtMatrix->GetNrows(); ++iRow ) {
//      for( int iCol = 0; iCol < sqrtMatrix->GetNcols(); ++iCol ) {
//        (*sqrtMatrix)[iRow][iCol] = 0;
//      }
//    }
//
//    for( int j = 0; j < sqrtMatrix->GetNrows(); ++j ) {
//      // diagonal terms first
//      double Ck = 0;
//      for( int k = 0; k < j; ++k ) {
//        Ck += C[j][k] * C[j][k];
//      } // k
//      C[j][j] = sqrt( fabs( V[j][j] - Ck ) );
//
//      // off-diagonal terms
//      for( int i = j+1; i < sqrtMatrix->GetNrows(); ++i ) {
//        Ck = 0;
//        for( int k = 0; k < j; ++k ) {
//          Ck += C[i][k] * C[j][k];
//        } //k
//        C[i][j] = ( V[i][j] - Ck ) / C[j][j];
//      }// i
//    } // j
//  }

}


//! Histogram Tools
namespace GenericToolbox {

  inline void resetHistogram(TH1D* hist_){
    hist_->Reset("ICESM");
    for(int iBin = 0 ; iBin <= hist_->GetNbinsX()+1 ; iBin++ ){
        hist_->SetBinContent(iBin,0);
        hist_->SetBinError(iBin,0);
    }
  }
  std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_) {
    std::vector<double> output(n_bins_ + 1); // add one extra bin for the boundary
    double xlogmin = TMath::Log10(X_min_);
    double xlogmax = TMath::Log10(X_max_);
    double dlogx = (xlogmax - xlogmin) / ((double) n_bins_);
    for (int i_bin = 0; i_bin <= n_bins_; i_bin++) {
      double xlog = xlogmin + i_bin * dlogx;
      output[i_bin] = TMath::Exp(TMath::Log(10) * xlog);
    }
    return output;
  }
  std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_) {
    std::vector<double> output(n_bins_ + 1); // add one extra bin for the boundary
    double dx = (X_max_ - X_min_) / ((double) n_bins_);
    for (int i_bin = 0; i_bin <= n_bins_; i_bin++) {
      double x = X_min_ + i_bin * dx;
      output[i_bin] = x;
    }
    return output;
  }
  TH1D *getTH1DlogBinning(std::string name_, std::string title_, int n_bins_, double X_min_, double X_max_) {

    TH1D *output = nullptr;
    std::vector<double> xbins = GenericToolbox::getLogBinning(n_bins_, X_min_, X_max_);
    output = new TH1D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0]);
    return output;

  }
  TH2D *getTH2DlogBinning(std::string name_, std::string title_, int nb_X_bins_, double X_min_, double X_max_,
                          int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_) {

    TH2D *output = nullptr;
    std::vector<double> xbins;
    std::vector<double> ybins;
    if (GenericToolbox::doesStringContainsSubstring(log_axis_, "X")) {
      xbins = GenericToolbox::getLogBinning(nb_X_bins_, X_min_, X_max_);
    } else {
      xbins = GenericToolbox::getLinearBinning(nb_X_bins_, X_min_, X_max_);
    }
    if (GenericToolbox::doesStringContainsSubstring(log_axis_, "Y")) {
      ybins = GenericToolbox::getLogBinning(nb_Y_bins_, Y_min_, Y_max_);
    } else {
      ybins = GenericToolbox::getLinearBinning(nb_Y_bins_, Y_min_, Y_max_);
    }

    output = new TH2D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0], ybins.size() - 1, &ybins[0]);
    return output;

  }


}


//! Canvas Tools
namespace GenericToolbox {

  void setDefaultPalette(){
    gStyle->SetPalette(kBird);
  }
  void setBlueRedPalette(){
    gStyle->SetPalette(kBlackBody);
    TColor::InvertPalette();
  }
  void setOrangePalette(){
    gStyle->SetPalette(kDarkBodyRadiator);
  }
  void fixTH2display(TH2 *histogram_){

    gPad->SetRightMargin(0.15);
    histogram_->GetZaxis()->SetTitleOffset(0.8);
    auto* pal = (TPaletteAxis*) histogram_->GetListOfFunctions()->FindObject("palette");
    // TPaletteAxis* pal = (TPaletteAxis*) histogram_->GetListOfFunctions()->At(0);
    if(pal != nullptr){
      pal->SetX1NDC(1 - 0.15 + 0.01);
      pal->SetX2NDC(1 - 0.15 + 0.05);
      pal->GetAxis()->SetMaxDigits(2);
      pal->Draw();
    }

  }
  void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_){

    for( int iPad = 0 ; iPad < canvas_->GetListOfPrimitives()->GetSize() ; iPad++ ){

      auto* pad = (TPad*) canvas_->GetListOfPrimitives()->At(iPad);
      auto* list = (TList*) pad->GetListOfPrimitives();

      TIter next(list);
      TObject *obj;

      while( (obj = next()) ){
        if( obj->InheritsFrom( TH1::Class() ) ) {
          auto* histTemp = (TH1*) obj;
          histTemp->GetXaxis()->SetRangeUser(Xmin_, Xmax_);
        }
        else if( obj->InheritsFrom( TFrame::Class() ) ){
          auto* frameTemp = (TFrame*) obj;
          frameTemp->SetX1(Xmin_);
          frameTemp->SetX2(Xmax_);
        }
      }

      pad->Update();

    }
    canvas_->Update();
  }

}


//! ROOT Internals
namespace GenericToolbox{

    static Int_t oldVerbosity = -1;

    void muteRoot(){
        oldVerbosity      = gErrorIgnoreLevel;
        gErrorIgnoreLevel = kFatal;
    }
    void unmuteRoot(){
        gErrorIgnoreLevel = oldVerbosity;
        oldVerbosity      = -1;
    }

}


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_IMPL_H
