/*
============================================================================
 Name        : HandleSystemInfo.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleSystemInfo implementation
============================================================================
*/

#include "HandleSystemInfo.h"

#include "MainEngine.h"
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"

CHandleSystemInfo::CHandleSystemInfo(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iSystemInfo(NULL)
,iUrl(NULL)
,iTitle(NULL)
,iBuf(NULL)
{
	// No implementation required
}


CHandleSystemInfo::~CHandleSystemInfo()
{
	this->CancelSendRequest();
	DELETE(iSystemInfo);
	DELETE(iUrl);
	DELETE(iTitle);
	DELETE(iBuf);
}

CHandleSystemInfo* CHandleSystemInfo::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleSystemInfo* self = new (ELeave)CHandleSystemInfo(aObserver,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleSystemInfo* CHandleSystemInfo::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleSystemInfo* self=CHandleSystemInfo::NewLC(aObserver,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleSystemInfo::ConstructL()
{
	iBoolNext=EFalse;
	iContentError=EFalse;
	TBuf8<30> tbufTemp;
	
	iUrl=HBufC8::NewL(300);
	iUrl->Des().Append(_L8("http://59.36.96.182/interface/no?m=1&n="));

	tbufTemp.Zero();
	tbufTemp.Copy(iMainEngine.GetIMEI());
	iUrl->Des().Append(tbufTemp);
}
void  CHandleSystemInfo::SendRequest()
{
	iHttpManager.SendGetRequest(*this,*iUrl,EHttpSystemInfo);
}
void  CHandleSystemInfo::CancelSendRequest()
{
	iHttpManager.CancelTransaction(this,EHttpSystemInfo);
}

const TDesC& CHandleSystemInfo::GetSystemInfo()
{
	if(!iSystemInfo)
	{
		iSystemInfo=HBufC::NewL(0);
	}
	return *iSystemInfo;
}
const TDesC& CHandleSystemInfo::GetInfo()
{
	if(!iBuf)
	{
		iBuf=HBufC::NewL(0);
	}
	return *iBuf;
}
const TDesC& CHandleSystemInfo::GetTitle()
{
	if(!iTitle)
	{
		iTitle=HBufC::NewL(0);
	}
	return *iTitle;
}
//From MOperationObserver
void CHandleSystemInfo::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{

	if(aEventType==EHttpNoError)
	{
		if(aType==EHttpSystemInfo)
		{
			TInt post=aEventData.Find(_L("|"));
			if(post<0)
			{
				iBoolNext=EFalse;
				iTitle=iMainEngine.GetDesById(ETurkeyTextRes_Setting_SystemMsg).Alloc();
				iSystemInfo=aEventData.Alloc();
			}
			else
			{
				if(aEventData.Left(post).Compare(_L("0"))==0)
				{
					iBoolNext=EFalse;
				}
				else
				{
					iBoolNext=ETrue;
				}

				TInt postl=aEventData.Right(aEventData.Length()-1-post).Find(_L("|"));
				if(post<0)
				{
					iTitle=iMainEngine.GetDesById(ETurkeyTextRes_Setting_SystemMsg).Alloc();
					iSystemInfo=aEventData.Right(aEventData.Length()-1-post).Alloc();
				}
				else
				{
					iTitle=aEventData.Left(post+postl+1).Right(postl).Alloc();
					iSystemInfo=aEventData.Right(aEventData.Length()-post-postl-2).Alloc();
				}
			}


		}
	}
	else
	{
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);
	}
	iObserver.HandleResponseEvent(aEventType,aType);
}