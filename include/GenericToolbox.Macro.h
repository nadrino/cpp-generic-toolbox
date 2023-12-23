//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_MACRO_H
#define CPP_GENERIC_TOOLBOX_MACRO_H

#define HAS_CPP_17 (__cplusplus >= 201703L)
#define HAS_CPP_14 (__cplusplus >= 201300L)
#define HAS_CPP_11 (__cplusplus >= 201103L)


#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

//! MACROS Tools
#define __CLASS_NAME__ GenericToolbox::getClassName(__PRETTY_FUNCTION__)
//#define __CLASS_NAME__ ( this != nullptr ? typeid(*this).name() )
#define __METHOD_NAME__ GenericToolbox::getMethodName(__PRETTY_FUNCTION__)

#define GET_VAR_NAME_VALUE(var) ( ((std::stringstream&) (std::stringstream() << #var << " = " << (var)) ).str() )
#define GET_VAR_NAME_VALUE_STREAM(var) #var << " = " << var
#define GET_VAR_NAME(var) std::string(#var)

/*
 * ENUM_EXPANDER will create the desired enum and will automatically attach a namespace which provides additional methods
 * For example: "ENUM_EXPAND(MyEnumType, 1, state1, state2, state3)" is equivalent to "enum MyEnumType{state1 = 1, state2, state3};"
 * In addition, you can search of any enum name as a string: "MyEnumTypeEnumNamespace::toString(1)" which will return "state1".
 * */
#define ENUM_EXPANDER(enumName_, intOffset_, v1_, ...) GT_INTERNALS_ENUM_EXPANDER(enumName_, intOffset_, v1_, __VA_ARGS__)

#define BIND_VAR_NAME(var) var, #var
#define BIND_VAR_REF_NAME(var) &(var), #var


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


#endif // CPP_GENERIC_TOOLBOX_MACRO_H
