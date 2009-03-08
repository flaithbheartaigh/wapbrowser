/*
* ============================================================================
*  Name     : CIAPConnect_v20Engine from IAPConnect_v20Engine.cpp
*  Part of  : IAPConnect_v20
*  Created  : 08.01.2004 by Forum Nokia
*  Implementation notes:
*       Engine class implementation
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

// INCLUDE FILES

#include "IAPConnect_v20.hrh"
#include "IAPConnect_v20Engine.h"
#include "IAPConnect_v20ProgressNotifier.h"

//#include <iapconnect_v20.rsg>

#include <eikenv.h>
#include <aknglobalnote.h> 

#include <nifman.h>

#include <TextResolver.h>
#include <aknquerydialog.h> 
#include <apsettingshandlerui.h> 
#include <aputils.h> 

// CONSTANTS
_LIT(KTextConnectionCreated,        "Connection created");
_LIT(KTextConnectionExists,         "Connection exists");
_LIT(KTextGeneralError,             "General error");
_LIT(KTextConnectionCancelled,      "Connection cancelled");
_LIT(KTextNotReady,                 "Active request already ongoing");
_LIT(KTextAlreadyReleased,          "Connection is already released");
_LIT(KTextNotConnected,             "There is no active connection");
_LIT(KTextConnectionReleased,       "Application has released the connection");
_LIT(KTextErrorCodePrompt,          "\nError code: ");

_LIT8(KTextConnectionInfoHeader,    "--- Connections ---");
_LIT8(KTextConnectionInfoTitle,     "\nConnection %d:\n");
_LIT8(KTextConnectionInfoClients,   " Number of clients: %d\n");
_LIT8(KTextConnectionInfoIds,       " IapId: %d\n NetId: %d\n");
_LIT8(KTextConnectionInfoProcessId, "  Process id: %d\n");
_LIT8(KTextConnectionInfoNoConnections, "--- No connections ---\n");

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::NewL(MIAPConnect_v20StateObserver* aObserver)
//
// Constructs CIAPConnect_v20Engine object
// ---------------------------------------------------------------------------
//
CIAPConnect_v20Engine* CIAPConnect_v20Engine::NewL(
    MIAPConnect_v20StateObserver* aObserver)
    {
    CIAPConnect_v20Engine* self = NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::NewLC(MIAPConnect_v20StateObserver* aObserver)
//
// Constructs CIAPConnect_v20Engine object
// ---------------------------------------------------------------------------
//    
CIAPConnect_v20Engine* CIAPConnect_v20Engine::NewLC(
    MIAPConnect_v20StateObserver* aObserver)
    {
    CIAPConnect_v20Engine* self = new (ELeave) CIAPConnect_v20Engine(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::CIAPConnect_v20Engine(
//    MIAPConnect_v20StateObserver* aObserver)
//
// Constructor
// ---------------------------------------------------------------------------
//  
CIAPConnect_v20Engine::CIAPConnect_v20Engine(MIAPConnect_v20StateObserver* aObserver)
: CActive(CActive::EPriorityStandard),
  iObserver(aObserver)
    {
    SetConnectionPreferences(ECommDbBearerUnknown, ETrue);
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::~CIAPConnect_v20Engine()
//
// Destructor
// ---------------------------------------------------------------------------
//  
CIAPConnect_v20Engine::~CIAPConnect_v20Engine()
    {
   	Cancel();

    delete iTextResolver;
	delete iProgressNotifier;

    iConnect.Close();
    iSocketServ.Close();

    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::ConstructL()
//
// Second phase constructor
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::ConstructL()
    {

   	iSocketServ.Connect();
    iConnect.Open(iSocketServ);

   	iTextResolver = CTextResolver::NewL();
    iProgressNotifier = CIAPConnect_v20ProgressNotifier::NewL(iObserver, iConnect);

	CActiveScheduler::Add(this);
   
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::ConnectL()
//
// Start connecting to IAP. 
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::ConnectL()
	{

	if (!IsActive())
		{ 
        iConnect.Start(iPref, iStatus);
		SetActive();
        }
	else 
		{
		CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
		globalNote->ShowNoteL(EAknGlobalInformationNote, KTextNotReady); 
		CleanupStack::PopAndDestroy();
		}

	}

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::RunL()
//
// Handle request completion events
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::RunL()
	{
 
    TInt statusCode = iStatus.Int();

    switch (statusCode)
	    {

    case KErrNone: // Connection created succesfully
        {
        PrintNoteL(KTextConnectionCreated);
        break;
        }

    case KErrNotFound: // Connection failed
        {
        PrintNoteL(KTextGeneralError);
        break;
        }

    case KErrCancel: // Connection attempt cancelled
        {
        PrintNoteL(KTextConnectionCancelled);
        break;
        }
        
    case KErrAlreadyExists: // Connection already exists
        {
        PrintNoteL(KTextConnectionExists);                
        break;
        }

    default:
        {
        TBuf<KErrorResolverMaxTextLength> errorText;
        // Use text resolver to resolve error text
        errorText = iTextResolver->ResolveError(statusCode);
        PrintErrorL(errorText, statusCode);
        break;
        }
        }

	}


// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::PrintErrorL(const TDesC& aErrorText, TInt aErrorCode)
//
// Shows error notification
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::PrintErrorL(
    const TDesC& aErrorText, 
    TInt aErrorCode) const
    {
    TBuf<KErrorResolverMaxTextLength+30> buf;
    
    buf.Append(aErrorText);
    buf.Append(KTextErrorCodePrompt);
    buf.AppendNum(aErrorCode);

	CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
	globalNote->ShowNoteL(EAknGlobalErrorNote, buf); 
	CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::PrintNoteL(const TDesC& aNoteText)
//
// Shows information notification
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::PrintNoteL(const TDesC& aNoteText) const
    {
	CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
	globalNote->ShowNoteL(EAknGlobalInformationNote, aNoteText); 
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::DoCancel()
//
// Cancels ongoing requests
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::DoCancel()
	{
    // Nothing to do here
	}


// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::TerminateConnectionL()
//
// This function should not be used in normal situation, since it terminates
// the active connection, even if there are other application using
// the connection. Calling the RConnection::Close() closes the connection.
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::TerminateConnectionL()
    {

    if (iProgressNotifier->GetState() == EIAPConnectStateConnected)
        {
        // Show confirmation dialog first
        if (ShowQueryDialogL())
            {
            // RConnection::Stop() terminates connection by stopping whole
            // network interface even if there are other clients using 
            // the connection. It is not recommended to use it unless it is 
            // really meaningful. 
            User::LeaveIfError(iConnect.Stop());
        
            }
        }
    else
        {
        PrintNoteL(KTextNotConnected);
        }
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::GetConnectionInfoL()
//
// This function collects information about active connections
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::GetConnectionInfoL(TDes8& aInfoBuf)
    { 

    TUint connCount = 0;
    // Count active connections
    iConnect.EnumerateConnections(connCount);

    if (connCount)
        {
        aInfoBuf.Append(KTextConnectionInfoHeader);
    
        for(TUint i = 1; i <= connCount; i++)
            {
            aInfoBuf.AppendFormat(KTextConnectionInfoTitle, i);
            
            TPckgBuf<TConnectionInfo> connInfo;
            // Get connection info
            User::LeaveIfError(iConnect.GetConnectionInfo(i, connInfo));
        
            aInfoBuf.AppendFormat(KTextConnectionInfoIds, 
                connInfo().iIapId, connInfo().iNetId);
        
            // Get info about clients using this connection
            TConnectionEnumArg args;
            args.iIndex = i;
            TConnEnumArgBuf enumBuf(args);
        
            TConnectionGetClientInfoArg clientInfoArg;
            clientInfoArg.iIndex = 1;
            TConnGetClientInfoArgBuf clientInfoBuf(clientInfoArg);
        
            TConnectionClientInfo clientInfo;
        
            // Query number of clients using the connection
            User::LeaveIfError(iConnect.Control
                (KCOLConnection, KCoEnumerateConnectionClients, enumBuf));
            
            aInfoBuf.AppendFormat(KTextConnectionInfoClients, 
                enumBuf().iCount);
        
            for(TUint j = 1; j <= enumBuf().iCount; j++)
                {
                clientInfoBuf().iIndex = j;

                // Get client info
                User::LeaveIfError(iConnect.Control(
                    KCOLConnection, KCoGetConnectionClientInfo, clientInfoBuf));
                
                clientInfo = clientInfoBuf().iClientInfo;

                aInfoBuf.AppendFormat(KTextConnectionInfoProcessId, 
                    clientInfo.iProcessId);
                }
            }

        }
    else 
        {
        aInfoBuf.Append(KTextConnectionInfoNoConnections);    
        }
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::IsConnected(TUint32& aIap)
//
// This function checks if there is an active connection. 
// ---------------------------------------------------------------------------
//
TBool CIAPConnect_v20Engine::IsConnected(TUint32& aIap)
    {
    // Check if there is an active connection
    TBool connected = EFalse;

    TUint connectionCount;
    //Enumerate currently active connections across all socket servers
    User::LeaveIfError(iConnect.EnumerateConnections(connectionCount));

    if (connectionCount)
        {
        TPckgBuf<TConnectionInfoV2> connectionInfo;
        for (TUint i = 1; i <= connectionCount; ++i)
            {
            iConnect.GetConnectionInfo(i, connectionInfo);
            
            if (connectionInfo().iIapId == aIap)
                {
                connected = ETrue;
                break;
                }
               
            }
        }
            
    return connected;

    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::ShowQueryDialogL()
//
// This function shows a query dialog to make sure that user wants to terminate
// the active connection.
// ---------------------------------------------------------------------------
//
TInt CIAPConnect_v20Engine::ShowQueryDialogL() const
    {
//     CAknQueryDialog* dlg = new(ELeave) CAknQueryDialog(CAknQueryDialog::ENoTone);
//     return dlg->ExecuteLD(R_IAPCONNECT_V20_TERMINATION_QUERY_DIALOG);

		return 0;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::SetConnectionPreferences(TUint aBearer, 
//                                                 TBool aDialog,
//                                                 TUint32 aIapId)
//
// This function sets connection preferences. 
// ---------------------------------------------------------------------------
//  
void CIAPConnect_v20Engine::SetConnectionPreferences(
    TUint aBearer, 
    TBool aDialog, 
    TUint32 aIapId) 
    {
    iPref.SetDirection(ECommDbConnectionDirectionOutgoing);
    if (aDialog)
        {
        iPref.SetDialogPreference(ECommDbDialogPrefPrompt);
        }
    else
        {
        iPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
        }
    iPref.SetIapId(aIapId);
    iPref.SetBearerSet(aBearer);
    iPref.SetDirection(ECommDbConnectionDirectionOutgoing);

    }

// ---------------------------------------------------------------------------
//  CIAPConnect_v20Engine::SelectAccessPointL(TUint32& aSelectedIap)
//
//  This function launch an access point selection list
// ---------------------------------------------------------------------------
// 
void CIAPConnect_v20Engine::SelectAccessPointL(TUint32& aIap)
    {

    // Create Ap Settings Handler. See SDK Help for parameters.
    CApSettingsHandler* settingsHandler = CApSettingsHandler::NewLC(
        ETrue, 
        EApSettingsSelListIsListPane, 
        EApSettingsSelMenuSelectNormal,
        KEApIspTypeAll, 
        EApBearerTypeAll, 
        KEApSortNameAscending);
    
    TUint32 originallyFocused(0);
    TUint32 aSelectedIap(0); 

    // Show the dialog
    settingsHandler->RunSettingsL(originallyFocused, aSelectedIap);

    // Original value 0 is not valid IAP id, so failed if 0 after RunSettingsL().
    if (aSelectedIap)
        {
        CCommsDatabase* db = CCommsDatabase::NewL();
        CleanupStack::PushL(db);
        CApUtils* apUtils = CApUtils::NewLC(*db);
        // Convert IAP id to CommsDb id
        aIap = apUtils->IapIdFromWapIdL(aSelectedIap);   
        CleanupStack::PopAndDestroy(2);
        }
    else
        {
        aIap = 0;
        }

    CleanupStack::PopAndDestroy();

    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20Engine::GetState()
//
// Queries the connection state from the progress notifier
// ---------------------------------------------------------------------------
//  
TInt CIAPConnect_v20Engine::GetState() const
    {
    return iProgressNotifier->GetState();
    }


// End of file