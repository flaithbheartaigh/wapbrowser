/*
============================================================================
 Name        : CoCoApplication.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Main application class
============================================================================
*/

#ifndef __COCOAPPLICATION_H__
#define __COCOAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CCoCoApplication application class.
* Provides factory to create concrete document object.
* An instance of CCoCoApplication is the application part of the
* AVKON application framework for the CoCo example application.
*/
class CCoCoApplication : public CAknApplication
    {
	public: // Functions from base classes
		CDictionaryStore* OpenIniFileLC(RFs& aFs) const;

    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidCoCoApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CCoCoDocument document object. The returned
        * pointer in not owned by the CCoCoApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __COCOAPPLICATION_H__

// End of File
