/*
============================================================================
 Name        : WapHttpEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWapHttpEngine declaration
============================================================================
*/

#ifndef WAPHTTPENGINE_H
#define WAPHTTPENGINE_H

#include "Define.h"
#include "HTTPEngine.h"

class MWapHttpEngineObserver;

class CWapHttpEngine 
	: public CBase
	, public MClientObserver
{
public:
	~CWapHttpEngine();
	static CWapHttpEngine* NewL();
	static CWapHttpEngine* NewLC();

private:
	CWapHttpEngine();
	void ConstructL();

public://From MClientObserver
	virtual void ClientEvent(const TDesC& aEventDescription,TInt aIndex);
	virtual void ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex);

public:
	void SetObserver(MWapHttpEngineObserver* aObserver);
	void IssueHTTPGetL(const TDesC8& aUrl);
	void IssueHTTPPostL(const TDesC8& aUrl,const TDesC8& aData,const TDesC8& aReferer);
	//void SetRefer(const TDesC8& aRefer);
	void CancelTransaction();

private:
	CHTTPEngine* HTTPEngine();
	
private:
	CHTTPEngine* iHTTPEngine;
	HBufC8* iReceiveData8;
	MWapHttpEngineObserver* iObserver;
};

#endif // WAPHTTPENGINE_H
