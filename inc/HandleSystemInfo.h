/*
============================================================================
 Name        : HandleSystemInfo.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleSystemInfo declaration
============================================================================
*/

#ifndef HANDLESYSTEMINFO_H
#define HANDLESYSTEMINFO_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HttpObserver.h"

class CMainEngine;
class MHandleEventObserver;
class CHttpManager;

/**
*  CHandleSystemInfo
* 
*/
class CHandleSystemInfo : public CBase,public MOperationObserver
{
public: // Constructors and destructor


	~CHandleSystemInfo();
	static CHandleSystemInfo* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleSystemInfo* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleSystemInfo(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);
public:
	void  SendRequest();
	void  CancelSendRequest();

	const TDesC& GetInfo();
	
	const TDesC& GetTitle();
	const TDesC& GetSystemInfo();

	TBool IsNextSystemInfo(){ return iBoolNext;}
private:
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;
	CHttpManager& iHttpManager;
	
	HBufC8* iUrl;

	HBufC* iBuf;
	HBufC* iSystemInfo;
	HBufC* iTitle;

	TBool iContentError;
	
	TBool iBoolNext;
};

#endif // HANDLESYSTEMINFO_H
