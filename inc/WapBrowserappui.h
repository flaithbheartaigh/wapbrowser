/*
============================================================================
 Name		: WapBrowserAppUi.h
 Author	  : 浮生若茶
 Version	 :
 Copyright   : Your copyright notice
 Description : Main application UI class (controller)
============================================================================
*/
#ifndef __WAPBROWSERAPPUI_H__
#define __WAPBROWSERAPPUI_H__
// INCLUDES
#include <aknappui.h>
#include "Define.h"

class CWapBrowserAppView;
class CWebClientEngine;
// CLASS DECLARATION
class CWapBrowserAppUi 
	: public CAknAppUi
	, public MWebClientObserver
	{
	public: // Constructors and destructor
		void ConstructL();
		CWapBrowserAppUi();
		virtual ~CWapBrowserAppUi();
	private:  // Functions from base classes
		void HandleCommandL( TInt aCommand );
		void HandleStatusPaneSizeChange();

	public://From MWebClientObserver
		virtual void ClientEvent(const TDesC& aEventDescription);
		virtual void ClientHeaderReceived(const TDesC& aHeaderData);
		virtual void ClientBodyReceived(const TDesC8& aBodyData,const TBool& isLast);

	public:
		void IssueHTTPGetL(const TDesC8& aUri);

	private: // Data
		CWapBrowserAppView* iAppView;
		CWebClientEngine* iWebClientEngine;
	};

#endif // __WAPBROWSERAPPUI_H__
// End of File
