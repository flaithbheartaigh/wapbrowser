/*
============================================================================
Name		: WapBrowserAppUi.cpp
Author	  : 浮生若茶
Version	 :
Copyright   : Your copyright notice
Description : Main application UI class (controller)
============================================================================
*/
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <WapBrowser.rsg>
#include <f32file.h>
#include <s32file.h>
#include "WapBrowser.pan"
#include "WapBrowserAppUi.h"
#include "WapBrowserAppView.h"
#include "WapBrowser.hrh"

#include "WmlParser.h"
#include "PageBuilder.h"
#include "WebClientEngine.h"
#include "Page.h"


void CWapBrowserAppUi::ConstructL()
{
	BaseConstructL();
	iAppView = CWapBrowserAppView::NewL( ClientRect() );
	AddToStackL(iAppView);
}

CWapBrowserAppUi::CWapBrowserAppUi()
{
}

CWapBrowserAppUi::~CWapBrowserAppUi()
{
	RemoveFromStack(iAppView);
	if ( iAppView )
	{
		delete iAppView;
		iAppView = NULL;
	}
}

void CWapBrowserAppUi::HandleCommandL( TInt aCommand )
{
	switch( aCommand )
	{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EWapBrowserCommand1:
			{
				CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
				CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
				CleanupStack::PopAndDestroy();	//parser
				iAppView->ShowPage(pageBuilder->FetchPage());
				CleanupStack::PopAndDestroy();	//pageBuilder
			}
			break;
		default:
			Panic( EWapBrowserUi );
			break;
	}

}

void CWapBrowserAppUi::HandleStatusPaneSizeChange()
{
	iAppView->SetRect( ClientRect() );	
} 

//////////////////////////////////////////////////////////////////////////
//From MWebClientObserver
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::ClientEvent(const TDesC& aEventDescription)
{

}

void CWapBrowserAppUi::ClientHeaderReceived(const TDesC& aHeaderData)
{

}

void CWapBrowserAppUi::ClientBodyReceived(const TDesC8& aBodyData,const TBool& isLast)
{

}

void CWapBrowserAppUi::IssueHTTPGetL(const TDesC8& aUri)
{
	if(NULL == iWebClientEngine)
	{
		iWebClientEngine = CWebClientEngine::NewL(*this);
	}
	iWebClientEngine->IssueHTTPGetL(aUri);
}

class CCommand
{
	public:
		virtual void Execute() = 0;
};

class CGetCommand : public CCommand
{
	public:
		CGetCommand(CWapBrowserAppUi& aWapBrowserAppUi)
			: iWapBrowserAppUi(aWapBrowserAppUi)
		{
		}
		~CGetCommand()
		{
			delete iUri;
		};

		void SetUri(const TDesC8& aUri)
		{
			iUri = aUri.Alloc();
		}

		virtual void Execute()
		{
			iWapBrowserAppUi.IssueHTTPGetL(*iUri);
		}
		// 	void SetProvider(CWapBrowserAppUi& aWapBrowserAppUi)
		// 	{
		// 	}
	private:
		CWapBrowserAppUi& iWapBrowserAppUi;
		HBufC8* iUri;
};
