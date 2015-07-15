//------------------------------------------
//  SQLiteService.h
//
//------------------------------------------

#ifndef _SQLITER_SERVICE_H_
#define _SQLITER_SERVICE_H_

#include <Ice/Ice.h>
#include <IceBox/IceBox.h>
#include <IceUtil/Mutex.h>   
#include <IceUtil/IceUtil.h>
#include <sys/time.h> 
#include <time.h>
#include <map>
#include <string>
#include <cstring>
#include <iostream>
#include <locale.h> 
#include "CppSQLite3.h"
#include "IceInterface.h"

using namespace std;
using namespace Ice;
using namespace WISCOM::DataBase;

typedef struct  tagCachedConnection{
    string      dbName;
    unsigned int refCount;
    CppSQLite3DB*  pDB;
}CachedConnection;

typedef struct  tagCachedDataSet{
    IceUtil::Time    activeTime;
    CppSQLite3Query  queryData;
}CachedDataSet;

class SQLiteService : public IceBox::Service {

public:
    Ice::CommunicatorPtr    iceCommunicator;   
    Ice::ObjectAdapterPtr   objectAdapter;

    virtual void start(const string& name, const Ice::CommunicatorPtr&, const Ice::StringSeq&);
    virtual void stop();
};

class SQLiteServant: public WISCOM::DataBase::SQLiteOperator, 
                     public IceUtil::TimerTask{
public:
    SQLiteServant();
    ~SQLiteServant();

    virtual int   openDB(const string& dbName, const Ice::Current&);
    virtual bool  closeDB(int dbHandle, const Ice::Current&);

    virtual bool  execDML(int dbHandle, const string& strSQL, const Ice::Current&);
    virtual bool  execDMLWithBlob(int dbHandle, const string& strSQL, 
        const BlobValuePtr& pData1, const BlobValuePtr& pData2, const BlobValuePtr& pData3,  const Ice::Current&);
    virtual string  execQuery(int dbHandle, const string& strSQL, const Ice::Current&);
    
    virtual int   numFields(const string& queryIdentity, const Ice::Current&);
    virtual int   fieldIndex(const string& queryIdentity, const string& fieldName, const Ice::Current&);    
    virtual string fieldName(const string& queryIdentity, int nFieldIndex, const Ice::Current&);  
    virtual int   fieldDataType(const string& queryIdentity, int nFieldIndex, const Ice::Current&);  
    
    virtual bool  fieldIsNullById(const string& queryIdentity, int nFieldIndex, const Ice::Current&);  
    virtual bool  fieldIsNullByName(const string& queryIdentity, const string& fieldName, const Ice::Current&);
    
    virtual bool  nextRow(const string& queryIdentity, const Ice::Current&);
    virtual Ice::Long  lastRowId(int dbHandle, const Ice::Current&);        
    virtual bool  eof(const string& queryIdentity, const Ice::Current&);

    virtual int   getIntFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&);
    virtual int   getIntFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&);
    
    virtual double  getFloatFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&);   
    virtual double  getFloatFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&);

    virtual string  getStringFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&);   
    virtual string  getStringFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&);
    
    virtual BlobValuePtr getBlobFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&);
    virtual BlobValuePtr getBlobFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&);
            
    virtual bool    interrupt(int dbHandle,  const Ice::Current&);        
    virtual bool    finalize(const string& queryIdentity, const Ice::Current&);

private:
    vector<CachedConnection>dbArray;
    map<string, CachedDataSet>dataSetArray;
    IceUtil::Mutex  mutex_dbArray;
    IceUtil::Mutex  mutex_dataSetArray;
    IceUtil::TimerPtr _timer;
    
    CppSQLite3Query* getDataSet(string queryIdentity);
    virtual void runTimerTask();
};

#endif 
