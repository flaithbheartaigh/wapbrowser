/*
* ============================================================================
*  Name     : CHTTPEngine of HTTPEngine.cpp
*  Part of  : HTTP Client Example
*  Created  : 06/20/2006 by Forum Nokia
*  Version  : 2.0
*  Copyright: Forum Nokia
* ============================================================================
*/
#include <httpstringconstants.h>
#include <commdbconnpref.h>

#include "PreDefine.h"
#include "HTTPEngine.h"
#include "TimeOut.h"
/*

_LIT8(KUserAgent, "SimpleClient 1.0");

_LIT8(KAccept, "* / *");

_LIT8(KProxy, "10.0.0.172:80");

_LIT8(KXOnlineIP1, "59.36.96.182");
_LIT8(KXOnlineIP2, "59.36.96.182:10891");

_LIT8(KXOnline, "X-Online-Host");

_LIT8(KXAgentType,"x-AgentType");
_LIT8(KXAgentTypeValue,"multiAgent/Symbian/V1.0");

_LIT8(KAcceptEnCoding,"Accept-Encoding");
_LIT8(KAcceptEnCodingValue,"NoEncoding");

_LIT8(KCallingPhone,"x-up-calling-line-id");
_LIT8(KCallingPhoneValue,"13912341250");

_LIT8(KCookie,"Cookie");
*/

const TInt KStatustextBufferSize = 256;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;
const TInt KDefaultBufferSize = 256;

const TInt KBeyondTime=60000;

CHTTPEngine* CHTTPEngine::NewL()
{
	return NULL;
}

CHTTPEngine* CHTTPEngine::NewL(MClientObserver& aObserver)
{
	CHTTPEngine* self = CHTTPEngine::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
}

CHTTPEngine* CHTTPEngine::NewLC(MClientObserver& aObserver)
{
	CHTTPEngine* self = new (ELeave) CHTTPEngine(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
}

CHTTPEngine::CHTTPEngine(MClientObserver& aObserver)
	:CActive(EPriorityStandard)
	, iObserver(&aObserver)
	, iPostData(NULL)
	, iCookie(NULL)
	, iXAgentType(NULL)
	, iUserAgent(NULL)

	, iRunning(EFalse)
	, iPeriodic(NULL)
	, iThreadIndex(0)
	, iRespStauts(0)
{
}

CHTTPEngine::CHTTPEngine()
	: CActive(EPriorityStandard)
	, iPostData(NULL)
	, iCookie(NULL)
	, iXAgentType(NULL)
	, iUserAgent(NULL)

	, iRunning(EFalse)
	, iPeriodic(NULL)
	, iThreadIndex(0)
	, iRespStauts(0)
{
}

CHTTPEngine::~CHTTPEngine()
{	
	Cancel();	//Cancel CActive

	iSession.Close();

	iConnection.Close();
	iSocketServ.Close();

	delete iPostData;
	iPostData = NULL;
	delete iPeriodic;
	iPeriodic = NULL;


	delete iCookie;
	iCookie = NULL;
	delete iXAgentType;
	iXAgentType = NULL;
	delete iUserAgent;
	iUserAgent = NULL;
}

void CHTTPEngine::ConstructL()
{

	delete iPeriodic;
	iPeriodic = NULL;

	iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
	iPeriodic->Start(1000, 10000 * 1000, TCallBack(Periodic, this));

	TRAPD(err, iSession.OpenL());
	if(err != KErrNone) 
	{

		_LIT(KErrMsg,"Cannot create session. Is internet access point configured?");
		_LIT(KExitingApp, "Exiting app.");

		User::Leave(err);
	}
	else
	{

		/*RStringPool pool = iSession.StringPool();
		iSession.FilterCollection().RemoveFilter( pool.StringF( HTTP::ERedirect, RHTTPSession::GetTable()));*/
	}

	InstallAuthenticationL(iSession);

#ifndef __SERIES60_3X__
	CHttpFilterProxyInterface::InstallFilterL(iSession);
#endif

	CActiveScheduler::Add(this);				// Add to scheduler
	//CActiveScheduler::Start();

	SetupConnectionL();
}

/*
void CHTTPEngine::AddHeaderL(RHTTPHeaders aHeaders, const TDesC8& aHdrField,const TDesC8& aHdrValue)
{
	RStringF fieldStr = iSession.StringPool().OpenFStringL(aHdrField);
	CleanupClosePushL(fieldStr);

	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);

	THTTPHdrVal val(valStr);

	aHeaders.SetFieldL(fieldStr, val);
	CleanupStack::PopAndDestroy(2);
}
*/

void CHTTPEngine::SetHeaderL(RHTTPHeaders aHeaders,TInt aHdrField,const TDesC8& aHdrValue)
{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(); 
}

void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri)
{

	/*HBufC8* tempUrl=HBufC8::NewL(aUri.Length());
	tempUrl->Des().Append(_L8("http:
	TInt post=aUri.Mid(7).Find(_L8("/"));
	if (post>0)
	{
		tempUrl->Des().Append(aUri.Mid(7+post));
	}*/

	TUriParser8 uri;
	uri.Parse(aUri);

	RStringF method = iSession.StringPool().StringF(HTTP::EGET,RHTTPSession::GetTable());

	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);

	/*华为产品应用
	if (iCookie)
	{
		RStringF cookieField = iSession.StringPool().StringF(HTTP::ECookie,RHTTPSession::GetTable());

		TBuf8<100> cookieInfo;
		cookieInfo.Zero ();
		cookieInfo.Append(iCookie->Des());

		hdr.SetRawFieldL(cookieField, (const TDesC8&)cookieInfo, _L8(""));
	}

	if(iUserAgent)
	{
		SetHeaderL(hdr, HTTP::EUserAgent, iUserAgent->Des());
	}
	else
	{
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	}
	if (iXAgentType)
	{
		AddHeaderL(hdr,KXAgentType,iXAgentType->Des());
	}
	else
	{
		AddHeaderL(hdr,KXAgentType,KXAgentTypeValue);
	}
	AddHeaderL(hdr,KAcceptEnCoding,KAcceptEnCodingValue);
*/

#ifndef __SERIES60_3X__
	SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
#endif

	/*if (aUri.Find(KXOnlineIP2)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP2);
	}
	else if (aUri.Find(KXOnlineIP1)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP1);
	}*/

	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+100;

	iRunning = ETrue;

	iNetStatus=0;
	_LIT(KConnecting,"Connecting...");
	ASSERT(iObserver);
	iObserver->ClientEvent(KConnecting,iThreadIndex);

	iTimer.Begin (KBeyondTime);

}

void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri,const TDesC8& aRange)
{
	/*HBufC8* tempUrl=HBufC8::NewL(aUri.Length());
	tempUrl->Des().Append(_L8("http:
		TInt post=aUri.Mid(7).Find(_L8("/"));
	if (post>0)
	{
		tempUrl->Des().Append(aUri.Mid(7+post));
	}*/

	TUriParser8 uri;
	uri.Parse(aUri);

	RStringF method = iSession.StringPool().StringF(HTTP::EGET,RHTTPSession::GetTable());

	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();

	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::ERange , aRange); 
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	/*华为产品应用
	if (iCookie)
	{
		RStringF cookieField = iSession.StringPool().StringF(HTTP::ECookie,RHTTPSession::GetTable());

		TBuf8<100> cookieInfo;
		cookieInfo.Zero ();
		cookieInfo.Append(iCookie->Des());

		hdr.SetRawFieldL(cookieField, (const TDesC8&)cookieInfo, _L8(""));
	}

	if(iUserAgent)
	{
		SetHeaderL(hdr, HTTP::EUserAgent, iUserAgent->Des());
	}
	else
	{
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	}
	if (iXAgentType)
	{
		AddHeaderL(hdr,KXAgentType,iXAgentType->Des());
	}
	else
	{
		AddHeaderL(hdr,KXAgentType,KXAgentTypeValue);
	}

	AddHeaderL(hdr,KAcceptEnCoding,KAcceptEnCodingValue);*/

#ifndef __SERIES60_3X__
	SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
#endif

	/*if (aUri.Find(KXOnlineIP2)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP2);
	}
	else if (aUri.Find(KXOnlineIP1)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP1);
	}*/

	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+100;

	iRunning = ETrue;

	iNetStatus=0;
	_LIT(KConnecting,"Connecting...");
	ASSERT(iObserver);
	iObserver->ClientEvent(KConnecting,iThreadIndex);

	iTimer.Begin (KBeyondTime);

}

void CHTTPEngine::IssueHTTPHeadL(const TDesC8& aUri)
{
	/*HBufC8* tempUrl=HBufC8::NewL(aUri.Length());
	tempUrl->Des().Append(_L8("http:
		TInt post=aUri.Mid(7).Find(_L8("/"));
	if (post>0)
	{
		tempUrl->Des().Append(aUri.Mid(7+post));
	}*/

	TUriParser8 uri;
	uri.Parse(aUri);

	RStringF method = iSession.StringPool().StringF(HTTP::EHEAD,RHTTPSession::GetTable());

	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();

	SetHeaderL(hdr, HTTP::EAccept, KAccept);

	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	/*华为产品应用
	if (iCookie)
	{
		RStringF cookieField = iSession.StringPool().StringF(HTTP::ECookie,RHTTPSession::GetTable());

		TBuf8<100> cookieInfo;
		cookieInfo.Zero ();
		cookieInfo.Append(iCookie->Des());

		hdr.SetRawFieldL(cookieField, (const TDesC8&)cookieInfo, _L8(""));
	}

	if(iUserAgent)
	{
		SetHeaderL(hdr, HTTP::EUserAgent, iUserAgent->Des());
	}
	else
	{
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	}
	if (iXAgentType)
	{
		AddHeaderL(hdr,KXAgentType,iXAgentType->Des());
	}
	else
	{
		AddHeaderL(hdr,KXAgentType,KXAgentTypeValue);
	}

	AddHeaderL(hdr,KAcceptEnCoding,KAcceptEnCodingValue);*/

#ifndef __SERIES60_3X__
	SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
#endif

	/*if (aUri.Find(KXOnlineIP2)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP2);
	}
	else if (aUri.Find(KXOnlineIP1)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP1);
	}*/

	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+100;

	iRunning = ETrue;

	iNetStatus=0;

	_LIT(KConnecting,"Connecting...");
	ASSERT(iObserver);
	iObserver->ClientEvent(KConnecting,iThreadIndex);

	iTimer.Begin (KBeyondTime);

}

void CHTTPEngine::IssueHTTPPostL(const TDesC8& aUri,const TDesC8& aContentType,const TDesC8& aBody)
{
	/*HBufC8* tempUrl=HBufC8::NewL(aUri.Length());
	tempUrl->Des().Append(_L8("http:
		TInt post=aUri.Mid(7).Find(_L8("/"));
	if (post>0)
	{
		tempUrl->Des().Append(aUri.Mid(7+post));
	}*/

	TUriParser8 uri;
	uri.Parse(aUri);

	delete iPostData;
	iPostData = aBody.AllocL();

	RStringF method = iSession.StringPool().StringF(HTTP::EPOST,RHTTPSession::GetTable());

	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();

	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EContentType, aContentType);
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);

	/*华为产品应用
	if (iCookie)
	{
		RStringF cookieField = iSession.StringPool().StringF(HTTP::ECookie,RHTTPSession::GetTable());

		TBuf8<100> cookieInfo;
		cookieInfo.Zero ();
		cookieInfo.Append(iCookie->Des());

		hdr.SetRawFieldL(cookieField, (const TDesC8&)cookieInfo, _L8(""));
	}

	if(iUserAgent)
	{
		SetHeaderL(hdr, HTTP::EUserAgent, iUserAgent->Des());
	}
	else
	{
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	}
	if (iXAgentType)
	{
		AddHeaderL(hdr,KXAgentType,iXAgentType->Des());
	}
	else
	{
		AddHeaderL(hdr,KXAgentType,KXAgentTypeValue);
	}
	AddHeaderL(hdr,KAcceptEnCoding,KAcceptEnCodingValue);*/

#ifndef __SERIES60_3X__
	SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
#endif

	/*if (aUri.Find(KXOnlineIP2)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP2);
	}
	else if (aUri.Find(KXOnlineIP1)>0)
	{
		AddHeaderL(hdr,KXOnline,KXOnlineIP1);
	}*/

	iTransaction.Request().SetBody(*this);

	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+aBody.Length()+100;

	iRunning = ETrue;

	iNetStatus=0;
	_LIT(KConnecting,"Connecting...");
	ASSERT(iObserver);
	iObserver->ClientEvent(KConnecting,iThreadIndex);

	iTimer.Begin (KBeyondTime);

}

void CHTTPEngine::CancelTransaction()
{
	if(!iRunning)
		return;

	iTransaction.Close();

	iRunning = EFalse;

	iNetStatus=0;
	iTotalSize=0;

	iRespStauts=0;

	_LIT(KTransactionCancelled, "Transaction cancelled");
	ASSERT(iObserver);
	iObserver->ClientEvent(KTransactionCancelled,iThreadIndex);
}

void CHTTPEngine::MHFRunL(RHTTPTransaction aTransaction,const THTTPEvent& aEvent)
{
	iNetStatus=aEvent.iStatus;

	switch (aEvent.iStatus)
	{
	case THTTPEvent::EGotResponseHeaders:
		{

			iTimer.Begin (KBeyondTime);
			RHTTPResponse resp = aTransaction.Response();
			TInt status = resp.StatusCode();

			iRespStauts=status;

			RHTTPHeaders headers = resp.GetHeaderCollection();

			TBuf<KStatustextBufferSize> statusText;
			statusText.Copy(resp.StatusText().DesC());

			TBuf<KDefaultBufferSize> text;
			_LIT(KHeaderReceived, "Http-Status:%d");
			text.Format(KHeaderReceived, status);

			_LIT8(KContentLength,"Content-Length"); 
			RStringF contentLength = iSession.StringPool().OpenFStringL(KContentLength);

			CleanupClosePushL( contentLength );
			THTTPHdrVal fieldVal;

			TInt err = headers.GetField(contentLength, 0, fieldVal);
			CleanupStack::PopAndDestroy();

			if (err == KErrNone)
			{
				iTotalSize = fieldVal.Int();
			}

			_LIT8(KContentType,"Content-Type"); 
			RStringF contentType = iSession.StringPool().OpenFStringL(KContentType);
			CleanupClosePushL( contentType );

			THTTPHdrVal fieldTypeVal;
			User::LeaveIfError(headers.GetField(contentType, 0, fieldTypeVal));
			CleanupStack::PopAndDestroy();

			ASSERT(iObserver);
			iObserver->ClientEvent(text,iThreadIndex);

#ifndef __WINS__

			HBufC8* contentTypeText=HBufC8::NewL(fieldTypeVal.Str().DesC().Length()+13);
			contentTypeText->Des().Append(KContentType);
			contentTypeText->Des().Append(_L(":"));
			contentTypeText->Des().Append(fieldTypeVal.Str().DesC());

			HBufC* hBuf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(*contentTypeText);

			ASSERT(iObserver);
			iObserver->ClientEvent(*hBuf,iThreadIndex);
			delete contentTypeText;
			contentTypeText = NULL;
			delete hBuf;
			hBuf = NULL;

			/*#else
			ASSERT(iObserver);
			ASSERT(iObserver);
			iObserver->ClientEvent(text,iThreadIndex);*/
#endif

		}
		break;

	case THTTPEvent::EGotResponseBodyData:
		{
			iTimer.Begin (KBeyondTime);

			MHTTPDataSupplier* body = aTransaction.Response().Body();
			TPtrC8 dataChunk;

			TBool isLast = body->GetNextDataPart(dataChunk);
			ASSERT(iObserver);
			iObserver->ClientBodyReceived(dataChunk,iThreadIndex);

			TBuf<KInfotextBufferSize> text;
			_LIT(KBodyPartReceived, "%d bytes received... ");
			text.Format(KBodyPartReceived, dataChunk.Length());

			ASSERT(iObserver);
			ASSERT(iObserver);
			iObserver->ClientEvent(text,iThreadIndex);

			body->ReleaseData();

			if (isLast)
			{
				iNetStatus=-888;

				_LIT(KBodyReceived,"Body received");
				ASSERT(iObserver);
				iObserver->ClientEvent(KBodyReceived,iThreadIndex);
				iNetStatus=0;
			}

		}
		break;

	case THTTPEvent::EResponseComplete:
		{

			_LIT(KTransactionComplete, "Transaction Complete");
			ASSERT(iObserver);
			iObserver->ClientEvent(KTransactionComplete,iThreadIndex);
		}
		break;

	case THTTPEvent::ESucceeded:
		{

			aTransaction.Close();
			iRunning = EFalse;

			_LIT(KTransactionSuccessful, "Transaction Successful");
			ASSERT(iObserver);
			iObserver->ClientEvent(KTransactionSuccessful,iThreadIndex);			
		}
		break;

	case THTTPEvent::EFailed:
		{

			aTransaction.Close();
			iRunning = EFalse;

			iNetStatus=-999; 
			_LIT(KTransactionFailed, "Transaction Failed");
			ASSERT(iObserver);
			iObserver->ClientEvent(KTransactionFailed,iThreadIndex);

			iNetStatus=0; 
		}
		break;

	default:
		{
			TBuf<KInfotextBufferSize> text;
			if (aEvent.iStatus < 0)
			{
				_LIT(KErrorStr, "Error: %d");
				text.Format(KErrorStr, aEvent.iStatus);

				iNetStatus=-999; 

				aTransaction.Close();
				iRunning = EFalse;
			} 
			else 
			{

				_LIT(KUnrecognisedEvent, "Unrecognised event: %d");
				text.Format(KUnrecognisedEvent, aEvent.iStatus);
			}

			if(aEvent.iStatus==KErrNotReady)
			{
				this->iObserver->ClientEvent(_L("Error:KErrNotReady"),iThreadIndex);
			}
			else if(aEvent.iStatus==KErrDisconnected)
			{
				this->iObserver->ClientEvent(_L("Error:KErrDisconnected"),iThreadIndex);
			}
			else
			{
				ASSERT(iObserver);
				iObserver->ClientEvent(text,iThreadIndex);
			}
		}
		break;
	}
}

TInt CHTTPEngine::MHFRunError(TInt aError,RHTTPTransaction /*aTransaction*/,const THTTPEvent& /*aEvent*/)
{

	TBuf<KInfotextBufferSize>	text;
	_LIT(KRunError, "MHFRunError: %d");
	text.Format(KRunError, aError);
	ASSERT(iObserver);
	iObserver->ClientEvent(text,iThreadIndex);

	return KErrNone;
}

TBool CHTTPEngine::GetNextDataPart(TPtrC8& aDataPart)
{
	if(iPostData)
	{

		aDataPart.Set(iPostData->Des());
	}
	return ETrue;
}

void CHTTPEngine::ReleaseData()
{

	delete iPostData;
	iPostData = NULL;
}

TInt CHTTPEngine::Reset()
{

	return KErrNone;
}

TInt CHTTPEngine::OverallDataSize()
{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
}

void CHTTPEngine::SetupConnectionL()
{
	iIap = 3;
	iStatus = KRequestPending ;
	if(SetupConnectionL(&iStatus,iIap) == KErrNone)
	{
		SetActive();
	}
}

TInt CHTTPEngine::SetupConnectionL(TRequestStatus* aStatus,TInt aIap)
{

	if( iConnectionSetupDone )
	{
		iSession.Close();

		iConnection.Close();
		iSocketServ.Close();

		this->ConstructL();
	}

	iConnectionSetupDone = ETrue;

	TInt retVal=0;

	retVal=iSocketServ.Connect();

	if(retVal!=KErrNone)
	{
		return retVal;
	}

	retVal=iConnection.Open(iSocketServ);

	if(retVal!=KErrNone)
	{
		return retVal;
	}

	TCommDbConnPref connectPref;

	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionOutgoing);
	connectPref.SetBearerSet(ECommDbBearerGPRS);

	connectPref.SetIapId(aIap);

	//iConnection.Start(connectPref,*aStatus);
	iConnection.Start(connectPref);

	RStringPool strPool = iSession.StringPool();
	RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();

	connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

	connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable() ),THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );

	//return KErrNone;
	return KErrGeneral;
}
/*

TInt CHTTPEngine::SetupConnectionL(TInt aIap)
{

	if( iConnectionSetupDone )
	{
		iSession.Close();

		iConnection.Close();
		iSocketServ.Close();

		this->ConstructL();
	}

	iConnectionSetupDone = ETrue;

	TInt retVal=0;

	retVal=iSocketServ.Connect();

	if(retVal!=KErrNone)
	{

		return retVal;
	}

	retVal=iConnection.Open(iSocketServ);

	if(retVal!=KErrNone)
	{

		return retVal;
	}

	TCommDbConnPref connectPref;

	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionOutgoing);
	connectPref.SetBearerSet(ECommDbBearerGPRS);

	connectPref.SetIapId(aIap);

	retVal=iConnection.Start(connectPref);

	if(retVal!=KErrNone)
	{

		return retVal;
	}

	RStringPool strPool = iSession.StringPool();
	RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();

	connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

	connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable() ),THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );

	return KErrNone;
}
*/

TBool CHTTPEngine::GetCredentialsL(const TUriC8& aURI,RString aRealm,RStringF aAuthenticationType,RString& aUsername,RString& aPassword) 
{
	TBuf<128> authType;
	TBuf<128> uri;
	TBuf<256> authText;
	authType.Copy(aAuthenticationType.DesC());
	uri.Copy(aURI.UriDes());
	_LIT(KAuthRequired, "%S requires %S authentication.");
	authText.Format(KAuthRequired, &uri, &authType);
	_LIT(KAuthNote, "Authentication required.");

	TBuf<256> userName;
	TBuf<256> password;

	TBuf8<256> temp;
	temp.Copy(userName);
	TRAPD(err, aUsername = aRealm.Pool().OpenStringL(temp));
	if (!err) 
	{
		temp.Copy(password);
		TRAP(err, aPassword = aRealm.Pool().OpenStringL(temp));
		if (!err) return ETrue;
	}

	return EFalse;
}
//////////////////////////////////////////////////////////////////////////
//From CActive
//////////////////////////////////////////////////////////////////////////
void CHTTPEngine::RunL()
{

	if(iStatus==KErrNone)
	{
/*
		if (iState == EInitialized) 
		{
			iConnectFlag=EFalse;

			iState=EConnectIAP;
			iConnectionStatusOne.iState=EStartIap;
			iHttpEngTwo->SetupConnectionL(&iStatus,iIAPValue);
			SetActive();	
		} 
		else if (iState == EConnectIAP) 
		{
			iState=EConnectSuccess;
			iConnectFlag=ETrue;
			this->CheckHttpArray();
		}
		else if (iState == EConnectSuccess) 
		{
			iConnectFlag=ETrue;	
		}

		iConnectCount=0;*/

	}
	else
	{

/*
		iConnectCount++;

		if(iConnectCount<=3)
		{
			//SetActive();	
		}
		else
		{
			iState=EConnectError;
		}*/

	}

}

void CHTTPEngine::DoCancel()
{

}

void CHTTPEngine::StopPeriodic()
{
	if(iPeriodic)
		iPeriodic->Cancel();
}

void CHTTPEngine::StartPeriodic()
{
	if(iPeriodic)
		iPeriodic->Start(1000, 10000 * 1000, TCallBack(Periodic, this));
}

TInt CHTTPEngine::Periodic(TAny *aPtr)
{
	STATIC_CAST(CHTTPEngine*, aPtr)->TimeSlice();	
	return 1;
}

void CHTTPEngine::TimeSlice()
{

	if(iRunning && iTimer.Check())
	{

		CancelTransaction();
		iNetStatus=-999; 

		_LIT(KTimeOut,"TimeOut");
		ASSERT(iObserver);
		iObserver->ClientEvent(KTimeOut,iThreadIndex);

		iNetStatus=0;

		iTimer.Begin (KBeyondTime);
	}
}


void CHTTPEngine::SetAgentType(TDesC8& aAgentType)
{
	delete iXAgentType;
	iXAgentType = NULL;

	iXAgentType=aAgentType.AllocL();
	iXAgentType->Des().LowerCase();
}

void CHTTPEngine::SetUserAgent(TDesC8& aUserAgent)
{
	delete iUserAgent;
	iUserAgent = NULL;

	iUserAgent=aUserAgent.AllocL();
	iUserAgent->Des().LowerCase();
}

void CHTTPEngine::SetCookie(TDesC8& aCookie)
{
	delete iCookie;
	iCookie = NULL;

	iCookie=aCookie.AllocL();
}

void CHTTPEngine::GetCookie(TDes8& aCookie)
{
	if (iCookie)
	{
		TPtrC8 cookieBuf;

		cookieBuf.Set(iCookie->Des());

		aCookie.Copy(cookieBuf);
	}
	else
	{
		aCookie.Zero ();
	}
}



void CHTTPEngine::SetObserver(MClientObserver* aClientObserver)
{
	iObserver = aClientObserver;
}

TInt CHTTPEngine::GetThreadIndex() 
{ 
	return iThreadIndex;
}

TInt CHTTPEngine::GetNetState()
{
	return iNetStatus;
}

TInt CHTTPEngine::GetContentLength()
{
	return iTotalSize;
}

TInt CHTTPEngine::GetSendSize()
{
	return iSendSize;
}

TInt CHTTPEngine::GetRespStatus()
{
	return iRespStauts;
}

TBool CHTTPEngine::IsRunning()
{ 
	return iRunning; 
}

void CHTTPEngine::SetThreadIndex(TInt aIndex) 
{ 
	iThreadIndex=aIndex;
}
