//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H

#include <chrono>
#include <sstream>
#include <string>
#include <cmath>

namespace GenericToolbox{

  TablePrinter::TablePrinter() = default;
  TablePrinter::~TablePrinter() = default;

  void TablePrinter::reset(){
    _colTitleList_.clear();
    _tableContent_.clear();
    _colMaxWidthList_.clear();
  }

  void TablePrinter::fillTable(const std::vector<std::vector<std::string>> &tableLines_){
    this->reset();
    if( tableLines_.empty() ) return;
    for( auto& colTitle : tableLines_[0] ){
      this->addCol(colTitle);
    }
    for( size_t iLine = 1 ; iLine < tableLines_.size() ; iLine++ ){
      this->addRow(tableLines_[iLine]);
    }
  }

  size_t TablePrinter::addCol(const std::string& colTitle_){
    _colTitleList_.emplace_back(colTitle_);
    _tableContent_.emplace_back();
    _colMaxWidthList_.emplace_back(-1);
    return _colTitleList_.size()-1;
  }
  size_t TablePrinter::addRow(const std::vector<std::string>& rowValues_){
    size_t rowIndex{0};
    size_t colIndex{0};
    for( auto& colTable : _tableContent_ ){
      colTable.emplace_back();
      if( not rowValues_.empty() ) colTable.back() = rowValues_[colIndex++];
      rowIndex = colTable.size()-1;
    }
    return rowIndex;
  }

  void TablePrinter::setTableContent(size_t colIndex_, size_t rowIndex_, const std::string& value_){
    if( colIndex_ >= _tableContent_.size() ) throw std::runtime_error("invalid col index");
    if( rowIndex_ >= _tableContent_[colIndex_].size() ) throw std::runtime_error("invalid row index");
    _tableContent_[colIndex_][rowIndex_] = value_;
  }
  int TablePrinter::getNbRows(){
    return int(_colTitleList_.size());
  }

  std::string TablePrinter::generateTableString(){
    std::stringstream ss;

    std::vector<size_t> paveColList(_tableContent_.size(),0);
    size_t totalLength{1}; // first line separator
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      paveColList[iCol] = GenericToolbox::getPrintSize(_colTitleList_[iCol]);
      for( int iRow = 0 ; iRow < int(_tableContent_[iCol].size()) ; iRow++ ){
        if(GenericToolbox::getPrintSize(_tableContent_[iCol][iRow]) > paveColList[iCol]) {
          paveColList[iCol] = GenericToolbox::getPrintSize(_tableContent_[iCol][iRow]);
        }
      }
      totalLength += paveColList[iCol];
      totalLength += 2; // space before + after
      totalLength += 1; // line separator
    }

    // ┌───────────┬───────────────┬──────────────────┐
    ss << "┌";
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString("─", paveColList[iCol]+2);
      ss << "┬";
    }
    ss << GenericToolbox::repeatString("─", paveColList.back()+2) << "┐" << std::endl;

    // │ Likelihood │ Current Value │ Avg. Slope /call │
    ss << "│";
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      ss << " " << GenericToolbox::padString(_colTitleList_[iCol], paveColList[iCol]);
      ss << " │";
    }
    ss << std::endl;

    // ├───────────┼───────────────┼──────────────────┤
    ss << "├";
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString("─", paveColList[iCol]+2);
      ss << "┼";
    }
    ss << GenericToolbox::repeatString("─", paveColList.back()+2) << "┤" << std::endl;

    // │     Total │ 9.9296422e-13 │             nanP │
    if( not _tableContent_.empty() ){
      for( int iRow = 0 ; iRow < int(_tableContent_[0].size()) ; iRow++ ){
        ss << "│";
        for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
          ss << " " << GenericToolbox::padString(_tableContent_[iCol][iRow], paveColList[iCol]);
          ss << " │";
        }
        ss << std::endl;
      }
    }


    // └───────────┴───────────────┴──────────────────┘
    ss << "└";
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString("─", paveColList[iCol]+2);
      ss << "┴";
    }
    ss << GenericToolbox::repeatString("─", paveColList.back()+2) << "┘";

    return ss.str();
  }
  void TablePrinter::printTable(){
    std::cout << generateTableString() << std::endl;
  }

}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H
