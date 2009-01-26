/*
============================================================================
 Name        : CoCoAppUi.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Main application UI class (controller)
============================================================================
*/

#ifndef __COCOAPPUI_H__
#define __COCOAPPUI_H__

// INCLUDES
#include <aknappui.h>
#include "IncomingCallObserver.h"

// FORWARD DECLARATIONS
class CCoCoAppView;
class CIncomingCallObserver;

// CLASS DECLARATION
/**
* CCoCoAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CCoCoAppUi : public CAknAppUi, public MIncomingCallObserverInterface
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CCoCoAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CCoCoAppUi();

        /**
        * ~CCoCoAppUi.
        * Virtual Destructor.
        */
        virtual ~CCoCoAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
 		*  size is changed.
        */

		void HandleStatusPaneSizeChange();
        
	protected:
		void HandleForegroundEventL(TBool aForeground);
		void HandleSystemEventL(const TWsEvent &aEvent);

	public: // MIncomingCallObserverInterface
		virtual void IncomingCallObserverInterface_HandleItL(TInt aType,const TDesC& aText);

	public:
		CCoCoAppView*	Container() const;

    private: // Data

        /**
        * The application view
        * Owned by CCoCoAppUi
        */
        CCoCoAppView* iAppView;
		CIncomingCallObserver* iObserver;    
        
    };

#endif // __COCOAPPUI_H__

// End of File
