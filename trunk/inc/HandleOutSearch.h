/*
============================================================================
 Name        : HandleOutSearch.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleOutSearch declaration
============================================================================
*/

#ifndef HANDLEOUTSEARCH_H
#define HANDLEOUTSEARCH_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "HttpObserver.h"
#include "MControlObserver.h"
#include "CoCoPreDefine.h"
#include "tinyxml.h"

// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CSearchResultTempItem;
class CHttpManager;
/**
*  CHandleOutSearch
* 
*/
class CHandleOutSearch : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleOutSearch();
	static CHandleOutSearch* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleOutSearch* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleOutSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public:
	CSearchResultTempItem& GetCurItem();

	void  SendHomePageRequest();
	void  SendPrePageRequest();
	void  SendNextPageRequest();
	void  SendEndPageRequest();
	void  SendGoToPageRequest(TInt aPage);

	void  SendRequest(const TDesC8& aUrl);
	void  SendKeyWordRequest(const TDesC8& aKeyWord);
	void  SendErrorInfoRequest();
	void  CancelSendRequest();

	void ClearTempBuf();

	const TDesC& GetInfo(){ return *iBuf;}
	const TDesC& GetErrorInfo(){ return *iErrorInfo;}

	TInt CheckUrl(const TDesC8& aUrl);
private:
	void HandlePageWml();
	void HandleChildElement(TiXmlNode* aElement);
	TInt CheckTag(const TDesC8& aTag);  //¹ýÂË±êÇ©

	TBool CheckTemp();
private:

	RPointerArray<CSearchResultTempItem> iPointArray;
	MHandleEventObserver& iObserver;

	CHttpManager& iHttpManager;
	CMainEngine& iMainEngine;
	HBufC8* iCurUrl;
	HBufC* iErrorInfo;
	HBufC* iBuf;

	TInt iCurIndex;

	TInt iCurPage;
	TInt iAllPage;

	TBool iFlag;

	TInt iState;
	TBool iContentError;
	TFileName iFileName;
};

#endif // HANDLEOUTSEARCH_H
