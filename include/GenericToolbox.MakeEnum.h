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
namespace MAKE_ENUM {

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
  enum MAKE_ENUM : EnumType {

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
  typedef MAKE_ENUM EnumTypeName;

// MAKE_ENUM -> return only enum entry names
#undef ENUM_NAME
#define ENUM_NAME(name_) // nothing
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG_TO_STR(__VA_ARGS__),
  static const char *names[] = { MAKE_ENUM };

// MAKE_ENUM -> return only enum entries
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG(__VA_ARGS__),
  static const EnumType indices[] = { MAKE_ENUM };

  static inline std::string toString( EnumType enumVal_ ){
    std::string out{};
    int nEntries{sizeof(indices)/sizeof(EnumType)};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( enumVal_ == indices[iEntry] ){
        return names[iEntry];
      }
    }
    return {"UNNAMED_ENUM"};
  }

  static inline EnumTypeName toEnum( const std::string& name_ ){
    int nEntries{sizeof(indices)/sizeof(EnumType)};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( name_ == names[iEntry] ){
        return static_cast<EnumTypeName>(indices[iEntry]);
      }
    }
    return ENUM_OVERFLOW;
  }

}

// clean up temp macros
#undef TEMP_EXPAND
#undef TEMP_ARGS_DUMMY
#undef TEMP_SELECT_FROM5
#undef TEMP_IS_EMPTY_IMPL
#undef TEMP_IS_EMPTY

#undef TEMP_ENUM_ENTRY1
#undef TEMP_ENUM_ENTRY2
#undef TEMP_FIRST_ARG
#undef TEMP_FIRST_ARG_TO_STR
#undef TEMP_GET_OVERLOADED_MACRO2

// clean up user macros
#undef ENUM_NAME
#undef ENUM_ENTRY

