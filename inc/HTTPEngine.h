/*
 * ============================================================================
 *  Name     : CHTTPEngine of HTTPEngine.h
 *  Part of  : HTTP Client Example
 *  Created  : 06/26/2006 by Forum Nokia
 *  Version  : 2.0
 *  Copyright: Forum Nokia
 * ============================================================================
 */

#ifndef __CLIENTENGINE_H__
#define __CLIENTENGINE_H__

#include <coecntrl.h>
#include <http\mhttpdatasupplier.h>
#include <http\mhttptransactioncallback.h>
#include <http\mhttpauthenticationcallback.h>
#include <es_sock.h>
#include <HttpObserver.h>



//#include "CoCoPreDefine.h"
#include "TimeOut.h"

class RHTTPSession;
class RHTTPTransaction;
class MClientObserver;


//#define _DEBUG_

class CHTTPEngine 
	: public CActive
	, public MHTTPTransactionCallback
	, public MHTTPDataSupplier
	, public MHTTPAuthenticationCallback
{
public:
	static CHTTPEngine* NewL();
	static CHTTPEngine* NewL(MClientObserver& iObserver);
	static CHTTPEngine* NewLC(MClientObserver& iObserver);

	~CHTTPEngine();

private:
	void ConstructL();
	CHTTPEngine(MClientObserver& iObserver);
	CHTTPEngine();

private://From MHTTPSessionEventCallback
	void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	TInt MHFRunError(	TInt aError,RHTTPTransaction aTransaction,const THTTPEvent& aEvent);

private://From MHTTPDataSupplier (needed for HTTP POST)
	void ReleaseData();
	TBool GetNextDataPart(TPtrC8& aDataPart);
	TInt Reset();
	TInt OverallDataSize();
	
private://From MHTTPAuthenticationCallback (needed for HTTP authentication)
	TBool GetCredentialsL(	const TUriC8& aURI,RString aRealm,RStringF aAuthenticationType,RString& aUsername,RString& aPassword);

private: // From CActive
	virtual void RunL();
	virtual void DoCancel();

public:
	void SetupConnectionL();
	TInt SetupConnectionL(TRequestStatus* aStatus,TInt aIap);
	//TInt SetupConnectionL(TInt aIap);

	void IssueHTTPGetL(const TDesC8& aUri);	
	void IssueHTTPGetL(const TDesC8& aUri,const TDesC8& aRange);
	void IssueHTTPHeadL(const TDesC8& aUri);
	void IssueHTTPPostL(const TDesC8& aUri,const TDesC8& aContentType,const TDesC8& aBody);
	void CancelTransaction();

	TBool IsRunning();

	void SetThreadIndex(TInt aIndex) ;
	TInt GetThreadIndex();

	TInt GetNetState();
	TInt GetContentLength();
	TInt GetSendSize();
	TInt GetRespStatus();

	void SetAgentType(TDesC8& aAgentType);
	void SetUserAgent(TDesC8& aUserAgent);
	void SetCookie(TDesC8& aCookie);
	void GetCookie(TDes8& aCookie);

	void SetObserver(MClientObserver* aClientObserver);

private:
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,const TDesC8& aHdrValue);
	//void AddHeaderL(RHTTPHeaders aHeaders, const TDesC8& aHdrField,const TDesC8& aHdrValue);

	//20070917张晖增加联网超时功能
	static TInt Periodic(TAny *aPtr);

	void StopPeriodic();
	void StartPeriodic();
	void TimeSlice();

private:
	// declare members
    RSocketServ 			iSocketServ;
    RConnection 			iConnection;

	RHTTPSession			iSession;
	RHTTPTransaction		iTransaction;

	MClientObserver*		iObserver;	// Used for passing body data and
										// events to UI.
	HBufC8*					iPostData;	// Data for HTTP POST
	TBool					iRunning;	// ETrue, if transaction running
	
	TBool 					iConnectionSetupDone;

	TInt iThreadIndex;

	TInt iNetStatus;
	TInt iTotalSize;
	TInt iSendSize;
	TInt iRespStauts;

	HBufC8*					iCookie;
	HBufC8*					iXAgentType;
	HBufC8*					iUserAgent;


	
	CMyTimer iTimer;

	CPeriodic*			iPeriodic;

	TRequestStatus iStatus;
	int iIap;
};

#endif // __CLIENTENGINE_H__
