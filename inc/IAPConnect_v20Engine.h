/*
* ============================================================================
*  Name     : CIAPConnect_v20Engine from IAPConnect_v20Engine.h
*  Part of  : IAPConnect_v20 by Forum Nokia
*  Created  : 30.9.2003 
*  Description:
*     Engine class declaration
*  Version  : 1.0
*  Copyright: Nokia Corporation
* ============================================================================
*/

#ifndef IAPCONNECT_V20ENGINE_H
#define IAPCONNECT_V20ENGINE_H

// INCLUDES
#include "IAPConnect_v20StateObserver.h"

#include <e32base.h>
#include <CommDbConnPref.h>
#include <es_sock.h>

// FORWARD DECLARATIONS;
class CIAPConnect_v20ProgressNotifier;
class CTextResolver;
class CCommDbOverrideSettings;


// CONSTANTS
const TUint32 KUndefinedIAPid = 0x00;

// CLASS DECLARATION

/**
* CIAPConnect_v20Engine  Controls the internet connection initiation and shutdown
*/
class CIAPConnect_v20Engine : public CActive
    {
public:
    
    /*
     * NewL()
     *  
     * Create CIAPConnect_v20Engine object
     *
     * Params:
     * MIAPConnect_v20StateObserver* aObserver State observer interface
     *
     */
    static CIAPConnect_v20Engine* NewL(MIAPConnect_v20StateObserver* aObserver);

    /*
     * NewL()
     *  
     * Create CIAPConnect_v20Engine object and store pointer to the cleanup stack
     *
     * Params:
     * MIAPConnect_v20StateObserver* aObserver State observer interface
     *
     */
    static CIAPConnect_v20Engine* NewLC(MIAPConnect_v20StateObserver* aObserver);

    /*
     * ~CIAPConnect_v20Engine()
     *  
     * Destructor
     *
     */
    ~CIAPConnect_v20Engine();
  
    /*
     * ConnectL()
     *  
     * Creates a connection to IAP
     *
     */
    void ConnectL();
        
    /*
     * TerminateConnectionL()
     *  
     * Terminate an active connection
     *
     */
    void TerminateConnectionL();

    /*
     * SetConnectionPreferences()
     *  
     * Sets connection preferences
     *
     * Params:
     * TUint aBearer    Bearer type
     * TBool aDialog    Show dialog? 
     * TUint32 aIapId   IAP to be connected, default KUndefinedIAPid
     *
     */
    void SetConnectionPreferences(TUint aBearer, TBool aDialog, 
        TUint32 aIapId=KUndefinedIAPid);

    /*
     * SelectAccessPointL()
     *  
     * Launch an access point selection list
     *
     * Params:
     * TUint32& aSelectedIap    IAP id of the selected access point
     *
     */
    void SelectAccessPointL(TUint32& aSelectedIap);

    /*
     * IsConnected()
     *  
     * Check if an active connection to specied IAP
     *
     * Params:
     * TUint32& aIap    IAP id
     *
     * Returns:
     * TBool    yes/no
     */
    TBool IsConnected(TUint32& aIap);

    /*
     * GetConnectionInfoL()
     *  
     * Get info about active connections
     *
     * Params:
     * TDes8& aInfoBuf    Text buffer containing the connection info
     *
     */
    void GetConnectionInfoL(TDes8& aInfoBuf);

    /*
     * GetConnectionInfoL()
     *  
     * Get connection state
     *
     * Params:
     * TInt     Connection state, see TIAPConnectEngineStates
     *
     */
    TInt GetState() const;

protected: // from CActive

    /*
     * RunL()
     *  
     * Handle request completion events
     *
     */
    void RunL();

    /*
     * DoCancel()
     *  
     * Cancel any outstanding requests
     *
     */
    void DoCancel();

private:

    /*
     * CIAPConnect_v20Engine()
     *  
     * Constructor
     *
     * Params:
     * MIAPConnect_v20StateObserver* aObserver State observer  
     *
     */
    CIAPConnect_v20Engine(MIAPConnect_v20StateObserver* aObserver);

    /*
     * ConstructL()
     *  
     * Second phase constructor
     *
     */
    void ConstructL();

    /*
     * PrintNoteL()
     *  
     * Show information note
     *
     * Params:
     * const TDesC& aNoteText   Text to be shown
     *
     */
    void PrintNoteL(const TDesC& aNoteText) const;
    
    /*
     * PrintErrorL()
     *  
     * Print error note
     *
     * Params:
     * const TDesC& aErrorText  Error text to be shown
     * TInt aErrorCode          Error code
     *
     */
    void PrintErrorL(const TDesC& aErrorText, TInt aErrorCode) const;
    
    /*
     * ShowQueryDialogL()
     *  
     * Show termination query dialog and returns true/false
     *
     * Returns:
     * TInt Response from the user
     */
    TInt ShowQueryDialogL() const;


private:

    TCommDbConnPref iPref;

    // Owns
    RConnection iConnect;
    RSocketServ iSocketServ;
    CTextResolver* iTextResolver;
    CIAPConnect_v20ProgressNotifier* iProgressNotifier;

    // Owned by CIAPAppUi
    MIAPConnect_v20StateObserver* iObserver;    

    };


#endif // IAPCONNECT_V20ENGINE_H
