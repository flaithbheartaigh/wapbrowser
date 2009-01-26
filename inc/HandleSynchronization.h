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
	//1Ϊ ��Ʒ���� ��2Ϊ ���а�3Ϊ �������� ��4Ϊ  ������� ��5Ϊ ���� ��6Ϊ ����
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

	void  HandleFirstResponseXmlData();  //����������ȡ�ĵ�һ����Ϣ
	void  HandleSecondResponseXmlData(); //����������ȡ�ĵڶ�����Ϣ

	void  HandleFirstXmlData(); //������������Ϣ
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
