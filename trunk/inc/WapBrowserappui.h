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
#include "Define.h"
#include "HttpObserver.h"

class CWapBrowserAppView;
class CWebClientEngine;
class CHTTPEngine;

// CLASS DECLARATION
class CWapBrowserAppUi 
	: public CAknAppUi
	, public MWebClientObserver
	, public MClientObserver
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

	public://From MClientObserver
		virtual void ClientEvent(const TDesC& aEventDescription,TInt aIndex);
		virtual void ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex);

	public:
		void IssueHTTPGetL(const TDesC8& aUri);

		static CWapBrowserAppUi* Static();
		void RequestPage(const TDesC8& aUrl);

	private:
		void Parse();
		void ParseData(HBufC8* aBuf);
		void RequestPage();
		void RequestConfig();
		void ParserConfig(HBufC8* aBuf);

		CHTTPEngine& HTTPEngine();

	private:
		enum TRequestType
		{
			ERequestNull,
			ERequestPage,
			ERequestConfig
		};

	private: // Data
		CWapBrowserAppView* iAppView;
		CWebClientEngine* iWebClientEngine;
		CHTTPEngine* iHTTPEngine;
		HBufC8* iReceiveData8;

		int iRequestType;

		TBool iIsRequesting;
	};

#endif // __WAPBROWSERAPPUI_H__
// End of File
