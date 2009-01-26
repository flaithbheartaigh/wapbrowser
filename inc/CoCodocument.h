/*
============================================================================
 Name        : CoCoDocument.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application document class (model)
============================================================================
*/

#ifndef __COCODOCUMENT_H__
#define __COCODOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CCoCoAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
* CCoCoDocument application class.
* An instance of class CCoCoDocument is the Document part of the
* AVKON application framework for the CoCo example application.
*/
class CCoCoDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CCoCoDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCoCoDocument.
        */
        static CCoCoDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CCoCoDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCoCoDocument.
        */
        static CCoCoDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CCoCoDocument
        * Virtual Destructor.
        */
        virtual ~CCoCoDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CCoCoAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CCoCoDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CCoCoDocument( CEikApplication& aApp );

    };

#endif // __COCODOCUMENT_H__

// End of File
