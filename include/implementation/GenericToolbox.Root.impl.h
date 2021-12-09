//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_IMPL_H

// STD Headers
#include "string"
#include "vector"

// ROOT Headers
#include <TMatrixDSymEigen.h>
#include <TPaletteAxis.h>
#include <TPad.h>
#include <TMath.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGlobal.h>
#include <TROOT.h>
#include <TFrame.h>
#include <TLeaf.h>

// This Project
#include <GenericToolbox.h>
#include <TDecompChol.h>

#include <utility>


//! Conversion Tools
namespace GenericToolbox {

  TH1D* convertTVectorDtoTH1D(const TVectorD* yValuesPtr_, const std::string &histTitle_, const std::string &yTitle_,
                              const std::string &xTitle_, TVectorD* yErrorsPtr_){

    auto* th1_histogram = new TH1D(histTitle_.c_str(), histTitle_.c_str(),
                                   yValuesPtr_->GetNrows(), -0.5, yValuesPtr_->GetNrows() - 0.5);

    for(int i_row = 0; i_row < yValuesPtr_->GetNrows(); i_row++)
    {
      th1_histogram->SetBinContent(i_row + 1, (*yValuesPtr_)[i_row]);
      if(yErrorsPtr_ != nullptr)
        th1_histogram->SetBinError(i_row + 1, (*yErrorsPtr_)[i_row]);
    }

    th1_histogram->SetLineWidth(2);
    th1_histogram->SetLineColor(kBlue);
    th1_histogram->GetXaxis()->SetTitle(xTitle_.c_str());
    th1_histogram->GetYaxis()->SetTitle(yTitle_.c_str());

    return th1_histogram;
  }
  TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_, const std::string &Y_title_, const std::string &X_title_, TVectorD *Y_errors_){
    TH1D* out = nullptr;
    auto* tVectorHandler = new TVectorD(int(Y_values_.size()), &Y_values_[0]);
    out = convertTVectorDtoTH1D(tVectorHandler, histTitle_, Y_title_, X_title_, Y_errors_);
    delete tVectorHandler;
    return out;
  }
  TH2D* convertTMatrixDtoTH2D(const TMatrixD* XY_values_, std::string graph_title_, const std::string &Z_title_,
                              const std::string &Y_title_, const std::string &X_title_){

    if(graph_title_.empty()){
      graph_title_ = XY_values_->GetTitle();
    }

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
  TH2D* convertTMatrixDtoTH2D(const TMatrixDSym* XY_values_, std::string graph_title_, const std::string &Z_title_,
                              const std::string &Y_title_, const std::string &X_title_){
    return convertTMatrixDtoTH2D((TMatrixD*) XY_values_, std::move(graph_title_), Z_title_, Y_title_, X_title_);
  }
  TVectorD *convertStdVectorToTVectorD(const std::vector<double> &vect_){

    auto *output = new TVectorD(int(vect_.size()));
    for(int i = 0 ; i < int(vect_.size()) ; i++){
      (*output)[i] = vect_[i];
    }
    return output;

  }
  TMatrixDSym *convertToSymmetricMatrix(TMatrixD *matrix_){
    return convertToSymmetricMatrix((const TMatrixD*) matrix_);
  }
  TMatrixDSym *convertToSymmetricMatrix(const TMatrixD *matrix_) {

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


//! Formula Tools
namespace GenericToolbox {

  inline TFormula* convertToFormula(TTreeFormula* treeFormula_){
    if( treeFormula_ == nullptr ) return nullptr;

    // Grab the appearing leaf names
    std::vector<std::string> leafNameList;
    for( int iLeaf = 0 ; iLeaf < treeFormula_->GetNcodes() ; iLeaf++ ){
      if( not GenericToolbox::doesElementIsInVector(treeFormula_->GetLeaf(iLeaf)->GetName(), leafNameList)){
        leafNameList.emplace_back(treeFormula_->GetLeaf(iLeaf)->GetName());
      }
    }

    // Make sure the longest leaves appear in the list first
    std::sort(leafNameList.begin(), leafNameList.end(), []
        (const std::string& first, const std::string& second){
      return first.size() > second.size();
    });

    std::vector<std::string> expressionBrokenDown;
    std::vector<bool> isReplacedElement;
    expressionBrokenDown.emplace_back(treeFormula_->GetExpFormula().Data());
    isReplacedElement.push_back(false);

    // Replace in the expression
    for( const auto& leafName : leafNameList ){

      // Defining sub pieces
      std::vector<std::vector<std::string>> expressionBreakDownUpdate(expressionBrokenDown.size(), std::vector<std::string>());
      std::vector<std::vector<bool>> isReplacedElementUpdate(isReplacedElement.size(), std::vector<bool>());

      int nExpr = int(expressionBrokenDown.size());
      for( int iExpr = nExpr-1 ; iExpr >= 0 ; iExpr-- ){

        if( isReplacedElement[iExpr] ){
          // Already processed
          continue;
        }

        if( not GenericToolbox::doesStringContainsSubstring(expressionBrokenDown[iExpr], leafName) ){
          // Leaf is not present in this chunk
          continue;
        }
        // Here, we know the leaf appear at least once

        // Adding update pieces
        expressionBreakDownUpdate.at(iExpr) = GenericToolbox::splitString(expressionBrokenDown[iExpr], leafName);
        isReplacedElementUpdate.at(iExpr) = std::vector<bool>(expressionBreakDownUpdate.at(iExpr).size(), false);

        // Look for leaves called as arrays
        int nSubExpr = int(expressionBreakDownUpdate.at(iExpr).size());
        for( int iSubExpr = nSubExpr-1 ; iSubExpr >= 1 ; iSubExpr-- ){

          std::string leafExprToReplace = leafName;

          // Look for an opening "["
          if( expressionBreakDownUpdate.at(iExpr)[iSubExpr][0] == '[' ){
            // It is an array call!
            size_t iChar;
            for( iChar = 0 ; iChar < expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() ; iChar++ ){
              leafExprToReplace += expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar];
              if( expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar] == ']' ){
                if( iChar+1 == expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() or expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar+1] != '[' ){
                  // Ok, it's the end of the array
                  break;
                }
              }
            }

            std::string untouchedSubExpr;
            iChar++;
            for( ; iChar < expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() ; iChar++ ){
              untouchedSubExpr += expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar];
            }
            expressionBreakDownUpdate.at(iExpr)[iSubExpr] = untouchedSubExpr;

            GenericToolbox::replaceSubstringInsideInputString(leafExprToReplace, "[", "(");
            GenericToolbox::replaceSubstringInsideInputString(leafExprToReplace, "]", ")");
          }
          else{
            // Not an array! We are good
          }

          GenericToolbox::insertInVector(expressionBreakDownUpdate.at(iExpr), "[" + leafExprToReplace + "]", iSubExpr);
          GenericToolbox::insertInVector(isReplacedElementUpdate.at(iExpr), true, iSubExpr);

        } // iSubExpr

        // Stripping empty elements
        for( int iSubExpr = nSubExpr-1 ; iSubExpr >= 0 ; iSubExpr-- ){
          if( expressionBreakDownUpdate.at(iExpr).at(iSubExpr).empty() ){
            expressionBreakDownUpdate.at(iExpr).erase(expressionBreakDownUpdate.at(iExpr).begin() + iSubExpr);
            isReplacedElementUpdate.at(iExpr).erase(isReplacedElementUpdate.at(iExpr).begin() + iSubExpr);
          }
        } // iSubExpr

        expressionBrokenDown.erase(expressionBrokenDown.begin() + iExpr);
        isReplacedElement.erase(isReplacedElement.begin() + iExpr);

        GenericToolbox::insertInVector(expressionBrokenDown, expressionBreakDownUpdate.at(iExpr), iExpr);
        GenericToolbox::insertInVector(isReplacedElement, isReplacedElementUpdate.at(iExpr), iExpr);

      } // iExpr

    } // Leaf

    std::string formulaStr = GenericToolbox::joinVectorString(expressionBrokenDown, "");

    return new TFormula(formulaStr.c_str(), formulaStr.c_str());

  }
  inline std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_){
    std::vector<std::string> output;
    if( formula_ == nullptr ) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back(GenericToolbox::splitString(formula_->GetParName(iPar), "(")[0]);
    }
    return output;
  }
  inline std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_){
    std::vector<std::vector<int>> output;
    if(formula_ == nullptr) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back();
      auto parCandidateSplit = GenericToolbox::splitString(formula_->GetParName(iPar), "(");

      if( parCandidateSplit.size() == 1 ){
        continue; // no index
      }
      else{
        // need to fetch the indices
        for( size_t iIndex = 1 ; iIndex < parCandidateSplit.size() ; iIndex++ ){
          std::string indexStr;
          for( char c : parCandidateSplit.at(iIndex) ){
            if( c == ')' ) break;
            indexStr += c;
          }
          output.back().emplace_back(std::stoi(indexStr));
        }
      }

    } // iPar

    return output;
  }
  inline TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_){
    auto* cwd = GenericToolbox::getCurrentTDirectory();
    ROOT::GetROOT()->cd();
    std::vector<Int_t> varObjList(expectedLeafNames_.size(),0);
    auto* fakeTree = new TTree("fakeTree", "fakeTree");
    for( size_t iVar = 0 ; iVar < expectedLeafNames_.size() ; iVar++ ){
      fakeTree->Branch(expectedLeafNames_.at(iVar).c_str(), &varObjList[iVar]);
    }
    fakeTree->Fill();
    auto* output = new TTreeFormula(formulaStr_.c_str(), formulaStr_.c_str(), fakeTree);
    output->SetTree(nullptr);
    delete fakeTree;
    cwd->cd();
    return output;
  }
  inline bool doesLoadedEntryPassCut(TTreeFormula* treeFormula_){
    // instances are distinct expressions which are separated with ";", for example: "var1 == 4; var2 == var3"
    // In practice, we never use multiple instance. In case we do this algo will understand the ";" as "&&"
    bool entryPassCut = true;
    for(int jInstance = 0; jInstance < treeFormula_->GetNdata(); jInstance++) {
      if ( treeFormula_->EvalInstance(jInstance) == 0 ) {
        entryPassCut = false;
        break;
      }
    }
    return entryPassCut;
  }

}


//! Files Tools
namespace GenericToolbox {

  bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_){
    bool fileIsValid = false;
    if(GenericToolbox::doesPathIsFile(inputFilePath_)) {
      auto old_verbosity = gErrorIgnoreLevel;
      gErrorIgnoreLevel  = kFatal;
      auto* tfileCandidatePtr  = TFile::Open(inputFilePath_.c_str(), "READ");
      if(doesTFileIsValid(tfileCandidatePtr)) {
        fileIsValid = true;
        for( const auto& objectPath : objectListToCheck_ ){
          if(tfileCandidatePtr->Get(objectPath.c_str()) == nullptr ){
            fileIsValid = false;
            break;
          }
        }
        tfileCandidatePtr->Close();
      }
      delete tfileCandidatePtr;
      gErrorIgnoreLevel = old_verbosity;
    }
    return fileIsValid;
  }
  bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_){

    if(tfileCandidatePtr_ == nullptr){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ is a nullptr" << std::endl;
      return false;
    }

    if(not tfileCandidatePtr_->IsOpen()){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName() << " is not opened."
                  << std::endl;
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsOpen() = " << tfileCandidatePtr_->IsOpen()
                  << std::endl;
      return false;
    }

    if( tfileCandidatePtr_->IsZombie() ){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->IsZombie()) << std::endl;
      }
      return false;
    }

    if( tfileCandidatePtr_->TestBit(TFile::kRecovered) ){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->TestBit(TFile::kRecovered)) << std::endl;
      }
      return false;
    }

    if(check_if_writable_ and not tfileCandidatePtr_->IsWritable()){
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName()
                  << " is not writable." << std::endl;
      if(GenericToolbox::Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsWritable() = " << tfileCandidatePtr_->IsWritable()
                  << std::endl;
      return false;
    }

    return true;
  }
  std::vector<TObject *> getListOfObjectFromTDirectory(TDirectory *directory_, const std::string &class_name_) {
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
  TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_){
    if( baseDir_ == nullptr ) return nullptr;
    if(baseDir_->GetDirectory(dirName_.c_str()) == nullptr){
      baseDir_->mkdir(dirName_.c_str());
    }
    return baseDir_->GetDirectory(dirName_.c_str());
  }
  TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_){
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
  inline TDirectory* getCurrentTDirectory(){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,23,02)
    return gDirectory.fValue.load();
#else
    // Prior releases of ROOT, gDirectory was returning a TDirectory*
    // Implementation has been made on the 16 Dec 2020:
    // https://github.com/root-project/root/commit/085e9c182b9f639d5921c75de284ae7f20168b6e
    return gDirectory;
#endif
  }
  inline void writeInFile(TDirectory* dir_, const TObject* objToSave_, const std::string &saveName_){
    if( dir_ == nullptr or objToSave_ == nullptr ) return;
    auto* prevDir = getCurrentTDirectory();
    dir_->cd();
    std::string className = objToSave_->ClassName();
    GenericToolbox::replaceSubstringInsideInputString(className, "<", "_");
    GenericToolbox::replaceSubstringInsideInputString(className, ">", "");
    objToSave_->Write(Form("%s_%s", saveName_.c_str(), className.c_str()));
    prevDir->cd();
  }


}

//! Trees Tools
namespace GenericToolbox {

  void disableUnhookedBranches(TTree* tree_){
    if(tree_ == nullptr){
      std::cout << "ERROR in " << __METHOD_NAME__ << ": " << GET_VAR_NAME_VALUE(tree_) << std::endl;
      return;
    }
    tree_->SetBranchStatus("*", false);
    auto* branchList = tree_->GetListOfBranches();
    for( int iBranch = 0 ; iBranch < branchList->GetEntries() ; iBranch++ ){
      if( tree_->GetBranch( branchList->At(iBranch)->GetName() )->GetAddress() != nullptr ){
        tree_->SetBranchStatus( branchList->At(iBranch)->GetName(), true );
      }
    } // iBranch
  }
  TMatrixD* getCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_){

    TMatrixD* outCovMatrix;

    std::vector<TLeaf*> leafList;
    for(int iLeaf = 0 ; iLeaf < tree_->GetListOfLeaves()->GetEntries() ; iLeaf++){
      // DONT SUPPORT ARRAYS AT THE MOMENT
      TLeaf* leafBufferPtr = tree_->GetLeaf(tree_->GetListOfLeaves()->At(iLeaf)->GetName());
      if(tree_->GetBranchStatus(leafBufferPtr->GetName()) == 1){ // check if this branch is active
        if(leafBufferPtr->GetNdata() == 1){
//          std::cout << "Adding: " << leafBufferPtr->GetName() << std::endl;
          leafList.emplace_back(leafBufferPtr);
        }
        else{
          std::cout << __METHOD_NAME__
                    << ": " << tree_->GetListOfLeaves()->At(iLeaf)->GetName()
                    << " -> array leaves are not supported yet." << std::endl;
        }
      }

    }

    // Initializing the matrix
    outCovMatrix = new TMatrixD(int(leafList.size()), int(leafList.size()));
    for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
      for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
        (*outCovMatrix)[iCol][iRow] = 0;
      }
    }

    // Compute mean of every variable
    std::vector<double> meanValueLeafList(leafList.size(),0);
    Long64_t nEntries = tree_->GetEntries();
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) GenericToolbox::displayProgressBar(iEntry, nEntries, "Compute mean of every variable");
      tree_->GetEntry(iEntry);
      for(size_t iLeaf = 0 ; iLeaf < leafList.size() ; iLeaf++){
        meanValueLeafList[iLeaf] += leafList[iLeaf]->GetValue(0);
      }
    }
    for(int iLeaf = 0 ; iLeaf < leafList.size() ; iLeaf++){
      meanValueLeafList[iLeaf] /= double(nEntries);
    }

    // Compute covariance
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) GenericToolbox::displayProgressBar(iEntry, nEntries, "Compute covariance");
      tree_->GetEntry(iEntry);
      for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
        for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
          (*outCovMatrix)[iCol][iRow] +=
            (leafList[iCol]->GetValue(0) - meanValueLeafList[iCol])
            *(leafList[iRow]->GetValue(0) - meanValueLeafList[iRow]);
        } // iRow
      } // iCol
    } // iEntry
    for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
      for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
        (*outCovMatrix)[iCol][iRow] /= double(nEntries);
      }
    }

    return outCovMatrix;

  }

}

//! Matrix Tools
namespace GenericToolbox {

  std::map<std::string, TMatrixD *> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_) {
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
  TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_){
    if(covMatrix_ == nullptr) return nullptr;
    auto* covMatrixSym = GenericToolbox::convertToSymmetricMatrix(covMatrix_);
    auto* out = getCholeskyMatrix(covMatrixSym);
    delete covMatrixSym;
    return out;
  }
  TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_){
    if(covMatrix_ == nullptr) return nullptr;
    auto* choleskyDecomposer = new TDecompChol((*covMatrix_));
    if( not choleskyDecomposer->Decompose() ){ return nullptr; }
    auto* output = (TMatrixD *)(((TMatrixD *)(choleskyDecomposer->GetU()).Clone())->T()).Clone();
    delete choleskyDecomposer;
    return output;
  }
  std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_){
    std::vector<double> out;
    throwCorrelatedParameters(choleskyCovMatrix_, out);
    return out;
  }
  void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_){
    if( choleskyCovMatrix_ == nullptr ) return;
    if( thrownParListOut_.size() != choleskyCovMatrix_->GetNcols() ){
      thrownParListOut_.resize(choleskyCovMatrix_->GetNcols(), 0);
    }
    TVectorD thrownParVec(choleskyCovMatrix_->GetNcols());
    for( int iPar = 0 ; iPar < choleskyCovMatrix_->GetNcols() ; iPar++ ){
      thrownParVec[iPar] = gRandom->Gaus();
    }
    thrownParVec *= (*choleskyCovMatrix_);
    for( int iPar = 0 ; iPar < choleskyCovMatrix_->GetNcols() ; iPar++ ){
      thrownParListOut_.at(iPar) = thrownParVec[iPar];
    }
  }
  TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ ){
    if( v_ == nullptr ) return nullptr;
    if( w_ == nullptr ) w_ = v_;
    auto* out = new TMatrixD(v_->GetNrows(), w_->GetNrows());
    for( int iX = 0 ; iX < v_->GetNrows() ; iX++ ){
      for( int jX = 0 ; jX < w_->GetNrows() ; jX++ ){
        (*out)[iX][jX] = (*v_)[iX] * (*w_)[jX];
      }
    }
    return out;
  }
  template<typename T> void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_){
    if( m_ == nullptr ) return;
    for( int iRow = 0 ; iRow < m_->GetNrows() ; iRow++ ){
      for( int iCol = 0 ; iCol < m_->GetNcols() ; iCol++ ){
        transformFunction_(m_, iRow, iCol);
      }
    }
  }
  template<typename T> auto makeIdentityMatrix(int dim_) -> TMatrixT<T>* {
    auto* out = new TMatrixT<T>(dim_, dim_);
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = 1;
    }
    return out;
  }
  template<typename T> TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_){
    if( v_ == nullptr ) return nullptr;
    auto* out = new TMatrixT<T>(v_->GetNrows(), v_->GetNrows());
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = (*v_)[iDiag];
    }
    return out;
  }
  template<typename T> TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_){
    if( m_ == nullptr ) return nullptr;
    auto* out = new TVectorT<T>(std::min(m_->GetNcols(), m_->GetNrows()));
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag] = (*m_)[iDiag][iDiag];
    }
    return out;
  }
  template<typename T> TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_){
    return GenericToolbox::getMatrixDiagonal((TMatrixT<T>*) m_);
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

  inline void drawHistHorizontalBars(TH1D* hist_){
    // Incompatible with zoom-in
    if(hist_ == nullptr) return;
    TLine *l;
    int n = hist_->GetNbinsX();
    Double_t x1,x2,y;
    for (int i=1; i<=n; i++) {
      y = hist_->GetBinContent(i);
      x1= hist_->GetBinLowEdge(i);
      x2 = hist_->GetBinWidth(i)+x1;
      l= new TLine(x1,y,x2,y);
      l->SetLineColor(hist_->GetLineColor());
//      l->Paint();
      l->Draw();
    }
  }
  inline void resetHistogram(TH1D* hist_){
    hist_->Reset("ICESM");
    transformBinContent(hist_, [](TH1D* h_, int iBin_){
      h_->SetBinContent(iBin_, 0);
      h_->SetBinError(iBin_, 0);
    }, true);
  }
  inline void rescalePerBinWidth(TH1D* hist_, double globalScaler_){
    transformBinContent(hist_, [&](TH1D* h_, int iBin_){
      h_->SetBinContent( iBin_,globalScaler_ * h_->GetBinContent(iBin_)/hist_->GetBinWidth(iBin_) );
    }, true);
  }
  void transformBinContent(TH1D* hist_, std::function<void(TH1D*, int)> transformFunction_, bool processOverflowBins_){
    int firstBin = processOverflowBins_ ? 0 : 1;
    int lastBin = processOverflowBins_ ? hist_->GetNbinsX() + 1 : hist_->GetNbinsX();
    for( int iBin = firstBin ; iBin <= lastBin ; iBin++ ){
      transformFunction_(hist_, iBin);
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
  TH1D *getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_) {

    TH1D *output = nullptr;
    std::vector<double> xbins = GenericToolbox::getLogBinning(n_bins_, X_min_, X_max_);
    output = new TH1D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0]);
    return output;

  }
  TH2D *getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
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
    // canvas_->Draw() is needed to propagate changes
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


#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ROOT_IMPL_H
