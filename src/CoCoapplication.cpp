/*
============================================================================
 Name        : CoCoApplication.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Main application class
============================================================================
*/

// INCLUDE FILES
#include "CoCoDocument.h"
#include "CoCoApplication.h"
#include "PreDefine.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
#ifdef __SERIES60_3X__
	const TUid KUidCoCoApp = { 0xe000bf65 };
#else
	const TUid KUidCoCoApp = { 0x03AF99E6 };
#endif

// -----------------------------------------------------------------------------
// CCoCoApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CCoCoApplication::CreateDocumentL()
    {
    // Create an CoCo document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CCoCoDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CCoCoApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CCoCoApplication::AppDllUid() const
{
    // Return the UID for the CoCo application
    return KUidCoCoApp;
}

CDictionaryStore* CCoCoApplication::OpenIniFileLC(RFs& aFs) const
	{
		return CEikApplication::OpenIniFileLC(aFs);
	}
// End of File
