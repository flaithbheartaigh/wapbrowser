/*
============================================================================
 Name        : WapHttpEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWapHttpEngine implementation
============================================================================
*/
#include "Define.h"
#include "WapHttpEngine.h"
#include "UtilityTools.h"


CWapHttpEngine::CWapHttpEngine()
{
}

CWapHttpEngine::~CWapHttpEngine()
{
	delete iHTTPEngine;
}

CWapHttpEngine* CWapHttpEngine::NewLC()
{
	CWapHttpEngine* self = new (ELeave)CWapHttpEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CWapHttpEngine* CWapHttpEngine::NewL()
{
	CWapHttpEngine* self=CWapHttpEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CWapHttpEngine::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//From MClientObserver
//////////////////////////////////////////////////////////////////////////
void CWapHttpEngine::ClientEvent(const TDesC& aEventDescription,TInt /*aIndex*/)
{
	UtilityTools::WriteLogsL(aEventDescription);
	RDebug::Print(aEventDescription);

	if(aEventDescription.Compare(_L("Transaction Successful")) == 0)
		//|| aEventDescription.Compare(_L("Transaction Complete")) == 0)
	{
		UtilityTools::WriteLogsL(_L("1"));
		if(iReceiveData8)
		{
			if(iObserver)
			{
				iObserver->HttpOk(*iReceiveData8);
			}
			delete iReceiveData8;
			iReceiveData8 = NULL;
		}
		else
		{
			if(iObserver)
			{
				iObserver->HttpEmpty();
			}
		}
	}
	else if(aEventDescription.Compare(_L("Transaction Failed")) == 0
		||  aEventDescription.Compare(_L("Error:KErrDisconnected")) == 0)
	{
		if(iObserver)
		{
			iObserver->HttpFailed();
		}
	}
}

void CWapHttpEngine::ClientBodyReceived(const TDesC8& aBodyData,TInt /*aIndex*/)
{
	if (iReceiveData8)
	{
		iReceiveData8 = iReceiveData8->ReAllocL(iReceiveData8->Length() + aBodyData.Length());
		iReceiveData8->Des().Append(aBodyData);
	}
	else
	{
		iReceiveData8 = HBufC8::NewL(aBodyData.Length());
		iReceiveData8->Des().Copy(aBodyData);
	}
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CWapHttpEngine::SetObserver(MWapHttpEngineObserver* aObserver)
{
	iObserver = aObserver;
}

void CWapHttpEngine::IssueHTTPGetL(const TDesC8& aUrl)
{
	TRACE(aUrl);
// 	if(NULL == iHTTPEngine)
// 	{
// 		iHTTPEngine = CHTTPEngine::NewL(*this);
// 	}
	ASSERT(iObserver);
	iHTTPEngine->IssueHTTPGetL(aUrl);
}

void CWapHttpEngine::IssueHTTPPostL(const TDesC8& aUrl,const TDesC8& aData,const TDesC8& aReferer)
{
	TRACE(aUrl);
	TRACE(aData);
// 	if(NULL == iHTTPEngine)
// 	{
// 		iHTTPEngine = CHTTPEngine::NewL(*this);
// 	}
	ASSERT(iObserver);
	//HTTPEngine()->IssueHTTPPostL(aUrl,_L8(""),aData);
	HTTPEngine()->IssueHTTPPostL(aUrl,aReferer,aData);
}

/*
void SetRefer(const TDesC8& aRefer)
{
	HTTPEngine()->
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
}
*/

void CWapHttpEngine::CancelTransaction()
{
	if(iHTTPEngine)
	{
		iHTTPEngine->CancelTransaction();
	}
}
//////////////////////////////////////////////////////////////////////////
//private:
//////////////////////////////////////////////////////////////////////////
CHTTPEngine* CWapHttpEngine::HTTPEngine()
{
	if(NULL == iHTTPEngine)
	{
		iHTTPEngine = CHTTPEngine::NewL(*this);
	}
	return iHTTPEngine;
}