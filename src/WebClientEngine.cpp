/*
 * ==============================================================================
 *  Name        : WebClientEngine.cpp
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


#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <http.h>

#include <e32base.h>
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h>
#include <hlplch.h>
#include "WebClientEngine.h"
#include "Define.h"


static void DumpFile(TDesC8 &aDes)
{
	_LIT(KDumpFile, "dumplog.txt");
	
	RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    RFile file;
    TInt err=file.Open(fs, KDumpFile, EFileWrite);
    CleanupClosePushL(file);
    if (err==KErrNotFound)     {
        err=file.Create(fs,KDumpFile,EFileWrite);
    }
    
    TInt aPos=0;
    file.Seek(ESeekEnd, aPos);
    file.Write(aDes);     file.Write(_L8("###"));     file.Close();
    CleanupStack::PopAndDestroy(2);
}
CWebClientEngine* CWebClientEngine::NewL(MWebClientObserver& aObserver)
{
	CWebClientEngine* self = CWebClientEngine::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
}

CWebClientEngine* CWebClientEngine::NewLC(MWebClientObserver& aObserver)
{
	CWebClientEngine* self = new (ELeave) CWebClientEngine( aObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CWebClientEngine::CWebClientEngine(MWebClientObserver& aObserver) :
	iObserver(aObserver), iRunning(EFalse)
{
	}

CWebClientEngine::~CWebClientEngine()
{
	iSession.Close();
	iConnection.Close();
	iSocketServ.Close();
}

void CWebClientEngine::ConstructL()
{
		TRAPD( err, iSession.OpenL() );
/*
	if (err != KErrNone)
	{
								CAknErrorNote* errorNote;
		errorNote = new (ELeave) CAknErrorNote;
				User::Leave(err);
	}
*/

				}

void CWebClientEngine::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField, RHTTPSession::GetTable() ), val);
	CleanupStack::PopAndDestroy( &valStr);
}

void CWebClientEngine::DumpRespHeadersL(RHTTPTransaction& aTransaction)
{
	RHTTPResponse resp = aTransaction.Response();
	RStringPool strP = aTransaction.Session().StringPool();
	RHTTPHeaders hdr = resp.GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();

	HBufC* headerField = HBufC::NewLC(KMaxHeaderNameLength + KMaxHeaderValueLength);
	HBufC* fieldValBuf = HBufC::NewLC(KMaxHeaderValueLength);

	while (it.AtEnd() == EFalse)
	{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = strP.StringF(fieldName);
		THTTPHdrVal fieldVal;
		if (hdr.GetField(fieldNameStr, 0, fieldVal) == KErrNone)
		{
			const TDesC8& fieldNameDesC = fieldNameStr.DesC();
			headerField->Des().Copy(fieldNameDesC.Left(KMaxHeaderNameLength));
			fieldValBuf->Des().Zero();
			switch (fieldVal.Type() )
			{
								case THTTPHdrVal::KTIntVal:
					fieldValBuf->Des().Num(fieldVal.Int() );
					break;
									case THTTPHdrVal::KStrFVal:
				{
					RStringF fieldValStr = strP.StringF(fieldVal.StrF() );
					const TDesC8& fieldValDesC = fieldValStr.DesC();
					fieldValBuf->Des().Copy(fieldValDesC.Left(KMaxHeaderValueLength));
				}
					break;
									case THTTPHdrVal::KStrVal:
				{
					RString fieldValStr = strP.String(fieldVal.Str() );
					const TDesC8& fieldValDesC = fieldValStr.DesC();
					fieldValBuf->Des().Copy(fieldValDesC.Left(KMaxHeaderValueLength) );
				}
					break;
									case THTTPHdrVal::KDateVal:
				{
					TDateTime date = fieldVal.DateTime();
					TBuf<KMaxDateTimeStringLength> dateTimeString;
					TTime t(date);
					t.FormatL(dateTimeString, KDateFormat);
					fieldValBuf->Des().Copy(dateTimeString);
				}
					break;
									default:
					break;
			}

						headerField->Des().Append(KColon);
			headerField->Des().Append( *fieldValBuf);
			iObserver.ClientHeaderReceived( *headerField);

						RStringF wwwAuth = strP.StringF(HTTP::EWWWAuthenticate, RHTTPSession::GetTable() );
			if (fieldNameStr == wwwAuth)
			{
								RStringF basic = strP.StringF(HTTP::EBasic, RHTTPSession::GetTable() );
				RStringF realm = strP.StringF(HTTP::ERealm, RHTTPSession::GetTable() );
				THTTPHdrVal realmVal;
				if ((fieldVal.StrF() == basic ) && ( !hdr.GetParam(wwwAuth, realm, realmVal)))
				{
					RStringF realmValStr = strP.StringF(realmVal.StrF() );
					fieldValBuf->Des().Copy(realmValStr.DesC() );
					headerField->Des().Copy(Krealm);
					headerField->Des().Append( *fieldValBuf);
					iObserver.ClientHeaderReceived( *headerField);
				}
			}
		}
		++it;
	}
	CleanupStack::PopAndDestroy(fieldValBuf);
	CleanupStack::PopAndDestroy(headerField);
}

void CWebClientEngine::HandleRunErrorL(TInt aError)
{
/*
				*/

}

void CWebClientEngine::IssueHTTPGetL(const TDesC8& aUri)
{
	SetupConnectionL();

		TUriParser8 uri;
	uri.Parse(aUri);

		RStringF method = iSession.StringPool().StringF(HTTP::EGET, RHTTPSession::GetTable());

			iTransaction = iSession.OpenTransactionL(uri, *this, method);

		RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);

			iTransaction.SubmitL();

	iRunning = ETrue;

		
}

void CWebClientEngine::CancelTransactionL()
{
	if ( !iRunning)
		return;

			iTransaction.Close();

		iRunning = EFalse;

	}


void CWebClientEngine::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{

	switch (aEvent.iStatus)
	{
		case THTTPEvent::EGotResponseHeaders:
		{
									
						RHTTPResponse resp = aTransaction.Response();
			TInt status = resp.StatusCode();

						TBuf<KMaxStatusTextLength> statusText;
			statusText.Copy(resp.StatusText().DesC() );

			
						DumpRespHeadersL(aTransaction);

		}
			break;

		case THTTPEvent::EGotResponseBodyData:
		{
									
						MHTTPDataSupplier* body = aTransaction.Response().Body();
			TPtrC8 dataChunk;

									TBool isLast = body->GetNextDataPart(dataChunk);
			iObserver.ClientBodyReceived(dataChunk, isLast); 

			DumpFile(dataChunk);

			
															if (isLast)
			{
			}

						body->ReleaseData();
		}
			break;

		case THTTPEvent::EResponseComplete:
		{
								}
			break;

		case THTTPEvent::ESucceeded:
		{
			
						aTransaction.Close();
			iRunning = EFalse;
		}
			break;

		case THTTPEvent::EFailed:
		{
						aTransaction.Close();
			iRunning = EFalse;
		}
			break;

		default:
											{
			if (aEvent.iStatus < 0)
			{
												aTransaction.Close();
				iRunning = EFalse;
			}
			else
			{
							}
		}
			break;
	}
}

TInt CWebClientEngine::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
{
	
	HandleRunErrorL( aError );
	return KErrNone;
}

/*
TBool CWebClientEngine::GetCredentialsL(const TUriC8& aUri, RString aRealm, RStringF aAuthenticationType, RString& aUsername, RString& aPassword)
{
	return ETrue;
#if 0	
			
		TBuf<KMaxUserNameLength> userName;
	TBuf<KMaxPasswordLength> password;
	CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL(userName, password);

	if ( !dlg->ExecuteLD( R_WEBCLIENT_DIALOG_USER_PASSWORD_QUERY))
		return EFalse; 
		TBuf8<KMaxUserNameLength> temp;
	temp.Copy(userName);
	TRAPD( err, aUsername = aRealm.Pool().OpenStringL( temp ))
	;
	if ( !err)
	{
		temp.Copy(password);
		TRAP( err, aPassword = aRealm.Pool().OpenStringL( temp ));
		if ( !err)
			return ETrue;
	}

			return EFalse;
#endif	
}
*/

void CWebClientEngine::SetupConnectionL()
{
	if (iConnectionSetupDone)
		return;

	iConnectionSetupDone = ETrue;

		User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iConnection.Open(iSocketServ));
	User::LeaveIfError(iConnection.Start());

		RStringPool strPool = iSession.StringPool();
	RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();

		connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable() ), THTTPHdrVal(iSocketServ.Handle()) );

	connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ), THTTPHdrVal(REINTERPRET_CAST(TInt, &(iConnection))) );
}

