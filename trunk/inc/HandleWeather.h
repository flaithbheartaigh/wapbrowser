/*
============================================================================
 Name        : HandleWeather.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleWeather declaration
============================================================================
*/

#ifndef HANDLEWEATHER_H
#define HANDLEWEATHER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HttpObserver.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CHttpManager;
/**
*  CHandleWeather
* 
*/
class CHandleWeather : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleWeather();
	static CHandleWeather* NewL(CMainEngine& aMainEngine);
	static CHandleWeather* NewLC(CMainEngine& aMainEngine);

private:
	CHandleWeather(CMainEngine& aMainEngine);
	void ConstructL();
public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);
public:
	void  SendRequest();
	void  AddRequest();
	void  CancelSendRequest();
private:
	void  HandleXml();
private:
	CMainEngine& iMainEngine;
	CHttpManager& iHttpManager;
	TBool iContentError;
	TFileName iFileName;
};

#endif // HANDLEWEATHER_H
