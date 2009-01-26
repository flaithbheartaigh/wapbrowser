/*
============================================================================
 Name        : HandleSynchronization.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleSynchronization declaration
============================================================================
*/

#ifndef HANDLESYNCHRONIZATION_H
#define HANDLESYNCHRONIZATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HttpObserver.h"
#include "tinyxml.h"

class CMainEngine;
class MHandleEventObserver;
class CContentItem;
// CLASS DECLARATION

enum TSynchronizationContent
{
	//1为 精品下载 ，2为 排行榜，3为 最新下载 ，4为  分类浏览 ，5为 友链 ，6为 帮助
	EContentBestDownload=1,
	EContentHotDownload,
	EContentNewDownload,
	EContentTypeDownload,
	EContentFriendLink,
	EContentHelp
};
class CHandleSynchronization : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleSynchronization();
	static CHandleSynchronization* NewL(CMainEngine& aMainEngine);
	static CHandleSynchronization* NewLC(CMainEngine& aMainEngine);

private:

	CHandleSynchronization(CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);
public:
	void  SendFirstRequest();
	void  SendSecondRequest(TInt aID);
	void  CancelSendRequest();

public:

	void  SetObserver(MHandleEventObserver* aObserver);

	void  HandleFirstResponseXmlData();  //处理联网获取的第一次信息
	void  HandleSecondResponseXmlData(); //处理联网获取的第二次信息

	void  HandleFirstXmlData(); //处理本身联网信息
	void  HandleSecondXmlData(CContentItem* aContentItem,TiXmlElement* aElement);
	void  HandleThirdXmlData(CContentItem* aContentItem,TiXmlElement* aElement);

	const TDesC& GetValue(TInt aOneID,TInt aTwoID,TInt aThreeID);
	TInt GetItemID(TInt aOneID,TInt aTwoID,TInt aThreeID);
	TInt GetCount(TInt aOneID,TInt aTwoID,TInt aThreeID);
	TBool IsUpdate(TInt aOneID,TInt aTwoID,TInt aThreeID);
private:
	void  ReWriteXMl();
	void  AddText(CContentItem* aItem);
	void  AddItem(CContentItem* aItem,TiXmlElement* aElement,const TDesC& aName,TInt aID);
	CContentItem* GetContentItem(TInt aID);
private:
	MHandleEventObserver* iObserver;

	CMainEngine& iMainEngine;
	
	RPointerArray<CContentItem> iArray;
	
	HBufC* iBuf;
	HBufC* iTempBuf;
	HBufC* iDataBuf;
	TInt iCurState;
	TInt iTimeFlag;
	TInt iPreTimeFlag;
	TBool iContentError;

	TFileName iSynFileName;
	TFileName iTempFileName;
};

#endif // HANDLESYNCHRONIZATION_H
