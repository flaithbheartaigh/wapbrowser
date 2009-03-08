/*
* ============================================================================
*  Name     : CIAPConnect_v20ProgressNotifier from IAPConnect_v20ProgressNotifier.h
*  Part of  : IAPConnect_v20
*  Created  : 12.10.2003 by Forum Nokia
*  Description:
*     Progress notifier class declaration.
*  Version  :
*  Copyright: Nokia Corporation
* ============================================================================
*/

#ifndef IAPCONNECT_V20PROGRESSNOTIFIER_H
#define IAPCONNECT_V20PROGRESSNOTIFIER_H

#include "IAPConnect_v20StateObserver.h"

#include <e32base.h>
#include <Es_sock.h>

class CIAPConnect_v20ProgressNotifier : public CActive
    {
public:

    /*
     * NewL()
     *  
     * Create CIAPConnect_v20ProgressNotifier object
     *
     * Params:
     * MIAPConnect_v20StateObserver* aObserver State observer interface
     * RConnection& aConnection Reference to the connection object
     *
     */
    static CIAPConnect_v20ProgressNotifier* 
        NewL(MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection);

    /*
     * NewLC()
     *  
     * Create CIAPConnect_v20ProgressNotifier object and place to cleanup stack
     *
     * MIAPConnect_v20StateObserver* aObserver State observer interface
     * RConnection& aConnection Reference to the connection object
     *
     */
    static CIAPConnect_v20ProgressNotifier* 
        NewLC(MIAPConnect_v20StateObserver* aObserver, RConnection& aConnection);

    /*
     * ~CIAPConnect_v20ProgressNotifier~()
     *  
     * Destructor
     *
     */
    ~CIAPConnect_v20ProgressNotifier();

    /*
     * SetState()
     *  
     * Set internal State
     *
     * Params:
     * TInt aState      State to be set
     *
     */
    void SetState(TInt aState);

    /*
     * GetState()
     *  
     * Get the internal state
     *
     * Returns:
     * TInt Internal state
     *
     */
    TInt GetState() const;

protected: // from CActive

    /*
     * DoCancel()
     *  
     * Cancel outstanding requests
     *
     */
    void DoCancel();

    /*
     * RunL()
     *  
     * Handle request completion events 
     *
     */
    void RunL();

private:
    
    /*
     * CIAPConnect_v20ProgressNotifier()
     *  
     * Constructor
     *
     * Params:
     * MIAPConnect_v20StateObserver* aObserver State observer interface
     * RConnection& aConnection Reference to the connection object
     *
     */
    CIAPConnect_v20ProgressNotifier(MIAPConnect_v20StateObserver* aObserver, 
        RConnection& aConnection);

    /*
     * ConstructL()
     *  
     * Second phase Constructor
     *
     */
    void ConstructL();

private:

    // Member variables
    
    TNifProgressBuf iProgress;
    TInt iState;
    
    RConnection& iConnection;

    // Owned by CIAPConnect_v20AppUi
    MIAPConnect_v20StateObserver* iObserver;    
    
    };

#endif // IAPCONNECT_V20PROGRESSNOTIFIER_H

