/*
============================================================================
 Name        : HandleInSearch.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleInSearch declaration
============================================================================
*/

#ifndef HANDLEINSEARCH_H
#define HANDLEINSEARCH_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <BADESCA.H>
#include "HttpObserver.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CHttpManager;
class CSearchResultTempItem;
/**
*  CHandleInSearch
* 
*/
class CHandleInSearch :public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleInSearch();
	static CHandleInSearch* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleInSearch* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleInSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType=0);

public:
	TInt GetCount();
	const TDesC& GetOneItem(TInt aIndex);

	TInt GetCurPage();
	TInt GetAllPage();

	const TDesC& GetInfo();

	void  SendRequest(const TDesC8& aKeyWord,TInt aIndex,TInt aPage);
	void  CancelSendRequest();

	const TDesC8& GetSendUrl();
	
	void SetSendUrl(const TDesC8& aUrl);
	
private:
	void ClearTempBuf();
	TBool CheckTempBuf(TInt aPage);
	void HandleXmlData();
private:

	RPointerArray<CSearchResultTempItem> iPointArray;
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;

	CHttpManager& iHttpManager;

	HBufC* iBuf;
	HBufC8* iKeyWord;

	HBufC8* iUrl;
	HBufC8* iCurUrl;
	TFileName iFileName;

	TInt iCurPage;
	TInt iAllPage;
	TInt iCurIndex;

	TBool iContentError;
};

#endif // HANDLEINSEARCH_H
