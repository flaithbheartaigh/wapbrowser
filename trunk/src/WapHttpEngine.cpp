/*
============================================================================
 Name        : WapHttpEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWapHttpEngine implementation
============================================================================
*/
#include "WapHttpEngine.h"
#include "UtilityTools.h"


CWapHttpEngine::CWapHttpEngine()
{
}

CWapHttpEngine::~CWapHttpEngine()
{
	delete iUrl;
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
			UtilityTools::WriteTestFileL(_L("C:\\Data\\Config.txt"),*iReceiveData8);
			if(!HandlePushPage() && iObserver)
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

#undef  TRACE
#define TRACE UtilityTools::WriteLogsL

void CWapHttpEngine::IssueHTTPGetL(const TDesC8& aUrl)
{
	TRACE(aUrl);
	delete iUrl;
	iUrl = NULL;
	iUrl = aUrl.Alloc();
// 	if(NULL == iHTTPEngine)
// 	{
// 		iHTTPEngine = CHTTPEngine::NewL(*this);
// 	}
	ASSERT(iObserver);
	HTTPEngine()->IssueHTTPGetL(aUrl);
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

TBool CWapHttpEngine::HandlePushPage()
{
	TRACE(_L("CWapHttpEngine::HandlePushPage"));
	ASSERT(iReceiveData8);

	//*iReceiveData8;

// 	<onevent type="onenterforward">
// 	<go href="http://218.16.120.168:80/miniwap/g.txt?t=68994"></go></onevent>

	TBool result = EFalse;
	if(iUrl)
	{
		TRACE(*iUrl);
		TPtrC8 urlPrefix;
		{
			_LIT8(KPrefix,	"<go href=\"http:");
			const TDesC8& prefix(KPrefix);
			urlPrefix.Set(*iUrl);
			//int pos = urlPrefix.Find(_L8(":"));
			int pos = urlPrefix.Find(prefix);
			urlPrefix.Set(iUrl->Mid(pos + prefix.Length()));
			pos += urlPrefix.Find(_L8(":"));
			urlPrefix.Set(iUrl->Left(pos));

		}
		TRACE(urlPrefix);		
		//int pos = iReceiveData8->Find(*iUrl);
		int pos = iReceiveData8->Find(urlPrefix);
		//int pos = iReceiveData8->Find(_L8("218.16.120.168"));
		TRACE(_L("pos:%d"),pos);
		if(-1 != pos)
		{
			TPtrC8 url(iReceiveData8->Mid(pos + 1));
			int pos2 = url.Find(_L8("\">"));
			TRACE(_L("pos2:%d"),pos2);
			if(-1 != pos2)
			{
				url.Set(iReceiveData8->Mid(pos,pos2 + 1));
				//TPtrC8 url2 = iReceiveData8->Mid(pos,pos2 - 1);
				//url = url.Mid(0,pos - 1);
				TRACE(url);
				IssueHTTPGetL(url);
				//if(&& iReceiveData8->Find(*iUrl))
				//return result;
				result = ETrue;
			}
		}
	}

	TRACE(_L("CWapHttpEngine::HandlePushPage result:%d"),result);
	return result;

/*
	if(iReceiveData8->Find(_L8("onenterforward")));
	{
		_LIT8(KFindString		,"<go href=\"");
		int pos = iReceiveData8->Find(KFindString) + KFindString.iTypeLength;
		TPtrC8 url = iReceiveData8->Mid(pos + 1);
		pos = url.Find(_L8("\""));
		url = url.Left(pos);
		IssueHTTPGetL(url);
		return ETrue;
	}
*/


	//return EFalse;
}