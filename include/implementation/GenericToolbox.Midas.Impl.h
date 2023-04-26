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
  namespace Midas{

    namespace Logger{

      template<typename T> inline void writeInBank(char* pevent, const std::string& name_, const T& data_){
        T* dataPtr{nullptr};
        bk_create( pevent, name_.c_str(), getTid(data_), (void **) &dataPtr );
        *dataPtr++ = data_;
        bk_close( pevent, dataPtr );
      }
      template<typename T> inline void writeInBank(char* pevent, const std::string& name_, const std::vector<T>& list_){
        if( list_.empty() ) return;
        T* dataPtr{nullptr};
        bk_create(pevent, name_.c_str(), getTid(list_[0]), (void **) &dataPtr );
        for( auto& entry : list_ ){ *dataPtr++ = entry; }
        bk_close( pevent, dataPtr );
      }
      template<typename T, typename L> inline void writeInBank(char* pevent, const std::string& name_, const T& array_,
          int size_, const L& getData_){
        if( size_ == 0 ) return;
        unsigned char* dataPtr{nullptr}; // 1 byte
        bk_create(pevent, name_.c_str(), getTid(getData_(array_[0])), (void **) &dataPtr );
        for( int iElm = 0 ; iElm < size_ ; iElm++ ){
          *dataPtr = getData_(array_[iElm]);
          dataPtr += sizeof( decltype(getData_) ); // multiple of 1 byte
        }
        bk_close( pevent, dataPtr );
      }
      template<typename T, typename L, typename LL> inline void writeInBank(char* pevent, const std::string& name_,
          const T& array_, int size_, const LL& grabSubarray_, int subSize_, const L& getData_){
        if( size_ == 0 or subSize_ == 0 ) return;
        unsigned char* dataPtr{nullptr}; // 1 byte
        bk_create(pevent, name_.c_str(), getTid(getData_(grabSubarray_(array_[0])[0])), (void **) &dataPtr );
        for( int iElm = 0 ; iElm < size_ ; iElm++ ){
          for( int iSubElm = 0 ; iSubElm < subSize_ ; iSubElm++ ){
            *dataPtr = getData_(grabSubarray_(array_[iElm])[iSubElm]);
            dataPtr += sizeof( decltype(getData_) ); // multiple of 1 byte
          }
        }
        bk_close( pevent, dataPtr );
      }
      template<typename T> WORD getTid(const T& data_){
        if     ( std::is_same<T, bool>::value )         return TID_BOOL;
        if     ( std::is_same<T, char>::value )         return TID_INT8;
        if     ( std::is_same<T, short>::value )        return TID_INT16;
        if     ( std::is_same<T, int>::value )          return TID_INT32;
        if     ( std::is_same<T, long>::value )         return TID_INT64;
        else if( std::is_same<T, unsigned char>::value )  return TID_UINT8;
        else if( std::is_same<T, unsigned short>::value ) return TID_UINT16;
        else if( std::is_same<T, unsigned int>::value )   return TID_UINT32;
        else if( std::is_same<T, unsigned long>::value )  return TID_UINT64;
        else if( std::is_same<T, float>::value )          return TID_FLOAT32;
        else if( std::is_same<T, double>::value )         return TID_FLOAT64;
      }

    }

    namespace Odb {

      inline HNDLE getKey(const std::string &path_) {
        HNDLE hDir = 0;
        HNDLE hKey = 0;
        int errorCode = db_find_key(hDB, hDir, path_.c_str(), &hKey);
        if (errorCode == DB_SUCCESS) {
          return hKey;
        } else {
          return 0;
        }
      }

      inline DWORD getKeyType(const std::string &path_) {
        INT type, num_values, item_size;
        db_get_key_info(hDB, getKey(path_), (char *) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);
        return type;
      }

      inline int getKeyNbValues(const std::string &path_) {
        INT type, num_values, item_size;
        db_get_key_info(hDB, getKey(path_), (char *) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);
        return num_values;
      }

      inline bool isKey(const std::string &path_) {
        return (GenericToolbox::Midas::Odb::getKey(path_) != 0);
      }

      template<class T>
      inline auto read(const std::string &path_, int index_) -> T {
        int errorCode;
        HNDLE hKey = getKey(path_);

        if (hKey == 0) {
          throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
        }

        DWORD typeId = getTypeId(T());

        void *bufferPtr;
        if (typeId == TID_BOOL) {
          bufferPtr = new unsigned int();
        } else {
          bufferPtr = new T();
        }
        INT bufferSize = rpc_tid_size(int(typeId));
        errorCode = db_get_data_index(hDB, hKey, bufferPtr, &bufferSize, index_, typeId);

        if (errorCode == SUCCESS) {
          return *reinterpret_cast<T *>(bufferPtr); // Return fetched value
        }

        throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\" / index: " + std::to_string(index_));
      }

      template<>
      inline auto read(const std::string &path_, int index_) -> std::string {
        /*
       * Because we can't directly cast void* to a string*, this template specialization is needed.
       * */
        int errorCode;
        HNDLE hKey = getKey(path_);

        if (hKey == 0) {
          throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
        }

        INT bufferSize = 256;
        char bufferStr[bufferSize];
        errorCode = db_get_data_index(hDB, hKey, &bufferStr, &bufferSize, index_, TID_STRING);
        if (errorCode == SUCCESS) {
          return std::string(bufferStr);
        }

        throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\" / index: " + std::to_string(index_));
      }

      template<class T>
      inline auto readVector(const std::string &path_) -> std::vector<T> {
        int errorCode;
        HNDLE hKey = getKey(path_);

        if (hKey == 0) {
          throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
        }

        DWORD typeId = getTypeId(T());

        int nbValues = getKeyNbValues(path_);
        INT bufferSize = rpc_tid_size(int(typeId)) * nbValues;

        std::vector<T> out;
        out.resize(nbValues);

        errorCode = db_get_data(hDB, hKey, out.data(), &bufferSize, typeId);

        if (errorCode == SUCCESS) {
          return out; // Return fetched value
        }

        throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\"");
      }

      template<class T, size_t N>
      inline void readArray(const std::string &path_, std::array<T, N> &out_) {
        int errorCode;
        HNDLE hKey = getKey(path_);

        if (hKey == 0) {
          throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
        }

        DWORD typeId = getTypeId(T());

        int nbValues = getKeyNbValues(path_);

        if (typeId == TID_BOOL) { throw std::runtime_error("bool array reader not implemented"); }

        if (nbValues > out_.size()) {
          throw std::runtime_error("ODB array is [" + std::to_string(nbValues) + "], greater than out array size: " +
                                   std::to_string(out_.size()));
        }

        INT bufferSize = rpc_tid_size(int(typeId)) * nbValues;

        errorCode = db_get_data(hDB, hKey, out_.data(), &bufferSize, typeId);

        if (errorCode != SUCCESS) {
          throw std::runtime_error("Could not fetch ODB key: \"" + path_ + "\"");
        }


      }

      inline std::vector<std::string> ls(const std::string &path_) {
        std::vector<std::string> subKeyNameList;
        int iKey;
        HNDLE hKey;
        HNDLE hSubKey;
        KEY subKey;
        hKey = getKey(path_.c_str());
        if (hKey == 0) {
          throw std::runtime_error("Could not find ODB key: \"" + path_ + "\"");
        }
        for (iKey = 0;; iKey++) {
          db_enum_key(hDB, hKey, iKey, &hSubKey);
          if (hSubKey == 0) break; // end of list reached
          db_get_key(hDB, hSubKey, &subKey);
          subKeyNameList.emplace_back(subKey.name);
        }
        return subKeyNameList;
      }
      //  template<class T> std::map<std::string, T> fetchMatchingValues(const std::string& regexPath_, const std::string& regexConditionPath_){
      //    GenericToolbox::doesStringContainsSubstring()
      //  }

      inline bool setKey(const std::string &path_, const DWORD &type_) {
        if (not isKey(path_)) {
          HNDLE hDir = 0;
          int errorCode;
          errorCode = db_create_key(hDB, hDir, path_.c_str(), type_);
          if (errorCode == DB_SUCCESS) {
            return true;
          } else {
            throw std::runtime_error("db_create_key failed with error: " + GET_VAR_NAME_VALUE(errorCode));
          }
        }
        return false;
      }

      template<class T>
      inline bool write(const std::string &path_, const T &objToWrite_, bool createKeyIsNotPresent_) {
        if (not isKey(path_)) {
          if (createKeyIsNotPresent_) {
            if (not setKey(path_, getTypeId(objToWrite_))) {
              throw std::runtime_error("Could not create key " + path_);
            }
          } else {
            return false;
          }
        }

        HNDLE hKey = 0;
        INT type, num_values, item_size;
        db_get_key_info(hDB, hKey, (char *) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);

        // TODO: UNFINISHED
        // Do a generic function to write void* raw data and specialize after
        throw std::runtime_error("UNFINISHED IMPL");
        db_set_data_index(hDB, hKey, &objToWrite_, item_size, 0, getTypeId(objToWrite_));
      }

      bool write(const std::string &path_, const std::string &objToWrite_, bool createKeyIsNotPresent_) {
        if (not isKey(path_)) {
          if (createKeyIsNotPresent_) {
            if (not setKey(path_, getTypeId(objToWrite_))) {
              throw std::runtime_error("Could not create key " + path_);
            }
          } else {
            return false;
          }
        }

        HNDLE hKey = 0;
        INT type, num_values, item_size;
        db_get_key_info(hDB, hKey, (char *) path_.c_str(), int(path_.size()), &type, &num_values, &item_size);

        // TODO: UNFINISHED
        throw std::runtime_error("UNFINISHED IMPL");
        db_set_data_index(hDB, hKey, &objToWrite_, item_size, 0, getTypeId(objToWrite_));
      }

      template<class T>
      inline DWORD getTypeId(const T &object_) {
        DWORD keyType = 0;
        if (std::is_same<T, bool>::value) {
          keyType = TID_BOOL;
        } else if (std::is_same<T, int>::value) {
#ifdef TID_INT32
          keyType = TID_INT32;
#else
          keyType = TID_INT;
#endif
        } else if (std::is_same<T, unsigned int>::value) {
#ifdef TID_UINT32
          keyType = TID_UINT32;
#else
          keyType = TID_INT;
#endif
        } else if (std::is_same<T, float>::value) {
          keyType = TID_FLOAT;
        } else if (std::is_same<T, double>::value) {
          keyType = TID_DOUBLE;
        } else if (std::is_same<T, std::string>::value) {
          keyType = TID_STRING;
        } else {
          throw std::runtime_error("Requested object type as Midas TID was not recognized/is not implemented.");
        }
        return keyType;
      }

      template<typename C, typename T>
      inline void initializeHotLink(const std::string &odbPath_, C &hotLinkContainer_,
                                    std::vector<const char *> &hotLinkDescriptor_, T &hotLinkStaticDispatcher_,
                                    void *this_) {
        HNDLE hkey;
        cm_get_experiment_database(&hDB, nullptr);
        db_create_record(hDB, 0, odbPath_.c_str(), strcomb(hotLinkDescriptor_.data()));
        db_find_key(hDB, 0, odbPath_.c_str(), &hkey);

        int errorCode = db_open_record(hDB, hkey, &hotLinkContainer_, sizeof(hotLinkContainer_), MODE_READ,
                                       &hotLinkStaticDispatcher_, this_);
        if (errorCode != DB_SUCCESS) {
          throw std::runtime_error("Cannot open Hot-Link in ODB.");
        }
      }
    }
  }
}

#endif//HATMIDASFRONTEND_GENERICTOOLBOX_ODB_IMPL_H
