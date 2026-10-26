#pragma once

#ifdef CPP_GENERIC_TOOLBOX_STRING_COMPILED
namespace GenericToolbox {
  namespace ColorCodes{
#ifndef CPP_GENERIC_TOOLBOX_NOCOLOR
    // reset
    const char* resetColor{"\x1b[0m"};

    // background color
    const char* redBackground{"\x1b[41m"};
    const char* greenBackground{"\x1b[42m"};
    const char* yellowBackground{"\x1b[43m"};
    const char* blueBackground{"\x1b[44m"};
    const char* magentaBackground{"\x1b[45m"};
    const char* cyanBackground{"\x1b[46m"};
    const char* greyBackground{"\x1b[47m"};

    // text color
    const char* redText{"\x1b[1;31m"};
    const char* greenText{"\x1b[1;32m"};
    const char* yellowText{"\x1b[1;33m"};
    const char* blueText{"\x1b[1;34m"};
    const char* magentaText{"\x1b[1;35m"};
    const char* cyanText{"\x1b[1;36m"};
    const char* whiteText{"\x1b[1;37m"};

    // lighter text color
    const char* redLightText{"\x1b[1;91m"};
    const char* greenLightText{"\x1b[1;92m"};
    const char* yellowLightText{"\x1b[1;93m"};
    const char* blueLightText{"\x1b[1;94m"};
    const char* magentaLightText{"\x1b[1;95m"};
    const char* cyanLightText{"\x1b[1;96m"};
    const char* whiteLightText{"\x1b[1;97m"};
    const char* greyText{"\x1b[1;90m"};
#else
    // reset
    const char* resetColor{};

    // background color
    const char* redBackground{};
    const char* greenBackground{};
    const char* yellowBackground{};
    const char* blueBackground{};
    const char* magentaBackground{};
    const char* cyanBackground{};
    const char* greyBackground{};

    // text color
    const char* redText{};
    const char* greenText{};
    const char* yellowText{};
    const char* blueText{};
    const char* magentaText{};
    const char* cyanText{};
    const char* whiteText{};

    const char* redLightText{};
    const char* greenLightText{};
    const char* yellowLightText{};
    const char* blueLightText{};
    const char* magentaLightText{};
    const char* cyanLightText{};
    const char* whiteLightText{};
    const char* greyText{};
#endif
    const std::array<const char*, 6> rainbowColorList{redText, greenText, yellowText, blueText, magentaText, cyanText};
  }

}
#endif

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_STRING_STATIC bool hasSubStr(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( ignoreCase_ ){ return toLowerCase(str_).find( toLowerCase( subStr_ ) ) != std::string::npos; }
    return str_.find( subStr_ ) != std::string::npos;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool startsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).begin() );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.begin() );
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool endsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).end() - long(subStrLower.size()) );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.end() - long(subStr_.size()) );
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool isMatching(const std::string& str_, const std::string& pattern_) {
    // List of regex special characters that need escaping
    const std::string specialChars = R"(\.^$|()[]{}+?)";

    // Escape all special characters in pattern_
    std::string escapedPattern;
    for( char c : pattern_ ){
      // Add escape character
      if (specialChars.find(c) != std::string::npos) { escapedPattern += '\\';  }
      escapedPattern += c;
    }

    // Replace '*' wildcard with regex equivalent ".*"
    std::regex pattern(std::regex_replace(escapedPattern, std::regex("\\*"), ".*"));
    return std::regex_match(str_, pattern);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string addUpDownBars(const std::string& str_, bool stripUnicode_) {
    std::stringstream ss;
    size_t strLength = str_.size();
    if( stripUnicode_ ) strLength = GenericToolbox::stripUnicode(str_).size();
    std::string bar = GenericToolbox::repeatString("─", int(strLength));
    ss << bar << std::endl << str_ << std::endl << bar;
    return ss.str();
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string highlightIf(bool condition_, const std::string& text_) {
    std::stringstream ss;
    ss << (condition_ ? ColorCodes::redBackground : "" );
    ss << text_;
    ss << ( condition_ ? ColorCodes::resetColor : "" );
    return ss.str();
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_) {
    std::string outputString;
    std::string inputStrStripped;
    stripUnicode_ ? inputStrStripped = GenericToolbox::stripUnicode(inputStr_) : inputStrStripped = inputStr_;
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

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toLowerCase(const std::string &inputStr_) {
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripUnicode(const std::string &inputStr_) {
    std::string outputStr(inputStr_);

    if( GenericToolbox::hasSubStr(outputStr, "\033") ){
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

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_, std::vector<std::string>* argBuffer_) {
    size_t iChar{0}; std::string out;
    char currentChar;
    while( iChar < inputStr_.size() ){

      // buffering
      currentChar = inputStr_[iChar++];

      // opening bracket?
      if ( currentChar == bra_ ){
        if(argBuffer_!= nullptr){ argBuffer_->emplace_back(); }

        int nestedLevel{0};
        while( iChar < inputStr_.size() ){

          // buffering
          currentChar = inputStr_[iChar++];

          // opening a nested bracket?
          if( currentChar == bra_ ) { nestedLevel++; }

          // closing a bracket?
          if( currentChar == ket_ ) {
            if( nestedLevel == 0 ){ break; } // closing the bracket -> safely leave the loop
            else                  { nestedLevel--; } // closing a nested bracket
          }

          // buffering bracket content
          if(argBuffer_!= nullptr){ argBuffer_->back() += currentChar; }

          // sanity check -> this loop is supposed to be left with "break;"
          if( iChar == inputStr_.size() and not allowUnclosed_ ){
            throw std::runtime_error("unclosed bracket: " + inputStr_);
          }
        }
      }
      else{ out += currentChar; }
    }

//    std::cout << inputStr_ << " -> " << out;
//    if(argBuffer_!= nullptr) std::cout << " args:" << GenericToolbox::toString(*argBuffer_);
//    std::cout << std::endl;

    return out;
  }
}

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
#else
namespace GenericToolbox {
namespace StringManagementUtils {
#ifdef CPP_GENERIC_TOOLBOX_STRING_COMPILED
std::regex ansiRegex("\033((\\[((\\d+;)*\\d+)?[A-DHJKMRcfghilmnprsu])|\\(|\\))");
#endif
}
}
#endif

namespace GenericToolbox {
CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getPrintSize(const std::string& str_) {
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

CPP_GENERIC_TOOLBOX_STRING_STATIC size_t getNLines(const std::string& str_) {
    if( str_.empty() ) return 0;
    return std::count(str_.begin(), str_.end(), '\n')+1;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string repeatString(const std::string &inputStr_, int amount_) {
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string trimString(const std::string &inputStr_, const std::string &strToTrim_) {
    std::string outputStr(inputStr_);
    GenericToolbox::trimInputString( outputStr, strToTrim_ );
    return outputStr;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar) {
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

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar) {
    std::string outStr = inputStr_;
    GenericToolbox::indentInputString(outStr, indentCount_, indentChar);
    return outStr;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string removeRepeatedCharacters(const std::string &inputStr_, const std::string &repeatedChar_) {
    std::string outStr = inputStr_;
    GenericToolbox::removeRepeatedCharInsideInputStr(outStr, repeatedChar_);
    return outStr;
  }

CPP_GENERIC_TOOLBOX_STRING_INLINE std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_) {
    std::string stripped_str = input_str_;
    GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_, substr_to_replace_);
    return stripped_str;
  }

CPP_GENERIC_TOOLBOX_STRING_INLINE std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_) {
    std::string stripped_str = input_str_;
    if(substr_to_look_for_.size() != substr_to_replace_.size()){
      throw std::runtime_error("vec size mismatch btw substr_to_look_for_(" + std::to_string(substr_to_look_for_.size()) + ") != substr_to_replace_(" + std::to_string(substr_to_replace_.size()) + ")");
    }
    for( size_t iSub = 0 ; iSub < substr_to_look_for_.size() ; iSub++ ){
      GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_[iSub], substr_to_replace_[iSub]);
    }
    return input_str_;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseDecimalValue(double val_, const std::string& format_, bool allowSubOne_) {
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

CPP_GENERIC_TOOLBOX_STRING_INLINE std::string parseUnitPrefix(double val_, int maxPadSize_) {
    std::stringstream ss;

    if( val_ < 0 ){
      ss << "-";
      val_ = -val_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(std::abs(val_));
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

CPP_GENERIC_TOOLBOX_STRING_INLINE std::string parseSizeUnits(double sizeInBytes_) {
    return parseUnitPrefix(sizeInBytes_) + "B";
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::vector<std::string> splitString(const std::string &inputString_, const std::string &delimiter_, bool removeEmpty_) {
    if( delimiter_.empty() ){ throw std::invalid_argument("splitString: delimiter must not be empty"); }

    std::vector<std::string> outputSliceList;
    size_t begin{0};
    size_t end;
    while( (end = inputString_.find(delimiter_, begin)) != std::string::npos ){
      if( not removeEmpty_ or end != begin ){
        outputSliceList.emplace_back(inputString_, begin, end - begin);
      }
      begin = end + delimiter_.size();
    }
    if( not removeEmpty_ or begin != inputString_.size() ){
      outputSliceList.emplace_back(inputString_, begin);
    }
    return outputSliceList;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string formatString( const std::string& strToFormat_ ) {
    return strToFormat_;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_) {
    size_t index = 0;
    while ((index = input_str_.find(substr_to_look_for_, index)) != std::string::npos) {
      input_str_.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_) {
    std::string doubledCharStr = doubledChar_+doubledChar_;
    std::string lastStr;
    do{
      lastStr = inputStr_;
      GenericToolbox::replaceSubstringInsideInputString(inputStr_, doubledCharStr, doubledChar_);
    } while( lastStr != inputStr_ );
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_) {
    if( GenericToolbox::endsWith(inputStr_, trailingChar_) ){
      inputStr_.erase(inputStr_.size() - trailingChar_.size());
    }
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar) {
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

CPP_GENERIC_TOOLBOX_STRING_STATIC void trimInputString(std::string &inputStr_, const std::string &strToTrim_) {
    while( GenericToolbox::startsWith(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(strToTrim_.size(), inputStr_.size());
    }
    while( GenericToolbox::endsWith(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(0, inputStr_.size() - strToTrim_.size());
    }
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC void removeEscapeCodes(std::string &inputStr_) {
    // https://www.eso.org/~ndelmott/ascii.html
    std::string escapeCodeStr{0x01};
    for( char escapeCode = 0x01 ; escapeCode <= 0x1F ; escapeCode++ ){
      escapeCodeStr[0] = escapeCode;
      GenericToolbox::replaceSubstringInsideInputString(inputStr_, escapeCodeStr, "");
    }
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string toHex(const void* address_, size_t nBytes_) {
    std::stringstream ss(std::string(2*nBytes_, 0));
    unsigned char* address{(unsigned char*)(address_) + nBytes_-1};
    do{ ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(*(address--)); } while(address >= address_);
    return ss.str();
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool toBool(const std::string& str) {
    auto result = false;    // failure to assert is false

    std::istringstream is(str);
    // first try simple integer conversion
    is >> result;

    if (is.fail()) {
      // simple integer failed; try boolean
      is.clear();
      is >> std::boolalpha >> result;
    }

    if( is.fail() ){ throw std::invalid_argument( str + " is not convertable to bool" ); }

    return result;
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseIntAsString(int intToFormat_) {
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

CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ ) {
    return hasSubStr(str_, subStr_, ignoreCase_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ ) {
    return startsWith(str_, subStr_, ignoreCase_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ ) {
    return endsWith(str_, subStr_, ignoreCase_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_) {
    return toString(vector_, jumpLine_, indentLine_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<unsigned char> &vector_, bool jumpLine_, bool indentLine_) {
    return toString(vector_, jumpLine_, indentLine_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string parseVectorAsString(const std::vector<char> &vector_, bool jumpLine_, bool indentLine_) {
    return toString(vector_, jumpLine_, indentLine_);
  }

CPP_GENERIC_TOOLBOX_STRING_STATIC std::string stripStringUnicode(const std::string &inputStr_) {
    return stripUnicode(inputStr_);
  }
}
