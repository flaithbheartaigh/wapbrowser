/*
============================================================================
 Name        : CacheEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CCacheEngine declaration
 描述		 : 实现预读与缓存
 说明		 : 			  				
============================================================================
*/

#ifndef CACHEENGINE_H
#define CACHEENGINE_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

#include "DialogObserver.h"
#include "HttpObserver.h"

class CMainEngine;
class CControlFactory;
class CDBEngine;
class CDialog;

class TPageRecord;

class MCacheObserver;

const TInt KInvalidIndex = -1;

class CCacheEngine : public CBase , public MDialogObserver , public MOperationObserver
{
public:
	~CCacheEngine();
	static CCacheEngine*	NewL(CMainEngine& aMainEngine,MCacheObserver& aObserver);
	static CCacheEngine*	NewLC(CMainEngine& aMainEngine,MCacheObserver& aObserver);

private:
	CCacheEngine(CMainEngine& aMainEngine,MCacheObserver& aObserver);
	void ConstructL();

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public:
	//以时间戳为参数向服务器发送请求,如果有更新,服务器会返回新的内容
	//该请求的结果将返回第一页数据
	//此请求应该先于RequestPageFileName被调用
	void	RequestSynchronize();

	//请求一个页面文件,这是一个异步函数,不会马上返回结果给调用方
	//请求的结果通过MCacheObserver在处理完成时返回给调用方
	//页号从1开始
	void	RequestPageFileName(TInt aPageIndex);

	void	SetTypeId(TInt aFirstTypeId,TInt aSecondTypeId = 0, TInt aThirdTypeId = 0);

	void	CleanupAllRecord();

private:
	//查找数据库记录,当找到与aIndex匹配的页面文件时回调
	TBool	ReadFileFromCache(TInt aIndex);

	//根据索引向服务器请求一个页面
	void	RequestPage(TInt aIndex);	
	
	//以下两个函数实现取消连接并重连,应该改名或分解为两个函数以明确其功能
	TInt	CancelRequestPage();

	void	CancelSynchronize();

	HBufC8*	MakeUrlLC(TInt aPageIndex,TInt aTimeStamp) const;

	//从数据库中查找第一个页面的记录
	TBool	GetFirstPageRecordFromDb(TPageRecord& aRecord) const;
	//从数据库查找某一个页面文件的数据
	TBool	GetRecordFromDB(TInt aPageIndex,TPageRecord& aRecord) const;
	//将一条记录写入数据库
	void	WriteRecordToDB(const TPageRecord& aRecord);

	//移除数据库中的所有记录,同时删除除首页外的所有缓冲文件
	void	DeleteAllCache();

	//处理页面请求的返回数据
	void	ParseNewPage(const TDesC& aXmlData);

	//处理页面及时间戳同步请求的返回数据
	void	ParseSynchronize(const TDesC& aXmlData);

	//根据服务器返回的XML中解析出新的记录
	TBool	ParseNewRecord(const TDesC& aXmlData,TPageRecord& aRecord) const;

	//更新首页
	void	UpdateFirstPage(const TPageRecord& aRecord);

	//创建一个临时文件
	TFileName	CreateTmpXmlFile(const TDesC& aXmlData) const;

	//生成一个临时文件名
	TFileName	MakeTempFileName() const;

	//生成一个缓冲文件名
	TFileName	MakeCacheFileName(TInt aPageIndex) const;

	//生成一个首页缓冲文件名
	TFileName	MakeFirstCacheFileName(TInt aPageIndex) const;

	void	SaveXmlFile(const TDesC& aFileName,const TDesC& aFileData) const;

	TFileName	GetFullName(const TDesC& aNameAndExt) const;

	TPageRecord*	CreateRecordLC(TInt aPageIndex = 0) const;

	//取消所有请求
	void	CancelRequest();		

	void	CreateWaitingDialog(const TDesC& aText,TInt aCmdEvent);
	void	CancelWaitingDialog();

private:
	CMainEngine&			iMainEngine;
	const CControlFactory&	iControlFactory;
	CDBEngine&				iDbEngine;
	MCacheObserver&			iObserver;
	RFs&					iFs;
	CDialog*				iWaitingDialog;


	TBool					iIsSynchronizing;
	//ParseSynchronize()函数中的判断使下列两个成员的存在成为必要,否则只用iWaitingDialog的值判断就足够了
	TBool					iIsWaitingSync;
	TBool					iIsWaitingPage;

	TInt					iFirstTypeId;
	TInt					iSecondTypeId;
	TInt					iThirdTypeId;

	TInt					iRequestingPageIndex;		//正在获取的页面索引

	//TInt					iCurPageIndex;				//保存当前处理页面,供同步返回时处理使用

	TBool					iTypeIsSet;

	//TBool					iAlreadyRelease;
};

#endif
