//
// Created by Nadrino on 28/08/2020.
//

#pragma once
#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H

#include "../GenericToolbox.ProgressBar.h"

#ifdef __SWITCH__
#include "switch.h"
#endif

// STD headers
#include <cmath>
#include <regex>
#include <array>
#include <ctime>
#include <vector>
#include <cstdio>
#include <string>
#include <memory>
#include <thread>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <utility>
#include <cstring>
#include <numeric>
#include <iostream>
#include <typeindex>
#include <stdexcept>
#include <algorithm>

// C headers
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/statvfs.h>


#if HAS_CPP_17 && USE_FILESYSTEM
#include "filesystem"
#endif



extern char* __progname;


// Displaying Tools
namespace GenericToolbox {

  template<typename T, typename TT> inline static std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){
    return ProgressBar::gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, title_);
  }
  template<typename T, typename TT> inline static bool showProgressBar(const T& iCurrent_, const TT& iTotal_){
    return ProgressBar::gProgressBar.template showProgressBar(iCurrent_, iTotal_);
  }
  template<typename T, typename TT> inline static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    return ProgressBar::gProgressBar.template getProgressBarStr(iCurrent_, iTotal_, title_, forcePrint_);
  }
  template<typename T, typename TT> inline static void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    return ProgressBar::gProgressBar.template displayProgressBar(iCurrent_, iTotal_, title_, forcePrint_);
  }
  inline static void resetLastDisplayedValue(){
    ProgressBar::gProgressBar.resetLastDisplayedValue();
  }

}

//! Conversion Tools
namespace GenericToolbox{

  template<typename T, typename TT> inline static std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_, bool indentLine_){
    std::stringstream ss;
    ss << "{ ";
    if( not iterable_.empty() ){
      if( jumpLine_ ){ ss << std::endl << "  "; }
      ss.str().reserve(256); // Reserve initial space
      auto elementIterator = iterable_.begin();
      ss << toStrFct_(*elementIterator);
      for( ++elementIterator; elementIterator != iterable_.end(); ++elementIterator ){
        ss << ", ";
        if( jumpLine_ ){
          ss << std::endl;
          if( indentLine_ ){ ss << "  "; }
        }
        ss << toStrFct_(*elementIterator);
      }
      if( not jumpLine_ ){ ss << " "; }
    }
    if( jumpLine_ ){ ss << std::endl; }
    ss << "}";
    return ss.str();
  }

}

//! Printout Tools
namespace GenericToolbox{

  inline static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

    auto anchorTimePoint = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds totalDurationToWait(nbMilliSecToWait_*1000);
    std::chrono::microseconds cumulatedDuration(0);
    std::chrono::microseconds loopUpdateMaxFrequency(nbMilliSecToWait_); // 1000x faster than the whole time

    GenericToolbox::displayProgressBar( 0, totalDurationToWait.count(), progressTitle_);
    while( true ){
      std::this_thread::sleep_for( loopUpdateMaxFrequency );
      cumulatedDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - anchorTimePoint);
      if( cumulatedDuration >= totalDurationToWait ){
        return;
      }
      else{
        GenericToolbox::displayProgressBar( cumulatedDuration.count(), totalDurationToWait.count(), progressTitle_);
      }
    }
    GenericToolbox::displayProgressBar( totalDurationToWait.count(), totalDurationToWait.count(), progressTitle_);

  }
  inline static std::string parseIntAsString(int intToFormat_){
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_);
    }
    intToFormat_/=1000.; // in K
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "K";
    }
    intToFormat_/=1000.; // in M
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "M";
    }
    intToFormat_/=1000.; // in G
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "G";
    }
    intToFormat_/=1000.; // in T
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "T";
    }
    intToFormat_/=1000.; // in P
    return std::to_string(intToFormat_) + "P";
  }
  inline static std::string highlightIf(bool condition_, const std::string& text_){
    std::stringstream ss;
    ss << (condition_ ? ColorCodes::redBackground : "" );
    ss << text_;
    ss << ( condition_ ? ColorCodes::resetColor : "" );
    return ss.str();
  }
  inline static std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_){
    std::string outputString;
    std::string inputStrStripped;
    stripUnicode_ ? inputStrStripped = GenericToolbox::stripStringUnicode(inputStr_) : inputStrStripped = inputStr_;
    double nbCharsPerColor = double(inputStrStripped.size()) / double(ColorCodes::rainbowColorList.size());
    int colorSlot{0};
    for( size_t iChar = 0 ; iChar < inputStrStripped.size() ; iChar++ ){
      if( nbCharsPerColor < 1 or iChar == 0 or ( int(iChar+1) / nbCharsPerColor) - colorSlot + 1 > 1 ){
        outputString += ColorCodes::rainbowColorList[colorSlot++];
      }
      outputString += inputStrStripped[iChar];
    }
    outputString += ColorCodes::resetColor;
    return outputString;
  }

}


//! Vector management
namespace GenericToolbox {

  // Content management
  template <typename T> inline static bool doesElementIsInVector(const T& element_, const std::vector<T>& vector_){
    return std::find(vector_.cbegin(), vector_.cend(), element_) != vector_.cend();
  }
  inline static bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_){
    return std::find(vector_.cbegin(), vector_.cend(), element_) != vector_.cend();
  }
  template <typename Elm, typename Val, typename Lambda> inline static int doesElementIsInVector(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_){
    return std::find_if( vector_.begin(), vector_.end(), [&](const Elm& t){ return fetchElmValueFct_(t) == value_; }) != vector_.end();
  }
  template <typename T> inline static int findElementIndex(const T& element_, const std::vector<T>& vector_ ){ // test
    auto it = std::find(vector_.cbegin(), vector_.cend(), element_);
    if( it == vector_.cend() ){ return -1; }
    return static_cast<int>( std::distance(vector_.cbegin(), it) );
  }
  inline static int findElementIndex(const char* element_, const std::vector<std::string>& vector_ ){
    auto it = std::find(vector_.cbegin(), vector_.cend(), element_);
    if( it == vector_.cend() ){ return -1; }
    return static_cast<int>( std::distance(vector_.cbegin(), it) );
  }
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_){
    if( insertBeforeThisIndex_ > vector_.size() ){
      throw std::runtime_error("GenericToolBox::insertInVector error: insertBeforeThisIndex_ >= vector_.size()");
    }
    if( vector_.empty() ){ vector_ = vectorToInsert_; return; }
    if( vectorToInsert_.empty() ){ return; }
    vector_.insert( vector_.cbegin() + insertBeforeThisIndex_, vectorToInsert_.crbegin(), vectorToInsert_.crend() );
  }
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_){
    insertInVector(vector_, {elementToInsert_}, insertBeforeThisIndex_);
  }
  template<typename T> static inline void addIfNotInVector(const T& element_, std::vector<T> &vector_){
    if( not GenericToolbox::doesElementIsInVector(element_, vector_) ){
      vector_.template emplace_back(element_);
    }
  }
  static inline void addIfNotInVector(const char* element_, std::vector<std::string> &vector_){
    if( not GenericToolbox::doesElementIsInVector(element_, vector_) ){
      vector_.emplace_back(element_);
    }
  }
  template <typename Elm, typename Val, typename Lambda> inline static int findElementIndex(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_){
    auto it = std::find_if( vector_.begin(), vector_.end(), [&](const Elm& t){ return fetchElmValueFct_(t) == value_; });
    if( it == vector_.end() ){ return -1; }
    else{ return std::distance( vector_.begin(), it ); }
  }

  // Generators
  template<typename T> static inline std::vector<size_t> indices(const std::vector<T> &vector_){
    std::vector<size_t> output(vector_.size(), 0);
    for( size_t iIndex = 0 ; iIndex < output.size() ; iIndex++ ){
      output.at(iIndex) = iIndex;
    }
    return output;
  }
  template <typename T> static inline std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ ){
    if( endIndex_ < 0 ){ endIndex_ += vector_.size(); }
    if( beginIndex_ >= endIndex_ ){ return std::vector<T> (); }
    return std::vector<T> ( &vector_[beginIndex_] , &vector_[endIndex_+1] );
  }
  template <typename T, typename TT> static inline std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ ){
    std::vector<TT> outVec;
    for(const auto& element : vector_){
      outVec.emplace_back(convertTypeFunction_(element));
    }
    return outVec;
  }

  // Printout / to string conversions
  template <typename T> static inline void printVector(const std::vector<T>& vector_, bool jumpLine_, bool indentLine_){
    std::cout << parseVectorAsString(vector_, jumpLine_, indentLine_) << std::endl;
  }
  template <typename T> static inline std::string parseVectorAsString(const std::vector<T>& vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::iterableToString(vector_, [&](const T& elm_){ return elm_; }, jumpLine_, indentLine_);
  }
  static inline std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::iterableToString(vector_, [&](const std::string& elm_){ return std::string{"\""+elm_+"\""}; }, jumpLine_, indentLine_);
  }

  // Stats
  template <typename T> static inline double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_){
    double outVal = 0;
    for( auto& element : vector_ ){ outVal += static_cast<double>(evalElementFct_(element)); }
    return outVal / vector_.size();
  }
  template<typename T> static inline double getAveragedSlope(const std::vector<T> &yValues_){
    auto xValues = yValues_;
    for( size_t iVal = 0 ; iVal < yValues_.size() ; iVal++ ){
      xValues.at(iVal) = iVal;
    }
    return getAveragedSlope(yValues_, xValues);
  }
  template<typename T, typename TT> static inline double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_){
    if(xValues_.size() != yValues_.size()){
      throw std::logic_error("x and y values list do have the same size.");
    }
    const auto n    = xValues_.size();
    const auto s_x  = std::accumulate(xValues_.begin(), xValues_.end(), 0.0);
    const auto s_y  = std::accumulate(yValues_.begin(), yValues_.end(), 0.0);
    const auto s_xx = std::inner_product(xValues_.begin(), xValues_.end(), xValues_.begin(), 0.0);
    const auto s_xy = std::inner_product(xValues_.begin(), xValues_.end(), yValues_.begin(), 0.0);
    const auto a    = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
    return a;
  }
  template <typename T> static inline double getStdDev(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_){
    double outVal = 0;
    double mean = getAverage(vector_, evalElementFct_);
    for( auto& element : vector_ ){
      outVal += std::pow(
        static_cast<double>(evalElementFct_(element)) - mean,
        2
      );
    }
    return sqrt( outVal / vector_.size() );
  }


  // Sorting
  template <typename T, typename Lambda> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_ ){
    std::vector<size_t> p(vectorToSort_.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
              [&](size_t i, size_t j){ return firstArgGoesFirstFct_(vectorToSort_.at(i), vectorToSort_.at(j)); });
    return p;
  }
  template <typename T> static inline std::vector<T> getSortedVector(const std::vector<T>& unsortedVector_, const std::vector<std::size_t>& sortPermutation_ ){
    if(unsortedVector_.empty() or sortPermutation_.size() != unsortedVector_.size()) return {};
    std::vector<T> sortedVec(unsortedVector_.size(), unsortedVector_[0]);
    std::transform(sortPermutation_.begin(), sortPermutation_.end(), sortedVec.begin(),
                   [&](std::size_t i){ return unsortedVector_[i]; });
    return sortedVec;
  }
  template <typename T> static inline void applyPermutation(std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_){
    std::vector<bool> done(vectorToPermute_.size(), false);
    for( std::size_t iEntry = 0; iEntry < vectorToPermute_.size(); iEntry++ ){
      if( done[iEntry] ){ continue; }
      done[iEntry] = true;
      std::size_t jPrev = iEntry;
      std::size_t jEntry = sortPermutation_[iEntry];
      while( iEntry != jEntry ){
        std::swap(vectorToPermute_[jPrev], vectorToPermute_[jEntry]);
        done[jEntry] = true;
        jPrev = jEntry;
        jEntry = sortPermutation_[jEntry];
      }
    }
  }
  template <typename T, typename Lambda> static inline void sortVector(std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_){
    std::sort(vectorToSort_.begin(), vectorToSort_.end(), firstArgGoesFirstFct_);
  }
  template <typename T, typename Lambda> static inline void removeEntryIf(std::vector<T>& vector_, const Lambda& removeIfFct_){
    vector_.erase( std::remove_if(vector_.begin(), vector_.end(), removeIfFct_), vector_.end() );
  }

  // Others
  template<typename T, typename TT> static inline T& getListEntry(std::list<T>& list_, TT index_){
    typename std::list<T>::iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }
  template<typename T, typename TT> static inline const T& getListEntry(const std::list<T>& list_, TT index_){
    typename std::list<T>::const_iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }

}


//! Map management
namespace GenericToolbox {

  template <typename K, typename  T> static inline bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ ){
    return ( map_.find(key_) != map_.end() );
  }
  template <typename K, typename T> static inline T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ ){
    auto it = map_.find(key_);
    if( it == map_.end() ){
      return nullptr;
    }
    return &( it->second );
  }
  template <typename T1, typename T2> static inline void appendToMap(std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_) {
    for(const auto& newEntry : mapToPushBack_){
      if(not overwrite_ and doesKeyIsInMap(newEntry.first, mapContainer_)){
        continue;
      }
      mapContainer_[newEntry.first] = newEntry.second;
    }
  }
  template <typename T> static inline std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ ){
    std::map<std::string, T> outSubMap;
    for(const auto& mapPair : map_){
      if(GenericToolbox::doesStringStartsWithSubstring(mapPair.first, keyStrStartWith_)){
        outSubMap[mapPair.first] = mapPair.second;
      }
    }
    return outSubMap;
  }
  template <typename T1, typename T2> static inline std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_){
    return GenericToolbox::iterableToString(
            map_,
            [&](const std::pair<T1, T2>& elm_){
              std::stringstream ss;
              ss << "{ " << elm_.first << ": " << elm_.second << " }";
              return ss.str();
            },
            enableLineJump_, enableLineJump_);
  }
  template <typename T1, typename T2, typename T3> static inline std::string parseMapAsString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_){
    return GenericToolbox::iterableToString(
        map_,
        [&](const std::pair<T1, std::pair<T2,T3>>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": {" << elm_.second.first << ", " << elm_.second.second << "} }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }
  template <typename T1, typename T2> static inline void printMap(const std::map<T1, T2>& map_, bool enableLineJump_){
    std::cout << parseMapAsString(map_, enableLineJump_) << std::endl;
  }

}


// String Management Tools
namespace GenericToolbox {

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
#else
  namespace StringManagementUtils{
    static std::regex ansiRegex("\033((\\[((\\d+;)*\\d+)?[A-DHJKMRcfghilmnprsu])|\\(|\\))");
  }
#endif

  static inline std::string addUpDownBars(const std::string& str_, bool stripUnicode_){
    std::stringstream ss;
    size_t strLength = str_.size();
    if( stripUnicode_ ) strLength = GenericToolbox::stripStringUnicode(str_).size();
    std::string bar = GenericToolbox::repeatString("─", int(strLength));
    ss << bar << std::endl << str_ << std::endl << bar;
    return ss.str();
  }

  static inline bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( ignoreCase_ ){ return toLowerCase(str_).find( toLowerCase( subStr_ ) ) != std::string::npos; }
    return str_.find( subStr_ ) != std::string::npos;
  }
  static inline bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).begin() );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.begin() );
  }
  static inline bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).end() - long(subStrLower.size()) );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.end() - long(subStr_.size()) );
  }
  static inline std::string toLowerCase(const std::string &inputStr_) {
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }
  static inline std::string stripStringUnicode(const std::string &inputStr_){
    std::string outputStr(inputStr_);

    if(GenericToolbox::doesStringContainsSubstring(outputStr, "\033")){
      // remove color
      std::string tempStr;
      auto splitOutputStr = GenericToolbox::splitString(outputStr, "\033");
      for(const auto& sliceStr : splitOutputStr){
        if(sliceStr.empty()) continue;
        if(tempStr.empty()){
          tempStr = sliceStr;
          continue;
        }
        // look for a 'm' char that determines the end of the color code
        bool mCharHasBeenFound = false;
        for(const char& c : sliceStr){
          if(not mCharHasBeenFound){
            if(c == 'm'){
              mCharHasBeenFound = true;
            }
          }
          else{
            tempStr += c;
          }
        }
      }
      outputStr = tempStr;
    }

    outputStr.erase(
            remove_if(
                    outputStr.begin(), outputStr.end(),
                    [](const char& c){return !isprint( static_cast<unsigned char>( c ) );}
            ),
            outputStr.end()
    );

    return outputStr;
  }
  static inline std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_, std::vector<std::string>* argBuffer_){
    size_t iChar{0}; std::string out;
    while( iChar < inputStr_.size() ){
      if ( inputStr_[iChar] == bra_ ){
        iChar++;
        if(argBuffer_!= nullptr){ argBuffer_->emplace_back(); }
        while(iChar < inputStr_.size()){
          if(inputStr_[iChar] == ket_ ) { iChar++; break; }
          if(argBuffer_!= nullptr){ argBuffer_->back() += inputStr_[iChar]; }
          iChar++;
          if(iChar == inputStr_.size()){
            if( not allowUnclosed_ ){
              throw std::runtime_error("unclosed bracket.");
            }
          }
        }
      }
      if(iChar < inputStr_.size()) out += inputStr_[iChar++];
    }
    return out;
  }
  static inline size_t getPrintSize(const std::string& str_){
    if( str_.empty() ) return 0;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
    // this is gcc 4.8 or earlier
// std::regex support is buggy, so don't use in this block
  return str_.size();
#else
// this is gcc 4.9 or later, or other compilers like clang
// hopefully std::regex support is ok here
    std::string::iterator::difference_type result = 0;
    std::for_each(
            std::sregex_token_iterator(str_.begin(), str_.end(), StringManagementUtils::ansiRegex, -1),
            std::sregex_token_iterator(),
            [&result](std::sregex_token_iterator::value_type const& e) {
              std::string tmp(e);
              result += std::count_if(tmp.begin(), tmp.end(), ::isprint);
            }
    );
    return result;
#endif
  }
  static inline size_t getNLines(const std::string& str_){
    if( str_.empty() ) return 0;
    return std::count(str_.begin(), str_.end(), '\n')+1;
  }
  static inline std::string repeatString(const std::string &inputStr_, int amount_){
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }
  static inline std::string trimString(const std::string &inputStr_, const std::string &strToTrim_){
    std::string outputStr(inputStr_);
    GenericToolbox::trimInputString( outputStr, strToTrim_ );
    return outputStr;
  }
  static inline std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar){
    std::string outputString;
//    int padDelta = int(inputStr_.size()) - int(padSize_);
    int padDelta = int(GenericToolbox::getPrintSize(inputStr_)) - int(padSize_);
    while( padDelta < 0 ){
      // add extra chars if needed
      outputString += padChar;
      padDelta++;
    }
    outputString += inputStr_;
    return outputString.substr(0, outputString.size() - padDelta);
  }
  static inline std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    std::string outStr = inputStr_;
    GenericToolbox::indentInputString(outStr, indentCount_, indentChar);
    return outStr;
  }
  static inline std::string removeRepeatedCharacters(const std::string &inputStr_, const std::string &repeatedChar_) {
    std::string outStr = inputStr_;
    GenericToolbox::removeRepeatedCharInsideInputStr(outStr, repeatedChar_);
    return outStr;
  }
  template<typename T> std::string joinVectorString(const std::vector<T> &stringList_, const std::string &delimiter_, int beginIndex_, int endIndex_) {
    std::stringstream ss;
    if( endIndex_ == 0 ) endIndex_ = int(stringList_.size());

    // circular permutation -> python style : tab[-1] = tab[tab.size - 1]
    if( endIndex_ < 0 and int(stringList_.size()) > std::abs(endIndex_) ) {
      endIndex_ = int(stringList_.size()) + endIndex_;
    }

    for(int iElm = beginIndex_ ; iElm < endIndex_; iElm++ ) {
      if( iElm > beginIndex_ ) ss << delimiter_;
      ss << stringList_[iElm];
    }

    return ss.str();
  }
  template<typename... Args> static inline std::string joinAsString(const std::string& delimiter, const Args&... args) {
    std::stringstream ss;
    int dummy[] = {0, ((void)(ss << args << delimiter), 0)...};
    (void)dummy;  // Avoid unused variable warning
    std::string result = ss.str();
    if( not result.empty() ) { result.resize(result.size() - delimiter.size()); } // remove the last delimiter
    return result;
  }

  std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_) {
    std::string stripped_str = input_str_;
    GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_, substr_to_replace_);
    return stripped_str;
  }
  std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_){
    std::string stripped_str = input_str_;
    if(substr_to_look_for_.size() != substr_to_replace_.size()){
      throw std::runtime_error("vec size mismatch btw substr_to_look_for_(" + std::to_string(substr_to_look_for_.size()) + ") != substr_to_replace_(" + std::to_string(substr_to_replace_.size()) + ")");
    }
    for( size_t iSub = 0 ; iSub < substr_to_look_for_.size() ; iSub++ ){
      GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_[iSub], substr_to_replace_[iSub]);
    }
    return input_str_;
  }
  static inline std::string parseDecimalValue(double val_, const std::string& format_, bool allowSubOne_){
    std::stringstream ss;

    // flip the sign
    if( val_ < 0 ){ ss << "-"; val_ = -val_; }

//    auto f = [](double val, const std::vector<>){
//
//    };
//
//    ss << formatString(format_, );

    return ss.str();
  }
  std::string parseUnitPrefix(double val_, int maxPadSize_){
    std::stringstream ss;

    if( val_ < 0 ){
      ss << "-";
      val_ = -val_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(fabs(val_));
    if( reducedVal > 0 ){
      if     ( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_;
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E3 << "K";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E6 << "M";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E9 << "G";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E12 << "T";
      }
      else {
        ss << val_/1E15 << "P";
      }
    } // K, M, G, T, P
    else{
      if( val_ < 1E-3 ){ // force scientific notation
        ss << std::scientific << val_;
      }
      else{
        ss << val_;
      }
    }


    return ss.str();
  }
  std::string parseSizeUnits(double sizeInBytes_){
    return parseUnitPrefix(sizeInBytes_) + "B";
  }
  static inline std::vector<std::string> splitString(const std::string &inputString_, const std::string &delimiter_, bool removeEmpty_) {

    std::vector<std::string> outputSliceList;

    const char *src = inputString_.c_str();
    const char *next = src;

    std::string out_string_piece;

    while ((next = std::strstr(src, delimiter_.c_str())) != nullptr) {
      out_string_piece = "";
      while (src != next) {
        out_string_piece += *src++;
      }
      outputSliceList.emplace_back(out_string_piece);
      /* Skip the delimiter_ */
      src += delimiter_.size();
    }

    /* Handle the last token */
    out_string_piece = "";
    while (*src != '\0')
      out_string_piece += *src++;

    outputSliceList.emplace_back(out_string_piece);

    if(not removeEmpty_){
      return outputSliceList;
    }
    else{
      std::vector<std::string> strippedOutput;
      for(const auto& slice : outputSliceList){
        if(not slice.empty()){
          strippedOutput.emplace_back(slice);
        }
      }
      return strippedOutput;
    }


  }
  static inline std::string formatString( const std::string& strToFormat_ ){
    return strToFormat_;
  }
  template<typename ... Args> std::string formatString(const std::string& strToFormat_, Args ... args) {
    size_t size = snprintf(nullptr, 0, strToFormat_.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, strToFormat_.c_str(), args ...);
    return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
  }

  static inline void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_){
    size_t index = 0;
    while ((index = input_str_.find(substr_to_look_for_, index)) != std::string::npos) {
      input_str_.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
  }
  static inline void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_){
    std::string doubledCharStr = doubledChar_+doubledChar_;
    std::string lastStr;
    do{
      lastStr = inputStr_;
      GenericToolbox::replaceSubstringInsideInputString(inputStr_, doubledCharStr, doubledChar_);
    } while( lastStr != inputStr_ );
  }
  static inline void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_){
    if( GenericToolbox::doesStringEndsWithSubstring(inputStr_, trailingChar_) ){
      inputStr_.erase(inputStr_.size() - trailingChar_.size());
    }
  }
  static inline void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    int originalSize = int(inputStr_.size());
    for( int iChar = originalSize-1 ; iChar >= 0 ; iChar-- ){
      if( iChar == 0 or inputStr_[iChar] == '\n'){
        int offSet = 1;
        if( iChar == 0 ) offSet = 0;
        for( unsigned int iIndent = 0 ; iIndent < indentCount_ ; iIndent++ ){
          inputStr_.insert(iChar+offSet, indentChar);
        }
      }
    }
  }
  static inline void trimInputString(std::string &inputStr_, const std::string &strToTrim_){
    while( GenericToolbox::doesStringStartsWithSubstring(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(strToTrim_.size(), inputStr_.size());
    }
    while( GenericToolbox::doesStringEndsWithSubstring(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(0, inputStr_.size() - strToTrim_.size());
    }
  }

}


// Conversion Tools
namespace GenericToolbox {

  static inline std::string toHex(const void* address_, size_t nBytes_){
    std::stringstream ss(std::string(2*nBytes_, 0));
    unsigned char* address{(unsigned char*)(address_) + nBytes_-1};
    do{ ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(*(address--)); } while(address >= address_);
    return ss.str();
  }
  template<typename T> static inline std::string toHex(const T& val_){
    return toHex(&val_, sizeof(val_));
  }
  template<typename T> static inline std::string toHexString(T integerVal_, size_t nbDigit_){
    std::stringstream stream;
    stream << "0x" << toHex(&integerVal_, sizeof(integerVal_));
    if( nbDigit_ == 0 ) return stream.str();
    else return "0x" + stream.str().substr(2 + sizeof(T)*2 - nbDigit_, nbDigit_);
  }
  template<typename T> static inline std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_) {
    std::stringstream ss;
    size_t nChunks = rawData_.size()*sizeof(T)/stackSize_;
    const unsigned char* address{&rawData_[0]};
    for( int iChunk=0 ; iChunk < nChunks ; iChunk++ ){
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::toHex(address, stackSize_);
      address += stackSize_;
    }

    if( address < &(rawData_.back())+sizeof(T) ) {
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::repeatString("_-", address+stackSize_ - (&(rawData_.back())+sizeof(T)));
      ss << GenericToolbox::toHex(address, (&(rawData_.back()) + sizeof(T)) - address);
    }

    ss << " }";
    return ss.str();
  }
  static inline bool toBool(const std::string& str) {
    auto result = false;    // failure to assert is false

    std::istringstream is(str);
    // first try simple integer conversion
    is >> result;

    if (is.fail()) {
      // simple integer failed; try boolean
      is.clear();
      is >> std::boolalpha >> result;
    }

    if( is.fail() ){
      throw std::invalid_argument( str + " is not convertable to bool" );
      return false;
    }

    return result;
  }

}


// OS Tools
namespace GenericToolbox{

  namespace Internals {
    static inline char * getEnvironmentVariable(char const envVarName_[]){
#if defined _WIN32 // getenv() is deprecated on Windows
      char *buf{nullptr};
    size_t sz;
    std::string val;
    if (_dupenv_s(&buf, &sz, envVarName_) || buf == nullptr) return val;
    val = buf;
    free(buf);
    return val;
#else
      return getenv(envVarName_);
#endif
    }
    static inline bool expandEnvironmentVariables(const char *fname, char *xname) {
      int n, ier, iter, lx, ncopy;
      char *inp, *out, *x, *t, *buff;
      const char *b, *c, *e;
      const char *p;
      int kBufSize{8196};
      buff = new char[kBufSize * 4];

      iter = 0; xname[0] = 0; inp = buff + kBufSize; out = inp + kBufSize;
      inp[-1] = ' '; inp[0] = 0; out[-1] = ' ';
      c = fname + strspn(fname, " \t\f\r");
      //VP  if (isalnum(c[0])) { strcpy(inp, WorkingDirectory()); strcat(inp, "/"); } // add $cwd

      strncat(inp, c, kBufSize);

      again:
      iter++; c = inp; ier = 0;
      x = out; x[0] = 0;

      p = nullptr; e = nullptr;
      if (c[0] == '~' && c[1] == '/') { // ~/ case
        std::string hd = GenericToolbox::getHomeDirectory();
        p = hd.c_str();
        e = c + 1;
        if (p) {                         // we have smth to copy
          strncpy(x, p, kBufSize);
          x += strlen(p);
          c = e;
        } else {
          ++ier;
          ++c;
        }
      }
//      else if (c[0] == '~' && c[1] != '/') { // ~user case
//        n = int(strcspn(c+1, "/ "));
//        // There is no overlap here as the buffer is segment in 4 strings of at most kBufSize
//        strncpy(buff, c+1, n+1); // strncpy copy 'size-1' characters.
//        std::string hd = GenericToolbox::getHomeDirectory(); // TO FIX this?
//        e = c+1+n;
//        if (!hd.empty()) {                   // we have smth to copy
//          p = hd.c_str();
//          strncpy(x, p, kBufSize);
//          x += strlen(p);
//          c = e;
//        } else {
//          x++[0] = c[0];
//          //++ier;
//          ++c;
//        }
//      }

      for ( ; c[0]; c++) {

        p = nullptr; e = nullptr;

        if (c[0] == '.' && c[1] == '/' && c[-1] == ' ') { // $cwd
          std::string wd = GenericToolbox::getCurrentWorkingDirectory();
          strncpy(buff, wd.c_str(), kBufSize);
          p = buff;
          e = c + 1;
        }
        if (p) {                          // we have smth to copy */
          strncpy(x, p, kBufSize); x += strlen(p); c = e-1; continue;
        }

        if (c[0] != '$') {                // not $, simple copy
          x++[0] = c[0];
        } else {                          // we have a $
          b = c+1;
          if (c[1] == '(') b++;
          if (c[1] == '{') b++;
          if (b[0] == '$')
            e = b+1;
          else
            for (e = b; isalnum(e[0]) || e[0] == '_'; e++) ;
          buff[0] = 0; strncat(buff, b, e-b);
          p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          if (!p) {                      // too bad, try UPPER case
            for (t = buff; (t[0] = toupper(t[0])); t++) ;
            p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          }
          if (!p) {                      // too bad, try Lower case
            for (t = buff; (t[0] = tolower(t[0])); t++) ;
            p = GenericToolbox::Internals::getEnvironmentVariable(buff);
          }
          if (!p && !strcmp(buff, "cwd")) { // it is $cwd
            std::string wd = GenericToolbox::getCurrentWorkingDirectory();
            strncpy(buff, wd.c_str(), kBufSize);
            p = buff;
          }
          if (!p && !strcmp(buff, "$")) { // it is $$ (replace by GetPid())
            snprintf(buff,kBufSize*4, "%d", getpid());
            p = buff;
          }
          if (!p) {                      // too bad, nothing can help
#ifdef WIN32
            // if we're on windows, we can have \\SomeMachine\C$ - don't
            // complain about that, if '$' is followed by nothing or a
            // path delimiter.
            if (c[1] && c[1]!='\\' && c[1]!=';' && c[1]!='/')
               ier++;
#else
            ier++;
#endif
            x++[0] = c[0];
          } else {                       // It is OK, copy result
            int lp = int(strlen(p));
            if (lp >= kBufSize) {
              // make sure lx will be >= kBufSize (see below)
              strncpy(x, p, kBufSize);
              x += kBufSize;
              break;
            }
            strcpy(x,p);
            x += lp;
            c = (b==c+1) ? e-1 : e;
          }
        }
      }

      x[0] = 0; lx = x - out;
      if (ier && iter < 3) { strncpy(inp, out, kBufSize); goto again; }
      ncopy = (lx >= kBufSize) ? kBufSize-1 : lx;
      xname[0] = 0; strncat(xname, out, ncopy);

      delete[] buff;

      if (ier || ncopy != lx) {
        return true;
      }

      return false;
    }
  }

  static inline std::string getHomeDirectory(){
    struct passwd *pw = getpwuid(getuid());
    return {pw->pw_dir};
  }
  static inline std::string getCurrentWorkingDirectory(){
#ifdef PATH_MAX
    char cwd[PATH_MAX];
#else
    char cwd[1024];
#endif
    if( getcwd(cwd, sizeof(cwd)) == nullptr ){
      throw std::runtime_error("getcwd() returned an invalid value.");
    }
    std::string output_cwd(cwd);
    return output_cwd;
  }
  static inline std::string expandEnvironmentVariables(const std::string &filePath_){
//#define USE_BASH_TO_EXPAND // VERY SLOW IN FACT...
#ifdef USE_BASH_TO_EXPAND
    std::array<char, PATH_MAX> buffer{};
    std::string result;

    std::string command = "echo " + filePath_;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if( pipe == nullptr ){ return filePath_; }

    std::FILE* pipe_ptr = pipe.get();
    while (std::size_t n = std::fread(buffer.data(), sizeof(char), buffer.size(), pipe_ptr)) {
      result.append(buffer.data(), n-1);
    }

    return result;
#else
    //    char outputName[PATH_MAX];
    char outputName[8192];
    Internals::expandEnvironmentVariables(filePath_.c_str(), outputName);

    return {outputName};
#endif
  }
  static inline std::string getExecutableName(){
    std::string outStr;
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) //check defines for your setup
    std::ifstream("/proc/self/comm") >> outStr;
#elif defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    outStr = buf;
#else
    outStr = __progname;
#endif
    return outStr;
  }

}

// FS Tools
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  static inline bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_){
    return doesStringEndsWithSubstring(filePath_, "." + extension_);
  }
  static inline std::string getFileExtension(const std::string& filePath_){
    if( filePath_.find_last_of('.') == size_t(-1) ) return {};
    return filePath_.substr(filePath_.find_last_of('.') + 1);
  }
  static inline std::string getFolderPathFromFilePath(const std::string &filePath_){
    if( filePath_.find_last_of("/\\") == size_t(-1) ) return {};
    return filePath_.substr(0,filePath_.find_last_of("/\\"));
  }
  static inline std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_){
#if HAS_CPP_17 && USE_FILESYSTEM
    std::filesystem::path pathObj(filePath_);
    return ( keepExtension_ ? pathObj.filename().string() : pathObj.stem().string());
#else
    const size_t pos = filePath_.find_last_of("/\\");
    const std::string filename = ( pos != std::string::npos ) ? filePath_.substr(pos + 1) : filePath_;
    return ( keepExtension_ or filename.find('.') == std::string::npos ) ? filename : filename.substr(0, filename.find_last_of('.'));
#endif
  }
  static inline std::string replaceFileExtension(const std::string& filePath_, const std::string& newExtension_){
    if( newExtension_.empty() ) return filePath_.substr(0, filePath_.find_last_of('.'));
    return filePath_.substr(0, filePath_.find_last_of('.')) + "." + newExtension_;
  }


  template<typename T1, typename T2> static inline std::string joinPath(const T1& str1_, const T2& str2_){
    std::stringstream ss;
    ss << str1_;
    if( not ss.str().empty() ){ ss << "/"; }
    ss << str2_;
    // explicit type specification as `auto` is not converted into a std::string for GCC 4.8.5
    std::string out{ss.str()};
    GenericToolbox::removeRepeatedCharInsideInputStr( out, "/" );
    GenericToolbox::removeTrailingCharInsideInputStr( out, "/" );
    return out;
  }
  template<typename T1, typename T2> static inline std::string joinPath(const std::vector<T1>& vec1_, const std::vector<T2>& vec2_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec1_), GenericToolbox::joinPath(vec2_));
  }
  template<typename T, typename T2> static inline std::string joinPath(const std::vector<T>& vec_, const T2& str_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec_), str_);
  }
  template<typename T1, typename T> static inline std::string joinPath(const T1& str_, const std::vector<T>& vec_){
    return GenericToolbox::joinPath(str_, GenericToolbox::joinPath(vec_));
  }

  template<typename T> static inline std::string joinPath(const std::vector<T>& vec_){
    std::string out;
    for( auto& elm : vec_ ){ out = GenericToolbox::joinPath(out, elm); }
    return out;
  }
  template<typename First, typename Second, typename... Args> static inline std::string joinPath(const First& first_, const Second& second_, const Args&... args_){
    // unfold to binary expression
    auto out{joinPath(first_, joinPath(second_, args_...))};
    return out;
  }
#if HAS_CPP_17 && USE_FILESYSTEM
  static inline std::filesystem::file_type fileTypeFromDt(int dt_){
    switch (dt_) {
      case DT_REG:
        return std::filesystem::file_type::regular;
      case DT_DIR:
        return std::filesystem::file_type::directory;
      case DT_LNK:
        return std::filesystem::file_type::symlink;
      case DT_BLK:
        return std::filesystem::file_type::block;
      case DT_CHR:
        return std::filesystem::file_type::character;
      case DT_FIFO:
        return std::filesystem::file_type::fifo;
      case DT_SOCK:
        return std::filesystem::file_type::socket;
      default:
        return std::filesystem::file_type::none;
    }
  }
#endif

  // -- with direct IO dependencies
  static inline bool doesPathIsValid(const std::string &filePath_){
    return ( access( filePath_.c_str(), F_OK ) == 0 );
  }
  static inline bool doesPathIsFile(const std::string &filePath_){
    struct stat info{};
    if( lstat(filePath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISREG(info.st_mode);
  }
  static inline bool doesPathIsFolder(const std::string &folderPath_){
    struct stat info{};
    if( lstat(folderPath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISDIR(info.st_mode);
  }
  static inline bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_){

    if( not doesPathIsFile(filePath1_) ) return false;
    if( not doesPathIsFile(filePath2_) ) return false;

    std::ifstream fileStream1(filePath1_);
    std::ifstream fileStream2(filePath2_);

    // Buffer size 1 Megabyte (or any number you like)
    size_t buffer_size = 1<<20;
    char *buffer1 = new char[buffer_size];
    char *buffer2 = new char[buffer_size];

    std::hash<std::string> hashBuffer1;
    std::hash<std::string> hashBuffer2;

    while (fileStream1 and fileStream2) {
      // Try to read next chunk of data
      fileStream1.read(buffer1, long(buffer_size));
      fileStream2.read(buffer2, long(buffer_size));

      // Get the number of bytes actually read
      if(fileStream1.gcount() != fileStream2.gcount()){
        return false;
      }

      size_t count = fileStream1.gcount();
      // If nothing has been read, break
      if( count == 0 ){
        break;
      }

      // Compare hash files
      if(hashBuffer1(buffer1) != hashBuffer2(buffer2))
        return false;

    }

    delete[] buffer1;
    delete[] buffer2;

    return true;
  }
  static inline bool mkdirPath(const std::string &newFolderPath_){
    bool result = false;
    if(doesPathIsFolder(newFolderPath_)) return true;

    std::string current_level;
    std::string level;
    std::stringstream ss(newFolderPath_);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')){
      current_level += level; // append folder to the current level
      if(current_level.empty()) current_level = "/";
      current_level = removeRepeatedCharacters(current_level, "/");
      // create current level
      if(not doesPathIsFolder(current_level)){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;

  }
  static inline bool deleteFile(const std::string &filePath_){
    // not using ::delete as if the specified file path is a symbolic link,
    // it deletes the link and the file or directory that the link refers to

    // ::unlink: If the specified file path is a symbolic link,
    // only the link itself is deleted, not the file or directory that the link refers to
    return ( ::unlink(filePath_.c_str()) == 0 );
  }
  static inline bool isFolderEmpty(const std::string &dirPath_){
    return getListOfEntriesInFolder( dirPath_ ).empty();
  }
  static inline bool deleteEmptyDirectory(const std::string &dirPath_){
    return (::rmdir(dirPath_.c_str()) == 0);
  }
  static inline bool copyFile(const std::string &source_file_path_, const std::string &destination_file_path_, bool force_){

    if( not doesPathIsFile(source_file_path_) ){
      return false;
    }

    if( doesPathIsFile(destination_file_path_) ){
      if( force_ ){
        deleteFile(destination_file_path_);
      }
      else{
        return false;
      }
    }

    std::ifstream  src(source_file_path_, std::ios::binary);
    std::ofstream  dst(destination_file_path_,   std::ios::binary);

    dst << src.rdbuf();

    return true;
  }
  static inline bool mvFile(const std::string &sourceFilePath_, const std::string &destinationFilePath_, bool force_) {
    if( not doesPathIsFile(sourceFilePath_) ){ return false; }

    if( doesPathIsFile(destinationFilePath_) ){
      if(force_){ deleteFile(destinationFilePath_); }
      else{ return false; }
    }
    else{
      std::string destination_folder_path = getFolderPathFromFilePath(destinationFilePath_);
      if(not doesPathIsFile(destination_folder_path)){ mkdirPath(destination_folder_path); }
    }

    return (std::rename(sourceFilePath_.c_str(), destinationFilePath_.c_str()) == 0);
  }
  static inline size_t getHashFile(const std::string &filePath_) {
    std::hash<std::string> hashString;
    return hashString(dumpFileAsString(filePath_));
  }
  static inline ssize_t getFileSize(const std::string& path_){
    struct stat st{};
    stat(path_.c_str(), &st);
    return ssize_t(st.st_size);
  }
  static inline long int getFileSizeInBytes(const std::string &filePath_){
    long int output_size = 0;
    if(doesPathIsFile(filePath_)){
      std::ifstream testFile(filePath_.c_str(), std::ios::binary);
      const auto begin = testFile.tellg();
      testFile.seekg (0, std::ios::end);
      const auto end = testFile.tellg();
      const auto fsize = (end-begin);
      output_size = fsize;
    }
    return output_size;
  }
  static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_){
    std::ofstream out(outFilePath_.c_str());
    out << stringToWrite_;
    out.close();
  }
  static inline std::string dumpFileAsString(const std::string &filePath_){
    std::string data;
    if(doesPathIsFile(filePath_)){
      std::ifstream input_file(filePath_.c_str(), std::ios::binary | std::ios::in );
      std::ostringstream ss;
      ss << input_file.rdbuf();
      data = ss.str();
      input_file.close();
    }
    return data;
  }
  static inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_){
    std::vector<std::string> lines;
    if(doesPathIsFile(filePath_)){
      std::string data = GenericToolbox::dumpFileAsString(filePath_);
      lines = GenericToolbox::splitString(data, "\n", skipEmptyLines_);
    }
    for(auto& line: lines){
      if(GenericToolbox::doesStringEndsWithSubstring(line, "\r")){
        line = line.substr(0, line.size()-1);
      }
    }

    return lines;
  }
  static inline std::vector<std::string> getListOfEntriesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_, int type_, size_t maxEntries_) {
    if( not doesPathIsFolder( folderPath_ ) ) return {};

    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) { std::cout << "Failed to open directory : " << folderPath_ << std::endl; return {}; }

    std::vector<std::string> nameElements;
    if(not entryNameRegex_.empty()){ nameElements = GenericToolbox::splitString(entryNameRegex_, "*"); }

    struct dirent* entry;
    std::vector<std::string> subFoldersList;
    while ( ( entry = readdir(directory) ) ) {
      if( type_ != -1 and entry->d_type != type_ ){ continue; }
      if( strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0 ){ continue; }

      if(not entryNameRegex_.empty()){
        std::string entryCandidate = entry->d_name;

        bool isValid{true};
        for( size_t iElement = 0 ; iElement < nameElements.size() ; iElement++ ){
          if(nameElements[iElement].empty()) continue;

          if( iElement == 0 ){
            if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
              isValid = false;
              break;
            }
          }
          else if( iElement+1 == nameElements.size() ){
            if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
              isValid = false;
            }
          }
          else{
            if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                    ){
              isValid = false;
              break;
            }
          }

          if( iElement+1 != nameElements.size() ){
            entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
          }
        }
        if( not isValid ) continue;
      }
      subFoldersList.emplace_back(entry->d_name);
      if( maxEntries_ != 0 and subFoldersList.size() >= maxEntries_ ){
        std::cout << "WARNING: max # of entries reached." << std::endl;
        break;
      }
    }
    closedir(directory);
    return subFoldersList;
  }
  static inline std::vector<std::string> getListOfSubFoldersInFolder(const std::string &folderPath_, const std::string &entryNameRegex_, size_t maxEntries_) {
    return GenericToolbox::getListOfEntriesInFolder(folderPath_, entryNameRegex_, DT_DIR, maxEntries_);
  }
  static inline std::vector<std::string> getListOfFilesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_, size_t maxEntries_){
    return GenericToolbox::getListOfEntriesInFolder(folderPath_, entryNameRegex_, DT_REG, maxEntries_);
  }

  // -- with direct IO dependencies
  static inline bool doesFolderIsEmpty(const std::string &folderPath_){
    if(not doesPathIsFolder(folderPath_)) return false;
    return getListOfEntriesInSubFolders(folderPath_).empty();
  }
  static inline std::vector<std::string> getListOfEntriesInSubFolders(const std::string &folderPath_, int type_){
    // WARNING : Recursive function

    // first, get the files in this folder
    std::vector<std::string> out;

    // then walk in sub-folders
    auto subFolderList = GenericToolbox::getListOfSubFoldersInFolder( folderPath_ );
    for(auto &subFolder : subFolderList ){

      // recursive ////////
      auto subFileList = GenericToolbox::getListOfEntriesInSubFolders( GenericToolbox::joinPath(folderPath_, subFolder), type_ );
      /////////////////////

      out.reserve( out.size() + subFileList.size() );
      for(auto &subFile : subFileList ){
        out.emplace_back( GenericToolbox::joinPath(subFolder, subFile) );
      }
    }

    GenericToolbox::insertInVector( out, GenericToolbox::getListOfEntriesInFolder(folderPath_, "", type_), out.size() );

    return out;
  }
  static inline std::vector<std::string> getListOfFilesInSubFolders(const std::string &folderPath_) {
    return getListOfEntriesInSubFolders(folderPath_, DT_REG);
  }
  static inline std::vector<std::string> getListOfFoldersInSubFolders(const std::string &folderPath_){
    return getListOfEntriesInSubFolders(folderPath_, DT_DIR);
  }

}



// Hardware Tools
#if defined(_WIN32)
// Windows
#include <windows.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <sys/ioctl.h>
#include <array>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>
#include <sys/ioctl.h>

#else
// Unsupported
#endif
namespace GenericToolbox{

  struct CpuStat{
    inline CpuStat(){ this->getCpuUsageByProcess(); }
    clock_t lastCPU{}, lastSysCPU{}, lastUserCPU{};
    inline double getCpuUsageByProcess(){
      double percent{0};
#if defined(_WIN32)
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) || defined(__APPLE__) && defined(__MACH__)
      struct tms timeSample{};
      clock_t now;

      now = times(&timeSample);
      if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
          timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
      }
      else{
        percent = double(timeSample.tms_stime - lastSysCPU) +
                  double(timeSample.tms_utime - lastUserCPU);
        percent /= double(now - lastCPU);
        percent *= 100;
      }
      lastCPU = now;
      lastSysCPU = timeSample.tms_stime;
      lastUserCPU = timeSample.tms_utime;
#endif
      return percent;
    }
  };
  static CpuStat cs{};

  static inline size_t getProcessMemoryUsage(){
    /**
     * Returns the current resident set size (physical memory use) measured
     * in bytes, or zero if the value cannot be determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof memCounter))
        return (size_t)memCounter.WorkingSetSize;
    return (size_t)0; /* get process mem info failed */

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size;
    return (size_t)0; /* query failed */


#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
//    long rss = 0L;
//    FILE* fp = NULL;
//    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
//        return (size_t)0L;      /* Can't open? */
//    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
//    {
//        fclose( fp );
//        return (size_t)0L;      /* Can't read? */
//    }
//    fclose( fp );
//    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
    // Physical Memory currently used by current process
    // https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    FILE* file = fopen("/proc/self/status", "r");
    size_t result{0};
    char line[128];

    while (fgets(line, 128, file) != nullptr){
      if (strncmp(line, "VmRSS:", 6) == 0){
        result = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[result-3] = '\0';
        result = size_t(atol(p));
        break;
      }
    }
    fclose(file);
    return result*1000;
#else
    // AIX, BSD, Solaris, and Unknown OS
    return (size_t)0L;          /* Unsupported. */

#endif
  }
  static inline size_t getProcessMaxMemoryUsage(){
    /**
     * Returns the peak (maximum so far) resident set size (physical
     * memory use) measured in bytes, or zero if the value cannot be
     * determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.PeakWorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size_max;
    return (size_t)0; /* query failed */

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    struct rusage rusage;
    if (!getrusage(RUSAGE_SELF, &rusage))
        return (size_t)rusage.ru_maxrss;
    return (size_t)0; /* query failed */

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    // AIX and Solaris
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);
#else
    // Unknown OS
    return (size_t)0L;          /* Unsupported. */
#endif
  }
  static inline double getCpuUsageByProcess(){
    return cs.getCpuUsageByProcess();
  }
  static inline long getProcessMemoryUsageDiffSinceLastCall(){
    size_t currentProcessMemoryUsage = getProcessMemoryUsage();
    long outVal = static_cast<long>(currentProcessMemoryUsage) - static_cast<long>(Hardware::lastProcessMemoryUsage);
    Hardware::lastProcessMemoryUsage = currentProcessMemoryUsage;
    return outVal;
  }
  static inline double getFreeDiskSpacePercent( const std::string& path_ ){
    return double( getFreeDiskSpace(path_) )/double( getTotalDiskSpace(path_) );
  }
  static inline unsigned long long getFreeDiskSpace( const std::string& path_ ){
    struct statvfs stat{};
    if( statvfs(path_.c_str(), &stat) != 0 ){ return 0; }
    return stat.f_bsize * static_cast<unsigned long long>(stat.f_bfree);
  }
  static inline unsigned long long getTotalDiskSpace( const std::string& path_ ){
    struct statvfs stat{};
    if( statvfs(path_.c_str(), &stat) != 0 ){ return 0; }
    return stat.f_bsize * static_cast<unsigned long long>(stat.f_blocks);
  }
  static inline int getTerminalWidth(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    outWith = (int)(csbi.dwSize.X);
//    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize winSize{};
    ioctl(fileno(stdout), TIOCGWINSZ, &winSize);
    outWith = (int)(winSize.ws_col);
//    outWith = (int)(winSize.ws_row);
#elif defined(__SWITCH__)
    outWith = consoleGetDefault()->consoleWidth;
#endif // Windows/Linux
    return outWith;
  }
  static inline int getTerminalHeight(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//    outWith = (int)(csbi.dwSize.X);
    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize w{};
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
//    outWith = (int)(w.ws_col);
    outWith = (int)(w.ws_row);
#elif defined(__SWITCH__)
    outWith = consoleGetDefault()->consoleHeight;
#endif // Windows/Linux
    return outWith;
  }
  static inline std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_) {
    // Inspired from: https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
    std::array<char, 128> buffer{};
    std::string resultStr;
    std::vector<std::string> output;
#if defined(_WIN32)
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd_.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_.c_str(), "r"), pclose);
#endif
    if (!pipe) {
//      throw std::runtime_error("popen() failed!");
    }
    else{
      while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        resultStr += buffer.data();
      }
      output = GenericToolbox::splitString(resultStr, "\n", true);
    }
    return output;
  }

}


// Time tools
namespace GenericToolbox{

  static inline std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_){

    std::stringstream ss;

    if( nbMicroSec_ < 0 ){
      ss << "-";
      nbMicroSec_ = -nbMicroSec_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(fabs(nbMicroSec_));
    if     ( (reducedVal = (reducedVal / 1000)) < 9 ){ // print in ms?
      ss << nbMicroSec_ << "us";                       // <- no
    }
    else if( (reducedVal = (reducedVal / 1000)) < 3 ){ // print in s?
      ss << nbMicroSec_/1E3 << "ms";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in min?
      ss << nbMicroSec_/1E6 << "s";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in h?
      ss << nbMicroSec_/1E6/60. << "min";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in d?
      ss << nbMicroSec_/1E6/3600. << "h";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in y?
      ss << nbMicroSec_/1E6/3600./24. << "d";
    }
    else {
      ss << nbMicroSec_/1E6/3600./24./365.25 << "y";
    }
    return ss.str();
  }
  static inline std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(double(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_)));
  }
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_){
    return GenericToolbox::parseTimeUnit(double(getElapsedTimeSinceLastCallInMicroSeconds(instance_)));
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
            newTimePoint - Internals::_lastTimePointMapStr_[key_]
    );
    Internals::_lastTimePointMapStr_[key_] = newTimePoint;
    return microseconds.count();
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_){
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
            newTimePoint - Internals::_lastTimePointMap_[instance_]
    );
    Internals::_lastTimePointMap_[instance_] = newTimePoint;
    return microseconds.count();
  }
  static inline std::string getNowDateString(const std::string& dateFormat_){
    std::stringstream ss;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4)
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    char buffer[128];
    std::strftime(buffer, sizeof(buffer), dateFormat_.c_str(), timeinfo);

    ss << buffer;
#else
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    ss << std::put_time(std::localtime(&in_time_t), dateFormat_.c_str());
#endif
    return ss.str();
  }

}

//! Polymorphism Tools
namespace GenericToolbox{

  template<class Derived, class ObjClass> static inline bool isDerivedFrom(ObjClass* objPtr_){
    return dynamic_cast< Derived* >( objPtr_ ) != nullptr;
  }
  template<class Derived, class Base> static inline bool isDerivedType(Base* objPtr_){
    return std::type_index(typeid(*objPtr_)) == std::type_index(typeid(Derived));
  }

}

// Misc Tools
namespace GenericToolbox{

  static inline std::string getClassName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    if (colons == std::string::npos)
      return "::";
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = colons - begin;

    return PRETTY_FUNCTION_.substr(begin, end);
  }
  static inline std::string getMethodName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = PRETTY_FUNCTION_.rfind('(') - begin;

    return PRETTY_FUNCTION_.substr(begin, end) + "()";
  }

}


// Macro Tools
#define GT_INTERNALS_VA_TO_STR(...) #__VA_ARGS__

#define GT_INTERNALS_ENUM_EXPANDER(_enumName_, _intOffset_, _v1_, ...)\
  enum _enumName_ { _v1_ =  _intOffset_, __VA_ARGS__, _enumName_##_OVERFLOW };\
  namespace _enumName_##EnumNamespace{\
    static const char *enumNamesAggregate = GT_INTERNALS_VA_TO_STR(_v1_, __VA_ARGS__); \
                                                                      \
    static inline size_t getEnumSize(){\
      return std::count(&enumNamesAggregate[0], &enumNamesAggregate[strlen(enumNamesAggregate)], ',')+1;\
    }                                                                 \
    static inline std::string getEnumStr(int enumValue_){\
      enumValue_ -= (_intOffset_);\
      if( enumValue_ < 0 || enumValue_ >= int(getEnumSize()) ) throw std::runtime_error("invalid enum.");\
      std::string out; std::stringstream ss{enumNamesAggregate};\
      while (enumValue_-- >= 0) { std::getline(ss, out, ','); } \
      return GenericToolbox::trimString(out, " ");\
    }\
    static inline std::vector<std::string> getEnumNamesList(){              \
      std::vector<std::string> out(getEnumSize());                    \
      for(int iEnum = 0 ; iEnum < int(out.size()) ; iEnum++){ out[iEnum] = getEnumStr(iEnum+(_intOffset_)); } \
      return out;              \
    }                                                              \
    static inline std::vector<_enumName_> getEnumList(){                   \
      std::vector<_enumName_> output(_enumName_##_OVERFLOW);         \
      for( int iIndex = _intOffset_ ; iIndex < _enumName_##_OVERFLOW ; iIndex++ ){     \
        output.at(iIndex) = (static_cast<_enumName_>(iIndex));      \
      }                                                            \
      return output;\
    }\
    static inline std::string toString(int enumValue_, bool excludeEnumName_ = false){      \
      if( excludeEnumName_ ) return getEnumStr(enumValue_);        \
      return {(#_enumName_) + std::string{"::"} + getEnumStr(enumValue_)};\
    }\
    static inline std::string toString(_enumName_ enumValue_, bool excludeEnumName_ = false){\
      return _enumName_##EnumNamespace::toString(static_cast<int>(enumValue_), excludeEnumName_);       \
    }\
    static inline int toEnumInt(const std::string& enumStr_, bool throwIfNotFound_ = false){\
      for( int enumIndex = _intOffset_ ; enumIndex < _enumName_::_enumName_##_OVERFLOW ; enumIndex++ ){             \
        if( _enumName_##EnumNamespace::toString(enumIndex) == enumStr_ ){ return enumIndex; } \
        if( _enumName_##EnumNamespace::toString(enumIndex, true) == enumStr_ ){ return enumIndex; } \
      }                                                            \
      if( throwIfNotFound_ ){                                        \
        std::cout << "Could not find \"" << enumStr_ << "\" in: " << GenericToolbox::parseVectorAsString(getEnumNamesList()) << std::endl; \
        throw std::runtime_error( enumStr_ + " not found in " + #_enumName_ );   \
      }                                                             \
/*      return _intOffset_ - 1; */ \
      return int(_enumName_##_OVERFLOW); /* returns invalid value */\
    }\
    static inline _enumName_ toEnum(const std::string& enumStr_, bool throwIfNotFound_ = false){                         \
      return static_cast<_enumName_>(_enumName_##EnumNamespace::toEnumInt(enumStr_, throwIfNotFound_));  \
    }\
  }

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
