/*
============================================================================
 Name        : HandleGrade.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleGrade implementation
============================================================================
*/

#include "HandleGrade.h"
#include "MainEngine.h"
#include <utf.h>
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"

CHandleGrade::CHandleGrade(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iBuf(NULL)
,iUrl(NULL)
{
	// No implementation required
}


CHandleGrade::~CHandleGrade()
{
	this->CancelSendRequest();
	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}
	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}
	if(iGrade)
	{
		delete iGrade;
		iGrade=NULL;
	}
	
}

CHandleGrade* CHandleGrade::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleGrade* self = new (ELeave)CHandleGrade( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleGrade* CHandleGrade::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleGrade* self=CHandleGrade::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleGrade::ConstructL()
{

}

//From MOperationObserver
void CHandleGrade::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}

	if(iGrade)
	{
		delete iGrade;
		iGrade=NULL;
	}

	if(aEventType==EHttpNoError)
	{
		
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);

		TInt point=iBuf->Find(_L("|"));
		if(point>0)
		{
			iGrade=HBufC::NewL(point);
			iGrade->Des().Copy(iBuf->Left(point));
			
			iBuf->Des().Delete(0,point+1);
		}
	}
	else
	{
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);
	}
	iObserver.HandleResponseEvent(aEventType,aType);
}

const TDesC& CHandleGrade::GetInfo()
{
	return *iBuf;
}
const TDesC& CHandleGrade::GetGrade()
{
	return *iGrade;
}

const TDesC8& CHandleGrade::GetSendUrl()
{
	return *iUrl;
}

void  CHandleGrade::SendRequest(const TDesC& aCode,TInt aGrade)
{
	TBuf8<20> code;
	code.Zero();
	code.Copy(aCode);

	TBuf8<10> pValue;
	pValue.Zero();
	pValue.Copy(iMainEngine.GetP());

	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}
	iUrl=HBufC8::NewL(300);

	iUrl->Des().Append(_L8("http://59.36.96.182/interface/g?m=1&p="));
	iUrl->Des().Append(pValue);
	iUrl->Des().Append(_L8("&c="));
	iUrl->Des().Append(code);
	iUrl->Des().Append(_L8("&n="));
	iUrl->Des().Append(iMainEngine.GetIMEI());
	iUrl->Des().Append(_L8("&g="));
	iUrl->Des().AppendNum(aGrade);
	iHttpManager.SendGetRequest(*this,*iUrl,EHttpGrade);
}

void  CHandleGrade::CancelSendRequest()
{
	iHttpManager.CancelTransaction(this,EHttpGrade);
}