//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H

#include "GenericToolbox.h"

#include <chrono>
#include <sstream>
#include <string>
#include <cmath>


#ifndef CPP_GENERIC_TOOLBOX_BATCH
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN "┌"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN "└"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN "┐"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN "┘"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT "├"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT "┤"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP "┬"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT "┴"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS "┼"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE "─"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE "│"
#else
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE "|"
#endif

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
    this->setColTitles(tableLines_[0]);
    for( size_t iLine = 1 ; iLine < tableLines_.size() ; iLine++ ){
      this->addTableLine(tableLines_[iLine]);
    }
  }

  inline size_t TablePrinter::setColTitles(const std::vector<std::string>& colTitles_){
    if( colTitles_.empty() ) throw std::runtime_error("colTitles_ is empty.");
    for( auto& colTitle : colTitles_ ){
      this->addColTitle(colTitle);
    }
    return _colTitleList_.size()-1;
  }
  size_t TablePrinter::addColTitle(const std::string& colTitle_){
    _colTitleList_.emplace_back(colTitle_);
    _tableContent_.emplace_back();
    _colMaxWidthList_.emplace_back(-1);
    return _colTitleList_.size()-1;
  }
  size_t TablePrinter::addTableLine(const std::vector<std::string>& colValues_, const std::string&  colorCode_){
    size_t rowIndex{0};
    size_t colIndex{0};
    for( auto& colTable : _tableContent_ ){
      colTable.emplace_back();
      if( not colValues_.empty() ) {
        if( not colorCode_.empty() ) colTable.back() += colorCode_;
        colTable.back() += colValues_[colIndex++];
        if( not colorCode_.empty() ) colTable.back() += GenericToolbox::ColorCodes::resetColor;
      }
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
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      paveColList[iCol] = GenericToolbox::getPrintSize(_colTitleList_[iCol]);
      for( int iRow = 0 ; iRow < int(_tableContent_[iCol].size()) ; iRow++ ){
        paveColList[iCol] = std::max(paveColList[iCol], GenericToolbox::getPrintSize(_tableContent_[iCol][iRow]));
      }
    }

    // ┌───────────┬───────────────┬──────────────────┐
    // or
    // #----------------------------------------------#
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN<< std::endl;

    // │ Likelihood │ Current Value │ Avg. Slope /call │
    // or
    // | Likelihood | Current value | Avg. Slope /call |
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      ss << " " << GenericToolbox::padString(_colTitleList_[iCol], paveColList[iCol]);
      ss << " " << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
    }
    ss << std::endl;

    // ├───────────┼───────────────┼──────────────────┤
    // or
    // |-----------|---------------|------------------|
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT << std::endl;

    // │     Total │ 9.9296422e-13 │             nanP │
    // or
    // |     Total | 9.9296422e-13 |             nanP |
    if( not _tableContent_.empty() ){
      for( int iRow = 0 ; iRow < int(_tableContent_[0].size()) ; iRow++ ){
        ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
        for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
          ss << " " << GenericToolbox::padString(_tableContent_[iCol][iRow], paveColList[iCol]);
          ss << " " << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
        }
        ss << std::endl;
      }
    }


    // └───────────┴───────────────┴──────────────────┘
    // or
    // #----------------------------------------------#
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN;

    return ss.str();
  }
  void TablePrinter::printTable(){
    std::cout << generateTableString() << std::endl;
  }

  template<typename T> TablePrinter &TablePrinter::operator<<(const T &data){
    // just fills a buffer

    std::stringstream ss;
    ss << data;
    _currentEntryBuffer_ += ss.str();

    return *this;
  }
  inline TablePrinter &TablePrinter::operator<<(Action action_){

    if(action_ == Action::Reset ){
      this->reset();
      _currentRow_ = 0;
      _currentEntryBuffer_ = "";
      _currentLineBuffer_.clear();
    }
    if(
        action_ == Action::NextColumn or
        action_ == Action::NextLine
    ){
      if( _colTitleList_.empty() or _lineBuffer_.size() < _colTitleList_.size() ){
        // drop the string buffer to the vector buffer
        if( not _colorBuffer_.empty() ){
          _lineBuffer_.emplace_back( _colorBuffer_ + _currentEntryBuffer_ + GenericToolbox::ColorCodes::resetColor );
        }
        else{
          _lineBuffer_.emplace_back( _currentEntryBuffer_ );
        }
      }

      // clear the buffer
      _currentEntryBuffer_ = "";
      _currentRow_++;
    }
    if( action_ == Action::NextLine or ( not _colTitleList_.empty() and _lineBuffer_.size() == _colTitleList_.size() ) ){
      if( _colTitleList_.empty() ){ this->setColTitles(_lineBuffer_); }
      else                        {
        this->addTableLine(_lineBuffer_);
      }
      _lineBuffer_.clear();
      _colorBuffer_ = "";
      _currentRow_ = 0;
    }

    return *this;
  }
}

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_IMPL_H
