// ssOleDbAPI.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SSOLEDBAPI_H__)
#define __SSOLEDBAPI_H__

#include "SQLAPI.h"

// API header(s)
#include <sqloledb.h>
#include <oledberr.h>

extern const GUID SA_DBPROPSET_DATASOURCEINFO;
extern const DBID SA_DB_NULLID;
extern const GUID SA_DBPROPSET_DBINIT;
extern const GUID SA_DBPROPSET_SQLSERVERDBINIT;
extern const GUID SA_DBPROPSET_ROWSET;
extern const IID SA_IID_IAccessor;
extern const GUID SA_DBSCHEMA_PROCEDURE_PARAMETERS;
extern const GUID SA_DBGUID_MSSQLXML;

extern void AddSSOleDbSupport();
extern void ReleaseSSOleDbSupport();

// API declarations
class ssOleDbAPI : public saAPI
{
public:
	ssOleDbAPI();
};

class SQLAPI_API ssOleDbConnectionHandles : public saConnectionHandles
{
public:
	ssOleDbConnectionHandles();

	IDBInitialize *pIDBInitialize;
	IDBCreateCommand *pIDBCreateCommand;
	ITransactionLocal *pITransactionLocal;
};

class SQLAPI_API ssOleDbCommandHandles : public saCommandHandles
{
public:
	ssOleDbCommandHandles();

	ICommandText *pICommandText;
	IMultipleResults *pIMultipleResults;
	IRowset *pIRowset;
};

extern ssOleDbAPI g_ssOleDbAPI;

#endif // !defined(__SSOLEDBAPI_H__)
