#include <SQLiteIceInterface.h>

#define     ICE_CONFIG_FILE    	    "config"
#define     ICE_CONNECT_TIMEOUT       600000       // milliseconds

#define     TRY_CATCH_ICE_SENTENCE(OPERATION)       \
            try{                                \
               OPERATION                        \
            }                                   \
            catch(const IceUtil::Exception& ex) \
            {                                   \
              cout<< ex << endl;                \
            }
            
SQLiteIceInterface::SQLiteIceInterface(const char* szDBServerIp)
{
	SQLiteOperatorPrx   =	NULL;
   
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    
    if  (access(ICE_CONFIG_FILE, F_OK) == 0)
		initData.properties->load(ICE_CONFIG_FILE);
    else {
        initData.properties->setProperty("Ice.Warn.Connections", "0");
        initData.properties->setProperty("Ice.Trace.Network", "0");
        initData.properties->setProperty("Ice.RetryIntervals", "0 100 500 1000");
        initData.properties->setProperty("Ice.MessageSizeMax","1024000");
        initData.properties->setProperty("Ice.ThreadPool.Server.Size","2");
    }
    
    iceCommunicator = Ice::initialize(initData);

	char szConnectStr[128];
	sprintf(szConnectStr,"SQLiter:tcp -h %s  -p 21000",szDBServerIp);            
    
    iceObjectPrx = iceCommunicator->stringToProxy(szConnectStr);
    iceObjectPrx = iceObjectPrx->ice_twoway();
    //iceObjectPrx = iceObjectPrx->ice_timeout(ICE_CONNECT_TIMEOUT);

	SQLiteOperatorPrx	=	::WISCOM::DataBase::SQLiteOperatorPrx::uncheckedCast(iceObjectPrx);
}

SQLiteIceInterface::~SQLiteIceInterface()
{
    if (iceCommunicator)
        iceCommunicator->destroy();
}

int SQLiteIceInterface::openDB(const char* szDBName)
{
    string  dbName  =   szDBName;
    
    if  (dbName.length() <= 0)
        return -1;
        
    int nRet    =   -1;

    TRY_CATCH_ICE_SENTENCE(
        nRet    =   SQLiteOperatorPrx->openDB(dbName);    
    )
    
    return 
        nRet;
}

bool SQLiteIceInterface::closeDB(int dbHandle)
{
    bool    bSuccessFlag    =   false;

    TRY_CATCH_ICE_SENTENCE(
        bSuccessFlag    =   SQLiteOperatorPrx->closeDB(dbHandle);
    )
    
    return
        bSuccessFlag;
}

bool SQLiteIceInterface::execDML(int dbHandle, const char* szSQL)
{
    bool    bSuccessFlag    =   false;
    
    TRY_CATCH_ICE_SENTENCE(
        bSuccessFlag = SQLiteOperatorPrx->execDML(dbHandle, szSQL);
    )    
    
    return
        bSuccessFlag;
}

bool SQLiteIceInterface::execDMLWithBlob(int dbHandle, const char* szSQL, 
               const unsigned char* pBlob1,  int nBlobSize1,
               const unsigned char* pBlob2,  int nBlobSize2,
               const unsigned char* pBlob3,  int nBlobSize3)
{
    bool       bRetFlag =   false;
    string     strSQL   =   szSQL;
    BlobValuePtr   pBlobValue1, pBlobValue2, pBlobValue3;

    pBlobValue1 =   new BlobValue;
    pBlobValue1->data.resize(nBlobSize1);
    memcpy(&(pBlobValue1->data[0]), pBlob1, nBlobSize1);
    
    pBlobValue2 =   new BlobValue;
    pBlobValue2->data.resize(nBlobSize2);
    memcpy(&(pBlobValue2->data[0]), pBlob2, nBlobSize2);

    pBlobValue3 =   new BlobValue;
    pBlobValue3->data.resize(nBlobSize3);
    memcpy(&(pBlobValue3->data[0]), pBlob3, nBlobSize3);
    
    TRY_CATCH_ICE_SENTENCE(
        bRetFlag = SQLiteOperatorPrx->execDMLWithBlob(
            dbHandle, strSQL, pBlobValue1, pBlobValue2, pBlobValue3);
    ) 
    
    if (pBlobValue1)
        pBlobValue1 =   NULL;
    
    if (pBlobValue2)
        pBlobValue2 =   NULL;
    
    if (pBlobValue3)
        pBlobValue3 =   NULL;
    
    return
        bRetFlag;
}

string SQLiteIceInterface::execQuery(int dbHandle, const char* szSQL)
{
    string  strRet  =   "";

    TRY_CATCH_ICE_SENTENCE(
        strRet = SQLiteOperatorPrx->execQuery(dbHandle, szSQL);
    )
    
    return
        strRet;
}

int SQLiteIceInterface::numFields(string queryIdentity)
{
    int     nRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       nRet =   SQLiteOperatorPrx->numFields(queryIdentity);
    )
    
    return  
        nRet; 
}

int SQLiteIceInterface::fieldIndex(string queryIdentity, string fieldName)
{
    int nRet    =   0;

    TRY_CATCH_ICE_SENTENCE(
       nRet =   SQLiteOperatorPrx->fieldIndex(queryIdentity, fieldName);
    )
    
    return  
        nRet; 
}

string SQLiteIceInterface::fieldName(string queryIdentity, int nFieldIndex)
{
    string  strFieldName    =   "";
    
    TRY_CATCH_ICE_SENTENCE(
       strFieldName =   SQLiteOperatorPrx->fieldName(queryIdentity, nFieldIndex);
    )
    
    return  
        strFieldName; 
}

int  SQLiteIceInterface::fieldDataType(string queryIdentity, int nFieldIndex)
{
    int nFieldDataType  =   -1;
    
    TRY_CATCH_ICE_SENTENCE(
       nFieldDataType =   SQLiteOperatorPrx->fieldDataType(queryIdentity, nFieldIndex);
    )
    
    return  
        nFieldDataType;
}

bool SQLiteIceInterface::fieldIsNullById(string queryIdentity, int nFieldIndex)
{
    bool    bRet    =   false;

    TRY_CATCH_ICE_SENTENCE(
       bRet =   SQLiteOperatorPrx->fieldIsNullById(queryIdentity, nFieldIndex);
    )

    return  
        bRet;
}

bool SQLiteIceInterface::fieldIsNullByName(string queryIdentity, string fieldName)
{
    bool    bRet    =   false;

    TRY_CATCH_ICE_SENTENCE(
       bRet =   SQLiteOperatorPrx->fieldIsNullByName(queryIdentity, fieldName);
    )
    
    return  
        bRet;
}

bool SQLiteIceInterface::nextRow(string  queryIdentity)
{
    bool    bRet    =   false;

    TRY_CATCH_ICE_SENTENCE(
        bRet    =   SQLiteOperatorPrx->nextRow(queryIdentity);
    )

    return  
        bRet;
}

long SQLiteIceInterface::lastRowId(int dbHandle)
{
    long    lRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       lRet =   SQLiteOperatorPrx->lastRowId(dbHandle);
    )
    
    return  
        lRet; 
}

bool SQLiteIceInterface::eof(string queryIdentity)
{
    bool    bRet    =   false;
    
    TRY_CATCH_ICE_SENTENCE(
       bRet =   SQLiteOperatorPrx->eof(queryIdentity);
    )
    
    return  
        bRet; 
}

int  SQLiteIceInterface::getIntField(string queryIdentity, int nFieldIndex)
{
    int nRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       nRet =   SQLiteOperatorPrx->getIntFieldValueByIndex(queryIdentity, nFieldIndex);
    )

    return  
        nRet;
}

int  SQLiteIceInterface::getIntField(string queryIdentity, const char* szFieldName)
{
    int nRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       nRet =   SQLiteOperatorPrx->getIntFieldValueByName(queryIdentity, szFieldName);
    )
    
    return  
        nRet;
}

double  SQLiteIceInterface::getFloatField(string queryIdentity, int nFieldIndex)
{
    double fRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       fRet =   SQLiteOperatorPrx->getFloatFieldValueByIndex(queryIdentity, nFieldIndex);
    )
    
    return  
        fRet;
}

double  SQLiteIceInterface::getFloatField(string queryIdentity, const char* szFieldName)
{
    double fRet    =   0;
    
    TRY_CATCH_ICE_SENTENCE(
       fRet =   SQLiteOperatorPrx->getFloatFieldValueByName(queryIdentity, szFieldName);
    )
    
    return  
        fRet;
}
    
string  SQLiteIceInterface::getStringField(string queryIdentity, int nFieldIndex)
{
    string  strRet  =   "";
    
    TRY_CATCH_ICE_SENTENCE(
       strRet =   SQLiteOperatorPrx->getStringFieldValueByIndex(queryIdentity, nFieldIndex);
    )
    
    return  
        strRet;
}

string  SQLiteIceInterface::getStringField(string queryIdentity, const char* szFieldName)
{
    string  strRet  =   "";
    
    TRY_CATCH_ICE_SENTENCE(
       strRet =   SQLiteOperatorPrx->getStringFieldValueByName(queryIdentity, szFieldName);
    )
    
    return  
        strRet;
}

unsigned char* SQLiteIceInterface::getBlobField(string queryIdentity, int nFieldIndex, int* pBlobSize)
{
    int nBlobBufferSize =   0;
    BlobValuePtr    pBlobValue  =   NULL;
    unsigned    char*   pBlobBuffer =   NULL;
    
    TRY_CATCH_ICE_SENTENCE(
       pBlobValue =   SQLiteOperatorPrx->getBlobFieldValueByIndex(queryIdentity, nFieldIndex);
    )
    
    if (pBlobValue)
        nBlobBufferSize =   pBlobValue->data.size();
    
    if (nBlobBufferSize >0) {
        pBlobBuffer =   (unsigned char *)malloc(nBlobBufferSize);
        if (pBlobBuffer) {
            memcpy(pBlobBuffer, &(pBlobValue->data[0]), nBlobBufferSize);
            *pBlobSize  =  nBlobBufferSize;
            return pBlobBuffer;
        }
    }
    
    *pBlobSize   =   0;
    return NULL;
}

unsigned char* SQLiteIceInterface::getBlobField(string queryIdentity, const char* szFieldName,  int* pBlobSize)
{
    int nBlobBufferSize =   0;
    BlobValuePtr    pBlobValue  =   NULL;
    unsigned    char*   pBlobBuffer =   NULL;
    
    TRY_CATCH_ICE_SENTENCE(
       pBlobValue =   SQLiteOperatorPrx->getBlobFieldValueByName(queryIdentity, szFieldName);
    )
    
    if (pBlobValue)
        nBlobBufferSize =   pBlobValue->data.size();
    
    if (nBlobBufferSize >0) {
        pBlobBuffer =   (unsigned char *)malloc(nBlobBufferSize);
        if (pBlobBuffer) {
            memcpy(pBlobBuffer, &(pBlobValue->data[0]), nBlobBufferSize);
            *pBlobSize  =  nBlobBufferSize;
            return pBlobBuffer;
        }
    }
    
    *pBlobSize   =   0;
    return NULL;
}

bool SQLiteIceInterface::interrupt(int dbHandle)
{
    bool    bRet    =   false;
    
    TRY_CATCH_ICE_SENTENCE(
        bRet    =   SQLiteOperatorPrx->interrupt(dbHandle);    
    )
    
    return
        bRet;
}

bool SQLiteIceInterface::finalize(string queryIdentity)
{
    bool    bRet    =   false;

    TRY_CATCH_ICE_SENTENCE(
       bRet =   SQLiteOperatorPrx->finalize(queryIdentity);
    )
    
    return
        bRet;
}