//------------------------------------------
//  SQLiteIceInterface.h

#ifndef    _SQLITE_ICE_INTERFACE_H_
#define    _SQLITE_ICE_INTERFACE_H_

#include <string>

// copied from sqlite3.h
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#ifdef  SQLITE_TEXT
#undef  SQLITE_TEXT
#else
# define SQLITE_TEXT     3
#endif
#define SQLITE3_TEXT     3

using namespace std;

class   SQLiteIceInterface{
public:
    SQLiteIceInterface(const char* szDBServerIp);
    ~SQLiteIceInterface();
    
    int     openDB(const char* szDBName);
    bool    closeDB(int dbHandle);

    bool    execDML(int dbHandle, const char* szSQL);
    bool    execDMLWithBlob(int dbHandle, const char* szSQL, 
               const unsigned char* pBlob1, int nBlobSize1,
               const unsigned char* pBlob2 = NULL,  int nBlobSize2 = 0,
               const unsigned char* pBlob3 = NULL,  int nBlobSize3 = 0);
    string  execQuery(int dbHandle, const char* szSQL);
    
    int     numFields(string queryIdentity);       
    int     fieldIndex(string queryIdentity, string fieldName);  
    string  fieldName(string queryIdentity, int nFieldIndex);    
    int     fieldDataType(string queryIdentity, int nFieldIndex);
    
    bool    fieldIsNullById(string queryIdentity, int nFieldIndex); 
    bool    fieldIsNullByName(string queryIdentity, string fieldName);  
    
    bool    nextRow(string  queryIdentity);
    long    lastRowId(int dbHandle);        
    bool    eof(string queryIdentity);

    int     getIntField(string queryIdentity, int nFieldIndex);
    int     getIntField(string queryIdentity, const char* szFieldName);
    
    double  getFloatField(string queryIdentity, int nFieldIndex);
    double  getFloatField(string queryIdentity, const char* szFieldName);
    
    string  getStringField(string queryIdentity, int nFieldIndex);
    string  getStringField(string queryIdentity, const char* szFieldName);
    
    unsigned char* getBlobField(string queryIdentity, int nFieldIndex, int* pBlobSize);
    unsigned char* getBlobField(string queryIdentity, const char* szFieldName,  int* pBlobSize);

    bool    interrupt(int dbHandle);        
    bool    finalize(string queryIdentity);
};

#endif // _SQLITE_ICE_CLIENT_H_

