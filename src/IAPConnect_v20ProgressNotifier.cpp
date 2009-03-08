/*
* ============================================================================
*  Name     : CIAPConnect_v20ProgressNotifier from 
*               IAPConnect_v20ProgressNotifier.cpp
*  Part of  : IAPConnect_v20
*  Created  : 12.10.2003 by Forum Nokia
*  Description:
*     Progress notifier class implementation.
*  Version  :
*  Copyright: Nokia Corporation
* ============================================================================
*/

// INCLUDES
#include "IAPConnect_v20ProgressNotifier.h"
#include "iapconnect_v20.hrh"
#include <in_iface.h>
#include <aknglobalnote.h>

// CONSTANTS
_LIT(KNotConnectedText,             "Not Connected");  
_LIT(KUnknownStateText,             "Unknown State");
_LIT(KTextConnectionUninitialised,  "Not Connected");  
_LIT(KTextStartingSelection,        "Starting Selection"); 
_LIT(KTextFinishedSelection,        "Finished Selection");
_LIT(KTextConnectionFailure,        "Connection Failure");

_LIT(KTextConnectionOpen,           "Connected");
_LIT(KTextConnectionClosed,         "Connection Closed");   

_LIT(KTextLinkLayerOpen,            "Link Layer Open"); 
_LIT(KTextLinkLayerClosed,          "Link Layer Closed");       

_LIT(KTextConnectionSuspended,      "Connection Suspended");
_LIT(KTextPreparingConnection,      "Preparing Connection");
_LIT(KTextConnecting,               "Connecting...");
_LIT(KTextConnectionStartingClose,  "Disconnecting");		
_LIT(KTextCsdStartingLogIn,         "Starting Login");
_LIT(KTextCsdFinishedLogIn,         "Finished Login");

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::NewL(
//    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
//
// Constructs CIAPConnect_v20ProgressNotifier object
// ---------------------------------------------------------------------------
//
CIAPConnect_v20ProgressNotifier* CIAPConnect_v20ProgressNotifier::NewL(
    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
    {
    CIAPConnect_v20ProgressNotifier* self = NewLC(aObserver, aConnection);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::NewLC(
//    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
//
// Constructs CIAPConnect_v20ProgressNotifier object
// ---------------------------------------------------------------------------
// 
CIAPConnect_v20ProgressNotifier* CIAPConnect_v20ProgressNotifier::NewLC(
    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
    {
    CIAPConnect_v20ProgressNotifier* self = 
        new (ELeave) CIAPConnect_v20ProgressNotifier(aObserver, aConnection);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::CIAPConnect_v20ProgressNotifier(
//    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
//
// Constructor
// ---------------------------------------------------------------------------
// 
CIAPConnect_v20ProgressNotifier::CIAPConnect_v20ProgressNotifier(
    MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection)
: CActive(CActive::EPriorityStandard), 
  iConnection(aConnection), 
  iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::~CIAPConnect_v20ProgressNotifier()
//
// Destructor
// ---------------------------------------------------------------------------
// 
CIAPConnect_v20ProgressNotifier::~CIAPConnect_v20ProgressNotifier()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::ConstructL()
//
// Second Phase Constructor
// ---------------------------------------------------------------------------
// 
void CIAPConnect_v20ProgressNotifier::ConstructL()
    {
    iObserver->ChangeStateL(KNotConnectedText);
    
    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::DoCancel()
//
// Cancel outstanding requests
// ---------------------------------------------------------------------------
// 
void CIAPConnect_v20ProgressNotifier::DoCancel()
    {
    iConnection.CancelProgressNotification();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::RunL()
//
// Handle request completion events
// ---------------------------------------------------------------------------
// 
void CIAPConnect_v20ProgressNotifier::RunL()
    {

    switch (iProgress().iStage)
        {
        // Connection unitialised
        case KConnectionUninitialised:
            iState = EIAPConnectStateNotConnected;
            iObserver->ChangeStateL(KTextConnectionUninitialised);
            break;

        // Starting connetion selection
        case KStartingSelection:
            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextStartingSelection);
            break;

        // Selection finished
        case KFinishedSelection:
            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextFinishedSelection);
            break;

        // Connection failure
        case KConnectionFailure:
            iState = EIAPConnectStateNotConnected;
            iObserver->ChangeStateL(KTextConnectionFailure);
            break;

        // Prepearing connection (e.g. dialing)
        case KPsdStartingConfiguration:
        case KPsdFinishedConfiguration:
        case KCsdFinishedDialling:
        case KCsdScanningScript:
        case KCsdGettingLoginInfo:
        case KCsdGotLoginInfo:
            
            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextPreparingConnection);
            break;
        
        // Creating connection (e.g. GPRS activation)
        case KCsdStartingConnect:
        case KCsdFinishedConnect:

            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextConnecting);
            break;

        // Starting log in
        case KCsdStartingLogIn:

            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextCsdStartingLogIn);
            break;

        // Finished login
        case KCsdFinishedLogIn:

            iState = EIAPConnectStateConnecting;
            iObserver->ChangeStateL(KTextCsdFinishedLogIn);
            break;

        // Connection open
        case KConnectionOpen:
        case KLinkLayerOpen:

            iState = EIAPConnectStateConnected;
            iObserver->ChangeStateL(KTextConnectionOpen);
            break;
            
        // Connection blocked or suspended
        case KDataTransferTemporarilyBlocked:
            
            iState = EIAPConnectStateConnected;
            iObserver->ChangeStateL(KTextConnectionSuspended);
            break;

        // Hangup or GRPS deactivation
        case KConnectionStartingClose:
            
            iState = EIAPConnectStateDisconnecting;
            iObserver->ChangeStateL(KTextConnectionStartingClose);
            break;

        // Connection closed
        case KConnectionClosed:
        case KLinkLayerClosed:

            iState = EIAPConnectStateNotConnected;
            iObserver->ChangeStateL(KTextConnectionClosed);
            break;

        // Unhandled state
        default:
            iState = EIAPConnectStateUnknown;     
            iObserver->ChangeStateL(KUnknownStateText);
            break;
        }

    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::SetState(TInt aState)
//
// Set internal state
// ---------------------------------------------------------------------------
// 
void CIAPConnect_v20ProgressNotifier::SetState(TInt aState)
    {
    iState=aState;
    }

// ---------------------------------------------------------------------------
// CIAPConnect_v20ProgressNotifier::GetState() const
//
// Get internal state
// ---------------------------------------------------------------------------
// 
TInt CIAPConnect_v20ProgressNotifier::GetState() const
    {
    return iState;
    }

