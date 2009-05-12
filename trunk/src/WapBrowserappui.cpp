/*
============================================================================
Name		: WapBrowserAppUi.cpp
Author	  : 浮生若茶
Version	 :
Copyright   : Your copyright notice
Description : Main application UI class (controller)
============================================================================
*/
#include "WapBrowserAppUi.h"
#include "WapBrowserAppView.h"

#include "PhoneNumEngine.h"
#include "ConfigEngine.h"
#include "WapEngine.h"



#include "WmlParser.h"
#include "PageBuilder.h"
#include "Page.h"
#include "HTTPEngine.h"
#include "UtilityTools.h"


#include "WapBrowser.hrh"
#include "WapBrowser.rsg"

enum TMenuCmd
{
	EAddCommand = ELastMenuCmd + 1,
	EAddCommand2,
	ETestMusicWml,
	ETestServiceWml,
	ETestOrderWml,
	EMenuRequestPage,
	ETestParse,
	ETestGetPhoneNum,
	ERequestConfig,
	ETestPost,
};

void CWapBrowserAppUi::ConstructL()
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ConstructL"));
	BaseConstructL();
	iAppView = CWapBrowserAppView::NewL( ClientRect() );
	AddToStackL(iAppView);

	Start();
}

CWapBrowserAppUi::CWapBrowserAppUi()
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::CWapBrowserAppUi"));
}

CWapBrowserAppUi::~CWapBrowserAppUi()
{
	delete iPhoneNumEngine;
	delete iConfigEngine;
	delete iWapEngine;

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
	case EWapBrowserCommand1:
		//		Test();
		TestReqeuestPage();
		break;

	case ETestMusicWml:
		TestMusicWml();
		break;

	case ETestServiceWml:
		TestServiceWml();
		break;

	case ETestOrderWml:
		TestOrderWml();
		break;

	case EMenuRequestPage:
		//RequestPageL();
		TestReqeuestPage();
		break;

	case ETestParse:
		WapEngineL().TestParse();
		//TestParse();
		break;

	case ETestGetPhoneNum:
		TestGetPhoneNum();
		break;

	case ERequestConfig:
		RequestConfig();
		break;

	case ETestPost:
		TestPost();
		break;

	case EEikCmdExit:
	case EAknSoftkeyExit:
		Exit();
		//RequestConfig();
		break;

	default:
		//Panic( EWapBrowserUi );
		break;
	}
}

void CWapBrowserAppUi::HandleStatusPaneSizeChange()
{
	iAppView->SetRect( ClientRect() );	
} 

void CWapBrowserAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	//return;
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::DynInitMenuPaneL aResourceId = %x aMenuPane = %x"),aResourceId,aMenuPane);
	if(aMenuPane && R_WAPBROWSER_MENU == aResourceId)
	//if(aMenuPane)
	{
		{
			UtilityTools::WriteLogsL(_L("1"));
			CEikMenuPaneItem::SData data;
			UtilityTools::WriteLogsL(_L("2"));
			data.iText.Append(_L("TestMusicWml"));
			UtilityTools::WriteLogsL(_L("3"));
			data.iCommandId = ETestMusicWml;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			UtilityTools::WriteLogsL(_L("4"));
			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
			UtilityTools::WriteLogsL(_L("5"));
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("TestServiceWml"));
			data.iCommandId = ETestServiceWml;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("TestOrderWml"));
			data.iCommandId = ETestOrderWml;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("RequestPageL"));
			data.iCommandId = EMenuRequestPage;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("TestParse"));
			data.iCommandId = ETestParse;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("TestGetPhoneNum"));
			data.iCommandId = ETestGetPhoneNum;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("Start a test session"));
			data.iCommandId = ERequestConfig;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("TestPost"));
			data.iCommandId = ETestPost;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}
		
/*
		{
			CEikMenuPaneItem::SData data;
			data.iText.Append(_L("RequestConfig"));
			data.iCommandId = ERequestConfig;
			data.iCascadeId=0;
			data.iFlags=0;
			data.iExtraText=KNullDesC;

			aMenuPane->AddMenuItemL(data);  //EWapBrowserCommand1);
		}*/

	}
	//else
	{
		//ASSERT(FALSE);
	}
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::DynInitMenuPaneL End"));
}
//////////////////////////////////////////////////////////////////////////
//public:
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::Start()
{
	//RequestConfig();
}

void CWapBrowserAppUi::RequestConfig()
{
	ConfigEngineL().RequestL();
}

void CWapBrowserAppUi::GetPhoneNumL()
{
	PhoneNumEngineL().RequestL(ConfigEngineL().MobileUrl());
}

HBufC8* CWapBrowserAppUi::CombineUrlL()
{
	const TDesC8& des(_L8("?mobile="));
	const TDesC8& serviceUrl = ConfigEngineL().ServiceUrl();
	const TDesC8& phoneNum = PhoneNumEngineL().PhoneNum();
	int length = 0;
	length += serviceUrl.Length();
	length += des.Length();
	length += phoneNum.Length();
	if(length > 0)
	{
		HBufC8* url = HBufC8::NewLC(length);
		url->Des().Append(serviceUrl);
		url->Des().Append(des);
		url->Des().Append(phoneNum);
		CleanupStack::Pop(url);
		return url;
	}
	return NULL;
}

void CWapBrowserAppUi::RequestPageL()
{
	HBufC8* url = CombineUrlL();
	CleanupStack::PushL(url);
	if(url)
	{
		//RequestPageL(*url);
		WapEngineL().RequestPageL(*url);
	}
	else
	{
		CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote;
		note->ExecuteLD(_L("Request Err"));
	}
	CleanupStack::PopAndDestroy();
}

void CWapBrowserAppUi::RequestPageL(const TDesC8& aUrl)
{
	WapEngineL().RequestPageL(aUrl);
}

void CWapBrowserAppUi::RequestPageL(const TDesC8& aUrl,const TDesC8& aBody)
{
	WapEngineL().RequestPageL(aUrl,aBody);
}

void CWapBrowserAppUi::UpdateWindow()
{
	if(iAppView)
	{
		iAppView->Layout();
		iAppView->DrawNow();
	}
}
//////////////////////////////////////////////////////////////////////////
//public static:
//////////////////////////////////////////////////////////////////////////
CWapBrowserAppUi* CWapBrowserAppUi::Static()
{
	return (CWapBrowserAppUi*)CCoeEnv::Static()->AppUi();
}
//////////////////////////////////////////////////////////////////////////
//private:
//////////////////////////////////////////////////////////////////////////
CPhoneNumEngine& CWapBrowserAppUi::PhoneNumEngineL()
{
	if(NULL == iPhoneNumEngine)
	{
		iPhoneNumEngine = CPhoneNumEngine::NewL();
		iPhoneNumEngine->SetAppUi(this);
	}
	return *iPhoneNumEngine;
}

CConfigEngine& CWapBrowserAppUi::ConfigEngineL()
{
	if(NULL == iConfigEngine)
	{
		iConfigEngine = CConfigEngine::NewL();
		iConfigEngine->SetAppUi(this);
	}
	return *iConfigEngine;
}

CWapEngine& CWapBrowserAppUi::WapEngineL()
{
	if(NULL == iWapEngine)
	{
		iWapEngine = CWapEngine::NewL();
		iWapEngine->SetAppUi(this);
		iWapEngine->SetAppView(iAppView);
	}
	return *iWapEngine;
}
//////////////////////////////////////////////////////////////////////////
//private:test:
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::TestReqeuestPage()
{
	RequestPageL(_L8("http://music.i139.cn/"));
	/*
	Parse();
	//parser->ParseFile("C:\\default_.xml");
	/*/	
	//RequestPageL(_L8("http://music.i139.cn/"));
	//RequestPageL(_L8("http://wap.cocobox.cn/index.do"));
	//RequestPageL(_L8("index.do"));
	//RequestPageL(_L8("portal/wap/menu.do?menuid=212134"));
	//RequestPageL(_L8(""));
	//RequestPageL(_L8("http://wap.monternet.com"));
	//RequestPageL(_L8("http://218.200.244.92/Order?action=4&SessionID=Jm5XLnV7bQ!1999436728!1235646807091&SPID=900636&ServiceID=03205590&SPURL=http://218.206.76.18:8080/dlWap/countDownload.do?	channelId=732&columnId=11769&colResId=265240&resourceFileId=711778&ws=7248343&ctype=100"));
	//*/


/*
	if(!iIsRequesting)
	{
		UtilityTools::WriteLogsL(_L("CWapEngine::RequestPageL"));
		//UtilityTools::WriteLogsL(aUrl);
		//HBufC8* buf = HBufC8::NewLC(255);
		HBufC8* buf = HBufC8::NewLC(aUrl.Length());
		//buf->Des().Append(_L8("http://wap.cocobox.cn/"));	
		//buf->Des().Append(_L8("http://wap.gd.monternet.com/"));
		buf->Des().Append(aUrl);
		//HTTPEngine().IssueHTTPGetL(*buf);
		CleanupStack::PopAndDestroy();
		//HTTPEngine().IssueHTTPGetL(aUrl);

		// 		iRequestType = ERequestPage;
		// 		iAppView->ShowWaiting();

		iIsRequesting = ETrue;
	}
*/
}

void CWapBrowserAppUi::TestParseConfig()
{
	/*
	//以下代码移至SymbianOsUnit
	HBufC8* buf = HBufC8::NewLC(1000);
	buf->Des().Append(_L8("mobile_url=http://wap.gd.monternet.com/portal/wap/menu.do?menuid=212134"));
	buf->Des().Append(_L8("0X0A"));
	buf->Des().Append(_L8("mobile_pre_str=mobileID="));
	buf->Des().Append(_L8("0X0A"));
	buf->Des().Append(_L8("mobile_len=11"));
	buf->Des().Append(_L8("0X0A"));
	buf->Des().Append(_L8("service_url=http://wap.monternet.com/"));

	ParserConfig(buf);
	CleanupStack::PopAndDestroy(buf);
	*/

/*
	HBufC8* fileData = UtilityTools::ReadFileLC(_L("C:\\Data\\g.txt"));
	//ParserConfig(fileData);
	CConfigData configData;
	configData.Parse(*fileData);
	UtilityTools::WriteLogsL(configData.MobileUrl());
	UtilityTools::WriteLogsL(configData.ServiceUrl());
	UtilityTools::WriteLogsL(configData.MobileLen());
	UtilityTools::WriteLogsL(configData.MobilePreStr());
	CleanupStack::PopAndDestroy(fileData);
*/

}

void CWapBrowserAppUi::TestGetPhoneNum()
{
	PhoneNumEngineL().TestParse();
}

void CWapBrowserAppUi::TestCombinUrl()
{
/*
	HBufC8* fileData = UtilityTools::ReadFileLC(_L("C:\\Data\\g.txt"));
	//ParserConfig(fileData);
	iConfigData.Parse(*fileData);
	CleanupStack::PopAndDestroy(fileData);

	fileData = UtilityTools::ReadFileLC(_L("C:\\Data\\m.xml"));
	//ParserConfig(fileData);
	iPhoneNumParser.Parse(*fileData);
	CleanupStack::PopAndDestroy(fileData);

	HBufC8* url = CombineUrl();
	if(url)
	{
		UtilityTools::WriteLogsL(*url);
	}
	else
	{
		UtilityTools::WriteLogsL(_L("no valid url"));
	}
	delete url;
*/
}

void CWapBrowserAppUi::TestOrderWml()
{
	WapEngineL().ParseFile(_L8("C:\\Data\\order.wml"));
}

void CWapBrowserAppUi::TestServiceWml()
{
	WapEngineL().ParseFile(_L8("C:\\Data\\service.wml"));
}

void CWapBrowserAppUi::TestMusicWml()
{
	WapEngineL().ParseFile(_L8("C:\\Data\\music.wml"));
}

#include "PostTester.h"

void CWapBrowserAppUi::TestPost()
{
	CPostTester* postTest = CPostTester::NewL();
	postTest->Test();
}
