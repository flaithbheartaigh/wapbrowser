/*
============================================================================
 Name        : CacheDatabase.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CCacheDatabase declaration
============================================================================
*/

#ifndef CACHEDATABASE_H
#define CACHEDATABASE_H

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs


_LIT(KSelect,		"SELECT ");
_LIT(KFrom,			" FROM ");
_LIT(KWhere,		" WHERE ");
_LIT(KEqual,		" = '");
_LIT(KOrderBy,		"' ORDER BY ");
_LIT(KDot,			", ");

//页面缓冲记录
_LIT(KPageCacheTable,		"PageCache");
_LIT(KFirstPageCacheTable,	"FirstPageCache");

_LIT(KPageCacheFileName,	"FileName");
_LIT(KPageCacheFirstType,	"FirstTypeId");
_LIT(KPageCacheSecondType,	"SecondTypeId");
_LIT(KPageCacheThirdType,	"ThirdTypeId");
_LIT(KPageCachePageIndex,	"PageIndex");
_LIT(KPageCacheTimeStamp,	"TimeStamp");


const int KTitleMaxLength = 60;            
const int KDescriptionMaxLength = 128;     

const int KBookItemMaxLength = 256;

_LIT(KSeparator,"|");

class CFileStore;

class TPageRecord;

class CCacheDatabase : public CBase
{
public: // Constructors and destructor
	~CCacheDatabase();
	static CCacheDatabase* NewL(CMainEngine& aMainEngine);
	static CCacheDatabase* NewLC(CMainEngine& aMainEngine);

private:
	CCacheDatabase(CMainEngine& aMainEngine);
	void ConstructL();

public:
	TInt OpenDb(const TFileName& aExistingBookFile);

	TInt CreateDb(const TFileName& aNewBookFile);

	TInt RemoveDb(const TFileName& aExistingBookFile);

	TInt Close();

	TBool IsOpen() const;

	//////////////////////////////////////////////////////////////////////////
	//记录页缓冲文件
	//////////////////////////////////////////////////////////////////////////
	//没有记录则添加记录,有记录则更新
	TBool AddPageCacheRecord(const TPageRecord& aRecord);
	//RemovePageCacheRecord
	//删除与TPageRecord中的索引匹配的记录
	TBool RemovePageCacheRecord(const TPageRecord& aRecord);
	//删除最旧的一条记录,同时返回记录
	TBool RemoveOldestPageCacheRecord(TPageRecord& aRecord);
	//删除所有记录
	TBool RemoveAllPageCacheRecord();
	//GetPageCacheRecord
	//aCondition:存放获取记录的条件
	//aResult:获取到的记录
	TBool GetPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult);
	//返回记录总数
	TInt PageCacheRecordCount() const;
	// 	TPageRecord GetOldestPageCacheRecord() const;

	//////////////////////////////////////////////////////////////////////////
	//用于页缓冲,缓冲所有栏目的第一页
	//////////////////////////////////////////////////////////////////////////
	//AddFirstPageCacheRecord
	//没有记录则添加记录,有记录则更新
	TBool AddFirstPageCacheRecord(const TPageRecord& aCondition);
	TBool RemoveFirstPageCacheRecord(const TPageRecord& aCondition);
	TBool GetFirstPageCacheRecord(const TPageRecord& aCondition,TPageRecord& aResult);


private:
	//用于页缓冲
	void CreatePageCacheTableL();
	//void DropPageCacheTable();

	//用于页缓冲,缓冲所有栏目的第一页
	void CreateFirstPageCacheTableL();


private: // Member data
	CMainEngine&		iMainEngine;
	RFs             	iFsSession;  /* For use of iFileStore                   */
	RDbStoreDatabase	iDb;/* For database operations                 */
	CFileStore*     	iFileStore;  /* For creating and opening database files */
	TBool           	iOpen;       /* Flag indicating iBookDb open status*/
};

#endif // CACHEDATABASE_H
