/*
============================================================================
 Name        : CoCoAppUi.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Main application UI class (controller)
============================================================================
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <CoCo.rsg>
#include <f32file.h>
#include <s32file.h>
#include <APGTASK.H>

#include "CoCo.pan"
#include "CoCoAppUi.h"
#include "CoCoAppView.h"
#include "CoCo.hrh"

#include "PreDefine.h"
#include "MainEngine.h"

#include "CoCoPreDefine.h"
#include "Window.h"
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCoCoAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCoCoAppUi::ConstructL()
    {
	UtilityTools::WriteLogsL(_L("CCoCoAppUi::ConstructL"));
    // Initialise app UI with standard value.
    BaseConstructL();

    // Create view object
    iAppView = CCoCoAppView::NewL(ApplicationRect());

	AddToStackL(iAppView);
#ifndef __WINS__
	iObserver = CIncomingCallObserver::NewL(this);
#endif
    }
// -----------------------------------------------------------------------------
// CCoCoAppUi::CCoCoAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCoCoAppUi::CCoCoAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCoCoAppUi::~CCoCoAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CCoCoAppUi::~CCoCoAppUi()
{
	RemoveFromStack(iAppView);
	if ( iAppView )
	{
		delete iAppView;
		iAppView = NULL;
	}
	if(iObserver)
	{
		delete iObserver;
		iObserver = NULL;
	}
}

// -----------------------------------------------------------------------------
// CCoCoAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CCoCoAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        case ECoCoCommand1:
            break;
        default:
            Panic( ECoCoUi );
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CCoCoAppUi::HandleStatusPaneSizeChange()
{
	iAppView->SetRect( ClientRect() );
	
} 

CCoCoAppView* CCoCoAppUi::Container() const
{
	ASSERT(NULL != iAppView);
	return iAppView;
}
void CCoCoAppUi::HandleForegroundEventL(TBool aForeground)
{
	if(aForeground)
	{
		iAppView->GetMainEnging().CurWindow()->HandleCommandL(KKeyForegroundEvent);
	}
	else
	{
		iAppView->GetMainEnging().CurWindow()->HandleCommandL(KKeyBackgroundEvent);
	}
}
void CCoCoAppUi::HandleSystemEventL(const TWsEvent &aEvent)
{
	TApaSystemEvent systemEvent = *(TApaSystemEvent*)aEvent.EventData();

    switch(systemEvent)
    {
        case EApaSystemEventShutdown:
        {
			HandleCommandL(EEikCmdExit);

            break;
        }
        default:
        {
            break;
        }
    }
}
void CCoCoAppUi::IncomingCallObserverInterface_HandleItL(TInt aType,const TDesC& aText)
{
	if(aType==0) //未有电话呼入
	{
		iAppView->GetMainEnging().CurWindow()->HandleCommandL(KKeyPhoneIdleEvent);
	}
	else if(aType==1) //电话呼入
	{
		iAppView->GetMainEnging().CurWindow()->HandleCommandL(KKeyPhoneIncomingEvent);
	}
	else if(aType==2)
	{
		#ifdef __SERIES60_3X__
			iAppView->GetMainEnging().SetIMEI(aText);
		#endif
	}
}
// End of File
