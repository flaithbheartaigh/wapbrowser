/*
* ============================================================================
*  Name     : MIAPConnect_v20StateObserver from IAPConnect_v20StateObserver.h
*  Part of  : IAPConnect_v20
*  Created  : 30.9.2003
*  Description:
*     State observer interface declaration.
*  Version  : 1.0
*  Copyright: 
* ============================================================================
*/

#ifndef IAPCONNECTSTATEOBSERVER_H
#define IAPCONNECTSTATEOBSERVER_H

// INCLUDES

#include <e32std.h>

// CLASS DECLARATION

/**
* MIAPConnect_v20StateObserver  State Observer interface
*/
class MIAPConnect_v20StateObserver 
    {
public:

    /*
     * ChangeStateL()
     *  
     * Notify State change
     * 
     * Params:
     * const TDesC& aText   State change text
     *
     */
    virtual void ChangeStateL(const TDesC& aText) = 0;
    };

#endif // IAPCONNECTSTATEOBSERVER_H

