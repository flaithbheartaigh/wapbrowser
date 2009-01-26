/*
============================================================================
 Name        : HandleGrade.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleGrade declaration
============================================================================
*/

#ifndef HANDLEGRADE_H
#define HANDLEGRADE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <BADESCA.H>
#include "HttpObserver.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CHttpManager;
/**
*  CHandleGrade
* 
*/
class CHandleGrade : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleGrade();
	static CHandleGrade* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleGrade* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleGrade(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);
public:
	const TDesC& GetInfo();
	const TDesC& GetGrade();

	void  SendRequest(const TDesC& aCode,TInt aGrade);
	void  CancelSendRequest();

	const TDesC8& GetSendUrl();
private:
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;
	CHttpManager& iHttpManager;

	HBufC* iBuf;

	HBufC8* iUrl;

	HBufC* iGrade;
};

#endif // HANDLEGRADE_H
