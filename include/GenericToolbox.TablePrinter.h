//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_H

#include "implementation/GenericToolbox.param.h"

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

namespace GenericToolbox{

  class TablePrinter{

  public:
    inline TablePrinter();
    inline virtual ~TablePrinter();

    inline void reset();

    inline void fillTable(const std::vector<std::vector<std::string>> &tableLines_);

    inline size_t setColTitles(const std::vector<std::string>& colTitles_);
    inline size_t addColTitle(const std::string& colTitle_);
    inline size_t addTableLine(const std::vector<std::string>& colValues_=std::vector<std::string>(), const std::string& colorCode_="");

    inline void setTableContent(size_t colIndex_, size_t rowIndex_, const std::string& value_);

    inline int getNbRows();

    inline std::string generateTableString();
    inline void printTable();

    template<typename T> inline TablePrinter &operator<<(const T &data);
    inline TablePrinter &operator<<(std::ostream &(*f)(std::ostream &));

  private:
    std::vector<std::string> _colTitleList_{};
    std::vector<std::vector<std::string>> _tableContent_{};
    std::vector<int> _colMaxWidthList_{};

    std::vector<std::string> _lineBuffer_;
    size_t _currentRow_{0};

  };

}

#include "implementation/GenericToolbox.TablePrinter.Impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_TABLE_PRINTER_H
