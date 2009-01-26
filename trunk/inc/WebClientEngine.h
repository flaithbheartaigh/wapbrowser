/*
* ==============================================================================
*  Name        : WebClientEngine.h
*  Part of     : WebClient
*  Interface   : 
*  Description : 
*  Version     : 
*
*  Copyright (c) 2005-2007 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef WEBCLIENTENGINE_H
#define WEBCLIENTENGINE_H

// INCLUDES
#include <coecntrl.h>
#include <http.h>
//#include <http\mhttpauthenticationcallback.h>
#include <es_sock.h>

// CONSTANTS
const TInt KMaxHeaderNameLength = 32;
const TInt KMaxHeaderValueLength = 128;
const TInt KMaxAuthTypeLength = 128;
const TInt KMaxDateTimeStringLength = 40;
const TInt KMaxStatusTextLength = 32;

// Used user agent for requests
_LIT8( KUserAgent, "WebClient 1.0" );

// This client accepts all content types.
// (change to e.g. "text/plain" for plain text only)
_LIT8( KAccept, "*/*" );

// Format for output of data/time values
_LIT( KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3" );

// Some texts for header output
_LIT( KColon, ": " );
_LIT( Krealm, "Realm: " );

// CLASS DECLARATION
class MWebClientObserver;

class CWebClientEngine : public CBase, 
						 public MHTTPTransactionCallback/*,
						 public MHTTPAuthenticationCallback*/
{
public:
	static CWebClientEngine* NewL(MWebClientObserver& aObserver);

	static CWebClientEngine* NewLC(MWebClientObserver& aObserver);

	~CWebClientEngine();

	void IssueHTTPGetL(const TDesC8& aUri);

	void CancelTransactionL();

	inline TBool IsRunning(){ return iRunning; };
	
	//inline void  RegWebClientObserver(MWebClientObserver& aObserver) { iObserver = aObserver; };

private:
	void ConstructL();

	CWebClientEngine(MWebClientObserver& iObserver);

	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,	const TDesC8& aHdrValue);

	void DumpRespHeadersL(RHTTPTransaction& aTransantion);

	void HandleRunErrorL(TInt aError);

	void SetupConnectionL();

private:
	void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

	TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

private:
//	TBool GetCredentialsL(const TUriC8& aUri, RString aRealm,
//					RStringF aAuthenticationType, RString& aUsername,
//					RString& aPassword);

private:
	// Data
	RSocketServ iSocketServ;
	RConnection iConnection;
	RHTTPSession iSession;
	RHTTPTransaction iTransaction;
	MWebClientObserver& iObserver; // Used for passing body data and
	// events to UI.
	TBool iRunning; // ETrue, if transaction running
	TBool iConnectionSetupDone; // ETrue, if connection setup is done
};

#endif // WEBCLIENTENGINE_H
