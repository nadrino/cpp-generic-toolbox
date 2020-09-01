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


//! Matrices Tools
namespace GenericToolbox{

  TMatrixDSym* convert_to_symmetric_matrix(TMatrixD* matrix_) {

    auto* symmetric_matrix            = (TMatrixD*)matrix_->Clone();
    auto* transposed_symmetric_matrix = new TMatrixD(*matrix_);

    transposed_symmetric_matrix->Transpose(*matrix_);
    *symmetric_matrix += *transposed_symmetric_matrix;
    for(int i_col = 0; i_col < matrix_->GetNcols(); i_col++) {
      for(int i_row = 0; i_row < matrix_->GetNrows(); i_row++) {
        (*symmetric_matrix)[i_row][i_col] /= 2.;
      }
    }

    auto* result = (TMatrixDSym*)symmetric_matrix->Clone(); // Convert to TMatrixDSym

    delete transposed_symmetric_matrix;
    delete symmetric_matrix;

    return result;
  }
  std::map<std::string, TMatrixD*> SVD_matrix_inversion(TMatrixD* matrix_, std::string output_content_)
  {
    std::map<std::string, TMatrixD*> results_handler;

    auto content_names = GenericToolbox::splitString(output_content_, ":");

    if(GenericToolbox::doesElementIsInVector("inverse_covariance_matrix", content_names)){
      results_handler["inverse_covariance_matrix"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if(GenericToolbox::doesElementIsInVector("regularized_covariance_matrix", content_names)){
      results_handler["regularized_covariance_matrix"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if(GenericToolbox::doesElementIsInVector("projector", content_names)){
      results_handler["projector"]
        = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if(GenericToolbox::doesElementIsInVector("regularized_eigen_values", content_names)){
      results_handler["regularized_eigen_values"]
        = new TMatrixD(matrix_->GetNrows(), 1);
    }

    // make sure all are 0
    for(const auto& matrix_handler : results_handler){
      for(int i_dof = 0; i_dof < matrix_handler.second->GetNrows(); i_dof++){
        for(int j_dof = 0; j_dof < matrix_handler.second->GetNcols(); j_dof++){
          (*matrix_handler.second)[i_dof][j_dof] = 0.;
        }
      }
    }


    // Covariance matrices are symetric :
    auto* symmetric_matrix        = convert_to_symmetric_matrix(matrix_);
    auto* Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto* Eigen_values            = &(Eigen_matrix_decomposer->GetEigenValues());
    auto* Eigen_vectors           = &(Eigen_matrix_decomposer->GetEigenVectors());

    double max_eigen_value = (*Eigen_values)[0];
    for(int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++){
      if(max_eigen_value < (*Eigen_values)[i_eigen_value]){
        max_eigen_value = (*Eigen_values)[i_eigen_value];
      }
    }

    for(int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++)
    {
      if((*Eigen_values)[i_eigen_value] > max_eigen_value*1E-5)
      {
        if(results_handler.find("regularized_eigen_values") != results_handler.end()){
          (*results_handler["regularized_eigen_values"])[i_eigen_value][0]
            = (*Eigen_values)[i_eigen_value];
        }
        for(int i_dof = 0; i_dof < matrix_->GetNrows(); i_dof++){
          for(int j_dof = 0; j_dof < matrix_->GetNrows(); j_dof++){
            if(results_handler.find("inverse_covariance_matrix") != results_handler.end()){
              (*results_handler["inverse_covariance_matrix"])[i_dof][j_dof]
                += (1. / (*Eigen_values)[i_eigen_value])
                   * (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if(results_handler.find("projector") != results_handler.end()){
              (*results_handler["projector"])[i_dof][j_dof]
                += (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if(results_handler.find("regularized_covariance_matrix") != results_handler.end()){
              (*results_handler["regularized_covariance_matrix"])[i_dof][j_dof]
                += (*Eigen_values)[i_eigen_value]
                   * (*Eigen_vectors)[i_dof][i_eigen_value]
                   * (*Eigen_vectors)[j_dof][i_eigen_value];
            }

          }
        }
      }
      else{
//            std::cout << ALERT << "Skipping i_eigen_value = " << (*Eigen_values)[i_eigen_value]
//                      << std::endl;
      }
    }

    // No memory leak ? : CHECKED
    delete Eigen_matrix_decomposer;
    delete symmetric_matrix;

    return results_handler;
  }

  TH1D* get_TH1D_from_TVectorD(std::string graph_title_, TVectorD* Y_values_, std::string Y_title_,
                               std::string X_title_, TVectorD* Y_errors_)
  {

    auto* th1_histogram = new TH1D(graph_title_.c_str(), graph_title_.c_str(),
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
  TH2D* get_TH2D_from_TMatrixD(TMatrixD* XY_values_, std::string graph_title_, std::string Z_title_,
                               std::string Y_title_, std::string X_title_)
  {

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

  TVectorD *get_TVectorD_from_vector(std::vector<double>& vect_){

    auto *output = new TVectorD(vect_.size());
    for(int i = 0 ; i < int(vect_.size()) ; i++){
      (*output)[i] = vect_[i];
    }
    return output;

  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOXROOTEXT_IMPL_H
