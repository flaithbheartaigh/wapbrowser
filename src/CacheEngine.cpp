/*
============================================================================
 Name        : CacheEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CCacheEngine implementation
============================================================================
*/
#include <coemain.h>
#include <bautils.h>
#include <s32file.h>

#include "CacheEngine.h"
#include "CacheObserver.h"
#include "Window.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "HttpManager.h"
#include "DBEngine.h"
#include "Dialog.h"
#include "tinyxml.h"
#include "PreDefine.h"

enum TEventCmd
{
	ECmdNull,
	ECmdReading,
	ECmdSynchronize,
//	ECmdCancel
};
enum TRequestType
{
	ERequestPage,
	ERequestSync
};

_LIT(KCacheFilePath,		"Cache\\");
_LIT(KTempFileName,			"Temp.xml");

_LIT(KFirstCacheFileName,	"FirstCacheFile");
_LIT(KCacheFileName,		"CacheFile");
_LIT(KXmlPostfix,			".xml");

_LIT(KAllCacheFileName,		"CacheFile-");			//通配符文件名,用于删除所有缓冲文件
//_LIT(KAllCacheFileName,		"CacheFile-*.xml");			//通配符文件名,用于删除所有缓冲文件

CCacheEngine::CCacheEngine(CMainEngine& aMainEngine,MCacheObserver& aObserver) : iMainEngine(aMainEngine)
																			   , iControlFactory(aMainEngine.ControlFactory())
																			   , iDbEngine(aMainEngine.DBEngine())
																			   , iObserver(aObserver)
																			   , iFs(CCoeEnv::Static()->FsSession())
																			   , iFirstTypeId(KInvalidIndex)
																			   , iSecondTypeId(KInvalidIndex)
																			   , iThirdTypeId(KInvalidIndex)
																			   , iRequestingPageIndex(KInvalidIndex)
{
}

void CCacheEngine::ConstructL()
{
	TFileName cachePath = GetFullName(KNullDesC);
	//如果存放缓存文件的目录不存在,就创建一个
	if(!BaflUtils::PathExists(iFs,cachePath))
	{
		TInt err = iFs.MkDir(cachePath);
		User::LeaveIfError(err);
	}
}

CCacheEngine::~CCacheEngine()
{
	iMainEngine.WriteLog16(_L("CCacheEngine::~CCacheEngine"));
	CancelWaitingDialog();
	CancelRequest();
	iMainEngine.WriteLog16(_L("CCacheEngine::~CCacheEngine End"));
}

CCacheEngine* CCacheEngine::NewL(CMainEngine& aMainEngine,MCacheObserver& aObserver)
{
	CCacheEngine* self = NewLC(aMainEngine,aObserver);
	CleanupStack::Pop();
	return self;
}

CCacheEngine* CCacheEngine::NewLC(CMainEngine& aMainEngine,MCacheObserver& aObserver)
{
	CCacheEngine* self = new (ELeave) CCacheEngine(aMainEngine,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CCacheEngine::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdNull:
		break;

	case ECmdReading:
		{
			TInt requestingPageIndex = CancelRequestPage();
			RequestPage(requestingPageIndex);
		}
		break;

	case ECmdSynchronize:
		CancelSynchronize();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MOperationObserver
//////////////////////////////////////////////////////////////////////////
void CCacheEngine::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	UtilityTools::WriteLogsL(_L("CCacheEngine::OperationEvent"));
#ifdef _DEBUG
	UtilityTools::WriteTestFileL(_L("C:\\TempXml.Xml"),aEventData);
#endif
	if(0 == aEventType)
	{
		switch(aType)
		{
		case ERequestPage:
			ParseNewPage(aEventData);
			break;

		case ERequestSync:
			ParseSynchronize(aEventData);
			break;

		default:
			ASSERT(EFalse);
			break;
		}
	}
	else
	{
		TBuf<50> buf;
		buf.Append(_L("Unknown Error "));
		buf.AppendFormat(_L("aEventType = %d"),aEventType);
		//iControlFactory.CreateTipDialog(buf,*this,ECmdNull);
		iMainEngine.WriteLog16(_L("aEventType = %d,aType = %d"),aEventType,aType);
		switch(aType)
		{
		case ERequestPage:
			{
				TInt requestingPageIndex = CancelRequestPage();
				RequestPage(requestingPageIndex);
			}
			break;

		case ERequestSync:
			CancelSynchronize();
			break;

		default:
			ASSERT(EFalse);
			break;
		}
	}
	{
		iIsWaitingPage = EFalse;
		iIsWaitingSync = EFalse;
		CancelWaitingDialog();
	}
	UtilityTools::WriteLogsL(_L("CCacheEngine::OperationEvent End"));
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CCacheEngine::RequestSynchronize()
{
	ASSERT(iTypeIsSet);
	TInt timeStamp = 0;
	TPageRecord* record = CreateRecordLC();
	if(iDbEngine.GetFirstPageCacheRecord(*record,*record))
	{
		if(BaflUtils::FileExists(iFs,record->iFileName))
		{
			timeStamp = record->iTimeStamp;
		}
		else
		{
			iDbEngine.RemoveFirstPageCacheRecord(*record);
		}
	}
	CleanupStack::PopAndDestroy();
	HBufC8* url = MakeUrlLC(1,timeStamp);
	iMainEngine.GetHttpManager()->SendGetRequest(*this,*url,ERequestSync);
	CleanupStack::PopAndDestroy();	//url
	iIsSynchronizing = ETrue;
}

void CCacheEngine::RequestPageFileName(TInt aPageIndex)
{
	ASSERT(iTypeIsSet);
	ASSERT(aPageIndex > 0);		//页号从1开始


	TBool readResult = ReadFileFromCache(aPageIndex);
	RDebug::Print(_L("this = %x"),this);
	if(readResult)
	{
		if(KInvalidIndex == iRequestingPageIndex)					//当前没有页被请求,对下一页的判断,否则跳过对下一页的判断
		{
			TInt nextPageIndex = aPageIndex + 1;
			TPageRecord* record = CreateRecordLC(nextPageIndex);
			if(!GetRecordFromDB(nextPageIndex,*record))				//检测下一页是否存在,如果不存在则向服务器请求下一页
			{
				//待优化,如果文件存在,记录不存在,写入记录
				RequestPage(nextPageIndex);
			}
			CleanupStack::PopAndDestroy();
		}
	}
	else
	{
		if(1 == aPageIndex)
		{
			//ASSERT(iIsSynchronizing);							//没有首页,也没有发同步请求,是错误的
			if(!iIsSynchronizing)
			{
				RequestSynchronize();
			}
			CreateWaitingDialog(iMainEngine.GetDesById(ECoCoTestRes_ReadingListInfo),ECmdSynchronize);
			iIsWaitingSync = ETrue;
		}
		else
		{
			ASSERT(!iIsWaitingPage);
			if(aPageIndex != iRequestingPageIndex)				//当前页没有被请求,做下面处理后显示等待框,否则直接显示等待框
			{
				if(KInvalidIndex != iRequestingPageIndex)		//如果对其它页的请求,取消当前请求,重发请求
				{
					ASSERT(NULL == iWaitingDialog);
					ASSERT(!iIsWaitingPage);
					CancelRequestPage();
				}
				RequestPage(aPageIndex);
			}
			RDebug::Print(_L("this = %x"),this);
			CreateWaitingDialog(iMainEngine.GetDesById(ECoCoTestRes_ReadingListInfo),ECmdReading);
			iIsWaitingPage = ETrue;
		}
	}
}

void CCacheEngine::SetTypeId(TInt aFirstTypeId,TInt aSecondTypeId, TInt aThirdTypeId)
{
	iFirstTypeId = aFirstTypeId;
	iSecondTypeId = aSecondTypeId;
	iThirdTypeId = aThirdTypeId;
	iTypeIsSet = ETrue;
}

void CCacheEngine::CleanupAllRecord()
{
// 	TFileName fileName;
// 	fileName.Append(iMainEngine.GetAppPath());
// 	fileName.Append(KCacheFilePath);
	BaflUtils::DeleteFile(iFs,GetFullName(KNullDesC));
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
TBool CCacheEngine::ReadFileFromCache(TInt aPageIndex)
{
	TBool readResult = EFalse;
	TPageRecord* record = CreateRecordLC(aPageIndex);
/*
	if(aPageIndex == 1)
	{
		readResult = GetFirstPageRecordFromDb(*record);
	}
	else
	{
	}*/

	//待优化,如果文件存在,记录不存在,写入记录
	readResult = GetRecordFromDB(aPageIndex,*record);
	if(readResult)
	{
		TFileName fileName = record->iFileName;
		ASSERT(BaflUtils::FileExists(iFs,fileName));
		iObserver.CacheUpdate(fileName);
	}
	CleanupStack::PopAndDestroy();
	return readResult;
}

//iRequestingPageIndex在此消息返回后要置为KInvlidIndex
void CCacheEngine::RequestPage(TInt aIndex)
{
	UtilityTools::WriteLogsL(_L("CCacheEngine::RequestPage"));
	ASSERT(KInvalidIndex == iRequestingPageIndex);		//同时只能有一个页请求
	HBufC8* url = MakeUrlLC(aIndex,0);
	UtilityTools::WriteLogsL(*url);
	iMainEngine.GetHttpManager()->SendGetRequest(*this,*url,ERequestPage);
	CleanupStack::PopAndDestroy();	//url
	iRequestingPageIndex = aIndex;
	UtilityTools::WriteLogsL(_L("CCacheEngine::RequestPage End"));
}

//////////////////////////////////////////////////////////////////////////
//CancelRequestPage()
//取消当前请求,并返回被取消的请求页的页码
//////////////////////////////////////////////////////////////////////////
TBool CCacheEngine::CancelRequestPage()
{
	ASSERT(iRequestingPageIndex > 1);
	iMainEngine.GetHttpManager()->CancelTransaction(this,ERequestPage);
	TInt requestingPageIndex = iRequestingPageIndex;
	iRequestingPageIndex = KInvalidIndex;
/*
	if(aReConnect)
	{
		RequestPage(requestingPageIndex);
	}*/


	ASSERT(((NULL == iWaitingDialog)^iIsWaitingPage));
	if(iWaitingDialog)
	{
		ASSERT(iIsWaitingPage);
		iIsWaitingPage = EFalse;
		CancelWaitingDialog();
		iObserver.CacheFailed();			//用户正在等待请求,发出失败通知
	}
	return requestingPageIndex;
}

//////////////////////////////////////////////////////////////////////////
//CancelSynchronize()
//实现以下操作:
//请求被取消或请求已失败时取消连接,重发请求,如果当前有对话框就取消对话框
//////////////////////////////////////////////////////////////////////////
void CCacheEngine::CancelSynchronize()
{
	ASSERT(iIsSynchronizing);
	iIsSynchronizing = EFalse;
	iMainEngine.GetHttpManager()->CancelTransaction(this,ERequestSync);
	RequestSynchronize();

	ASSERT(((NULL == iWaitingDialog)^iIsWaitingSync));
	if(iWaitingDialog)
	{
		ASSERT(iIsWaitingSync);
		iIsWaitingSync = EFalse;
		CancelWaitingDialog();
		iObserver.CacheFailed();			//用户正在等待请求,发出失败通知
	}
}

HBufC8* CCacheEngine::MakeUrlLC(TInt aPageIndex,TInt aTimeStamp) const
{
	ASSERT(aPageIndex > 0);

	_LIT(KAnd,	"&");
	HBufC8* url = HBufC8::NewLC(1000);
	TPtr8 ptr = url->Des();
	//ptr.Append(_L("http://59.36.96.182/coco/e?"));
	ptr.Append(_L("http://59.36.96.182/coco/e?"));

	ptr.AppendFormat(_L8("pid=%d"),iFirstTypeId);
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("cid=%d"),iSecondTypeId);
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("ccid=%d"),iThirdTypeId);
	ptr.Append(KAnd);
	ptr.Append(_L8("p="));
	ptr.Append(iMainEngine.GetP());
	ptr.Append(KAnd);
	ptr.Append(_L8("n="));
	ptr.Append(iMainEngine.GetIMEI());
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("l=%d"),15);
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("pg=%d"),aPageIndex);
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("t=%d"),aTimeStamp);
	ptr.Append(KAnd);
	ptr.AppendFormat(_L8("m=%d"),0);
	ptr.Append(KAnd);
	ptr.Append(_L8("v="));
	ptr.Append(iMainEngine.GetDesById(0));

	iMainEngine.WriteLog8(ptr);

	return url;
}

TBool CCacheEngine::GetFirstPageRecordFromDb(TPageRecord& aRecord) const
{
	return iDbEngine.GetFirstPageCacheRecord(aRecord,aRecord);
}

TBool CCacheEngine::GetRecordFromDB(TInt aPageIndex,TPageRecord& aRecord) const
{
	TBool getResult = ETrue;
	if(1 == aPageIndex)
	{
		getResult = iDbEngine.GetFirstPageCacheRecord(aRecord,aRecord);
		if(getResult)
		{
			if(!BaflUtils::FileExists(iFs,aRecord.iFileName))		//只有记录没有文件
			{
				iDbEngine.RemoveFirstPageCacheRecord(aRecord);
				getResult = EFalse;
			}
		}
	}
	else
	{
		getResult = iDbEngine.GetPageCacheRecord(aRecord,aRecord);
		if(getResult)
		{
			if(!BaflUtils::FileExists(iFs,aRecord.iFileName))		//只有记录没有文件
			{
				iDbEngine.RemovePageCacheRecord(aRecord);
				getResult = EFalse;
			}
		}
	}
	return getResult;
}

void CCacheEngine::WriteRecordToDB(const TPageRecord& aRecord)
{
	ASSERT(1 < aRecord.iPageIndex);
	//当记录数超过上限时,删除最老的一条记录及其对应的缓冲文件
	TInt maxRecordCount = iMainEngine.GetContentInfoMaxNum();		//从设置项中获取
	iMainEngine.WriteLog16(_L("maxRecordCount = %d"),maxRecordCount);
	TInt recordCount = iDbEngine.PageCacheRecordCount();
	if(recordCount == maxRecordCount)
	{
		TPageRecord* record = CreateRecordLC();
		iDbEngine.RemoveOldestPageCacheRecord(*record);
		BaflUtils::DeleteFile(iFs,record->iFileName);
		CleanupStack::PopAndDestroy();
	}
	if(!iDbEngine.AddPageCacheRecord(aRecord))
	{
		iDbEngine.UpdatePageCacheRecord(aRecord);
	}

	recordCount = iDbEngine.PageCacheRecordCount();
	ASSERT(recordCount <= maxRecordCount);
	iMainEngine.WriteLog16(_L("CCacheEngine::WriteRecordToDB End"));
}

void CCacheEngine::DeleteAllCache()
{
	iMainEngine.WriteLog16(_L("CCacheEngine::DeleteAllCache"));
	TPageRecord* record = CreateRecordLC();
	iDbEngine.RemovePageCacheRecordGroup(*record);
	CleanupStack::PopAndDestroy();

	//这里有问题
	TFileName fileName = GetFullName(KAllCacheFileName);
	fileName.AppendNum(iFirstTypeId);
	fileName.Append(_L("-"));
	if(0 != iSecondTypeId)
	{
		fileName.AppendNum(iSecondTypeId);
		fileName.Append(_L("-"));
		if(0 != iThirdTypeId)
		{
			fileName.AppendNum(iSecondTypeId);
			fileName.Append(_L("-"));
		}
	}
	fileName.Append(_L("*"));
	fileName.Append(KXmlPostfix);

	UtilityTools::WriteLogsL(fileName);
	BaflUtils::DeleteFile(iFs,fileName);
	iMainEngine.WriteLog16(_L("CCacheEngine::DeleteAllCache End"));
}

void CCacheEngine::ParseNewPage(const TDesC& aXmlData)
{
	TPageRecord* record = CreateRecordLC();
	TBool parseResult = ParseNewRecord(aXmlData,*record);
	iRequestingPageIndex = KInvalidIndex;
	if(parseResult)		//新页解析成功
	{
		ASSERT(1 < record->iPageIndex);
		WriteRecordToDB(*record);
		if(iIsWaitingPage)
		{
// 			iIsWaitingPage = EFalse;
// 			CancelWaitingDialog();
			//待优化,如果文件存在,记录不存在,写入记录
			TInt nextPageIndex = record->iPageIndex + 1;
			RequestPage(nextPageIndex);
			ASSERT(BaflUtils::FileExists(iFs,record->iFileName));
			iObserver.CacheUpdate(record->iFileName);
		}
	}
	else
	{
		if(iIsWaitingPage)
		{
			iObserver.CacheFailed();
		}
	}
/*
	else				//页解析失败
	{
		if(iIsWaitingPage)
		{
			//iControlFactory.CreateTipDialog(_L("New Page Invalid!"),*this,ECmdNull);
		}
	}
*/

	iIsWaitingPage = EFalse;
	CancelWaitingDialog();
	CleanupStack::PopAndDestroy();	//record
}

void CCacheEngine::ParseSynchronize(const TDesC& aXmlData)
{
	ASSERT(iIsSynchronizing);
	iIsSynchronizing = EFalse;
	TPageRecord* newRecord = CreateRecordLC();
	TBool parseResult = ParseNewRecord(aXmlData,*newRecord);
	if(parseResult)		//页解析成功
	{
		//ASSERT(1 == newRecord->iPageIndex);			//页号不为1,说明解析错误
	
		if(0 != newRecord->iTimeStamp)							//时间戳已经更新
		{
			DeleteAllCache();
			UpdateFirstPage(*newRecord);

			if(KInvalidIndex == iRequestingPageIndex)
			{
				//待优化,如果文件存在,记录不存在,写入记录
				RequestPage(2);													//请求下一页数据
			}

			if(iIsWaitingSync)
			{
				ASSERT(!iIsWaitingPage);
				ASSERT(BaflUtils::FileExists(iFs,newRecord->iFileName));
				iObserver.CacheUpdate(newRecord->iFileName);
			}
		}
	}
	else
	{
		if(iIsWaitingSync)
		{
			iObserver.CacheFailed();
		}
	}
	iIsWaitingSync = EFalse;
	CancelWaitingDialog();			//此时应该有等待对话框正在显示
	CleanupStack::PopAndDestroy();
}

TBool CCacheEngine::ParseNewRecord(const TDesC& aXmlData,TPageRecord& aRecord) const
{
	TBool parseResult = EFalse;

	TFileName tmpFileName = CreateTmpXmlFile(aXmlData);
	HBufC8* fileName = UtilityTools::ConvertToHBufC8L(tmpFileName);
	CleanupStack::PushL(fileName);
	TiXmlDocument* doc = new TiXmlDocument((const char*)(fileName->Des().PtrZ()));
	CleanupStack::PopAndDestroy();	//fileName

	TInt newTimeStamp = -1;			//-1表示无效时间
	TInt pageIndex = 0;
	TInt totalPageNum = 0;
	if(!doc->LoadFile())
	{
		//iControlFactory.CreateTipDialog(_L("parse failed!"),(MDialogObserver&)(*this),ECmdNull);
	}
	else
	{
		TiXmlElement* root = doc->RootElement();

		if(0 == strcmp(root->Value(),"f"))		//根结点为f标记
		{
			newTimeStamp = (root->Attribute("t",&newTimeStamp)) ? newTimeStamp : -1;	//-1表示没有该属性,为非首页页面

			TiXmlElement* element = root->FirstChildElement("cp");
			if(element)
			{
				TiXmlNode* node = element->FirstChild();
				if(node)
				{
					pageIndex = atoi( node->Value() );
				}
			}
			element = root->FirstChildElement("ap");
			if (element)
			{
				TiXmlNode* node = root->FirstChildElement("ap")->FirstChild();
				if(node)
				{
					totalPageNum = atoi( node->Value() );
				}
			}
			parseResult = ETrue;
		}
		else
		{
			//iControlFactory.CreateTipDialog(_L("parse failed!"),(MDialogObserver&)(*this),ECmdNull);
		}
	}
	delete doc;
	if(parseResult)		//解析成功
	{
		if(1 == pageIndex)
		{
			aRecord.iFileName = MakeFirstCacheFileName(pageIndex);
		}
		else
		{
			aRecord.iFileName = MakeCacheFileName(pageIndex);
		}
		aRecord.iFirstTypeId = iFirstTypeId;
		aRecord.iSecondTypeId = iSecondTypeId;
		aRecord.iThirdTypeId = iThirdTypeId;
		aRecord.iPageIndex = pageIndex;
		aRecord.iTimeStamp = newTimeStamp > 0 ? newTimeStamp : 0;
		//////////////////////////////////////////////////////////////////////////
		if(0 == newTimeStamp)		//是首页,而且首页未更新,直接删除临时文件
		{
			BaflUtils::DeleteFile(iFs,tmpFileName);
			parseResult = EFalse;
		}
		else						//newTimeStamp等于-1,不是首页,newTimeStamp大于0,首页已更新,删除旧文件,并将临时文件改名
		{
			if (BaflUtils::FileExists(iFs,aRecord.iFileName))
			{
				BaflUtils::DeleteFile(iFs,aRecord.iFileName);
			}
			BaflUtils::RenameFile(iFs,tmpFileName,aRecord.iFileName);
		}
	}
	else
	{
		BaflUtils::DeleteFile(iFs,tmpFileName);
	}
	return parseResult;
}

void CCacheEngine::UpdateFirstPage(const TPageRecord& aRecord)
{
	if(!iDbEngine.AddFirstPageCacheRecord(aRecord))
	{
		iDbEngine.UpdateFirstPageCacheRecord(aRecord);
	}
}

TFileName CCacheEngine::CreateTmpXmlFile(const TDesC& aXmlData) const
{
	TFileName fileName = MakeTempFileName();
	SaveXmlFile(fileName,aXmlData);
	ASSERT(BaflUtils::FileExists(iFs,fileName));
	return fileName;
}

TFileName CCacheEngine::MakeTempFileName() const
{	
	TFileName fileName = GetFullName(KTempFileName);
	iMainEngine.WriteLog16(fileName);

	TBool fileExists = BaflUtils::FileExists(iFs,fileName);
#ifdef _DEBUG
		ASSERT(!fileExists);								//编码时确保程序能够正确及时的清除临时文件		
#else
		if(fileExists)
		{
			BaflUtils::DeleteFile(iFs,fileName);			//发布版本删除某些特殊原因导致的意外存在的文件
		}
#endif
	return fileName;
}

TFileName CCacheEngine::MakeCacheFileName(TInt aPageIndex) const
{
	//用栏目类型与页号串接组成一个单独的文件名
	TFileName fileName = GetFullName(KCacheFileName);
	fileName.Append(_L("-"));
	fileName.AppendNum(iFirstTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(iSecondTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(iThirdTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(aPageIndex);
	fileName.Append(KXmlPostfix);

	iMainEngine.WriteLog16(fileName);			
	return fileName;
}

TFileName CCacheEngine::MakeFirstCacheFileName(TInt aPageIndex) const
{
	//用栏目类型与页号串接组成一个单独的文件名
	TFileName fileName = GetFullName(KFirstPageCacheTable);
	fileName.Append(_L("-"));
	fileName.AppendNum(iFirstTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(iSecondTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(iThirdTypeId);
	fileName.Append(_L("-"));
	fileName.AppendNum(aPageIndex);
	fileName.Append(KXmlPostfix);

	iMainEngine.WriteLog16(fileName);		
	return fileName;
}

void CCacheEngine::SaveXmlFile(const TDesC& aFileName,const TDesC& aFileData) const
{
	UtilityTools::WriteTestFileL(aFileName,aFileData);
}

TFileName CCacheEngine::GetFullName(const TDesC& aNameAndExt) const
{
	TFileName fileName;
	fileName.Append(iMainEngine.GetAppPath());
	fileName.Append(KCacheFilePath);
	fileName.Append(aNameAndExt);
	iMainEngine.WriteLog16(fileName);
	return fileName;
}

TPageRecord* CCacheEngine::CreateRecordLC(TInt aPageIndex) const
{
	TPageRecord* record = new (ELeave) TPageRecord;
	CleanupStack::PushL(record);
	record->iFileName.Zero();
	record->iFirstTypeId = iFirstTypeId;
	record->iSecondTypeId = iSecondTypeId;
	record->iThirdTypeId = iThirdTypeId;
	record->iPageIndex = aPageIndex;
	record->iTimeStamp = 0;
	return record;
}

void CCacheEngine::CancelRequest()
{
	iMainEngine.GetHttpManager()->CancelTransaction(this,ERequestSync);
	iMainEngine.GetHttpManager()->CancelTransaction(this,ERequestPage);
}

void CCacheEngine::CreateWaitingDialog(const TDesC& aText,TInt aCmdEvent)
{
	ASSERT(NULL == iWaitingDialog);
	//iWaitingDialog = iControlFactory.CreateWaitDialogWithoutCancel(aText,*this);
	iWaitingDialog = iControlFactory.CreateWaitDialog(aText,*this,aCmdEvent);
}

void CCacheEngine::CancelWaitingDialog()
{
	if(iWaitingDialog)
	{
		iMainEngine.CurWindow()->RemoveAndDeleteControl(iWaitingDialog);
		iWaitingDialog = NULL;
	}
}