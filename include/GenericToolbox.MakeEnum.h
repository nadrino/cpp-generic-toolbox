//
// Created by Nadrino on 13/12/2023.
//


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"


/*
 *  example of usage (in a src or header file):
 *   #define MAKE_ENUM \
 *    ENUM_NAME( MyEnum ) \
 *    ENUM_TYPE( size_t ) \
 *    ENUM_ENTRY( FOO, 0 ) \
 *    ENUM_ENTRY( BAR, 10 ) \
 *    ENUM_ENTRY( BOO )
 *   #include "GenericToolbox.MakeEnum.h"
 *   #undef MAKE_ENUM
 *   int i = MyEnum::FOO;
 *   std::string s = MyEnum::toString(i);
 */


// only do it if an enum has been created
#ifdef MAKE_ENUM

// define temp macros
#define TEMP_GET_OVERLOADED_MACRO2(_1,_2,NAME,...) NAME
#define TEMP_FIRST_ARG_TO_STR(entry_, ...) #entry_
#define TEMP_FIRST_ARG(entry_, ...) entry_
#define TEMP_ENUM_ENTRY2(entry_, val_) entry_ = val_,
#define TEMP_ENUM_ENTRY1(entry_) entry_,
#define TEMP_ENUM_OVERFLOW2(entry_, val_) entry_ = val_
#define TEMP_ENUM_OVERFLOW1(entry_) entry_

#define TEMP_MERGE_DEPLOY(X, Y) X##Y
#define TEMP_MERGE(X, Y) TEMP_MERGE_DEPLOY(X, Y) // use a proxy for evaluating X and Y before merging

#define TEMP_GET_SECOND(_1, _2, ...) _2
#define TEMP_HANDLE_DEFAULT(default_, ...) TEMP_GET_SECOND(dummy,##__VA_ARGS__, default_)
#define TEMP_DISPATCH_ENUM_TYPE( val ) TEMP_HANDLE_DEFAULT(int, val)
#define TEMP_DISPATCH_ENUM_OVERFLOW( val ) TEMP_HANDLE_DEFAULT(ENUM_OVERFLOW, val)

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
  typedef TEMP_DISPATCH_ENUM_TYPE( MAKE_ENUM ) EnumType;
#undef ENUM_TYPE
#define ENUM_TYPE(...)  // nothing

#undef ENUM_NAME
#define ENUM_NAME(name_) name_
  enum TEMP_MERGE(MAKE_ENUM, EnumType) : EnumType{
#undef ENUM_NAME
#define ENUM_NAME( ... ) // nothing

// MAKE_ENUM -> return only enum entries
#undef ENUM_ENTRY
#define ENUM_ENTRY( ... ) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_ENTRY2, TEMP_ENUM_ENTRY1)(__VA_ARGS__)
    MAKE_ENUM
#undef ENUM_ENTRY
#define ENUM_ENTRY( ... ) // nothing

#undef ENUM_OVERFLOW
#define ENUM_OVERFLOW( ... ) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_OVERFLOW2, TEMP_ENUM_OVERFLOW1)(__VA_ARGS__)
    TEMP_DISPATCH_ENUM_OVERFLOW(MAKE_ENUM)
  };
#undef ENUM_OVERFLOW
#define ENUM_OVERFLOW(...) TEMP_FIRST_ARG(__VA_ARGS__) // get only the enum name
  static const EnumType overflowValue{ TEMP_DISPATCH_ENUM_OVERFLOW(MAKE_ENUM) };
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
  friend bool operator!=(const StructType& lhs, const StructType& rhs){ return lhs.value != rhs.value; }


  static int getEnumSize(){
    // MAKE_ENUM -> return only enum entries
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG(__VA_ARGS__),
    static const EnumType indices[] = { MAKE_ENUM };
    return sizeof(indices)/sizeof(EnumType);
  }
  static EnumType getEnumVal(int index_){
    static const EnumType indices[] = { MAKE_ENUM };
    if( index_ < 0 or index_ >= int(sizeof(indices)/sizeof(EnumType)) ){ return StructType::overflowValue; }
    return indices[index_];
  }
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) // nothing

  static std::string getEnumEntryToStr(int index_){
    if( index_ < 0 or index_ >= getEnumSize() ){ return {"UNNAMED_ENUM"}; }
// MAKE_ENUM -> return only enum entry names
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) TEMP_FIRST_ARG_TO_STR(__VA_ARGS__),
    static const char *names[] = { MAKE_ENUM };
#undef ENUM_ENTRY
#define ENUM_ENTRY(...) // nothing
    return names[index_];
  }
  static StructType toEnum( const std::string& name_ ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( name_ == getEnumEntryToStr(iEntry) ){
        return getEnumVal(iEntry);
      }
    }
    return StructType::overflowValue;
  }

  static std::string toString( EnumTypeName value_ ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( value_ == getEnumVal(iEntry) ){
        return getEnumEntryToStr(iEntry);
      }
    }
    return {};
  }
  static std::string toString( EnumType value_ ){ return toString( static_cast<EnumTypeName>(value_) ); }
  static inline std::string toString( StructType enum_ ){ return toString( enum_.value ); }
  [[nodiscard]] inline std::string toString() const { return toString( this->value ); }
  friend std::ostream& operator<< (std::ostream& stream, const StructType& this_){ stream << this_.toString(); return stream; }

};


// clean up X macros
#undef ENUM_NAME
#undef ENUM_TYPE
#undef ENUM_ENTRY
#undef ENUM_OVERFLOW

// clean up temp macros
#undef TEMP_DISPATCH_ENUM_OVERFLOW
#undef TEMP_DISPATCH_ENUM_TYPE

#undef TEMP_HANDLE_DEFAULT
#undef TEMP_GET_SECOND

#undef TEMP_MERGE

#undef TEMP_ENUM_ENTRY1
#undef TEMP_ENUM_ENTRY2
#undef TEMP_FIRST_ARG
#undef TEMP_FIRST_ARG_TO_STR
#undef TEMP_GET_OVERLOADED_MACRO2

#endif // #ifdef MAKE_ENUM
