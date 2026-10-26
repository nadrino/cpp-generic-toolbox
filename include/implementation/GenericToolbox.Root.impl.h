#pragma once

namespace GenericToolbox {
namespace Parameters {
#ifdef CPP_GENERIC_TOOLBOX_ROOT_COMPILED
int _verboseLevel_ = 0;
#endif
}

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* toTMatrixD(TH2D* input_) {
    if( input_ == nullptr ){
      GTLogError << "input TH2D is nullptr." << std::endl;
      return nullptr;
    }

    std::unique_ptr<TMatrixD> out{nullptr};

    out = std::make_unique<TMatrixD>(input_->GetNbinsX(), input_->GetNbinsY());

    for( int iCol = 0 ; iCol < input_->GetNbinsX() ; iCol++ ){
      for( int iRow = 0 ; iRow < input_->GetNbinsY() ; iRow++ ){
        (*out)[iCol][iRow] = input_->GetBinContent(iCol, iRow);
      }
    }

    return out.release();
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const TVectorD* yValuesPtr_, const std::string &histTitle_, const std::string &yTitle_,
                                     const std::string &xTitle_, TVectorD* yErrorsPtr_) {

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_, const std::string &Y_title_, const std::string &X_title_, TVectorD *Y_errors_) {
    TH1D* out = nullptr;
    auto* tVectorHandler = new TVectorD(int(Y_values_.size()), &Y_values_[0]);
    out = convertTVectorDtoTH1D(tVectorHandler, histTitle_, Y_title_, X_title_, Y_errors_);
    delete tVectorHandler;
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixD* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_) {

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D* convertTMatrixDtoTH2D(const TMatrixDSym* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_) {
    return convertTMatrixDtoTH2D((TMatrixD*) XY_values_, std::move(graph_title_), Z_title_, Y_title_, X_title_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD *convertStdVectorToTVectorD(const std::vector<double> &vect_) {

    auto *output = new TVectorD(int(vect_.size()));
    for(int i = 0 ; i < int(vect_.size()) ; i++){
      (*output)[i] = vect_[i];
    }
    return output;

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym *convertToSymmetricMatrix(const TMatrixD *matrix_) {

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixDSym *convertToSymmetricMatrix(TMatrixD *matrix_) {
    return convertToSymmetricMatrix((const TMatrixD*) matrix_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_) {
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
              std::sqrt((*covarianceMatrix_)[iRow][iRow]*(*covarianceMatrix_)[iCol][iCol]);
        }

      }
    }

    return correlationMatrix;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TFormula* convertToFormula(TTreeFormula* treeFormula_) {
    if( treeFormula_ == nullptr ) return nullptr;

    // Grab the appearing leaf names
    std::vector<std::string> leafNameList;
    for( int iLeaf = 0 ; iLeaf < treeFormula_->GetNcodes() ; iLeaf++ ){
      if( not isIn(treeFormula_->GetLeaf(iLeaf)->GetName(), leafNameList)){
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

        if( not GenericToolbox::hasSubStr(expressionBrokenDown[iExpr], leafName) ){
          // Leaf is not present in this chunk
          continue;
        }
        // Here, we know the leaf appear at least once

        // Adding update pieces
        expressionBreakDownUpdate.at(iExpr) = splitString(expressionBrokenDown[iExpr], leafName);
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

            replaceSubstringInsideInputString(leafExprToReplace, "[", "(");
            replaceSubstringInsideInputString(leafExprToReplace, "]", ")");
          }
          else{
            // Not an array! We are good
          }

          insertInVector(expressionBreakDownUpdate.at(iExpr), "[" + leafExprToReplace + "]", iSubExpr);
          insertInVector(isReplacedElementUpdate.at(iExpr), true, iSubExpr);

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

        insertInVector(expressionBrokenDown, expressionBreakDownUpdate.at(iExpr), iExpr);
        insertInVector(isReplacedElement, isReplacedElementUpdate.at(iExpr), iExpr);

      } // iExpr

    } // Leaf

    std::string formulaStr = joinVectorString(expressionBrokenDown, "");

    return new TFormula(formulaStr.c_str(), formulaStr.c_str());

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_) {
    std::vector<std::string> output;
    if( formula_ == nullptr ) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back(splitString(formula_->GetParName(iPar), "(")[0]);
    }
    return output;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_) {
    std::vector<std::vector<int>> output;
    if(formula_ == nullptr) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back();
      auto parCandidateSplit = splitString(formula_->GetParName(iPar), "(");

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_) {
    auto* cwd = getCurrentTDirectory();
    ::ROOT::GetROOT()->cd();
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesEntryPassCut(TTreeFormula* treeFormula_) {
    // instances are distinct expressions which are separated with ";", for example: "var1 == 4; var2 == var3"
    // In practice, we never use multiple instance. In case we do, this algo will understand the ";" as "&&"
    for(int jInstance = 0; jInstance < treeFormula_->GetNdata(); jInstance++) {
      if( treeFormula_->EvalInstance(jInstance) == 0 ) { return false; }
    }
    return true;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void enableSelectedBranches(TTree* tree_, TTreeFormula* formula_) {
    for( int iLeaf = 0 ; iLeaf < formula_->GetNcodes() ; iLeaf++ ){
      if( formula_->GetLeaf(iLeaf) == nullptr ) continue; // for "Entry$" like dummy leaves
      tree_->SetBranchStatus(formula_->GetLeaf(iLeaf)->GetBranch()->GetName(), true);
    }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TFile* openExistingTFile(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_) {
    TFile* outPtr{nullptr};

    if( not isFile(inputFilePath_) ){
      throw std::runtime_error("Could not find file: \"" + inputFilePath_ + "\"");
    }
    auto old_verbosity = gErrorIgnoreLevel;
    gErrorIgnoreLevel  = kFatal;
    gEnv->SetValue("TFile.Recover", 0);
    outPtr = TFile::Open(inputFilePath_.c_str(), "READ");
    gErrorIgnoreLevel = old_verbosity;

    if( not doesTFileIsValid(outPtr) ){
      throw std::runtime_error("Invalid TFile: \"" + inputFilePath_ + "\"");
    }

    for( const auto& objectPath : objectListToCheck_ ){
      if( outPtr->Get(objectPath.c_str()) == nullptr ){
        throw std::runtime_error("Could not find: \"" + objectPath + "\" in \"" + inputFilePath_ + "\"");
      }
    }

    return outPtr;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_) {
    bool fileIsValid = false;
    gEnv->SetValue("TFile.Recover", 0);
    if(isFile(inputFilePath_)) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_) {

    if(tfileCandidatePtr_ == nullptr){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ is a nullptr" << std::endl;
      return false;
    }

    if(not tfileCandidatePtr_->IsOpen()){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName() << " is not opened."
                  << std::endl;
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsOpen() = " << tfileCandidatePtr_->IsOpen()
                  << std::endl;
      return false;
    }

    if( tfileCandidatePtr_->IsZombie() ){
      if(Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->IsZombie()) << std::endl;
      }
      return false;
    }

    if( tfileCandidatePtr_->TestBit(TFile::kRecovered) ){
      if(Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->TestBit(TFile::kRecovered)) << std::endl;
      }
      return false;
    }

    if(check_if_writable_ and not tfileCandidatePtr_->IsWritable()){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName()
                  << " is not writable." << std::endl;
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsWritable() = " << tfileCandidatePtr_->IsWritable()
                  << std::endl;
      return false;
    }

    return true;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TObject *> getListOfObjectFromTDirectory(TDirectory *directory_, const std::string &className_, bool cloneObj_) {
    return getObjectList<TObject>(directory_, cloneObj_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsTDirectory(TDirectory* directory_, const std::string& className_) {
    std::vector<std::string> output{};
    if( directory_ == nullptr ) return output;

    for (int iEntry = 0; iEntry < directory_->GetListOfKeys()->GetSize(); iEntry++) {
      std::string entryName{directory_->GetListOfKeys()->At(iEntry)->GetName()};
      if( className_.empty() or directory_->Get(entryName.c_str())->ClassName() == className_ ){
        output.emplace_back(entryName);
      }
    }

    return output;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<std::string> lsSubDirTDirectory(TDirectory* directory_) {
    return lsTDirectory(directory_, "TDirectoryFile");
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_) {
    if( baseDir_ == nullptr ) return nullptr;
    if(baseDir_->GetDirectory(dirName_.c_str()) == nullptr){
      baseDir_->mkdir(dirName_.c_str());
    }
    return baseDir_->GetDirectory(dirName_.c_str());
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_) {
    return mkdirTFile(outputFile_->GetDirectory(""), dirName_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TFile *> getListOfOpenedTFiles() {
    std::vector<TFile *> output;
    // TIter next_iter(gROOT->GetListOfGlobals());
    auto *global_obj_list = (TList *) ::ROOT::GetROOT()->GetListOfGlobals();
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TDirectory* getCurrentTDirectory() {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,28,04)
    return gDirectory.fValue->load();
#elif ROOT_VERSION_CODE >= ROOT_VERSION(6,23,02)
    return gDirectory.fValue.load();
#else
    // Prior releases of ROOT, gDirectory was returning a TDirectory*
    // Implementation has been made on the 16 Dec 2020:
    // https://github.com/root-project/root/commit/085e9c182b9f639d5921c75de284ae7f20168b6e
    return gDirectory;
#endif
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject* objToSave_, std::string saveName_, bool forceWriteFile_) {
    if( objToSave_ == nullptr ){ return; }

    // use TObject name
    if( saveName_.empty() ) saveName_ = objToSave_->GetName();

    // Building custom extension:
    std::string className = objToSave_->ClassName();
    replaceSubstringInsideInputString(className, "<", "_");
    replaceSubstringInsideInputString(className, ">", "");
    if( className == "TMatrixT_double" ) className = "TMatrixD";
    else if( className == "TMatrixTSym_double" ) className = "TMatrixDSym";

    // check if the ext isn't already included
    if( endsWith( saveName_, className ) ){
      writeInTFile(dir_, objToSave_, saveName_, forceWriteFile_);
    }
    else{
      // use the standard function
      writeInTFile(dir_, objToSave_, saveName_+"_"+className, forceWriteFile_);
    }


  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_, bool forceWriteFile_) {
    if( dir_ == nullptr or objToSave_ == nullptr ) return;

    // Object name:
    if( saveName_.empty() ) saveName_ = objToSave_->GetName();

    // create the appropriate TDirectory path
    auto subPath{GenericToolbox::splitString(saveName_, "/", true)};
    if( subPath.size() > 1 ){
      TDirectory* dir = dir_;

      for( size_t iFolder = 0 ; iFolder < subPath.size()-1 ; iFolder++ ){
        dir = GenericToolbox::mkdirTFile(dir, generateCleanBranchName(subPath[iFolder]));
      }

      saveName_ = subPath.back(); // only keep the last bit for the object name
      dir_ = dir; // new dir is the sub-dir
    }

    // Cleaning up object name
    saveName_ = generateCleanBranchName(saveName_);

    // write the object
    dir_->WriteObject(objToSave_, Form("%s", saveName_.c_str()), "overwrite");

    // Force TFile Write?
    if( forceWriteFile_ ){ triggerTFileWrite(dir_); }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const TObject& objToSave_, std::string saveName_, bool forceWriteFile_) {
    writeInTFileWithObjTypeExt(dir_, &objToSave_, std::move(saveName_), forceWriteFile_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFileWithObjTypeExt(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_) {
    writeInTFileWithObjTypeExt(dir_, TNamed(saveName_, objToSave_.c_str()), std::move(saveName_), forceWriteFile_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const TObject& objToSave_, std::string saveName_, bool forceWriteFile_) {
    writeInTFile(dir_, &objToSave_, std::move(saveName_), forceWriteFile_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void writeInTFile(TDirectory* dir_, const std::string& objToSave_, std::string saveName_, bool forceWriteFile_) {
    writeInTFile(dir_, TNamed(saveName_, objToSave_.c_str()), std::move(saveName_), forceWriteFile_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void triggerTFileWrite(TDirectory* dir_) {
    if( dir_ == nullptr ) return;
    if( dir_->GetFile() != nullptr ) dir_->GetFile()->Write();
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void disableUnhookedBranches(TTree* tree_) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TLeaf*> getEnabledLeavesList(TTree* tree_, bool includeArrayLeaves_) {
    std::vector<TLeaf*> leafList;

    leafList.reserve( tree_->GetListOfLeaves()->GetEntries() );
    for( int iBranch = 0 ; iBranch < tree_->GetListOfBranches()->GetEntries() ; iBranch++ ){

      auto* br{dynamic_cast<TBranch*>(tree_->GetListOfBranches()->At(iBranch))};
      if( br == nullptr or tree_->GetBranchStatus( br->GetName() ) != 1 ){ continue; }

      for( int iLeaf = 0 ; iLeaf < br->GetListOfLeaves()->GetEntries() ; iLeaf++ ){

        auto* lf{dynamic_cast<TLeaf*>( br->GetListOfLeaves()->At(iLeaf) )};
        if( lf == nullptr or lf->GetNdata() == 0 ){ continue; }

        if( includeArrayLeaves_ or lf->GetNdata() == 1 ){
          leafList.emplace_back( lf );
        }
        else{
          // DON'T SUPPORT ARRAYS AT THE MOMENT
          std::cout << __METHOD_NAME__
                    << ": " << tree_->GetListOfLeaves()->At(iLeaf)->GetName()
                    << " -> array leaves are not supported yet." << std::endl;
        }
      } // iLeaf
    } // iBranch

    return leafList;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TVectorD* generateMeanVectorOfTree(TTree* tree_, bool showProgressBar_) {
    TVectorD* outMeanVector;
    std::vector<TLeaf*> leafList = getEnabledLeavesList(tree_, false);

    outMeanVector = new TVectorD(int(leafList.size()));
    for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){ (*outMeanVector)[iLeaf] = 0; }

    Long64_t nEntries = tree_->GetEntries();
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) displayProgressBar(iEntry, nEntries, "Compute mean of every variable");
      tree_->GetEntry(iEntry);
      for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){
        (*outMeanVector)[iLeaf] += leafList[iLeaf]->GetValue(0);
      }
    }
    for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){
      (*outMeanVector)[iLeaf] /= double(nEntries);
    }

    return outMeanVector;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* generateCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_, TVectorD* meanValueLeafList_) {

    TMatrixD* outCovMatrix;

    // Generate covariance matrix of all ENABLED branches of the input tree
    std::vector<TLeaf*> leafList = getEnabledLeavesList(tree_, false);

    // Initializing the matrix
    outCovMatrix = new TMatrixD(int(leafList.size()), int(leafList.size()));
    for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
      for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
        (*outCovMatrix)[iCol][iRow] = 0;
      }
    }

    // Compute mean of every variable
    if( meanValueLeafList_ == nullptr ){
      meanValueLeafList_ = generateMeanVectorOfTree(tree_, showProgressBar_);
    }

    // Compute covariance
    Long64_t nEntries = tree_->GetEntries();
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) displayProgressBar(iEntry, nEntries, "Compute covariance");
      tree_->GetEntry(iEntry);
      for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
        for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
          (*outCovMatrix)[iCol][iRow] +=
              (leafList[iCol]->GetValue(0) - (*meanValueLeafList_)[iCol])
              *(leafList[iRow]->GetValue(0) - (*meanValueLeafList_)[iRow]);
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::string generateCleanBranchName(const std::string& name_) {
    return generateCleanFileName(name_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void generateCompareTree(TTree* tree1_, TTree* tree2_, TDirectory* outDir_) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::map<std::string, TMatrixD *> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_) {
    std::map<std::string, TMatrixD *> results_handler;

    auto content_names = splitString(outputContent_, ":");

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getEigenValues(TMatrixD *matrix_) {
    auto *symmetric_matrix = convertToSymmetricMatrix(matrix_);
    auto *Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto *Eigen_values = &(Eigen_matrix_decomposer->GetEigenValues());

    std::vector<double> output;
    for (int i_dim = 0; i_dim < matrix_->GetNcols(); i_dim++) {
      output.emplace_back((*Eigen_values)[i_dim]);
    }
    std::sort(output.begin(), output.end(), std::greater<double>());
    return output;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_) {
    if(covMatrix_ == nullptr) return nullptr;
    auto* covMatrixSym = convertToSymmetricMatrix(covMatrix_);
    auto* out = getCholeskyMatrix(covMatrixSym);
    delete covMatrixSym;
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_) {
    if(covMatrix_ == nullptr) return nullptr;
    auto* choleskyDecomposer = new TDecompChol((*covMatrix_));
    if( not choleskyDecomposer->Decompose() ){ return nullptr; }
    auto* output = (TMatrixD *)(((TMatrixD *)(choleskyDecomposer->GetU()).Clone())->T()).Clone();
    delete choleskyDecomposer;
    return output;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_) {
    std::vector<double> out;
    throwCorrelatedParameters(choleskyCovMatrix_, out);
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ ) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE void drawHistHorizontalBars(TH1D* hist_) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE void resetHistogram(TH1D* hist_) {
    hist_->Reset("ICESM");
    transformBinContent(hist_, [](TH1D* h_, int iBin_){
      h_->SetBinContent(iBin_, 0);
      h_->SetBinError(iBin_, 0);
    }, true);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void rescalePerBinWidth(TH1D* hist_, double globalScaler_) {
    transformBinContent(hist_, [&](TH1D* h_, int iBin_){
      h_->SetBinContent( iBin_,globalScaler_ * h_->GetBinContent(iBin_)/hist_->GetBinWidth(iBin_) );
    }, true);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void transformBinContent(TH1D* hist_, const std::function<void(TH1D*, int)>& transformFunction_, bool processOverflowBins_) {
    int firstBin = processOverflowBins_ ? 0 : 1;
    int lastBin = processOverflowBins_ ? hist_->GetNbinsX() + 1 : hist_->GetNbinsX();
    for( int iBin = firstBin ; iBin <= lastBin ; iBin++ ){
      transformFunction_(hist_, iBin);
    }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE Range fetchYRange(TH1* h_, bool withError_, const Range& caps_) {
    Range out{std::nan(""), std::nan("")};
    if( h_ == nullptr ) return out;
    for( int iBin = 1 ; iBin <= h_->GetNbinsX() ; iBin++ ){
      double minValCandidate = h_->GetBinContent(iBin);
      double maxValCandidate = h_->GetBinContent(iBin);

      if( withError_ ){
        minValCandidate -= h_->GetBinError(iBin);
        maxValCandidate += h_->GetBinError(iBin);
      }

      if( std::isnan(caps_.min) or minValCandidate > caps_.min ){
        out.min = std::min(minValCandidate, out.min); // std::nan on right
      }
      if( std::isnan(caps_.max) or maxValCandidate < caps_.max ){
        out.max = std::max(maxValCandidate, out.max);
      }
    }
    // try to avoid returning nan:
    if( std::isnan(out.min) ) out.min = caps_.min;
    if( std::isnan(out.max) ) out.max = caps_.max;
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(TH1* h_, const Range& margins_) {
    Range out{std::nan("unset"), std::nan("unset")};
    if( h_ == nullptr ) return out;
    for( int iBin = 1 ; iBin <= h_->GetNbinsX() ; iBin++ ){
      double minValCandidate = h_->GetBinContent(iBin) - h_->GetBinError(iBin);
      double maxValCandidate = h_->GetBinContent(iBin) + h_->GetBinError(iBin);
      out.min = std::min(minValCandidate, out.min); // std::nan on right
      out.max = std::max(maxValCandidate, out.max);
    }
    double yRange = (out.max-out.min);
    out.min -= margins_.min * yRange;
    out.max += margins_.max * yRange;
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE Range getYBounds(const std::vector<TH1*>& h_, const Range& margins_) {
    Range out{std::nan("unset"), std::nan("unset")};
    for( auto& hist : h_ ){
      if( hist == nullptr ) continue;
      auto bounds = getYBounds(hist, margins_);
      if( out.min != out.min ) out.min = bounds.min;
      if( out.max != out.max ) out.max = bounds.max;
      out.min = std::min(out.min, bounds.min);
      out.max = std::max(out.max, bounds.max);
    }
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_) {
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_) {
    std::vector<double> output(n_bins_ + 1); // add one extra bin for the boundary
    double dx = (X_max_ - X_min_) / ((double) n_bins_);
    for (int i_bin = 0; i_bin <= n_bins_; i_bin++) {
      double x = X_min_ + i_bin * dx;
      output[i_bin] = x;
    }
    return output;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH1D *getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_) {

    TH1D *output = nullptr;
    std::vector<double> xbins = getLogBinning(n_bins_, X_min_, X_max_);
    output = new TH1D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0]);
    return output;

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE TH2D *getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_) {

    TH2D *output = nullptr;
    std::vector<double> xbins;
    std::vector<double> ybins;
    if( hasSubStr(log_axis_, "X") ){
      xbins = getLogBinning(nb_X_bins_, X_min_, X_max_);
    } else {
      xbins = getLinearBinning(nb_X_bins_, X_min_, X_max_);
    }
    if( hasSubStr(log_axis_, "Y") ){
      ybins = getLogBinning(nb_Y_bins_, Y_min_, Y_max_);
    } else {
      ybins = getLinearBinning(nb_Y_bins_, Y_min_, Y_max_);
    }

    output = new TH2D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0], ybins.size() - 1, &ybins[0]);
    return output;

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE std::vector<TH2D*> chopTH2D(TH2D* h, int iBranchSeparation) {
    std::vector<TH2D*> out;
    if(!h) return out;

    int n = h->GetNbinsX();
    if(n != h->GetNbinsY()) return out;
    if(iBranchSeparation <= 0 || iBranchSeparation >= n) return out;

    int n1 = iBranchSeparation;     // size of first block
    int n2 = n - iBranchSeparation; // size of second block

    // Create the two submatrices
    TH2D* h0 = new TH2D(
        TString::Format("%s_block0", h->GetName()),
        TString::Format("%s block 0", h->GetTitle()),
        n1, 0, n1,
        n1, 0, n1);

    TH2D* h1 = new TH2D(
        TString::Format("%s_block1", h->GetName()),
        TString::Format("%s block 1", h->GetTitle()),
        n2, 0, n2,
        n2, 0, n2);

    // Copy labels for block0
    for(int i = 1; i <= n1; i++){
      const char* labX = h->GetXaxis()->GetBinLabel(i);
      const char* labY = h->GetYaxis()->GetBinLabel(i);
      if(labX && strlen(labX)) h0->GetXaxis()->SetBinLabel(i, labX);
      if(labY && strlen(labY)) h0->GetYaxis()->SetBinLabel(i, labY);
    }

    // Copy labels for block1
    for(int i = 1; i <= n2; i++){
      const char* labX = h->GetXaxis()->GetBinLabel(iBranchSeparation + i);
      const char* labY = h->GetYaxis()->GetBinLabel(iBranchSeparation + i);
      if(labX && strlen(labX)) h1->GetXaxis()->SetBinLabel(i, labX);
      if(labY && strlen(labY)) h1->GetYaxis()->SetBinLabel(i, labY);
    }

    // Copy content into block0 (top-left)
    for(int ix = 1; ix <= n1; ix++){
      for(int iy = 1; iy <= n1; iy++){
        h0->SetBinContent(ix, iy, h->GetBinContent(ix, iy));
      }
    }

    // Copy content into block1 (bottom-right)
    for(int ix = 1; ix <= n2; ix++){
      for(int iy = 1; iy <= n2; iy++){
        int gx = iBranchSeparation + ix;
        int gy = iBranchSeparation + iy;
        h1->SetBinContent(ix, iy, h->GetBinContent(gx, gy));
      }
    }

    out.push_back(h0);
    out.push_back(h1);
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TGraph* graph_) {
    if( graph_->GetN() < 1 ){ return true; }
    return not std::any_of(&(graph_->GetY()[0]), &(graph_->GetY()[graph_->GetN()]), [](Double_t val_){ return val_ != 1.; });
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool isFlatAndOne(const TSpline3* spline_) {
    if( spline_->GetNp() < 1 ){ return true; }
    bool isFlatOne{true};
    for( int iKnot = 0 ; iKnot < int(spline_->GetNp()) ; iKnot++ ){
      double xBuff{}, yBuff{}; spline_->GetKnot(iKnot, xBuff, yBuff);
      if( yBuff != 1. ){
        isFlatOne = false;
        break;
      }
    }
    return isFlatOne;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE bool hasUniformlySpacedKnots(const TGraph* graph_, double tolerance_) {
    // Check if the spline has uniformly spaced knots.  There is a flag for
    // this is TSpline3, but it's not uniformly (or ever) filled correctly.
    bool uniform{true};
    for (int i = 1; i < graph_->GetN()-1; ++i) {
      double d1 = graph_->GetX()[i-1];
      d1 = graph_->GetX()[i] - d1;
      double d2 = graph_->GetX()[i];
      d2 = graph_->GetX()[i+1] - d2;
      if (std::abs((d1-d2)/(d1+d2)) > tolerance_) {
        uniform = false;
        break;
      }
    }
    return uniform;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void setDefaultPalette() {
    gStyle->SetPalette(kBird);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void setBlueRedPalette() {
    gStyle->SetPalette(kBlackBody);
    TColor::InvertPalette();
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void setT2kPalette() {
    int NRGBs = 3;
    int NCont = 255;

    std::vector<double> stops{0.00, 0.50, 1.000};
    std::vector<double> red{0.00, 1.00, 1.00};
    std::vector<double> green{0.00, 1.00, 0.00};
    std::vector<double> blue{1.00, 1.00, 0.00};

    TColor::CreateGradientColorTable(NRGBs,&stops[0],&red[0],&green[0],&blue[0],NCont);
    gStyle->SetNumberContours(NCont+1);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void setOrangePalette() {
    gStyle->SetPalette(kDarkBodyRadiator);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void fixTH2display(TH2 *histogram_) {
    if( histogram_ == nullptr ) return;

    if( gPad != nullptr ) gPad->SetRightMargin(0.15);
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

CPP_GENERIC_TOOLBOX_ROOT_INLINE void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_) {

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

CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TCanvas* canvas_) {

    if( canvas_ == nullptr ){ return; }

    int nPrimitives( canvas_->GetListOfPrimitives()->GetEntries() );
    for( int iPrim = 0 ; iPrim < nPrimitives ; iPrim++ ){
      if( not canvas_->GetListOfPrimitives()->At( iPrim )->InheritsFrom("TH1") ){ continue; }
      cleanupForDisplay( (TH1*) canvas_->GetListOfPrimitives()->At( iPrim ) );
    }

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void cleanupForDisplay(TH1* hist_) {

    if( hist_ == nullptr ){ return; }
    int nBins( hist_->GetNcells() );
    for( int iBin = 0 ; iBin < nBins ; iBin++ ){
      if( std::isnan( hist_->GetBinContent(iBin) ) ){ hist_->SetBinContent(iBin, 0); }
      if( std::isnan( hist_->GetBinError(iBin) ) ){ hist_->SetBinError(iBin, 0); }
    }

  }

#ifdef CPP_GENERIC_TOOLBOX_ROOT_COMPILED
Int_t oldVerbosity = -1;
#endif

CPP_GENERIC_TOOLBOX_ROOT_INLINE void muteRoot() {
    oldVerbosity      = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void unmuteRoot() {
    gErrorIgnoreLevel = oldVerbosity;
    oldVerbosity      = -1;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE char findOriginalVariableType(const AnyType& obj_) {
    if( obj_.getType() == typeid(Bool_t) ){ return 'O'; }
    if( obj_.getType() == typeid(Char_t) ){ return 'B'; }
    if( obj_.getType() == typeid(UChar_t) ){ return 'b'; }
    if( obj_.getType() == typeid(Short_t) ){ return 'S'; }
    if( obj_.getType() == typeid(UShort_t) ){ return 's'; }
    if( obj_.getType() == typeid(Int_t) ){ return 'I'; }
    if( obj_.getType() == typeid(UInt_t) ){ return 'i'; }
    if( obj_.getType() == typeid(Float_t) ){ return 'F'; }    // `F` : a 32 bit floating point (`Float_t`)
    if( obj_.getType() == typeid(Float16_t) ){ return 'f'; }  // `f` : a 24 bit floating point with truncated mantissa
    if( obj_.getType() == typeid(Double_t) ){ return 'D'; }   // `D` : a 64 bit floating point (`Double_t`)
    if( obj_.getType() == typeid(Double32_t) ){ return 'd'; } // `d` : a 24 bit truncated floating point (`Double32_t`)
    if( obj_.getType() == typeid(Long64_t) ){ return 'L'; }
    if( obj_.getType() == typeid(ULong64_t) ){ return 'l'; }
    if( obj_.getType() == typeid(Long_t) ){ return 'G'; } // `G` : a long signed integer, stored as 64 bit (`Long_t`)
    if( obj_.getType() == typeid(ULong_t) ){ return 'g'; } // `g` : a long unsigned integer, stored as 64 bit (`ULong_t`)
    return char(0xFF); // OTHER??
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const std::string& leafTypeName_) {
    AnyType out;
    leafToAnyType(leafTypeName_, out);
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE AnyType leafToAnyType(const TLeaf* leaf_) {
    AnyType out;
    leafToAnyType(leaf_, out);
    return out;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const TLeaf* leaf_, AnyType& out_) {
    if( leaf_ == nullptr ){ throw std::runtime_error("leaf_ is nullptr"); }
    leafToAnyType(leaf_->GetTypeName(), out_);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void leafToAnyType(const std::string& leafTypeName_, AnyType& out_) {
    if( leafTypeName_.empty() ){ throw std::runtime_error("empty leafTypeName_ provided."); }

      // Int like variables
    else if( leafTypeName_ == "Bool_t" )      { out_ = Bool_t(); }
    else if( leafTypeName_ == "Char_t" )      { out_ = Char_t(); }
    else if( leafTypeName_ == "UChar_t" )     { out_ = UChar_t(); }
    else if( leafTypeName_ == "Short_t" )     { out_ = Short_t(); }
    else if( leafTypeName_ == "UShort_t" )    { out_ = UShort_t(); }
    else if( leafTypeName_ == "Int_t" )       { out_ = Int_t(); }
    else if( leafTypeName_ == "UInt_t" )      { out_ = UInt_t(); }
    else if( leafTypeName_ == "Long_t" )      { out_ = Long_t(); }
    else if( leafTypeName_ == "ULong_t" )     { out_ = ULong_t(); }
    else if( leafTypeName_ == "Long64_t" )    { out_ = Long64_t(); }
    else if( leafTypeName_ == "ULong64_t" )   { out_ = ULong64_t(); }

      // Floating Variables
    else if( leafTypeName_ == "Float16_t" )   { out_ = Float16_t(); }
    else if( leafTypeName_ == "Float_t" )     { out_ = Float_t(); }
    else if( leafTypeName_ == "Double32_t" )  { out_ = Double32_t(); }
    else if( leafTypeName_ == "Double_t" )    { out_ = Double_t(); }

      // TObjects (can't be loaded as objects)
    else if( leafTypeName_ == "TGraph" )      { out_ = (TGraph*)(nullptr); }
    else if( leafTypeName_ == "TSpline3" )    { out_ = (TSpline3*)(nullptr); }
    else if( leafTypeName_ == "TClonesArray" ){ out_ = (TClonesArray*)(nullptr); }

      // Others
    else{
      std::cout << "leafToAnyType: WARNING: leafType = \"" << leafTypeName_ << "\" not set. Assuming ptr object..." << std::endl;
      out_ = (void*)(nullptr);
    }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void CorrelatedVariablesSampler::initializeImpl() {
    if( _covarianceMatrixPtr_ == nullptr ){
      throw std::runtime_error("Can't init while _covarianceMatrixPtr_ is not set");
    }

    // https://root.cern.ch/doc/master/classTDecompChol.html
    _choleskyDecomposer_ = std::make_shared<TDecompChol>(*_covarianceMatrixPtr_ );
    if( not _choleskyDecomposer_->Decompose() ){
      TVectorD eigenVal;
      _covarianceMatrixPtr_->EigenVectors(eigenVal);
      for( int iVar = 0 ; iVar < eigenVal.GetNrows() ; ++iVar ) {
        if( eigenVal[iVar] < 0 ) {
          eigenVal.Print();
          GTLogThrow("Can't decompose covariance matrix.");
        }
      }
    }

    // Decompose a symmetric, positive definite matrix: A = U^T * U
    _sqrtMatrix_ = std::make_shared<TMatrixD>(_choleskyDecomposer_->GetU());

    _sqrtMatrix_->T(); // Transpose it to get the left side one

    _rangeList_.resize(_sqrtMatrix_->GetNrows(), {std::nan("-inf"),std::nan("+inf")});
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void CorrelatedVariablesSampler::extractBlocks() {
    GTLogThrowIf(_covarianceMatrixPtr_ == nullptr, "_covarianceMatrixPtr_ is not set");
    blockIndexLists.clear();

    // find the blocks
    for( int iDiag = 0 ; iDiag < _covarianceMatrixPtr_->GetNrows() ; iDiag++ ){
      bool alreadyProcessed = false;
      for( auto& indices : blockIndexLists ) {
        if( GenericToolbox::isIn(iDiag, indices) ){ alreadyProcessed = true; break; }
      }
      if( alreadyProcessed ){ continue; }
      blockIndexLists.emplace_back();
      blockIndexLists.back().emplace_back(iDiag);
      for( int jDiag = iDiag+1 ; jDiag < _covarianceMatrixPtr_->GetNrows() ; jDiag++ ) {
        if((*_covarianceMatrixPtr_)[iDiag][jDiag] != 0) {
          blockIndexLists.back().emplace_back(jDiag);
        }
      }
    }

    // don't go any further!
    if( blockIndexLists.size() == 1 ){ return; }

    GTLogInfo << "Found " << blockIndexLists.size() << " matrix sub-block in "
    << _covarianceMatrixPtr_->GetNrows() << "x" << _covarianceMatrixPtr_->GetNrows() << " cov matrix." << std::endl;

    // sanity check
    int sum{0}; for(auto& block: blockIndexLists){ sum += block.size(); }
    if( _covarianceMatrixPtr_->GetNrows() != sum ){
      GTDebugVar(_covarianceMatrixPtr_->GetNrows());
      GTDebugVar(sum);
      GTLogError << "Blocks are: " << std::endl;
      for(auto& block: blockIndexLists){ GTDebugVar(GenericToolbox::toString(block)); }
      GTLogThrow("Something is wrong. DEV should check.");
    }


    for( auto& blockIndexList : blockIndexLists ){
      auto* blockMatrix = new TMatrixDSym(blockIndexList.size());
      for( int iRow = 0 ; iRow < blockIndexList.size() ; iRow++ ){
        for( int iCol = iRow ; iCol < blockIndexList.size() ; iCol++ ) {
          (*blockMatrix)[iRow][iCol] = (*_covarianceMatrixPtr_)[blockIndexList[iRow]][blockIndexList[iCol]];
          (*blockMatrix)[iCol][iRow] = (*blockMatrix)[iRow][iCol];
        }
      }
      diagonalPerBlock.emplace_back();
      diagonalPerBlock.back().setCovarianceMatrixPtr(blockMatrix);
      diagonalPerBlock.back().setNbMaxTries(nMaxTries);
      diagonalPerBlock.back().initialize();

      for( int iRow = 0 ; iRow < blockIndexList.size() ; iRow++ ) {
        diagonalPerBlock.back().getParLimitList()[iRow] = _rangeList_[blockIndexList[iRow]];
      }
    }

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void CorrelatedVariablesSampler::throwCorrelatedVariables(TVectorD& output_) {

    if( not diagonalPerBlock.empty() ){
      for( size_t iBlock = 0 ; iBlock < diagonalPerBlock.size() ; iBlock++ ) {
        TVectorD blockOutput( blockIndexLists[iBlock].size());
        try {
          diagonalPerBlock[iBlock].throwCorrelatedVariables(blockOutput);
        }
        catch( ... ){
          GTLogError << "Could not throw parameters of block " << iBlock << std::endl;
          GTLogError << "par indices: " << toString(blockIndexLists[iBlock]) << std::endl;
          GTLogError << "par bounds: " << toString(diagonalPerBlock[iBlock].getParLimitList(), false) << std::endl;
          diagonalPerBlock[iBlock].getCovarianceMatrixPtr()->Print();
          GTLogThrow("Tried more than " << nMaxTries << " times.");
        }

        int iIdx{0};
        for( auto& blockIndexList : blockIndexLists[iBlock] ){
          output_[blockIndexList] = blockOutput[iIdx++];
        }
      }
      return;
    }

    // should be a full block!

    // https://math.stackexchange.com/questions/446093/generate-correlated-normal-random-variables
    this->throwIfNotInitialized(__METHOD_NAME__);
    if(output_.GetNrows() != _covarianceMatrixPtr_->GetNrows()){
      std::stringstream ss;
      ss << __METHOD_NAME__ << ": ";
      ss << "Provided output TVector does not have the same dimension as the cov matrix";
      ss << " -> cov=" << _covarianceMatrixPtr_->GetNrows() << " != vector=" << output_.GetNrows();
      throw std::runtime_error( ss.str() );
    }

    bool isThrowSuccessful{false};
    size_t nThrows{0};
    do {
      nThrows++;
      GTLogThrowIf( nThrows > nMaxTries, "Too many throws" );

      for( int iVar = 0 ; iVar < output_.GetNrows() ; iVar++ ){
        output_[iVar] = _prng_->Gaus(0, 1);
      }

      output_ *= (*_sqrtMatrix_);

      isThrowSuccessful = true;
      for( int iVar = 0 ; iVar < output_.GetNrows() ; iVar++ ){
        if( _rangeList_[iVar].isUnbounded() ){ continue; }
        if( not _rangeList_[iVar].isInBounds(output_[iVar]) ){
          isThrowSuccessful = false;
          break;
        }
      }
    }
    while( not isThrowSuccessful );

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE size_t TreeBuffer::getBranchByteOffset(const std::string& branchName_) {
    for( auto& bb : _branchInfoList_ ){
      if( bb.name == branchName_ ){ return bb.byteOffset; }
    }

    // otherwise create it
    expandBuffer(branchName_);
    return _branchInfoList_.back().byteOffset;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::expandBuffer(const std::string& branchName_) {
    auto* b = _treePtr_->GetBranch(branchName_.c_str());
    if( b == nullptr ){ throw std::runtime_error(branchName_ + " not found"); }

    // Calculating the requested buffer size
    size_t bufferSize{0};
    auto* leavesList = b->GetListOfLeaves();
    int nLeaves = leavesList->GetEntries();
    for( int iLeaf = 0 ; iLeaf < nLeaves ; iLeaf++ ){
      auto* l = (TLeaf*) leavesList->At(iLeaf);
      if( l->GetNdata() != 0 ){
        // primary type leaf (int, double, long, etc...)
        bufferSize += l->GetNdata() * l->GetLenType();
      }
      else{
        // pointer-like obj (TGraph, TClonesArray...)
        bufferSize += 2 * l->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      }
    }

    if( bufferSize == 0 ){ throw std::runtime_error("buffer size is 0"); }

    _branchInfoList_.emplace_back();
    _branchInfoList_.back().name = branchName_;
    _branchInfoList_.back().byteOffset = _chainBuffer_.size();

    _chainBuffer_.resize(_chainBuffer_.size() + bufferSize, 0);
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE int TreeBuffer::addExpression(const std::string& exprStr_) {
    if( exprStr_.empty() ){ throw std::invalid_argument("expression is empty"); }
    if( _treePtr_ == nullptr ){ throw std::runtime_error("chain is null"); }

    // check if it already exists
    auto idx{getExpressionBufferIndex(exprStr_)};
    if( idx != -1 ){ return idx; }

    // need to parse the expression
    std::vector<std::string> argBuffer;
    auto strippedLeafExpr = GenericToolbox::stripBracket(exprStr_, '[', ']', false, &argBuffer);
    if( strippedLeafExpr.empty() ){ throw std::runtime_error(__METHOD_NAME__ + " Bad leaf form expression: " + exprStr_); }

    // first, check if the remaining expr is a leaf
    auto* leafPtr = _treePtr_->GetLeaf(strippedLeafExpr.c_str());

    // we support 1 dim array at maximum
    if( leafPtr != nullptr and argBuffer.size() <= 1 ){

      // add a slot for ROOT to drop the data
      auto offset = getBranchByteOffset( leafPtr->GetBranch()->GetName() );
      auto anySlot = GenericToolbox::leafToAnyType(leafPtr->GetTypeName());

      if( argBuffer.empty() ) {
        // not an array
        _expressionBufferList_.emplace_back(std::make_unique<ExpressionBuffer>());
      }
      else{
        // array-like
        try{
          // try for a static index
          int index = std::stoi(argBuffer[0]);
          auto exp = std::make_unique<ExpressionBuffer>();
          // translating the array index
          offset += index * leafPtr->GetLenType();
          _expressionBufferList_.emplace_back(std::move(exp));
        }
        catch(...){
          // add the arg as a whole new expression /!\ recursive
          addExpression(argBuffer[0]);
          auto exp = std::make_unique<NestedArrayExpression>();
          exp->setObjSize( anySlot.getPlaceHolderPtr()->getVariableSize() );
          exp->setExpPtrArrayIndex( const_cast<ExpressionBuffer *>(getExpressionBuffer(argBuffer[0])) );
          _expressionBufferList_.emplace_back(std::move(exp));
        }
      }

      _expressionBufferList_.back()->setByteOffset(offset);

      // creating the typed buffer
      _expressionBufferList_.back()->setBuffer(anySlot);

      // keep a copy of the original expression
      _expressionBufferList_.back()->setExpression(exprStr_);
    }
    else{
      // directly use a formula
      auto exp = std::make_unique<FormulaExpression>();
      exp->setExpression(exprStr_);
      exp->setExpression(exprStr_);
      exp->setBuffer( leafToAnyType("Double_t") );
      exp->parseFormula(_treePtr_);
      _expressionBufferList_.emplace_back(std::move(exp));
    }

    return static_cast<int>(_expressionBufferList_.size())-1;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::saveExpressions() {
    for( auto& eb : _expressionBufferList_ ){
      eb->save(_chainBuffer_.data());
    }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::initialize() {
    if(_treePtr_ == nullptr){ throw std::runtime_error("chain not initialized"); }

    // notification
    _notifier_.setOnNotifyFct([&]{ this->notify(); });
    _treePtr_->SetNotify( &_notifier_ );
    this->notify();
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE const TreeBuffer::ExpressionBuffer* TreeBuffer::getExpressionBuffer(const std::string& exprStr_) const {
    auto idx{getExpressionBufferIndex(exprStr_)};
    if( idx == -1 ){ return nullptr; }
    return _expressionBufferList_[idx].get();
  }

[[nodiscard]] CPP_GENERIC_TOOLBOX_ROOT_INLINE int TreeBuffer::getExpressionBufferIndex(const std::string& exprStr_) const {
    int idx{0};
    for( auto& exp : _expressionBufferList_){
      if( exp->getExpression() == exprStr_ ){ return idx; }
      idx++;
    }
    return -1;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::notify() {
    // hook the branch addresses to the new TTree

    // reset all branch status to 0
    _treePtr_->SetBranchStatus("*", false);

    for(auto& br: _branchInfoList_) {
      // it should exist
      auto* branchPtr = _treePtr_->GetBranch(br.name.c_str());
      if( branchPtr == nullptr ){ throw std::logic_error(br.name + "Branch name doesn't match"); }

      _treePtr_->SetBranchStatus(br.name.c_str(), true);

      // set the address where ROOT will write the data
      branchPtr->SetAddress( &_chainBuffer_[br.byteOffset] );
    }

    for( auto& exp : _expressionBufferList_ ){
      exp->notify(_treePtr_);
    }

  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::ExpressionBuffer::save(const uint8_t* src_) {
    std::memcpy(
      _buffer_.getPlaceHolderPtr()->getVariableAddress(),
      getBufferAddress(src_),
      _buffer_.getPlaceHolderPtr()->getVariableSize()
      );
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE const uint8_t* TreeBuffer::NestedArrayExpression::getBufferAddress(const uint8_t* src_) const {
    // make sure the value is up-to-date (treat TFormula first for instance)
    expPtrArrayIndex->save(src_);
    return this->ExpressionBuffer::getBufferAddress(src_) + expPtrArrayIndex->getBuffer().getValueAsLong() * objSize;
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::FormulaExpression::parseFormula(TTree* tree_) {
    if( tree_ == nullptr ){ throw std::runtime_error("tree_ is null"); }
    _formulaPtr_ = std::make_unique<TTreeFormula>(
      _expression_.c_str(), _expression_.c_str(), tree_
    );

    if( _formulaPtr_ == nullptr ){ throw std::runtime_error("couldn't create the formula"); }

    // ROOT Hot fix: https://root-forum.cern.ch/t/ttreeformula-evalinstance-return-0-0/16366/10
    _formulaPtr_->GetNdata();

    if( _formulaPtr_->GetNdim() == 0 ){
      throw std::runtime_error(__METHOD_NAME__+": \"" + _expression_ + "\" could not be parsed by the TTree");
    }
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::FormulaExpression::notify(TTree* tree_) {
    _formulaPtr_->Notify();
    GenericToolbox::enableSelectedBranches(tree_, _formulaPtr_.get());
  }

CPP_GENERIC_TOOLBOX_ROOT_INLINE void TreeBuffer::FormulaExpression::save(const uint8_t* src_) {
    // src_ will be ignored since there is no buffer in that case
    double buffer = _formulaPtr_->EvalInstance(0);
    std::memcpy(
      _buffer_.getPlaceHolderPtr()->getVariableAddress(),
      &buffer,
      _buffer_.getPlaceHolderPtr()->getVariableSize()
      );
  }
}
