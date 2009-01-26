/*
============================================================================
 Name        : HandleAutoUpdate.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleAutoUpdate declaration
============================================================================
*/

#ifndef HANDLEAUTOUPDATE_H
#define HANDLEAUTOUPDATE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HttpObserver.h"

class CMainEngine;
class MHandleEventObserver;
class CHttpManager;

/**
*  CHandleAutoUpdate
* 
*/
class CHandleAutoUpdate : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleAutoUpdate();
	static CHandleAutoUpdate* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleAutoUpdate* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleAutoUpdate(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);
public:
	void  SendRequest();
	void  CancelSendRequest();

	const TDesC& GetInfo();
	
	const TDesC& GetUpdateUrl();
	inline TBool IsUpdate(){ return iBool;}

private:
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;
	
	HBufC8* iUrl;

	HBufC* iBuf;

	TBool iBool;
};

#endif // HANDLEAUTOUPDATE_H
