//
// Created by Adrien BLANCHET on 31/10/2022.
//

#ifndef HATMIDASFRONTEND_GENERICTOOLBOX_ODB_H
#define HATMIDASFRONTEND_GENERICTOOLBOX_ODB_H

#include "midas.h"

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <array>





namespace GenericToolbox {
  namespace Midas {

    namespace Logger {

      template<typename T> inline void writeInBank(char* pevent, const std::string& name_, const T& data_);
      template<typename T> inline void writeInBank(char* pevent, const std::string& name_, const std::vector<T>& list_);
      template<typename T, typename L> inline void writeInBank(char* pevent, const std::string& name_, const std::vector<T>& list_, const L& lambda_);
      template<typename T, typename L> inline void writeInBank(char* pevent, const std::string& name_, const T& array_, int size_, const L& getData_);
      template<typename T, typename L, typename LL> inline void writeInBank(char* pevent, const std::string& name_, const T& array_, int size_, const LL& grabSubarray_, int subSize_, const L& getData_);
      template<typename T> WORD getTid(const T& data_);

    }

    namespace Odb{
      // Read-only
      inline HNDLE getKey(const std::string &path_);
      inline DWORD getKeyType(const std::string &path_);
      inline int getKeyNbValues(const std::string &path_);
      template<class T> inline DWORD getTypeId(const T& object_);
      inline bool isKey(const std::string &path_);
      template<class T> inline auto read(const std::string &path_, int index_ = 0) -> T;
      template<class T> inline auto readVector(const std::string &path_) -> std::vector<T>;
      template<class T, size_t N> inline void readArray(const std::string &path_, std::array<T, N>& out_);
      inline std::vector<std::string> ls(const std::string &path_);
      template<class T> inline std::map<std::string, T> fetchMatchingValues(const std::string& regexPath_, const std::string& regexConditionPath_ = "");

      // Write
      inline bool setKey(const std::string &path_, const DWORD& type_);
      template<class T> inline bool write(const std::string &path_, const T &objToWrite_, bool createKeyIsNotPresent_ = false);
      inline bool write(const std::string &path_, const std::string &objToWrite_, bool createKeyIsNotPresent_ = false);

      /* initializeHotLink() make the hotLink definition at the Midas level.
     *
     * It needs the struct hotLinkContainer_ and the hotLinkDescriptor_. Those can be defined by the HOTLINK_DISPATCH macro
     * T& hotLinkStaticDispatcher_ is a static function which specify what to do when a hotLink is triggered.
     * It is supposed to be a function like "void (*)(INT hDB_, INT hkey, void* this_)".
     *
     * Although it is static, one can call back the reference of the involved class with the last arg "this_"
     */
      template <typename C, typename T>
      inline void initializeHotLink(const std::string& odbPath_, C& hotLinkContainer_,
                                    std::vector<const char*>& hotLinkDescriptor_, T& hotLinkStaticDispatcher_,
                                    void* this_= nullptr);
    }

  }
}

/*
 * Tools to dispatch hotlinks easily
 */

#define FIELD_DECL(TYPE_, NAME_, TAG_)  TYPE_ NAME_;
#define FIELD_DESCRIPTOR(TYPE_, NAME_, TAG_)   #NAME_ " = " #TYPE_ " : " #TAG_,

#define DECLARE_STRUCT(STYPE_) struct { STYPE_(FIELD_DECL) }
#define STRUCT_DESCRIPTOR(STYPE_) { "[.]", STYPE_(FIELD_DESCRIPTOR) "", nullptr }

/* HOTLINK_DISPATCH is a macro which helps to deploy the collection of hotlinks.
 * First one need to define a meta macro which holds the definition of each hotlink objects:
 *
 * #define MY_HOTLINK_FIELDS(FIELD_) \
 *   FIELD_(BOOL, runColdStartScript, n) \
 *   FIELD_(BOOL, runPedestalScript, n)
 *
 * Then one can dispatch the hotlink definition with HOTLINK_DISPATCH(hotLinkName, MY_HOTLINK_FIELDS).
 */
#define HOTLINK_DISPATCH(NAME_, HOTLINK_FIELDS_) \
  typedef DECLARE_STRUCT(HOTLINK_FIELDS_) (NAME_); \
  NAME_ NAME_##_Container {};                    \
  std::vector<const char*> NAME_##_Descritor = STRUCT_DESCRIPTOR(HOTLINK_FIELDS_);


#include "implementation/GenericToolbox.Midas.Impl.h"

#endif//HATMIDASFRONTEND_GENERICTOOLBOX_ODB_H
