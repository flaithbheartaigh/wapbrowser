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

#include "Define.h"

class CWapBrowserAppView;
class CPhoneNumEngine;
class CConfigEngine;
class CWapEngine;

class CWapBrowserAppUi 
	: public CAknAppUi
{
public: 
	void ConstructL();
	CWapBrowserAppUi();
	virtual ~CWapBrowserAppUi();

private: // Functions from base classes
	void HandleCommandL( TInt aCommand );
	void HandleStatusPaneSizeChange();
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

public:
	void Start();
	void RequestConfig();
	void GetPhoneNumL();
	HBufC8* CombineUrlL();
	void RequestPageL();
	void RequestPageL(const TDesC8& aUrl);
	void RequestPageL(const TDesC8& aUrl,const TDesC8& aBody);
	void UpdateWindow();

public://static
	static CWapBrowserAppUi* Static();

private:
	CPhoneNumEngine& PhoneNumEngineL();
	CConfigEngine& ConfigEngineL();
	CWapEngine& WapEngineL();

private://Test
	void TestReqeuestPage();
	void TestParseConfig();
	void TestGetPhoneNum();
	void TestCombinUrl();
	void TestOrderWml();
	void TestServiceWml();
	void TestMusicWml();
	void TestPost();

private:
	CWapBrowserAppView* iAppView;
	CPhoneNumEngine* iPhoneNumEngine;
	CConfigEngine* iConfigEngine;
	CWapEngine* iWapEngine;
};

#endif // __WAPBROWSERAPPUI_H__
// End of File
