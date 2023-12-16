//
// Created by Adrien Blanchet on 13/12/2023.
//


// example of usage (in a src or header file):
// #define MAKE_ENUM \
//  ENUM_NAME( MyEnum ) \
//  ENUM_ENTRY( FOO, 0 ) \
//  ENUM_ENTRY( BAR, 10 ) \
//  ENUM_ENTRY( BOO )
// #include "GenericToolbox.MakeEnum.h"
// #undef MAKE_ENUM
// int i = MyEnum::FOO;
// std::string s = MyEnum::toString(i);


// define temp macros
#define TEMP_GET_OVERLOADED_MACRO2(_1,_2,NAME,...) NAME
#define TEMP_FIRST_ARG_TO_STR(entry_, ...) #entry_
#define TEMP_FIRST_ARG(entry_, ...) entry_
#define TEMP_ENUM_ENTRY2(entry_, val_) entry_ = val_,
#define TEMP_ENUM_ENTRY1(entry_) entry_,

#define TEMP_MERGE(X, Y) X##Y

#define TEMP_EXPAND(x) x
#define TEMP_ARGS_DUMMY(...) dummy,##__VA_ARGS__
#define TEMP_SELECT_FROM5(_1,_2,_3,_4,_5,num,...) num
#define TEMP_IS_EMPTY_IMPL(...) TEMP_EXPAND(TEMP_SELECT_FROM5(__VA_ARGS__,0,0,0,0,1))
#define TEMP_IS_EMPTY(...) TEMP_EXPAND(TEMP_IS_EMPTY_IMPL(TEMP_ARGS_DUMMY(__VA_ARGS__)))


// MAKE_ENUM -> return only enum name
#undef ENUM_TYPE
#define ENUM_TYPE(type_) // nothing
#undef ENUM_NAME
#define ENUM_NAME(name_) name_
#undef ENUM_ENTRY
#define ENUM_ENTRY(...)  // do nothing
struct MAKE_ENUM {

  typedef MAKE_ENUM StructType;

#undef ENUM_TYPE
#define ENUM_TYPE(type_) type_
#undef ENUM_NAME
#define ENUM_NAME(name_) // nothing
#if TEMP_IS_EMPTY( MAKE_ENUM )
  typedef int EnumType;
#else
  typedef MAKE_ENUM EnumType;
#endif

#undef ENUM_TYPE
#define ENUM_TYPE(type_) // nothing
#undef ENUM_NAME
#define ENUM_NAME(name_) name_
#undef ENUM_ENTRY
#define ENUM_ENTRY(...)  // do nothing
  enum TEMP_MERGE(MAKE_ENUM, EnumType) : EnumType {

// MAKE_ENUM -> return only enum entries
#undef ENUM_TYPE
#define ENUM_TYPE(...)
#undef ENUM_NAME
#define ENUM_NAME(name_) // do nothing
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_ENTRY2, TEMP_ENUM_ENTRY1)(__VA_ARGS__)
    MAKE_ENUM ENUM_OVERFLOW
  };

#undef ENUM_NAME
#define ENUM_NAME(name_) name_
#undef ENUM_ENTRY
#define ENUM_ENTRY(...)  // do nothing
  typedef TEMP_MERGE(MAKE_ENUM, EnumType) EnumTypeName;

  // store the actual value
  EnumTypeName value{};

  // can't use "StructType" for CTors
  MAKE_ENUM() = default;
  MAKE_ENUM(EnumTypeName value_) : value(value_) {}
  MAKE_ENUM(EnumType value_) : value(static_cast<EnumTypeName>(value_)) {}

  StructType& operator=(EnumTypeName value_){ this->value = value_; return *this; }
  StructType& operator=(EnumType value_){ this->value = static_cast<EnumTypeName>(value_); return *this; }


  static int getEnumSize(){
    // MAKE_ENUM -> return only enum entries
#undef ENUM_NAME
#define ENUM_NAME(name_) // nothing
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG(__VA_ARGS__),
    static const EnumType indices[] = { MAKE_ENUM };
    return sizeof(indices)/sizeof(EnumType);
  }
  static EnumType getEnumVal(int index_){
    static const EnumType indices[] = { MAKE_ENUM };
    if( index_ < 0 or index_ > sizeof(indices)/sizeof(EnumType) ){ return ENUM_OVERFLOW; }
    return indices[index_];
  }

  static std::string getEnumEntryToStr(int index_){
    if( index_ < 0 or index_ > getEnumSize() ){ return {"UNNAMED_ENUM"}; }
// MAKE_ENUM -> return only enum entry names
#undef ENUM_NAME
#define ENUM_NAME(name_) // nothing
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG_TO_STR(__VA_ARGS__),
    static const char *names[] = { MAKE_ENUM };
    return names[index_];
  }
  static inline StructType toEnum( const std::string& name_ ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( name_ == getEnumEntryToStr(iEntry) ){
        return getEnumVal(iEntry);
      }
    }
    return ENUM_OVERFLOW;
  }

//// MAKE_ENUM -> return only enum entries
//#undef ENUM_ENTRY
//#define ENUM_ENTRY(...) TEMP_FIRST_ARG(__VA_ARGS__),
//  static const EnumType indices[] = { MAKE_ENUM };

  [[nodiscard]] inline std::string toString() const {
    std::string out{};
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( value == getEnumVal(iEntry) ){
        return getEnumEntryToStr(iEntry);
      }
    }
    return {"UNNAMED_ENUM"};
  }

};

// clean up temp macros
#undef TEMP_EXPAND
#undef TEMP_ARGS_DUMMY
#undef TEMP_SELECT_FROM5
#undef TEMP_IS_EMPTY_IMPL
#undef TEMP_IS_EMPTY

#undef TEMP_MERGE

#undef TEMP_ENUM_ENTRY1
#undef TEMP_ENUM_ENTRY2
#undef TEMP_FIRST_ARG
#undef TEMP_FIRST_ARG_TO_STR
#undef TEMP_GET_OVERLOADED_MACRO2

// clean up user macros
#undef ENUM_NAME
#undef ENUM_ENTRY

