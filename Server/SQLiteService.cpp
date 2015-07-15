#include "SQLiteService.h"

#define     TRY_CATCH_SQLITE_SENTENCE(OPERATION) \
                                                 \
            try{                                 \
               OPERATION                         \
            }                                    \
            catch(CppSQLite3Exception& e)        \
            {                                    \
               cout<< e.errorCode() << ":"       \
                   << e.errorMessage() << endl;  \
            }
            
#define     CHECK_DB_VALID_SENTENCE(ERR_MESSAGE, ERR_FLAG)  \
                                                        \
            IceUtil::Mutex::Lock lock(mutex_dbArray);   \
                                                        \
            if ((dbHandle<0) || (dbHandle>=dbArray.size())) { \
                cout<<ERR_MESSAGE << ":invalid dbHandle :" << dbHandle  << endl; \
                return  ERR_FLAG;           \
            }                               \
                                            \
            CppSQLite3DB*   pDB =   dbArray[dbHandle].pDB;  \
            if (NULL == pDB)    {                           \
              cout<<ERR_MESSAGE<< ":pDB is NULL"<< endl;    \
              return ERR_FLAG;                              \
            }                    

#define     GET_DATASET_SENTENCE(ERR_MESSAGE, ERR_FLAG) \
                                                        \
            IceUtil::Mutex::Lock lock(mutex_dataSetArray); \
                                                        \
            CppSQLite3Query*    pCurrentDataSet = NULL; \
            pCurrentDataSet =   getDataSet(queryIdentity);  \
                                                        \
            if (NULL == pCurrentDataSet) {              \
                cout <<ERR_MESSAGE <<":getDataSet failed" << endl; \
                return  ERR_FLAG;                       \
            }
            
SQLiteServant::SQLiteServant()
{

}

SQLiteServant::~SQLiteServant()
{
    if (!dbArray.empty())    {
        for(int i=0; i<dbArray.size(); i++)  {
            if (dbArray[i].pDB) {
                dbArray[i].pDB->close();
                delete dbArray[i].pDB;
            }
        }
        
        dbArray.clear();
    }

    map<string, CachedDataSet>::iterator  dataSetItr;
    
    dataSetItr  =   dataSetArray.begin();
    while(dataSetItr != dataSetArray.end())  {
        TRY_CATCH_SQLITE_SENTENCE(
            dataSetItr->second.queryData.finalize();
        )
        
        dataSetItr++;
    }
    dataSetArray.clear();

}

int  SQLiteServant::openDB(const string& dbName, const Ice::Current&)
{
    IceUtil::Mutex::Lock lock(mutex_dbArray);    
    
    bool    bSuccessFlag  = false;
    CachedConnection  m_connection;
    
    for(int i=0; i < dbArray.size(); i++)    {
        m_connection  =   dbArray[i];
        if (dbName == m_connection.dbName)  {
           if (0 == m_connection.refCount)  {
                try{
                    m_connection.pDB->open(dbName.c_str());
                    bSuccessFlag    =   true;
                }
                catch(CppSQLite3Exception& e) {
                    cout << "openDB: " << e.errorCode() << ":" << e.errorMessage() << endl;
                    return -1;
                }
           }
           
           ++dbArray[i].refCount;
           return i;
        }
    }

    CppSQLite3DB*  pDB  =   new CppSQLite3DB;
    try {
        pDB->open(dbName.c_str());
        bSuccessFlag   =   true;
    }
    catch(CppSQLite3Exception& e) {
		cout << "openDB: " << e.errorCode() << ":" << e.errorMessage() << endl;
    }
    
    if (!bSuccessFlag)  {
        if (pDB)
            delete pDB;
        
        return -1;
    }

    m_connection.dbName     =   dbName;
    m_connection.pDB        =   pDB;
    m_connection.refCount   =   1;

    dbArray.push_back(m_connection);

    return
        (dbArray.size() -1);
}

bool SQLiteServant::closeDB(int dbHandle, const Ice::Current&)
{
    CHECK_DB_VALID_SENTENCE("closeDB", false);
        
    if (dbArray[dbHandle].refCount > 0)
        dbArray[dbHandle].refCount--;
    
    if (0 == dbArray[dbHandle].refCount) {
        TRY_CATCH_SQLITE_SENTENCE(
            dbArray[dbHandle].pDB->close();
        )
    }
    
    return  true;
}

bool  SQLiteServant::execDML(int dbHandle, const string& strSQL, const Ice::Current&)
{   
    CHECK_DB_VALID_SENTENCE("execDML", false);
    
    int nChanges    =   -1;

    TRY_CATCH_SQLITE_SENTENCE(
        nChanges    =   pDB->execDML(strSQL.c_str());
    )
    
    if (nChanges < 0)
        return false;
    
    return 
        true;
}

bool  SQLiteServant::execDMLWithBlob(int dbHandle, const string& strSQL, 
    const BlobValuePtr& pData1, const BlobValuePtr& pData2, const BlobValuePtr& pData3,  const Ice::Current&)
{
    CHECK_DB_VALID_SENTENCE("execDMLWithBlob", false);
    
    int nChanges    =   -1;
    
    CppSQLite3Statement stmt = pDB->compileStatement(strSQL.c_str());    
    
    if (pData1->data.size() > 0)
        stmt.bind(1, &(pData1->data[0]), pData1->data.size());
    
    if (pData2->data.size() > 0)
        stmt.bind(2, &(pData2->data[0]), pData2->data.size());

    if (pData3->data.size() > 0)
        stmt.bind(3, &(pData3->data[0]), pData3->data.size());

    TRY_CATCH_SQLITE_SENTENCE(
        nChanges    =           stmt.execDML();
    )
    
    if (nChanges < 0)
        return false;
    
    return 
        true;
}               

string   SQLiteServant::execQuery(int dbHandle, const string& strSQL, const Ice::Current&)
{
    string  strIdentity     =   "";

    CHECK_DB_VALID_SENTENCE("execQuery", strIdentity);
    
    CachedDataSet   m_DataSet;

    try{
        m_DataSet.queryData     =   pDB->execQuery(strSQL.c_str());
        strIdentity             =   IceUtil::generateUUID();
        m_DataSet.activeTime    =   IceUtil::Time::now();
    }
    catch (CppSQLite3Exception& e) {
        cout<< e.errorCode() << ":"<< e.errorMessage() << endl; 
        return  strIdentity;
    }
    
    dataSetArray.insert(pair<string, CachedDataSet>(strIdentity, m_DataSet));

    return 
        strIdentity;
}

int   SQLiteServant::numFields(const string& queryIdentity, const Ice::Current&)
{
    int nNumFields  =   0;
    
    GET_DATASET_SENTENCE("numFields", nNumFields);

    TRY_CATCH_SQLITE_SENTENCE(
        nNumFields  =   pCurrentDataSet->numFields();
    )
    
    return  
        nNumFields;
}

int   SQLiteServant::fieldIndex(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    int nFieldIndex  =   -1;
    
    GET_DATASET_SENTENCE("fieldIndex", nFieldIndex);
    
    TRY_CATCH_SQLITE_SENTENCE(
        nFieldIndex  =   pCurrentDataSet->fieldIndex(fieldName.c_str());
    )
    
    return  
        nFieldIndex;
}

string SQLiteServant::fieldName(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    string      strFieldName    =   "";

    GET_DATASET_SENTENCE("fieldName", strFieldName);
    
    TRY_CATCH_SQLITE_SENTENCE(
        strFieldName  =   pCurrentDataSet->fieldName(nFieldIndex);
    )
    
    return  
        strFieldName;
}

int   SQLiteServant::fieldDataType(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    int     nFieldDataType  =   -1;

    GET_DATASET_SENTENCE("fieldDataType", nFieldDataType);
    
    TRY_CATCH_SQLITE_SENTENCE(
        nFieldDataType  =   pCurrentDataSet->fieldDataType(nFieldIndex);
    )
    
    return  
        nFieldDataType;
}

bool  SQLiteServant::fieldIsNullById(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    bool    bRet    =   true;
    
    GET_DATASET_SENTENCE("fieldIsNullById", bRet);
    
    TRY_CATCH_SQLITE_SENTENCE(
        bRet  =   pCurrentDataSet->fieldIsNull(nFieldIndex);
    )
    
    return  
        bRet;
}    
    
bool  SQLiteServant::fieldIsNullByName(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    bool    bRet    =   true;

    GET_DATASET_SENTENCE("fieldIsNullByName", bRet);
    
    TRY_CATCH_SQLITE_SENTENCE(
        bRet  =   pCurrentDataSet->fieldIsNull(fieldName.c_str());
    )
    
    return  
        bRet;
}

bool  SQLiteServant::nextRow(const string& queryIdentity, const Ice::Current&)
{
    GET_DATASET_SENTENCE("nextRow", false);
    
    TRY_CATCH_SQLITE_SENTENCE(
        pCurrentDataSet->nextRow();
    )
    
    return  true;
}

Ice::Long  SQLiteServant::lastRowId(int dbHandle, const Ice::Current&)
{
    long    lRet    =   0;
    
    CHECK_DB_VALID_SENTENCE("lastRowId",lRet);
    
    TRY_CATCH_SQLITE_SENTENCE(
        lRet    =   pDB->lastRowId();
    )

    return 
        lRet;
}

bool  SQLiteServant::eof(const string& queryIdentity, const Ice::Current&)
{
    bool    bRet    =   true;
    
    GET_DATASET_SENTENCE("eof", bRet);
    
    TRY_CATCH_SQLITE_SENTENCE(
        bRet    =   pCurrentDataSet->eof();
    )
    
    return 
        bRet;
}

int   SQLiteServant::getIntFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    int nValue  =   0;
    
    GET_DATASET_SENTENCE("getIntFieldValueByIndex", nValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        nValue  =   pCurrentDataSet->getIntField(nFieldIndex);
    )
    
    return 
        nValue;
}

int   SQLiteServant::getIntFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    int nValue  =   0;

    GET_DATASET_SENTENCE("getIntFieldValueByName", nValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        nValue  =   pCurrentDataSet->getIntField(fieldName.c_str());
    )

    return 
        nValue;
}

double  SQLiteServant::getFloatFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    double  fValue  =   0;

    GET_DATASET_SENTENCE("getFloatFieldValueByIndex", fValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        fValue  =   pCurrentDataSet->getFloatField(nFieldIndex);
    )

    return 
        fValue;
}

double  SQLiteServant::getFloatFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    double  fValue  =   0;

    GET_DATASET_SENTENCE("getFloatFieldValueByName", fValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        fValue  =   pCurrentDataSet->getFloatField(fieldName.c_str());
    )

    return 
        fValue;
}

string  SQLiteServant::getStringFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    string  strValue    =   "";

    GET_DATASET_SENTENCE("getStringFieldValueByIndex", strValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        strValue    =   pCurrentDataSet->getStringField(nFieldIndex);
    )

    return 
        strValue;
}

string  SQLiteServant::getStringFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    string  strValue    =   "";

    GET_DATASET_SENTENCE("getStringFieldValueByName", strValue);
    
    TRY_CATCH_SQLITE_SENTENCE(
        strValue    =   pCurrentDataSet->getStringField(fieldName.c_str());
    )

    return 
        strValue;
}

BlobValuePtr SQLiteServant::getBlobFieldValueByIndex(const string& queryIdentity, int nFieldIndex, const Ice::Current&)
{
    int     nBufferSize     =   0;
    BlobValuePtr    pBlob   =   NULL;
    const  unsigned char*  pBuffer =   NULL;

    GET_DATASET_SENTENCE("getBlobFieldValueByIndex", NULL);
    
    TRY_CATCH_SQLITE_SENTENCE(
        pBuffer =   (const unsigned char*)pCurrentDataSet->getBlobField(nFieldIndex, nBufferSize);
    )
    
    if (pBuffer)    {
        CppSQLite3Binary blob;
        blob.setBinary(pBuffer, nBufferSize);

        const unsigned char *pBuffer = blob.getBinary();
        int nBufferSize = blob.getBinaryLength();
        
        pBlob   =   new BlobValue;
        pBlob->data.resize(nBufferSize);
        memcpy(&(pBlob->data[0]), pBuffer, nBufferSize);
    }

    return
        pBlob;
}

BlobValuePtr SQLiteServant::getBlobFieldValueByName(const string& queryIdentity, const string& fieldName, const Ice::Current&)
{
    int     nBufferSize     =   0;
    BlobValuePtr    pBlob   =   NULL;
    const   unsigned char*  pBuffer =   NULL;

    GET_DATASET_SENTENCE("getBlobFieldValueByName", NULL);
    
    TRY_CATCH_SQLITE_SENTENCE(
        pBuffer =   (const unsigned char*)pCurrentDataSet->getBlobField(fieldName.c_str(), nBufferSize);
    )

    if (pBuffer)    {
        CppSQLite3Binary blob;
        blob.setBinary(pBuffer, nBufferSize);
        
        const unsigned char *pBuffer = blob.getBinary();
        int nBufferSize = blob.getBinaryLength();
        
        pBlob   =   new BlobValue;
        pBlob->data.resize(nBufferSize);
        memcpy(&(pBlob->data[0]), pBuffer, nBufferSize);
    }

    return
        pBlob;
}

bool  SQLiteServant::interrupt(int dbHandle,  const Ice::Current&)
{
    CHECK_DB_VALID_SENTENCE("interrupt", false);    

    pDB->interrupt();
    
    return  true;
}

bool SQLiteServant::finalize(const string& queryIdentity, const Ice::Current&)
{
    IceUtil::Mutex::Lock lock(mutex_dataSetArray);    
    
    map<string, CachedDataSet>::iterator  dataSetItr;
    
    dataSetItr  =   dataSetArray.find(string(queryIdentity));
    
    if (dataSetItr != dataSetArray.end())    {
        TRY_CATCH_SQLITE_SENTENCE(
            dataSetItr->second.queryData.finalize();
        )
        
        dataSetArray.erase(dataSetItr);
        return true;
    }
    
    return false;
}

CppSQLite3Query* SQLiteServant::getDataSet(string queryIdentity)
{
    map<string, CachedDataSet>::iterator  dataSetItr;
    
    dataSetItr  =   dataSetArray.find(string(queryIdentity));
    
    if (dataSetItr != dataSetArray.end())
        return    &(dataSetItr->second.queryData);
    
    return NULL;
}

void    SQLiteServant::runTimerTask()
{
    IceUtil::Mutex::Lock lock(mutex_dataSetArray);    
    
    IceUtil::Time   nowTime =   IceUtil::Time::now();
    
    map<string, CachedDataSet>::iterator  dataSetItr;
    
    dataSetItr  =   dataSetArray.begin();
    while(dataSetItr != dataSetArray.end())  {
        if ((nowTime - dataSetItr->second.activeTime) 
                > IceUtil::Time::seconds(10))   {
            TRY_CATCH_SQLITE_SENTENCE(
                dataSetItr->second.queryData.finalize();
            )
                    
            dataSetArray.erase(dataSetItr);        
        }
        else 
            ++dataSetItr;
    }
}

void SQLiteService::start(const string& name, const Ice::CommunicatorPtr& ic, const Ice::StringSeq&)
{
    try{
        iceCommunicator = ic;
        objectAdapter = iceCommunicator->createObjectAdapterWithEndpoints("SQLiteService", "tcp -p 21000");
    
        SQLiteServant*   pServant   =   new SQLiteServant;
        objectAdapter->add(pServant, iceCommunicator->stringToIdentity("SQLiter"));
 
        IceUtil::TimerPtr timer = new IceUtil::Timer();
        timer->scheduleRepeated(pServant, IceUtil::Time::seconds(3));
        objectAdapter->activate();
    
        iceCommunicator->waitForShutdown();
        iceCommunicator->destroy();
    }
    catch(const Ice::Exception& ex)
    {
        cerr << ex << endl;
        if(iceCommunicator)
        {
            try {
                iceCommunicator->destroy();
            }
            catch(const Ice::Exception& ex)
            {
                cerr << ex << endl;
            }
        }
    }
}

void SQLiteService::stop()
{
    objectAdapter->deactivate();
}

extern "C" {
    IceBox::Service*
    createSQLiteDBServer(Ice::CommunicatorPtr communicator) {
        return new SQLiteService;
    }
}
