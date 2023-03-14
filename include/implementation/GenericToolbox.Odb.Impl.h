//
// Created by Adrien BLANCHET on 31/10/2022.
//

#ifndef HATMIDASFRONTEND_GENERICTOOLBOX_ODB_IMPL_H
#define HATMIDASFRONTEND_GENERICTOOLBOX_ODB_IMPL_H

#include "GenericToolbox.h"

#include "midas.h"

#include "string"
#include "map"
#include "vector"
#include "iostream"


extern HNDLE hDB; // defined with Midas

namespace GenericToolbox {
  namespace Odb{

    inline HNDLE getKey(const std::string &path_){
      HNDLE hDir = 0;
      HNDLE hKey = 0;
      int errorCode = db_find_key(hDB, hDir, path_.c_str(), &hKey);
      if( errorCode == DB_SUCCESS ){
        return hKey;
      }
      else{
        return 0;
      }
    }
    inline DWORD getKeyType(const std::string &path_){
      HNDLE hKey = 0;
      INT type, num_values, item_size;
      db_get_key_info(hDB, hKey, (char*) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);
      return type;
    }
    inline bool isKey(const std::string &path_){
      return ( GenericToolbox::Odb::getKey(path_) != 0 );
    }
    template<class T> inline auto read(const std::string &path_, int index_) -> T {
      int errorCode;
      HNDLE hKey = getKey(path_);

      if( hKey == 0 ){
        throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
      }

      DWORD typeId = getTypeId(T());

      void* bufferPtr;
      if( typeId == TID_BOOL ){
        bufferPtr = new unsigned int();
      }
      else{
        bufferPtr = new T();
      }
      INT bufferSize = rpc_tid_size(int(typeId));
      errorCode = db_get_data_index(hDB, hKey, bufferPtr, &bufferSize, index_, typeId);

      if( errorCode == SUCCESS ){
        return *reinterpret_cast<T*>(bufferPtr); // Return fetched value
      }

      throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\" / index: " + std::to_string(index_));
    }
    template <> inline auto read(const std::string &path_, int index_) -> std::string {
      /*
     * Because we can't directly cast void* to a string*, this template specialization is needed.
     * */
      int errorCode;
      HNDLE hKey = getKey(path_);

      if( hKey == 0 ){
        throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
      }

      INT bufferSize = 256;
      char bufferStr[bufferSize];
      errorCode = db_get_data_index(hDB, hKey, &bufferStr, &bufferSize, index_, TID_STRING);
      if( errorCode == SUCCESS ){
        return std::string(bufferStr);
      }

      throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\" / index: " + std::to_string(index_));
    }
    inline std::vector<std::string> ls(const std::string &path_){
      std::vector<std::string> subKeyNameList;
      int iKey;
      HNDLE hKey;
      HNDLE hSubKey;
      KEY   subKey;
      hKey = getKey(path_.c_str());
      if(hKey == 0){
        throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
      }
      for( iKey=0 ;  ; iKey++ ){
        db_enum_key(hDB, hKey, iKey, &hSubKey);
        if( hSubKey == 0 ) break; // end of list reached
        db_get_key(hDB, hSubKey, &subKey);
        subKeyNameList.emplace_back( subKey.name );
      }
      return subKeyNameList;
    }
    //  template<class T> std::map<std::string, T> fetchMatchingValues(const std::string& regexPath_, const std::string& regexConditionPath_){
    //    GenericToolbox::doesStringContainsSubstring()
    //  }

    inline bool setKey(const std::string &path_, const DWORD& type_){
      if( not isKey(path_) ){
        HNDLE hDir = 0;
        int errorCode;
        errorCode = db_create_key(hDB, hDir, path_.c_str(), type_);
        if( errorCode == DB_SUCCESS ){
          return true;
        }
        else{
          throw std::runtime_error("db_create_key failed with error: " + GET_VAR_NAME_VALUE(errorCode));
        }
      }
      return false;
    }
    template<class T> inline bool write(const std::string &path_, const T &objToWrite_, bool createKeyIsNotPresent_) {
      if( not isKey(path_)){
        if( createKeyIsNotPresent_ ){
          if( not setKey(path_, getTypeId(objToWrite_)) ){
            throw std::runtime_error("Could not create key " + path_);
          }
        }
        else{
          return false;
        }
      }

      HNDLE hKey = 0;
      INT type, num_values, item_size;
      db_get_key_info(hDB, hKey, (char*) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);

      // TODO: UNFINISHED
      // Do a generic function to write void* raw data and specialize after
      throw std::runtime_error("UNFINISHED IMPL");
      db_set_data_index(hDB, hKey, &objToWrite_, item_size, 0, getTypeId(objToWrite_));
    }
    bool write(const std::string &path_, const std::string &objToWrite_, bool createKeyIsNotPresent_){
      if( not isKey(path_)){
        if( createKeyIsNotPresent_ ){
          if( not setKey(path_, getTypeId(objToWrite_)) ){
            throw std::runtime_error("Could not create key " + path_);
          }
        }
        else{
          return false;
        }
      }

      HNDLE hKey = 0;
      INT type, num_values, item_size;
      db_get_key_info(hDB, hKey, (char*) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);

      // TODO: UNFINISHED
      throw std::runtime_error("UNFINISHED IMPL");
      db_set_data_index(hDB, hKey, &objToWrite_, item_size, 0, getTypeId(objToWrite_));
    }

    template<class T> inline DWORD getTypeId(const T& object_){
      DWORD keyType = 0;
      if     ( std::is_same<T, bool>::value ) {
        keyType = TID_BOOL;
      }
      else if( std::is_same<T, int>::value ){
#ifdef TID_INT32
        keyType = TID_INT32;
#else
        keyType = TID_INT;
#endif
      }
      else if( std::is_same<T, unsigned int>::value ){
#ifdef TID_UINT32
        keyType = TID_UINT32;
#else
        keyType = TID_INT;
#endif
      }
      else if( std::is_same<T, float>::value ){
        keyType = TID_FLOAT;
      }
      else if( std::is_same<T, double>::value ){
        keyType = TID_DOUBLE;
      }
      else if( std::is_same<T, std::string>::value ){
        keyType = TID_STRING;
      }
      else{
        throw std::runtime_error("Requested object type as Midas TID was not recognized/is not implemented.");
      }
      return keyType;
    }

    template <typename C, typename T>
    inline void initializeHotLink(const std::string& odbPath_, C& hotLinkContainer_,
                           std::vector<const char*>& hotLinkDescriptor_, T& hotLinkStaticDispatcher_,
                           void* this_){
      HNDLE hkey;
      cm_get_experiment_database(&hDB, nullptr);
      db_create_record(hDB, 0, odbPath_.c_str(),strcomb(hotLinkDescriptor_.data()));
      db_find_key(hDB, 0, odbPath_.c_str(), &hkey);

      int errorCode = db_open_record(hDB, hkey, &hotLinkContainer_, sizeof(hotLinkContainer_), MODE_READ, &hotLinkStaticDispatcher_, this_);
      if( errorCode != DB_SUCCESS ){
        throw std::runtime_error("Cannot open Hot-Link in ODB.");
      }
    }

  }
}

#endif//HATMIDASFRONTEND_GENERICTOOLBOX_ODB_IMPL_H
