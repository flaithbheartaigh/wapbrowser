/*
 * ============================================================================
 *  Name     : CHTTPEngine of HTTPEngine.cpp
 *  Part of  : HTTP Client Example
 *  Created  : 06/20/2006 by Forum Nokia
 *  Version  : 2.0
 *  Copyright: Forum Nokia
 * ============================================================================
 */

#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <uri8.h>
#include <http.h>
#include <chttpformencoder.h>
#include <HttpStringConstants.h>
#include <http\RHTTPTransaction.h>
#include <http\RHTTPSession.h>
#include <http\RHTTPHeaders.h>
#ifndef __SERIES60_3X__
	#include <HttpFilterProxyInterface.h> 
#endif
#include <utf.h>
#include "StaticCommonTools.h"
#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <CommDbConnPref.h>

#include "HTTPEngine.h"
#include "TimeOut.h"

#include "UtilityTools.h"

// Used user agent for requests
_LIT8(KUserAgent, "SimpleClient 1.0");

// This client accepts all content types.
// (change to e.g. "text/plain" for plain text only)
_LIT8(KAccept, "*/*");
_LIT8(KProxy, "10.0.0.172:80");

const TInt KStatustextBufferSize = 32;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;
const TInt KDefaultBufferSize = 256;

//20070917张晖增加超时时间
const TInt KBeyondTime=60000;
// ----------------------------------------------------------------------------
// CHTTPEngine::NewL()
//
// Creates instance of CHTTPEngine.
// ----------------------------------------------------------------------------
CHTTPEngine* CHTTPEngine::NewL(MClientObserver& aObserver)
	{
    CHTTPEngine* self = CHTTPEngine::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::NewLC()
//
// Creates instance of CHTTPEngine.
// ----------------------------------------------------------------------------
CHTTPEngine* CHTTPEngine::NewLC(MClientObserver& aObserver)
	{
    CHTTPEngine* self = new (ELeave) CHTTPEngine(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::CHTTPEngine()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CHTTPEngine::CHTTPEngine(MClientObserver& aObserver)
:	iObserver(aObserver),
	iPostData(NULL),
	iRunning(EFalse),
	iPeriodic(NULL),
	iThreadIndex(0)
	{
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::~CHTTPEngine()
//
// Destructor.
// ----------------------------------------------------------------------------
CHTTPEngine::~CHTTPEngine()
	{	

		iSession.Close();
    // and finally close handles
		
		iConnection.Close();
		iSocketServ.Close();
   
		delete iPostData;

		delete iPeriodic;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CHTTPEngine::ConstructL()
	{
		//初始化和启动时钟
	if(iPeriodic)
	{
		delete iPeriodic;
		iPeriodic=NULL;
	}

	iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
	iPeriodic->Start(1000, 10000 * 1000, TCallBack(Periodic, this));

		// Open RHTTPSession with default protocol ("HTTP/TCP")
	TRAPD(err, iSession.OpenL());
	if(err != KErrNone) 
		{
		// Most common error; no access point configured, and session creation
		// leaves with KErrNotFound.
		_LIT(KErrMsg,
			"Cannot create session. Is internet access point configured?");
		_LIT(KExitingApp, "Exiting app.");
		CEikonEnv::Static()->InfoWinL(KErrMsg, KExitingApp);
		User::Leave(err);
		}

	// Install this class as the callback for authentication requests. When
	// page requires authentication the framework calls GetCredentialsL to get
	// user name and password.
	InstallAuthenticationL(iSession);

	#ifndef __SERIES60_3X__
		CHttpFilterProxyInterface::InstallFilterL(iSession);
	#endif
	
	}

// ----------------------------------------------------------------------------
// CHTTPEngine::AddHeaderL()
//
// Used to add header value to HTTP request
// ----------------------------------------------------------------------------
void CHTTPEngine::AddHeaderL(RHTTPHeaders aHeaders, const TDesC8& aHdrField, 
					const TDesC8& aHdrValue)
{
	RStringF fieldStr = iSession.StringPool().OpenFStringL(aHdrField);
	CleanupClosePushL(fieldStr);

	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);

	THTTPHdrVal val(valStr);

	aHeaders.SetFieldL(fieldStr, val);
	CleanupStack::PopAndDestroy(2);
}

// ----------------------------------------------------------------------------
// CHTTPEngine::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CHTTPEngine::SetHeaderL(RHTTPHeaders aHeaders,
							 TInt aHdrField,
							 const TDesC8& aHdrValue)
	{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,
		RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(); // valStr
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::IssueHTTPGetL()
//
// Start a new HTTP GET transaction.
// ----------------------------------------------------------------------------
void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri)
	{
	//SetupConnectionL();	
		
	// Parse string to URI (as defined in RFC2396)
	TUriParser8 uri;
	uri.Parse(aUri);

	// Get request method string for HTTP GET
	RStringF method = iSession.StringPool().StringF(HTTP::EGET,
		RHTTPSession::GetTable());

	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	// Set headers for request; user agent and accepted content type
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	#ifndef __SERIES60_3X__
		SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
	#endif


	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+100;

	iRunning = ETrue;

	iNetStatus=0;

	_LIT(KConnecting,"Connecting...");
	iObserver.ClientEvent(KConnecting,iThreadIndex);

	//20070917张晖增加判断超时
	iTimer.Begin (KBeyondTime);
	}

void CHTTPEngine::IssueHTTPGetL(const TDesC8& aUri,const TDesC8& aRange)
	{
		//SetupConnectionL();	

		// Parse string to URI (as defined in RFC2396)
		TUriParser8 uri;
		uri.Parse(aUri);

		// Get request method string for HTTP GET
		RStringF method = iSession.StringPool().StringF(HTTP::EGET,
			RHTTPSession::GetTable());

		// Open transaction with previous method and parsed uri. This class will
		// receive transaction events in MHFRunL and MHFRunError.
		iTransaction = iSession.OpenTransactionL(uri, *this, method);

		// Set headers for request; user agent and accepted content type
		RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
		SetHeaderL(hdr, HTTP::EAccept, KAccept);
		SetHeaderL(hdr, HTTP::ERange , aRange); 
		#ifndef __SERIES60_3X__
			SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
		#endif

		// Submit the transaction. After this the framework will give transaction
		// events via MHFRunL and MHFRunError.
		iTransaction.SubmitL();
 
		iSendSize=aUri.Length ()+100;

		iRunning = ETrue;
		
		iNetStatus=0;

		_LIT(KConnecting,"Connecting...");
		iObserver.ClientEvent(KConnecting,iThreadIndex);

		//20070917张晖增加判断超时
		if (iThreadIndex==100)
		{
//		::WriteLogsTemp(_L("HTTPGet Begin"));
		}

		iTimer.Begin (KBeyondTime);
	}

void CHTTPEngine::IssueHTTPHeadL(const TDesC8& aUri)
	{
		//SetupConnectionL();	

		// Parse string to URI (as defined in RFC2396)
		TUriParser8 uri;
		uri.Parse(aUri);

		// Get request method string for HTTP GET
		RStringF method = iSession.StringPool().StringF(HTTP::EHEAD,
			RHTTPSession::GetTable());

		// Open transaction with previous method and parsed uri. This class will
		// receive transaction events in MHFRunL and MHFRunError.
		iTransaction = iSession.OpenTransactionL(uri, *this, method);

		// Set headers for request; user agent and accepted content type
		RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
		SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
		SetHeaderL(hdr, HTTP::EAccept, KAccept);
		#ifndef __SERIES60_3X__
			SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
		#endif

		// Submit the transaction. After this the framework will give transaction
		// events via MHFRunL and MHFRunError.
		iTransaction.SubmitL();

		iSendSize=aUri.Length ()+100;

		iRunning = ETrue;

		iNetStatus=0;

		_LIT(KConnecting,"Connecting...");
		iObserver.ClientEvent(KConnecting,iThreadIndex);

		//20070917张晖增加判断超时
		iTimer.Begin (KBeyondTime);
	}

// ----------------------------------------------------------------------------
// CHTTPEngine::IssueHTTPPostL()
//
// Start a new HTTP POST transaction.
// ----------------------------------------------------------------------------
void CHTTPEngine::IssueHTTPPostL(const TDesC8& aUri,
								 const TDesC8& aContentType,
								 const TDesC8& aBody)
	{
	//SetupConnectionL();	
		
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUri);

	// Copy data to be posted into member variable; iPostData is used later in
	// methods inherited from MHTTPDataSupplier.
	delete iPostData;
	iPostData = aBody.AllocL();

	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EPOST,
		RHTTPSession::GetTable());

	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);

	// Set headers for request; user agent, accepted content type and body's
	// content type.
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EContentType, aContentType);
	#ifndef __SERIES60_3X__
		SetHeaderL(hdr, HTTP::EProxyAddress, KProxy);
	#endif

	// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
	// are called when framework needs to send body data.
	MHTTPDataSupplier* dataSupplier = this;
	iTransaction.Request().SetBody(*dataSupplier);

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iSendSize=aUri.Length ()+aBody.Length()+100;

	iRunning = ETrue;

	iNetStatus=0;

	_LIT(KConnecting,"Connecting...");
	iObserver.ClientEvent(KConnecting,iThreadIndex);

	//20070917张晖增加判断超时
	iTimer.Begin (KBeyondTime);
	}

// ----------------------------------------------------------------------------
// CHTTPEngine::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CHTTPEngine::CancelTransaction()
	{
	if(!iRunning)
		return;

	// Close() also cancels transaction (Cancel() can also be used but
	// resources allocated by transaction must be still freed with Close())
	iTransaction.Close();

	// Not running anymore
	iRunning = EFalse;

	iNetStatus=0;
	iTotalSize=0;

	_LIT(KTransactionCancelled, "Transaction cancelled");
	iObserver.ClientEvent(KTransactionCancelled,iThreadIndex);
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CHTTPEngine::MHFRunL(RHTTPTransaction aTransaction,
						  const THTTPEvent& aEvent)
	{
	iNetStatus=aEvent.iStatus;

	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. Use
			// aTransaction.Response() to get the response. However, it's not
			// necessary to do anything with the response when this event occurs.

			// Get HTTP status code from header (e.g. 200)
			//20070917张晖增加判断超时
				if (iThreadIndex==100)
				{
//					::WriteLogsTemp(_L("EGotResponseHeaders Begin"));
				}

			iTimer.Begin (KBeyondTime);

			RHTTPResponse resp = aTransaction.Response();
			TInt status = resp.StatusCode();

			// Get HTTP header 
			RHTTPHeaders headers = resp.GetHeaderCollection();

			// Get status text (e.g. "OK")
			TBuf<KStatustextBufferSize> statusText;
			statusText.Copy(resp.StatusText().DesC());

			TBuf<KDefaultBufferSize> text;
			_LIT(KHeaderReceived, "Http-Status:%d");
			text.Format(KHeaderReceived, status);

			//Get Content-Length
			_LIT8(KContentLength,"Content-Length"); // get down file size
			RStringF contentLength = iSession.StringPool().OpenFStringL(KContentLength);

			CleanupClosePushL( contentLength );
			THTTPHdrVal fieldVal;

			TInt err = headers.GetField(contentLength, 0, fieldVal);
			CleanupStack::PopAndDestroy();

			if (err == KErrNone)
			{
				iTotalSize = fieldVal.Int();
			}

			//Get Content-type
			_LIT8(KContentType,"Content-Type"); // get down file type
			RStringF contentType = iSession.StringPool().OpenFStringL(KContentType);
			CleanupClosePushL( contentType );

			THTTPHdrVal fieldTypeVal;
			User::LeaveIfError(headers.GetField(contentType, 0, fieldTypeVal));
			CleanupStack::PopAndDestroy();

		#ifndef __WINS__
			HBufC8* contentTypeText=HBufC8::NewLC(fieldTypeVal.Str().DesC().Length()+13);
			contentTypeText->Des().Append(KContentType);
			contentTypeText->Des().Append(_L(":"));
			contentTypeText->Des().Append(fieldTypeVal.Str().DesC());
			
			HBufC* hBuf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(*contentTypeText);
			CleanupStack::PushL(hBuf);
			iObserver.ClientEvent(*hBuf,iThreadIndex);
			CleanupStack::PopAndDestroy(2);
		#else
			iObserver.ClientEvent(text,iThreadIndex);
		#endif
			
			}
			break;

		case THTTPEvent::EGotResponseBodyData:
			{
			// Part (or all) of response's body data received. Use
			// aTransaction.Response().Body()->GetNextDataPart() to get the actual
			// body data.
	
			//20070917张晖增加判断超时
				if (iThreadIndex==100)
				{
//			::WriteLogsTemp(_L("EGotResponseBodyData Begin"));
				}

			iTimer.Begin (KBeyondTime);

			// Get the body data supplier
			MHTTPDataSupplier* body = aTransaction.Response().Body();
			TPtrC8 dataChunk;

			// GetNextDataPart() returns ETrue, if the received part is the last
			// one.
			TBool isLast = body->GetNextDataPart(dataChunk);
			iObserver.ClientBodyReceived(dataChunk,iThreadIndex);

			TBuf<KInfotextBufferSize> text;
			_LIT(KBodyPartReceived, "%d bytes received... ");
			text.Format(KBodyPartReceived, dataChunk.Length());

			iObserver.ClientEvent(text,iThreadIndex);

			// Always remember to release the body data.
			body->ReleaseData();

			// NOTE: isLast may not be ETrue even if last data part received.
			// (e.g. multipart response without content length field)
			// Use EResponseComplete to reliably determine when body is completely
			// received.
			if (isLast)
			{
				iNetStatus=-888;

				_LIT(KBodyReceived,"Body received");
				iObserver.ClientEvent(KBodyReceived,iThreadIndex);

				iNetStatus=0;
			}

			}
			break;

		case THTTPEvent::EResponseComplete:
			{
			// Indicates that header & body of response is completely received.
			// No further action here needed.
				if (iThreadIndex==100)
				{
//			::WriteLogsTemp(_L("EResponseComplete Begin"));
				}

			_LIT(KTransactionComplete, "Transaction Complete");
			iObserver.ClientEvent(KTransactionComplete,iThreadIndex);
			}
			break;

		case THTTPEvent::ESucceeded:
			{
			// Transaction can be closed now. It's not needed anymore.
				if (iThreadIndex==100)
				{
//			::WriteLogsTemp(_L("ESucceeded Begin"));
				}

			aTransaction.Close();
			iRunning = EFalse;

			// Indicates that transaction succeeded.
			_LIT(KTransactionSuccessful, "Transaction Successful");
			iObserver.ClientEvent(KTransactionSuccessful,iThreadIndex);			
			}
			break;

		case THTTPEvent::EFailed:
			{
				if (iThreadIndex==100)
				{
//			::WriteLogsTemp(_L("EFailed Begin"));
				}

			aTransaction.Close();
			iRunning = EFalse;

			// Transaction completed with failure.
			iNetStatus=-999; 

			_LIT(KTransactionFailed, "Transaction Failed");
			iObserver.ClientEvent(KTransactionFailed,iThreadIndex);

			iNetStatus=0; 
			}
			break;

		default:
			// There are more events in THTTPEvent, but they are not usually
			// needed. However, event status smaller than zero should be handled
			// correctly since it's error.
			{
				if (iThreadIndex==100)
				{
//			::WriteLogsTemp(_L("default Begin"));
//			::WriteLogsTemp(_L("default Error:%d"),aEvent.iStatus);
				}

			TBuf<KInfotextBufferSize> text;
			if (aEvent.iStatus < 0)
				{
				_LIT(KErrorStr, "Error: %d");
				text.Format(KErrorStr, aEvent.iStatus);
				
				iNetStatus=-999; 

				// Just close the transaction on errors
				aTransaction.Close();
				iRunning = EFalse;
				} 
			else 
				{
				// Other events are not errors (e.g. permanent and temporary
				// redirections)
				_LIT(KUnrecognisedEvent, "Unrecognised event: %d");
				text.Format(KUnrecognisedEvent, aEvent.iStatus);
				}

			if(aEvent.iStatus==KErrNotReady)
			{
				this->iObserver.ClientEvent(_L("Error:KErrNotReady"),iThreadIndex);
			}
			else if(aEvent.iStatus==KErrDisconnected)
			{
				this->iObserver.ClientEvent(_L("Error:KErrDisconnected"),iThreadIndex);
			}
			else
			{
				iObserver.ClientEvent(text,iThreadIndex);
			}

			iNetStatus=0; 
			}
			break;
		}
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CHTTPEngine::MHFRunError(TInt aError,
							  RHTTPTransaction /*aTransaction*/,
							  const THTTPEvent& /*aEvent*/)
	{
	// Just notify about the error and return KErrNone.
		if (iThreadIndex==100)
		{
//	::WriteLogsTemp(_L("MHFRunError-1"));
		}

	TBuf<KInfotextBufferSize>	text;
	_LIT(KRunError, "MHFRunError: %d");
	text.Format(KRunError, aError);
	iObserver.ClientEvent(text,iThreadIndex);

	if (iThreadIndex==100)
	{
//	::WriteLogsTemp(_L("MHFRunError-2"));
	}
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CHTTPEngine::GetNextDataPart(TPtrC8& aDataPart)
	{
	if(iPostData)
		{
		// Provide pointer to next chunk of data (return ETrue, if last chunk)
		// Usually only one chunk is needed, but sending big file could require
		// loading the file in small parts.
		aDataPart.Set(iPostData->Des());
		}
	return ETrue;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers)
// ----------------------------------------------------------------------------
void CHTTPEngine::ReleaseData()
	{
	// It's safe to delete iPostData now.
	delete iPostData;
	iPostData = NULL;
	}

// ----------------------------------------------------------------------------
// CHTTPEngine::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data
// supplier that it should return to the first part of the data.
// In practise an error has occured while sending data, and framework needs to
// resend data.
// ----------------------------------------------------------------------------
TInt CHTTPEngine::Reset()
	{
	// Nothing needed since iPostData still exists and contains all the data.
	// (If a file is used and read in small parts we should seek to beginning
	// of file and provide the first chunk again in GetNextDataPart() )
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound (it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content
// length in header).
// ----------------------------------------------------------------------------
TInt CHTTPEngine::OverallDataSize()
	{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
	}


// ----------------------------------------------------------------------------
// CHTTPEngine::SetupConnectionL()
//
// The method set the internet access point and connection setups.
// ----------------------------------------------------------------------------	
void CHTTPEngine::SetupConnectionL(TRequestStatus* aStatus,TInt aIap)
    {
	//只有再联网被断开了才能被调用此函数
	if( iConnectionSetupDone )
	{
		iSession.Close();
	
		iConnection.Close();
		iSocketServ.Close();

		this->ConstructL();
	}
    
    iConnectionSetupDone = ETrue;
	

//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL1"));
    //open socket server and start the connection
    User::LeaveIfError(iSocketServ.Connect());
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL2"));
    User::LeaveIfError(iConnection.Open(iSocketServ));
//    //::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL3"));
	// Now we have the iap Id. Use it to connect for the connection.
	// Create a connection preference variable.
	TCommDbConnPref connectPref;

	// setup preferences 
	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionOutgoing);
	connectPref.SetBearerSet(ECommDbBearerGPRS);
	//Sets the CommDb ID of the IAP to use for this connection
	connectPref.SetIapId(aIap);

    //User::LeaveIfError(iConnection.Start(connectPref));
	iConnection.Start(connectPref,*aStatus);
    //set the sessions connection info
    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    
    //to use our socket server and connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,
        RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable() ), 
        THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL end"));
    }

	// ----------------------------------------------------------------------------
// CHTTPEngine::SetupConnectionL()
//
// The method set the internet access point and connection setups.
// ----------------------------------------------------------------------------	
void CHTTPEngine::SetupConnectionL(TInt aIap)
    {
    //if( iConnectionSetupDone )
    //    return;
    //
    //iConnectionSetupDone = ETrue;

	 //open socket server and start the connection
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL_F1"));
    User::LeaveIfError(iSocketServ.Connect());
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL_F2"));
    User::LeaveIfError(iConnection.Open(iSocketServ));
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL_F3"));
	// Now we have the iap Id. Use it to connect for the connection.
	// Create a connection preference variable.
	TCommDbConnPref connectPref;

	// setup preferences 
	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionOutgoing);
	connectPref.SetBearerSet(ECommDbBearerGPRS);
	//Sets the CommDb ID of the IAP to use for this connection
	connectPref.SetIapId(aIap);

    User::LeaveIfError(iConnection.Start(connectPref));
	//iConnection.Start(connectPref,iStatus);
    //set the sessions connection info
    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    
    //to use our socket server and connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,
        RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable() ), 
        THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );
//	//::WriteLogsTemp(_L("CHTTPEngine::SetupConnectionL_F_end"));
    }

// end of file
TBool CHTTPEngine::GetCredentialsL(const TUriC8& aURI,
								   RString aRealm, 
								   RStringF aAuthenticationType,
								   RString& aUsername, 
								   RString& aPassword) 
	{
	TBuf<128> authType;
	TBuf<128> uri;
	TBuf<256> authText;
	authType.Copy(aAuthenticationType.DesC());
	uri.Copy(aURI.UriDes());
	_LIT(KAuthRequired, "%S requires %S authentication.");
	authText.Format(KAuthRequired, &uri, &authType);
	_LIT(KAuthNote, "Authentication required.");
	CEikonEnv::Static()->InfoWinL(KAuthNote, authText);

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

	//=========================================================================
	//==========20070917张晖增加时钟事件
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
		if (iThreadIndex==100)
		{
//			::WriteLogsTemp(_L("TimeSlice Begin"));
		}

		if(iRunning && iTimer.Check())
		{
			if (iThreadIndex==100)
			{
//				::WriteLogsTemp(_L("TimeSlice -999"));
			}
			CancelTransaction();

			iNetStatus=-999; 

			_LIT(KTimeOut,"TimeOut");
			iObserver.ClientEvent(KTimeOut,iThreadIndex);

			iNetStatus=0;

			iTimer.Begin (KBeyondTime);
		}
	}
