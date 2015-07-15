#ifndef _WISCOM_DATABASE_ICE_INTERFACE_
#define _WISCOM_DATABASE_ICE_INTERFACE_

module  WISCOM{
    module DataBase {
    
        sequence<byte> ByteSeq;
        
        class BlobValue{
            ByteSeq data;
        };
        
        interface   SQLiteOperator{
            int     openDB(string dbName);
            bool    closeDB(int dbHandle);
            
            bool    execDML(int dbHandle, string strSQL);
            bool    execDMLWithBlob(int dbHandle, string strSQL, 
                      BlobValue data1, BlobValue data2, BlobValue data3);
            string  execQuery(int dbHandle, string strSQL);
            
            int     numFields(string queryIdentity);       
            int     fieldIndex(string queryIdentity, string fieldName);  
            string  fieldName(string queryIdentity, int nFieldIndex);    
            int     fieldDataType(string queryIdentity, int nFieldIndex);
            
            bool    fieldIsNullById(string queryIdentity, int nFieldIndex); 
            bool    fieldIsNullByName(string queryIdentity, string fieldName);
            
            bool    nextRow(string queryIdentity);
            long    lastRowId(int dbHandle);        
            bool    eof(string queryIdentity);
            
            int     getIntFieldValueByIndex(string queryIdentity, int nFieldIndex);
            int     getIntFieldValueByName(string queryIdentity, string fieldName);
            
            double  getFloatFieldValueByIndex(string queryIdentity, int nFieldIndex);   
            double  getFloatFieldValueByName(string queryIdentity, string fieldName);
            
            string  getStringFieldValueByIndex(string queryIdentity, int nFieldIndex);
            string  getStringFieldValueByName(string queryIdentity, string fieldName);
            
            BlobValue  getBlobFieldValueByIndex(string queryIdentity, int nFieldIndex);
            BlobValue  getBlobFieldValueByName(string queryIdentity, string fieldName);
            
            bool    interrupt(int dbHandle);       
            bool    finalize(string queryIdentity);
        };
    };
};

#endif //_WISCOM_DATABASE_ICE_INTERFACE_
