/*
============================================================================
 Name        : HandleAutoUpdate.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleAutoUpdate implementation
============================================================================
*/

#include "HandleAutoUpdate.h"
#include "MainEngine.h"
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"

_LIT8(KUPDATE,"http://59.36.96.182/interface/upgrade?m=3Gest|");
CHandleAutoUpdate::CHandleAutoUpdate(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iUrl(NULL)
,iBuf(NULL)
{
	// No implementation required
}

CHandleAutoUpdate::~CHandleAutoUpdate()
{
	CancelSendRequest();
	DELETE(iUrl);
	DELETE(iBuf);
}

CHandleAutoUpdate* CHandleAutoUpdate::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleAutoUpdate* self = new (ELeave)CHandleAutoUpdate( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleAutoUpdate* CHandleAutoUpdate::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleAutoUpdate* self=CHandleAutoUpdate::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleAutoUpdate::ConstructL()
{
	TBuf8<30> tbufTemp;

	iUrl=HBufC8::NewL(200);
	iUrl->Des().Append(KUPDATE);

	tbufTemp.Zero();
	tbufTemp.Copy(iMainEngine.GetDesById(ECoCoVersionNumber));
	iUrl->Des().Append(tbufTemp);

	iUrl->Des().Append(_L8("&n="));

	tbufTemp.Zero();
	tbufTemp.Copy(iMainEngine.GetIMEI());
	iUrl->Des().Append(tbufTemp);

	iBool=EFalse;

}
void  CHandleAutoUpdate::SendRequest()
{
	iMainEngine.GetHttpManager()->SendGetRequest(*this,*iUrl,EHttpAutoUpdate);
}
void  CHandleAutoUpdate::CancelSendRequest()
{
	iMainEngine.GetHttpManager()->CancelTransaction(this,EHttpAutoUpdate);
}

const TDesC& CHandleAutoUpdate::GetInfo()
{
	return *iBuf;
}

const TDesC& CHandleAutoUpdate::GetUpdateUrl()
{
	return *iBuf;
}

//From MOperationObserver
void CHandleAutoUpdate::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{

	if(aEventType==EHttpNoError)
	{
		if(aType==EHttpAutoUpdate)
		{
			TInt post=aEventData.Find(_L("http://"));
			if(post<0)
			{
				iBool=EFalse;
			}
			else
			{
				iBool=ETrue;
			}
		}
	}

	iBuf=HBufC::NewL(aEventData.Length());
	iBuf->Des().Copy(aEventData);
	
	iObserver.HandleResponseEvent(aEventType,aType);
}