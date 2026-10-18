//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_STRING_H
#define CPP_GENERIC_TOOLBOX_STRING_H

#include "GenericToolbox.Build.h"

// ***************************
//! String related tools
// ***************************

#include "GenericToolbox.Macro.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <array>
#include <cmath>
#include <regex>
#include <map>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


// Declaration section
namespace GenericToolbox{

  namespace ColorCodes{
#ifdef CPP_GENERIC_TOOLBOX_STRING_COMPILED
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* resetColor;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* redBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* greenBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* yellowBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* blueBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* magentaBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* cyanBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* greyBackground;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* redText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* greenText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* yellowText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* blueText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* magentaText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* cyanText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* whiteText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* redLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* greenLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* yellowLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* blueLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* magentaLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* cyanLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* whiteLightText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const char* greyText;
    extern CPP_GENERIC_TOOLBOX_STRING_API const std::array<const char*, 6> rainbowColorList;
#else
#ifndef CPP_GENERIC_TOOLBOX_NOCOLOR
    // reset
    static const char* resetColor{"\x1b[0m"};

    // background color
    static const char* redBackground{"\x1b[41m"};
    static const char* greenBackground{"\x1b[42m"};
    static const char* yellowBackground{"\x1b[43m"};
    static const char* blueBackground{"\x1b[44m"};
    static const char* magentaBackground{"\x1b[45m"};
    static const char* cyanBackground{"\x1b[46m"};
    static const char* greyBackground{"\x1b[47m"};

    // text color
    static const char* redText{"\x1b[1;31m"};
    static const char* greenText{"\x1b[1;32m"};
    static const char* yellowText{"\x1b[1;33m"};
    static const char* blueText{"\x1b[1;34m"};
    static const char* magentaText{"\x1b[1;35m"};
    static const char* cyanText{"\x1b[1;36m"};
    static const char* whiteText{"\x1b[1;37m"};

    // lighter text color
    static const char* redLightText{"\x1b[1;91m"};
    static const char* greenLightText{"\x1b[1;92m"};
    static const char* yellowLightText{"\x1b[1;93m"};
    static const char* blueLightText{"\x1b[1;94m"};
    static const char* magentaLightText{"\x1b[1;95m"};
    static const char* cyanLightText{"\x1b[1;96m"};
    static const char* whiteLightText{"\x1b[1;97m"};
    static const char* greyText{"\x1b[1;90m"};
#else
    // reset
    static const char* resetColor{};

    // background color
    static const char* redBackground{};
    static const char* greenBackground{};
    static const char* yellowBackground{};
    static const char* blueBackground{};
    static const char* magentaBackground{};
    static const char* cyanBackground{};
    static const char* greyBackground{};

    // text color
    static const char* redText{};
    static const char* greenText{};
    static const char* yellowText{};
    static const char* blueText{};
    static const char* magentaText{};
    static const char* cyanText{};
    static const char* whiteText{};

    static const char* redLightText{};
    static const char* greenLightText{};
    static const char* yellowLightText{};
    static const char* blueLightText{};
    static const char* magentaLightText{};
    static const char* cyanLightText{};
    static const char* whiteLightText{};
    static const char* greyText{};
#endif
    static const std::array<const char*, 6> rainbowColorList{redText, greenText, yellowText, blueText, magentaText, cyanText};
#endif
  }

  // -- Quick checks
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool hasSubStr(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool startsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool endsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool isMatching(const std::string& str_, const std::string& pattern_);

  // -- Conversion Tools
  template<typename T, typename TT> static std::string toString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true);
  template<typename T> static std::string toString(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_ = false);
  template<> std::string toString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_);
  template<> std::string toString(const std::vector<unsigned char> &vector_, bool jumpLine_, bool indentLine_);
  template<> std::string toString(const std::vector<char> &vector_, bool jumpLine_, bool indentLine_);
  template<typename T, size_t N> static std::string toString(const std::array<T, N>& array_);
  template <typename T1, typename T2> static std::string toString(const std::map<T1, T2>& map_, bool enableLineJump_ = true);
  template <typename T1, typename T2, typename T3> static std::string toString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_ = true);

  // -- Transformations
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toLowerCase(const std::string &inputStr_);

  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripUnicode(const std::string &inputStr_);

  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_ = true, std::vector<std::string>* argBuffer_ = nullptr);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string repeatString(const std::string &inputStr_, int amount_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar = ' ');
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string removeRepeatedCharacters(const std::string& inputStr_, const std::string &repeatedChar_);
  template<typename T> static std::string joinVectorString(const std::vector<T> &stringList_, const std::string &delimiter_, int beginIndex_ = 0, int endIndex_ = 0);
  template<typename... Args> static std::string joinAsString(const std::string &delimiter_, const Args&... args);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::vector<std::string> splitString(const std::string& inputString_, const std::string &delimiter_, bool removeEmpty_ = false);

  // -- Transformations (Fast)
  CPP_GENERIC_TOOLBOX_STRING_STATIC void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  CPP_GENERIC_TOOLBOX_STRING_STATIC void trimInputString(std::string &inputStr_, const std::string &strToTrim_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeEscapeCodes(std::string &inputStr_);

  // -- Parsing
  CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getPrintSize(const std::string& str_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getNLines(const std::string& str_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseDecimalValue(double val_, const std::string& format_="%s%s", bool allowSubOne_ = true);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseUnitPrefix(double val_, int maxPadSize_=-1);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseSizeUnits(double sizeInBytes_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string formatString( const std::string& strToFormat_ ); // overrider: make sure this is the one used when no extra args are provided.
  template<typename ... Args> static std::string formatString(const std::string& strToFormat_, Args ... args );


  // -- Representations
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toHex(const void* address_, size_t nBytes_);
  template<typename T> static std::string toHex(const T& val_);
  template<typename T> static std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool toBool(const std::string& str);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseIntAsString(int intToFormat_);

  // -- Aesthetic
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string addUpDownBars(const std::string& str_, bool stripUnicode_ = true);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string highlightIf(bool condition_, const std::string& text_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_ = true);

}


// Implementation section
namespace GenericToolbox {

  // -- Quick checks
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool hasSubStr(const std::string& str_, const std::string& subStr_, bool ignoreCase_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool startsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool endsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool isMatching(const std::string& str_, const std::string& pattern_);

  // -- Aesthetic
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string addUpDownBars(const std::string& str_, bool stripUnicode_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string highlightIf(bool condition_, const std::string& text_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_);

  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toLowerCase(const std::string &inputStr_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripUnicode(const std::string &inputStr_);

  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_, std::vector<std::string>* argBuffer_);

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
#else
  namespace StringManagementUtils{
    #ifdef CPP_GENERIC_TOOLBOX_STRING_COMPILED
extern CPP_GENERIC_TOOLBOX_STRING_API std::regex ansiRegex;
#else
static std::regex ansiRegex("\033((\\[((\\d+;)*\\d+)?[A-DHJKMRcfghilmnprsu])|\\(|\\))");
#endif
  }
#endif
  CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getPrintSize(const std::string& str_);

  CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getNLines(const std::string& str_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string repeatString(const std::string &inputStr_, int amount_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string removeRepeatedCharacters(const std::string &inputStr_, const std::string &repeatedChar_);
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
  template<typename... Args> static std::string joinAsString(const std::string& delimiter, const Args&... args) {
    std::stringstream ss;
    int dummy[] = {0, ((void)(ss << args << delimiter), 0)...};
    (void)dummy;  // Avoid unused variable warning
    std::string result = ss.str();
    if( not result.empty() ) { result.resize(result.size() - delimiter.size()); } // remove the last delimiter
    return result;
  }

  std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseDecimalValue(double val_, const std::string& format_, bool allowSubOne_);
  std::string parseUnitPrefix(double val_, int maxPadSize_);
  std::string parseSizeUnits(double sizeInBytes_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::vector<std::string> splitString(const std::string &inputString_, const std::string &delimiter_, bool removeEmpty_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string formatString( const std::string& strToFormat_ );
  template<typename ... Args> std::string formatString(const std::string& strToFormat_, Args ... args) {
    size_t size = snprintf(nullptr, 0, strToFormat_.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, strToFormat_.c_str(), args ...);
    return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
  }

  CPP_GENERIC_TOOLBOX_STRING_STATIC void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void trimInputString(std::string &inputStr_, const std::string &strToTrim_);
  CPP_GENERIC_TOOLBOX_STRING_STATIC void removeEscapeCodes(std::string &inputStr_);

  // -- conversions
  template<typename T, typename TT> static std::string toString(const T& iterable_, const TT& toStrFct_, bool jumpLine_, bool indentLine_){
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
  template<typename T> static std::string toString(const std::vector<T> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const T& elm_){ return elm_; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const std::string& elm_){ return std::string{"\""+elm_+"\""}; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<unsigned char> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const unsigned char& elm_){ return std::string{"0x"+GenericToolbox::toHex(elm_)}; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<char> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const unsigned char& elm_){ return std::string{"0x"+GenericToolbox::toHex(elm_)}; }, jumpLine_, indentLine_);
  }
  template<typename T, size_t N> static std::string toString(const std::array<T, N>& array_){
    return std::string(std::begin(array_), std::end(array_));
  }
  template <typename T1, typename T2> static std::string toString(const std::map<T1, T2>& map_, bool enableLineJump_){
    return GenericToolbox::toString(
        map_,
        [&](const std::pair<T1, T2>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": " << elm_.second << " }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }
  template <typename T1, typename T2, typename T3> static std::string toString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_){
    return GenericToolbox::toString(
        map_,
        [&](const std::pair<T1, std::pair<T2,T3>>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": {" << elm_.second.first << ", " << elm_.second.second << "} }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }

  template<typename T> static std::string toHex(const T& val_){ return toHex(&val_, sizeof(val_)); }
  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toHex(const void* address_, size_t nBytes_);
  template<typename T> static std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_) {
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
  CPP_GENERIC_TOOLBOX_STRING_STATIC bool toBool(const std::string& str);

  CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseIntAsString(int intToFormat_);

  // deprecated
  GT_DEPRECATED("renamed: hasSubStr") CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  GT_DEPRECATED("renamed: startsWith") CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  GT_DEPRECATED("renamed: endsWith") CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);

  template<typename T, typename TT> GT_DEPRECATED("renamed: toString") static std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true){
    return toString(iterable_, toStrFct_, jumpLine_, indentLine_);
  }
  template <typename T> GT_DEPRECATED("renamed: toString") static std::string parseVectorAsString(const std::vector<T>& vector_, bool jumpLine_=true, bool indentLine_=true){
    return toString(vector_, jumpLine_, indentLine_);
  }
  GT_DEPRECATED("renamed: toString") CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_=true, bool indentLine_=true);
  GT_DEPRECATED("renamed: toString") CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<unsigned char> &vector_, bool jumpLine_=true, bool indentLine_=true);
  GT_DEPRECATED("renamed: toString") CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<char> &vector_, bool jumpLine_=true, bool indentLine_=true);
  template <typename T1, typename T2> GT_DEPRECATED("renamed: toString") static std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_=true){ return toString(map_, enableLineJump_); }
  template <typename T1, typename T2, typename T3> GT_DEPRECATED("renamed: toString") static std::string parseMapAsString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_){ return toString(map_, enableLineJump_); }

  GT_DEPRECATED("renamed: stripUnicode") CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripStringUnicode(const std::string &inputStr_);

  template<typename T> static std::string toHexString(T integerVal_, size_t nbDigit_){
    std::stringstream stream;
    stream << "0x" << toHex(&integerVal_, sizeof(integerVal_));
    if( nbDigit_ == 0 ) return stream.str();
    else return "0x" + stream.str().substr(2 + sizeof(T)*2 - nbDigit_, nbDigit_);
  }

}


#ifndef CPP_GENERIC_TOOLBOX_STRING_COMPILED
#include "implementation/GenericToolbox.String.impl.h"
#endif

#endif // CPP_GENERIC_TOOLBOX_STRING_H
