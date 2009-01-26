/*
============================================================================
 Name        : CacheDatabase.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CCacheDatabase implementation
============================================================================
*/
#include <coemain.h>
#include <badesca.h>    // CDesCArrayFlat
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers
#include <eikenv.h>

#include "CacheDatabase.h"
#include "PreDefine.h"


const int KCustomSqlMaxLength = 256;
const int KArrayGranularity = 5;     // for CDesCArrayFlat


CCacheDatabase::CCacheDatabase(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
														 , iFsSession(CCoeEnv::Static()->FsSession())
{
	// No implementation required
}


CCacheDatabase::~CCacheDatabase()
{
	Close();
}

CCacheDatabase* CCacheDatabase::NewLC(CMainEngine& aMainEngine)
{
	CCacheDatabase* self = new (ELeave)CCacheDatabase(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CCacheDatabase* CCacheDatabase::NewL(CMainEngine& aMainEngine)
{
	CCacheDatabase* self=CCacheDatabase::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CCacheDatabase::ConstructL()
{
	//初始化数据库
	_LIT(KDBNAME,"soarsky.db");

	TFileName fileName;
	fileName.Copy (iMainEngine.GetAppPath());
	fileName.Append (KDBNAME);

	TInt err = OpenDb (fileName);
	ASSERT(KErrNone == err);		//在本程序中,数据库文件一定由CDBEngine创建成功

	CreatePageCacheTableL();
	CreateFirstPageCacheTableL();
}
//////////////////////////////////////////////////////////////////////////
//public: 数据库基本操作
//////////////////////////////////////////////////////////////////////////
TInt CCacheDatabase::OpenDb(const TFileName& aExistingFile)
{
	Close();

	if(!BaflUtils::FileExists(iFsSession, aExistingFile))
	{
		return KErrNotFound;
	}

	TRAPD(error,
		iFileStore = CPermanentFileStore::OpenL(iFsSession, aExistingFile,EFileRead|EFileWrite);
		iFileStore->SetTypeL(iFileStore->Layout());
		iDb.OpenL(iFileStore,iFileStore->Root())
		);
	if(error!=KErrNone)
	{
		return error;
	}

	iOpen = ETrue;
	return KErrNone;
}


TInt CCacheDatabase::RemoveDb(const TFileName& aExistingFile)
{
	Close();

	if(!BaflUtils::FileExists(iFsSession, aExistingFile))
	{
		return KErrNotFound;
	}

	// It is enough to delete the database file directly. Because this example
	// demonstrates DDL statements, it first opens and drops the Books table.

	TInt error = OpenDb(aExistingFile);
	if(error!=KErrNone)
	{
		return error;
	}

	//DropBookMarksTable();
	Close();

	iFsSession.Delete(aExistingFile);
	return KErrNone;
}

TInt CCacheDatabase::Close()
{
	iDb.Close();
	if(iFileStore)
	{
		delete iFileStore;
		iFileStore = NULL;
	}
	iOpen = EFalse;
	return KErrNone;
}

TBool CCacheDatabase::IsOpen() const
{
	return iOpen;
}
//////////////////////////////////////////////////////////////////////////
//public: 页缓冲记录读取
//////////////////////////////////////////////////////////////////////////
/*

//========================================================================================================
//==================================Page Cache Table======================================================
void CDBEngine::CreatePageCacheTableL()
{
	TDbCol fileNameCol(KPageCacheFileName, EDbColText);
	TDbCol firstTypeCol(KPageCacheFirstType, EDbColInt32);  
	TDbCol secondTypeCol(KPageCacheSecondType, EDbColInt32);
	TDbCol thirdTypeCol(KPageCacheThirdType, EDbColInt32);
	TDbCol pageIndexCol(KPageCachePageIndex, EDbColInt32);
	TDbCol timeStampCol(KPageCacheTimeStamp, EDbColInt32);

	fileNameCol.iAttributes = TDbCol::ENotNull;
	firstTypeCol.iAttributes = TDbCol::ENotNull;
	secondTypeCol.iAttributes = TDbCol::ENotNull;
	thirdTypeCol.iAttributes = TDbCol::ENotNull;
	pageIndexCol.iAttributes = TDbCol::ENotNull;
	timeStampCol.iAttributes = TDbCol::ENotNull;

	CDbColSet* pageCacheColSet = CDbColSet::NewLC();
	pageCacheColSet ->AddL(fileNameCol);
	pageCacheColSet ->AddL(firstTypeCol);
	pageCacheColSet ->AddL(secondTypeCol);
	pageCacheColSet ->AddL(thirdTypeCol);
	pageCacheColSet ->AddL(pageIndexCol);
	pageCacheColSet ->AddL(timeStampCol);

	User::LeaveIfError(iDb.CreateTable(KPageCacheTable,	*pageCacheColSet ));
	CleanupStack::PopAndDestroy(pageCacheColSet);
}

_LIT(KPageCacheTable,		"PageCache");
_LIT(KPageCacheFileName,	"FileName");
_LIT(KPageCacheFirstType,	"FirstTypeId");
_LIT(KPageCacheSecondType,	"SecondTypeId");
_LIT(KPageCacheThirdType,	"ThirdTypeId");
_LIT(KPageCachePageIndex,	"PageIndex");
_LIT(KPageCacheTimeStamp,	"TimeStamp");

//添加一条记录,当超过上限时移除最早的一条记录
TBool CDBEngine::AddPageCacheRecord(const TPageRecord& aRecord)
{
	if(aParmName.Length()==0 || aParmValue.Length ()==0)
	{
		return KErrGeneral;
	}

	TBuf<KCustomSqlMaxLength> sqlStr;

	sqlStr.Append(KSelect);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KDot);
	sqlStr.Append(KParmValueCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KSystemParmTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aParmName);
	sqlStr.Append (_L("'"));

	RDbView view;    // Create a view on the database
	TInt error = view.Prepare(iDb, TDbQuery(sqlStr, EDbCompareFolded));
	if(error!=KErrNone)
	{
		return error;
	}
	error = view.EvaluateAll();
	if(error!=KErrNone)
	{
		return error;
	}

	//::WriteLogsL(_L("SetSystemParmWithSQL-3")); 
	TInt nCount = view.CountL();
	//::WriteLogsL(_L("SetSystemParmWithSQL-4")); 
	if(nCount==0)
	{
		//::WriteLogsL(_L("SetSystemParmWithSQL-5-1")); 
		TRAP(error,
			view.InsertL();  // Insert a row. Column order matches sql select statement
		view.SetColL(1, aParmName);
		view.SetColL(2, aParmValue);
		);
		if(error!=KErrNone)
		{
			return error;
		}

		TRAP(error, view.PutL()); // Complete insertion
		if(error!=KErrNone)
		{
			return error;
		} 
		view.Close();
		//::WriteLogsL(_L("SetSystemParmWithSQL-5-2")); 
		return KErrNone;
	}
	else
	{
		view.Close();

		//::WriteLogsL(_L("SetSystemParmWithSQL-6-1")); 
		_LIT(KSQLUpdateStart, "UPDATE SystemParm SET ParmValue = '");
		_LIT(KSQLUpdateMiddle, "' WHERE ParmName = '");
		_LIT(KSQLUpdateEnd, "'");

		TBuf<KCustomSqlMaxLength> sqlStr;
		sqlStr.Append(KSQLUpdateStart);
		sqlStr.Append(aParmValue);
		sqlStr.Append(KSQLUpdateMiddle);
		sqlStr.Append(aParmName);
		sqlStr.Append(KSQLUpdateEnd);
		//::WriteLogsL(sqlStr); 
		//::WriteLogsL(_L("SetSystemParmWithSQL-6-2")); 
		TInt nRet;

		nRet=iDb.Execute(sqlStr);
		//::WriteLogsL(_L("SetSystemParmWithSQL-6-3")); 
		if(nRet)
		{
			//::WriteLogsL(_L("SetSystemParmWithSQL-6-4")); 
			return KErrNone;
		}
		else
		{
			//::WriteLogsL(_L("SetSystemParmWithSQL-6-5")); 
			return KErrGeneral;
		}
	}	
}

TBool CDBEngine::RemovePageCacheRecord(const TPageRecord& aRecord)
{
	_LIT(KDeleteFrom, "DELETE FROM ");
	_LIT(KWhere, " WHERE ");
	_LIT(KEqual, " = '");
	_LIT(KAnd, " AND ");

	// Sql: DELETE FROM Books
	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KDeleteFrom);
	sqlStr.Append(KDownLoadHistoryTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KDownLoadFileNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aFileName);
	sqlStr.Append(_L("'"));
	//sqlStr.Append(KAnd);
	//sqlStr.Append(KDownLoadURLCol);
	//sqlStr.Append(KEqual);
	//sqlStr.Append(aURL);
	//sqlStr.Append(_L("'"));

	RDbUpdate updOp;
	TRequestStatus incStat(1);
	TInt updStat = updOp.Execute(iDb, sqlStr, EDbCompareFolded);

	while (updStat==KErrNone && incStat ==1)
	{
		updOp.Next(incStat);           // Start async operation. It returns
		// immediately.
		User::WaitForRequest(incStat); // For simplicity wait completion here.
	}
	TInt aResultCount = updOp.RowCount();

	updOp.Close();

	if(updStat!=KErrNone)
	{
		return updStat;       // System wide error code
	}
	else
	{
		return incStat.Int(); // KErrNone or system wide error code	
	}

}

TBuf CDBEngine::RemoveOldestPageCacheRecord(TPageRecord& aRecord)
{

}

TBool CDBEngine::RemoveAllPageCacheRecord()
{

}

TBool CDBEngine::GetPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult)
{
	TPtrC nParmValue;
	TInt nRet;

	TBuf<KCustomSqlMaxLength> sqlStr;
	sqlStr.Append(KSelect);
	sqlStr.Append(KParmValueCol);
	sqlStr.Append(KFrom);
	sqlStr.Append(KPageCacheTable);
	sqlStr.Append(KWhere);
	sqlStr.Append(KParmNameCol);
	sqlStr.Append(KEqual);
	sqlStr.Append(aParmName);
	sqlStr.Append(_L("'"));

	RDbView view;
	User::LeaveIfError(
		view.Prepare(iDb, TDbQuery(sqlStr), view.EReadOnly));

	User::LeaveIfError(view.EvaluateAll());

	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL(colSet);

	if(view.FirstL()) 
	{
		view.GetL();

		nParmValue.Set(view.ColDes(colSet->ColNo(KParmValueCol)));

		aParmValue.Zero ();
		aParmValue.Append (nParmValue);

		nRet=0;
	}
	else
	{
		aParmValue.Zero ();

		nRet= 1;
	}

	CleanupStack::PopAndDestroy(colSet);
	view.Close();

	return nRet;	

}

TInt CDBEngine::PageCacheRecordCount() const
{
	
	return 0;
}

//========================================================================================================
//==================================First Page Cache Table======================================================
void CDBEngine::CreateFirstPageCacheTableL()
{
	TDbCol fileNameCol(KPageCacheFileName, EDbColText);
	TDbCol firstTypeCol(KPageCacheFirstType, EDbColInt32);  
	TDbCol secondTypeCol(KPageCacheSecondType, EDbColInt32);
	TDbCol thirdTypeCol(KPageCacheThirdType, EDbColInt32);
	TDbCol pageIndexCol(KPageCachePageIndex, EDbColInt32);
	TDbCol timeStampCol(KPageCacheTimeStamp, EDbColInt32);

	fileNameCol.iAttributes = TDbCol::ENotNull;
	firstTypeCol.iAttributes = TDbCol::ENotNull;
	secondTypeCol.iAttributes = TDbCol::ENotNull;
	thirdTypeCol.iAttributes = TDbCol::ENotNull;
	pageIndexCol.iAttributes = TDbCol::ENotNull;
	timeStampCol.iAttributes = TDbCol::ENotNull;

	CDbColSet* pageCacheColSet = CDbColSet::NewLC();
	pageCacheColSet ->AddL(fileNameCol);
	pageCacheColSet ->AddL(firstTypeCol);
	pageCacheColSet ->AddL(secondTypeCol);
	pageCacheColSet ->AddL(thirdTypeCol);
	pageCacheColSet ->AddL(pageIndexCol);
	pageCacheColSet ->AddL(timeStampCol);

	User::LeaveIfError(iDb.CreateTable(KFirstPageCacheTable,	*pageCacheColSet ));
	CleanupStack::PopAndDestroy(pageCacheColSet);
}

TBool CDBEngine::AddFirstPageCacheRecord(const TPageRecord& aCondition)
{

}

TBool CDBEngine::RemoveFirstPageCacheRecord(const TPageRecord& aCondition)
{

}

TBool CDBEngine::GetFirstPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult)
{

}


*/
