/*
============================================================================
 Name        : CoCoDocument.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application document class (model)
============================================================================
*/

// INCLUDE FILES
#include "CoCoAppUi.h"
#include "CoCoDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCoCoDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCoCoDocument* CCoCoDocument::NewL( CEikApplication&
                                                          aApp )
    {
    CCoCoDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCoCoDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCoCoDocument* CCoCoDocument::NewLC( CEikApplication&
                                                           aApp )
    {
    CCoCoDocument* self =
        new ( ELeave ) CCoCoDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCoCoDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCoCoDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCoCoDocument::CCoCoDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCoCoDocument::CCoCoDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCoCoDocument::~CCoCoDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CCoCoDocument::~CCoCoDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CCoCoDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CCoCoDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CCoCoAppUi ) );
    }

// End of File
