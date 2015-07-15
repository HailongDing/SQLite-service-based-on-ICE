//---------------------------------------------------------------
//
//---------------------------------------------------------------
#include <string>
#include <string.h> 
#include <stdio.h>
#include <stdlib.h> 
#include "SQLiteInterfaceWithoutIce.h"

using namespace std;

unsigned char*  ReadPicFile(const char* picName, int* pSize);
bool SavePicToFile(const unsigned char* pBuffer, int nBufferSize);

int main()
{
    int     dbHandle; 
    char    szTestDBName[100];
    SQLiteIceInterface*  pDBProxy;
    
    pDBProxy =   new SQLiteIceInterface("172.18.13.12");
    
    srand (time(NULL));
    sprintf(szTestDBName, "DB_%d", rand());
    dbHandle    =   pDBProxy->openDB(szTestDBName);
    if (-1 == dbHandle) {
        printf("open SQLite database error!\n");
        return -1;
    }
    
    // initialize 
    char*   pSQL    =    "create table  employeelist(\
                recid   INTEGER PRIMARY KEY,     \
                name    VARCHAR(40) NOT	NULL,    \
                salary  REAL    NOT	NULL,        \
                age     INTEGER,                 \
                phonenumber VARCHAR(40) NOT	NULL,\
                address     VARCHAR(40),         \
                portraitpic    BLOB(10000),      \
                residencepic   BLOB(10000))";
    
    if (!pDBProxy->execDML(dbHandle, pSQL))    {
        printf("execDML error! \n");
        return  -1;
    }
    
    // insert 
    char    szSQL[1024];
    for(int i=1; i<=10; i++) {
        memset(szSQL, 0x00, sizeof(szSQL));
        sprintf(szSQL, "insert into employeelist(name,salary,age,phonenumber,address) \
                values('name_%d',%f,%d,'%d','addr_%d')",i,i*1000.1,i*10,i*10101,i);
        if (!pDBProxy->execDML(dbHandle,szSQL))  {
            printf("execDML error!\n");
            break;
        }
    }

    string queryIdentity   =   pDBProxy->execQuery(
        dbHandle,"select name,salary,age,phonenumber,address from employeelist"); 
    if ("" == queryIdentity)    {
        printf("execQuery error!\n");
        return  -1;
    }

    // query data 
    string  strName;
    float   fSalary;
    int     nAge;
    string  strPhoneNum;
    string  strAddress;
    int rowCount    =   0;
    while(!pDBProxy->eof(queryIdentity))    {
        rowCount++;
        strName =   pDBProxy->getStringField(queryIdentity, "name");
        fSalary =   pDBProxy->getFloatField(queryIdentity,"salary");
        nAge    =   pDBProxy->getIntField(queryIdentity,"age");
        strPhoneNum = pDBProxy->getStringField(queryIdentity,"phonenumber");
        strAddress  = pDBProxy->getStringField(queryIdentity,"address");
        
        printf("row %d \n",rowCount);
        printf("name=%s, salary=%f, age=%d, phonenumber=%s, address=%s\n",
            strName.c_str(),fSalary,nAge,strPhoneNum.c_str(),strAddress.c_str());
        
        pDBProxy->nextRow(queryIdentity);
    }
    pDBProxy->finalize(queryIdentity);

    // insert blob
    int nPicSize1,nPicSize2;
    unsigned char*  pPicBuffer1;
    unsigned char*  pPicBuffer2;
    pPicBuffer1 =   ReadPicFile("portraitpic.jpg", &nPicSize1);
    pPicBuffer2 =   ReadPicFile("residencepic.jpg", &nPicSize2);
    if ((NULL != pPicBuffer1) && (NULL != pPicBuffer2)) {
        for(int j=1; j<=10; j++)    {
            memset(szSQL, 0x00, sizeof(szSQL));
            sprintf(szSQL, "insert into   employeelist\
                (name,salary,phonenumber,portraitpic,residencepic) \
                values('name2_%d',%f,'%d',?,?)", j,j*1000.1,j*10101);
            if (!pDBProxy->execDMLWithBlob(dbHandle, szSQL, 
                (unsigned char*)pPicBuffer1, nPicSize1, 
                (unsigned char*)pPicBuffer2, nPicSize2, NULL, 0))   {
                printf("execDMLWithBlob error!\n");
                break;
            }
        }

        free(pPicBuffer1);
        free(pPicBuffer2);
    }
    
    // query 
    queryIdentity   =   pDBProxy->execQuery(dbHandle, "select * from employeelist"); 
    if ("" == queryIdentity)    {
        printf("execQuery error!\n");
        return  -1;
    }

    int     nFieldIndex;
    string  strFielName;
    int nTotalFieldNum  =   pDBProxy->numFields(queryIdentity);
    printf("Total field number = %d \n", nTotalFieldNum);
    for(int k=0; k<nTotalFieldNum; k++) {
        strFielName =   pDBProxy->fieldName(queryIdentity, k);
        nFieldIndex =   pDBProxy->fieldIndex(queryIdentity, strFielName.c_str());
        printf("fieldIndex:%d, fieldName:%s \n", nFieldIndex, strFielName.c_str());
    }
    
    int     nFieldDataType;
    int     nValue;
    float   fValue;
    string  strValue;
    bool    bValue;
    int     nBlobSize   =   0;
    int     nIndex      =   0;
    unsigned char* pBlobValue   =   NULL;
    while(!pDBProxy->eof(queryIdentity))    {
        printf("The %d row: \n", ++nIndex);
        for(int s=0; s<nTotalFieldNum; s++) {
            nFieldDataType =   pDBProxy->fieldDataType(queryIdentity, s);
            printf("fieldIndex: %d, nFieldDataType:%d \n", s+1, nFieldDataType);
            switch(nFieldDataType)  {
                case SQLITE_INTEGER:
                    nValue  =  pDBProxy->getIntField(queryIdentity, s); 
                    break;
                    
                case SQLITE_FLOAT:
                    fValue =  pDBProxy->getFloatField(queryIdentity, s); 
                    break;
                    
                case SQLITE_BLOB:
                    pBlobValue = pDBProxy->getBlobField(queryIdentity, s, &nBlobSize); 
                    if (pBlobValue)   {
                        SavePicToFile(pBlobValue, nBlobSize);
                        free(pBlobValue);
                    }
                    break;
                    
                case SQLITE_TEXT:
                    strValue = pDBProxy->getStringField(queryIdentity, s);   
                    break;
                    
                case SQLITE_NULL:
                    bValue  =   pDBProxy->fieldIsNullById(queryIdentity, s);
                    break;
                    
                default:
                    printf("unknown field data type. \n");
                    break;
            }
        }
        pDBProxy->nextRow(queryIdentity);
    }
    pDBProxy->finalize(queryIdentity);
    
    
    pDBProxy->closeDB(dbHandle);
    
    if (pDBProxy)
       delete   pDBProxy;
    
    return  0;
}

unsigned char*  ReadPicFile(const char* picName, int* pSize)
{
    if ((NULL == picName) || (NULL == pSize))
        return NULL;
    
    int nSize   =   0;
    FILE*   pFile   =   NULL;
    unsigned char* pBuffer  =   NULL;
    
    *pSize  =   0;
    
    pFile   =   fopen(picName,"rb");
    if (NULL == pFile)
        return NULL;
    
    fseek(pFile, 0, SEEK_END);
    nSize   =   ftell(pFile);
    pBuffer =   (unsigned char *)malloc(nSize);
    if (pBuffer)    {
        fseek(pFile, 0, SEEK_SET);
        if (fread(pBuffer, 1, nSize, pFile) > 0)
            *pSize  =   nSize;
    }
    
    fclose(pFile);
    
    return
        pBuffer;
}

bool SavePicToFile(const unsigned char* pBuffer, int nBufferSize)
{
    static  int nFileIndex  =   0;
    
    if ((NULL == pBuffer) || (0 == nBufferSize))
        return false;
    
    char    szFileName[100];
    sprintf(szFileName, "image%d.jpg", ++nFileIndex);
    FILE*   pFile   =   fopen(szFileName, "wb");
    if (NULL == pFile)
        return false;
    
    fwrite(pBuffer, 1, nBufferSize, pFile);
    fclose(pFile);
    
    return  true;
}