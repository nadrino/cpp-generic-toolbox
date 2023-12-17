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
#define TEMP_ENUM_OVERFLOW2(entry_, val_) entry_ = val_
#define TEMP_ENUM_OVERFLOW1(entry_) entry_

#define TEMP_MERGE(X, Y) X##Y

#define TEMP_EXPAND(x) x
#define TEMP_ARGS_DUMMY(...) dummy,##__VA_ARGS__
#define TEMP_SELECT_FROM5(_1,_2,_3,_4,_5,num,...) num
#define TEMP_IS_EMPTY_IMPL(...) TEMP_EXPAND(TEMP_SELECT_FROM5(__VA_ARGS__,0,0,0,0,1))
#define TEMP_IS_EMPTY(...) TEMP_EXPAND(TEMP_IS_EMPTY_IMPL(TEMP_ARGS_DUMMY(__VA_ARGS__)))


#define ENUM_NAME(...)  // nothing
#define ENUM_TYPE(...)  // nothing
#define ENUM_ENTRY(...) // nothing
#define ENUM_OVERFLOW(...) // nothing

#undef ENUM_NAME
#define ENUM_NAME(name_) name_
struct MAKE_ENUM {
  typedef MAKE_ENUM StructType;
#undef ENUM_NAME
#define ENUM_NAME(...) // nothing

#undef ENUM_TYPE
#define ENUM_TYPE(type_) type_
#if TEMP_IS_EMPTY( MAKE_ENUM )
  typedef int EnumType;
#else
  typedef MAKE_ENUM EnumType;
#endif
#undef ENUM_TYPE
#define ENUM_TYPE(...)  // nothing

#undef ENUM_NAME
#define ENUM_NAME(name_) name_
  enum TEMP_MERGE(MAKE_ENUM, EnumType) : EnumType {
#undef ENUM_NAME
#define ENUM_NAME(...) // nothing

// MAKE_ENUM -> return only enum entries
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_ENTRY2, TEMP_ENUM_ENTRY1)(__VA_ARGS__)
    MAKE_ENUM
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) // nothing

#undef ENUM_OVERFLOW
#define ENUM_OVERFLOW(...) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_OVERFLOW2, TEMP_ENUM_OVERFLOW1)(__VA_ARGS__)
#if TEMP_IS_EMPTY( MAKE_ENUM )
    ENUM_OVERFLOW
  };
  static const EnumType overflowValue{ENUM_OVERFLOW};
#else
    MAKE_ENUM
  };
#undef ENUM_OVERFLOW
#define ENUM_OVERFLOW(...) TEMP_FIRST_ARG(__VA_ARGS__)
  static const EnumType overflowValue{MAKE_ENUM};
#endif
#undef ENUM_OVERFLOW
#define ENUM_OVERFLOW(...) // nothing



#undef ENUM_NAME
#define ENUM_NAME(name_) name_
  typedef TEMP_MERGE(MAKE_ENUM, EnumType) EnumTypeName;

  // store the actual value
  EnumTypeName value{};

  // can't use "StructType" for CTors
  MAKE_ENUM() = default;
  MAKE_ENUM(EnumTypeName value_) : value(value_) {}
  MAKE_ENUM(EnumType value_) : value(static_cast<EnumTypeName>(value_)) {}
#undef ENUM_NAME
#define ENUM_NAME(name_) // nothing

  StructType& operator=(EnumTypeName value_){ this->value = value_; return *this; }
  StructType& operator=(EnumType value_){ this->value = static_cast<EnumTypeName>(value_); return *this; }

  friend bool operator==(const StructType& lhs, const StructType& rhs){ return lhs.value == rhs.value; }


  static int getEnumSize(){
    // MAKE_ENUM -> return only enum entries
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG(__VA_ARGS__),
    static const EnumType indices[] = { MAKE_ENUM };
    return sizeof(indices)/sizeof(EnumType);
  }
  static EnumType getEnumVal(int index_){
    static const EnumType indices[] = { MAKE_ENUM };
    if( index_ < 0 or index_ > sizeof(indices)/sizeof(EnumType) ){ return StructType::overflowValue; }
    return indices[index_];
  }
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) // nothing

  static std::string getEnumEntryToStr(int index_){
    if( index_ < 0 or index_ > getEnumSize() ){ return {"UNNAMED_ENUM"}; }
// MAKE_ENUM -> return only enum entry names
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG_TO_STR(__VA_ARGS__),
    static const char *names[] = { MAKE_ENUM };
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) // nothing
    return names[index_];
  }
  static inline StructType toEnum( const std::string& name_ ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( name_ == getEnumEntryToStr(iEntry) ){
        return getEnumVal(iEntry);
      }
    }
    return StructType::overflowValue;
  }

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


// clean up X macros
#undef ENUM_NAME
#undef ENUM_TYPE
#undef ENUM_ENTRY
#undef ENUM_OVERFLOW

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

