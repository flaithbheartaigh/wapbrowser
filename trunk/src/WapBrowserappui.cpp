/*
============================================================================
Name		: WapBrowserAppUi.cpp
Author	  : 浮生若茶
Version	 :
Copyright   : Your copyright notice
Description : Main application UI class (controller)
============================================================================
*/
#include "WapBrowser.pan"
#include "WapBrowserAppUi.h"
#include "WapBrowserAppView.h"
#include "WapBrowser.hrh"

#include "WmlParser.h"
#include "PageBuilder.h"
//#include "WebClientEngine.h"
#include "Page.h"
#include "HTTPEngine.h"
#include "UtilityTools.h"



void TestBookMark()
{
	//////////////////////////////////////////////////////////////////////////
	//2nd
	//////////////////////////////////////////////////////////////////////////

#ifndef __SERIES60_3X__

	/*
	_LIT( KBookmarkDbPath, "c:\\system\\data\\" );
	_LIT( KBookmarkDbFile, "Bookmarks1.db" );

	CAddFavouritesDb* CAddFavouritesDb::NewL()
	{
	CAddFavouritesDb* favdb = new (ELeave) CAddFavouritesDb();
	CleanupStack::PushL( favdb );
	favdb->ConstructL( KBookmarkDbPath, KBookmarkDbFile );
	CleanupStack::Pop();    
	return favdb;
	}

	CAddFavouritesDb * addBookmrk = CAddFavouritesDb::NewL();
	CleanupStack::PushL( addBookmrk );
	User::LeaveIfError(addBookmrk->OpenL());
	CFavouritesItem* bookmrkItem = CFavouritesItem::NewLC();
	bookmrkItem->SetType( CFavouritesItem::EItem );
	bookmrkItem->SetNameL( _L("AddBookmark") );
	bookmrkItem->SetUrlL( _L("http://www.forum.nokia.com") );
	addBookmrk->AddL(*bookmrkItem , ETrue);
	addBookmrk->Close();
	CleanupStack::PopAndDestroy( bookmrkItem );
	CleanupStack::PopAndDestroy( addBookmrk );
	*/
 
	//LIBRARY favouritesengine.lib
#else
	//////////////////////////////////////////////////////////////////////////
	//3nd
	//////////////////////////////////////////////////////////////////////////
	// Link against: favouritesengine.lib 

	RFavouritesSession iSession;
	User::LeaveIfError(iSession.Connect());
	CleanupClosePushL(iSession);

	RFavouritesDb db;
	// KBrowserBookmarks is picked up from the header
	User::LeaveIfError(db.Open(iSession, KBrowserBookmarks)); 
	CleanupClosePushL(db);

	CFavouritesItem* item =  CFavouritesItem::NewLC();
	item->SetNameL(_L("Google UK"));
	item->SetParentFolder(KFavouritesRootUid);
	item->SetType(CFavouritesItem::EItem);
	item->SetUrlL(_L("http://www.google.co.uk/"));

	User::LeaveIfError(db.Add(*item, ETrue));

	CleanupStack::PopAndDestroy(3, &iSession); // db, itemThe equivalent to add the same bookmark to the OSS browser in S60 3rd edition would be: 

#endif
}

void CWapBrowserAppUi::ConstructL()
{
	BaseConstructL();
	iAppView = CWapBrowserAppView::NewL( ClientRect() );
	AddToStackL(iAppView);
	//RequestConfig();

	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ConstructL"));
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

	//TestBookMark();
	//Parse();
	//iAppView->ShowWaiting();
}

CWapBrowserAppUi::CWapBrowserAppUi()
: iHTTPEngine(NULL)
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::CWapBrowserAppUi"));
}

CWapBrowserAppUi::~CWapBrowserAppUi()
{
	//iAppView->StopShowWaiting();

	delete iHTTPEngine;
	RemoveFromStack(iAppView);
	if ( iAppView )
	{
		delete iAppView;
		iAppView = NULL;
	}
}
#include "PhoneNumParser.h"

void CWapBrowserAppUi::HandleCommandL( TInt aCommand )
{
	switch( aCommand )
	{
	case EWapBrowserCommand1:
		//RequestPage();
		//Parse();
		{
			CPhoneNumParser parser;
			parser.Parse(_L8("http://211.139.164.132/wap/?mobileID=15710788274fasfdasfadsfdasfdsafewqfasdfewqfadgasdfsafdasdfaewra"));
			UtilityTools::WriteLogsL(parser.PhoneNum());
		}
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
//////////////////////////////////////////////////////////////////////////
//From MClientObserver
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::ClientEvent(const TDesC& aEventDescription,TInt aIndex)
{
	//User::InfoPrint(aEventDescription);
	UtilityTools::WriteLogsL(aEventDescription);
	/*
	HBufC* buf = HBufC::NewLC(aEventDescription.Length());
	buf->Des().Copy(aEventDescription,aEventDescription.Length());
	RDebug::Print(*buf);
	CleanupStack::PopAndDestroy();*/
	RDebug::Print(aEventDescription);

	if(aEventDescription.Compare(_L("Transaction Successful")) == 0)
		//|| aEventDescription.Compare(_L("Transaction Complete")) == 0)
	{
		//UtilityTools::WriteTestFileL(_L("C:\\default_.xml"),)

		UtilityTools::WriteLogsL(_L("1"));
		if(iReceiveData8)
		{
			iIsRequesting = FALSE;
			iAppView->StopShowWaiting();
			UtilityTools::WriteLogsL(_L("2"));
			//TRAPD(err,UtilityTools::WriteFileL(*iReceiveData8,0,_L("C:\\data\\default_.txt")));
			_LIT(KTempFileName,		"C:\\data\\temp.xml");
			_LIT8(KTempFileName8,		"C:\\data\\temp.xml");
			TRAPD(err,UtilityTools::WriteFileL(*iReceiveData8,0,KTempFileName));
			if(KErrNone != err)
			{
				//UtilityTools::ShowDialog();
				UtilityTools::WriteLogsL(_L("err = %d"),err);
			}
			UtilityTools::WriteLogsL(_L("3"));
			switch(iRequestType)
			{
			case ERequestPage:
				ParseFile(KTempFileName8);
				//ParseData(iReceiveData8);
				break;

			case ERequestConfig:
				iConfigData.Parse(iReceiveData8);
				//ParserConfig(iReceiveData8);
				break;
			}
			delete iReceiveData8;
			iReceiveData8 = NULL;
			//Parse();
			UtilityTools::WriteLogsL(_L("4"));
		}
	}
	else if(aEventDescription.Compare(_L("Transaction Failed")) == 0
		||  aEventDescription.Compare(_L("Error:KErrDisconnected")) == 0)
	{
		iAppView->StopShowWaiting();
	}
}

#include "UtilityTools.h"

void CWapBrowserAppUi::ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex)
{
	/*
	HBufC* buf = HBufC::NewLC(aBodyData.Length());
	buf->Des().Copy(aBodyData);
	RDebug::Print(*buf);
	CleanupStack::PopAndDestroy();
	*/

	//UtilityTools::WriteLogsL(aBodyData);

	if (iReceiveData8)
	{
		iReceiveData8 = iReceiveData8->ReAllocL(iReceiveData8->Length() + aBodyData.Length());
		iReceiveData8->Des().Append(aBodyData);
	}
	else
	{
		iReceiveData8 = HBufC8::NewL(aBodyData.Length());
		iReceiveData8->Des().Copy(aBodyData);
	}

}

void CWapBrowserAppUi::IssueHTTPGetL(const TDesC8& aUri)
{
	/*
	if(NULL == iWebClientEngine)
	{
	iWebClientEngine = CWebClientEngine::NewL(*this);
	}
	iWebClientEngine->IssueHTTPGetL(aUri);*/

}

CWapBrowserAppUi* CWapBrowserAppUi::Static()
{
	return (CWapBrowserAppUi*)CCoeEnv::Static()->AppUi();
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::Parse()
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::Parse"));
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
	CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
	//parser->ParseFile("C:\\Event1.xml");
	//parser->ParseFile("C:\\Default.xml");
	if(parser->ParseFile("C:\\Data\\Default.xml"))
	{
		UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::Parse 1"));
		iAppView->ShowPage(pageBuilder->FetchPage());
		UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::Parse 2"));
	}
	//parser->ParseData("")
	CleanupStack::PopAndDestroy();	//parser
	CleanupStack::PopAndDestroy();	//pageBuilder
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::Parse End"));
}

void CWapBrowserAppUi::ParseData(HBufC8* aBuf)
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData"));
	ASSERT(aBuf);
	//UtilityTools::WriteFileL(*aBuf,0,_L("C:\\test.xml"));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData pageBuilder"));
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData parser"));
	CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData parser->ParseData"));
	TPtr8 ptr(aBuf->Des());
	UtilityTools::WriteLogsL(ptr.Left(128));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData parser->ParseData"));
	const char* data = (const char*)ptr.PtrZ();
	//TRAPD(err,(const char* data = (const char*)aBuf->Des().PtrZ()));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData parser->ParseData err = %d"));
	if(parser->ParseData(data))
	{
		UtilityTools::WriteLogsL(_L("ShowPage"));
		iAppView->ShowPage(pageBuilder->FetchPage());
		UtilityTools::WriteLogsL(_L("ShowPage End"));
	}
	CleanupStack::PopAndDestroy();	//parser
	CleanupStack::PopAndDestroy();	//pageBuilder
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseData End"));
}

void CWapBrowserAppUi::ParseFile(const TDesC8& aFileName)
{
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile"));
	//	ASSERT(aBuf);
	//UtilityTools::WriteFileL(*aBuf,0,_L("C:\\test.xml"));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile pageBuilder"));
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile parser"));
	CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile parser->ParseFile"));
	//TPtr8 ptr(aBuf->Des());
	//UtilityTools::WriteLogsL(ptr.Left(128));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile parser->ParseFile"));
	//const char* data = (const char*)ptr.PtrZ();
	//TRAPD(err,(const char* data = (const char*)aBuf->Des().PtrZ()));
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile parser->ParseFile err = %d"));
	//TPtr8 fileName(aFileName);
	TBuf8<100> fileName = aFileName;
	if(parser->ParseFile((const char*)fileName.PtrZ()))
	{
		UtilityTools::WriteLogsL(_L("ShowPage"));
		iAppView->ShowPage(pageBuilder->FetchPage());
		UtilityTools::WriteLogsL(_L("ShowPage End"));
	}
	CleanupStack::PopAndDestroy();	//parser
	CleanupStack::PopAndDestroy();	//pageBuilder
	UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::ParseFile End"));
}

void CWapBrowserAppUi::RequestPage()
{
	/*
	Parse();
	//parser->ParseFile("C:\\default_.xml");
	/*/	
	RequestPage(_L8("http://music.i139.cn/"));
	//RequestPage(_L8("http://wap.cocobox.cn/index.do"));
	//RequestPage(_L8("index.do"));
	//RequestPage(_L8("portal/wap/menu.do?menuid=212134"));
	//RequestPage(_L8(""));
	//RequestPage(_L8("http://wap.monternet.com"));
	//RequestPage(_L8("http://218.200.244.92/Order?action=4&SessionID=Jm5XLnV7bQ!1999436728!1235646807091&SPID=900636&ServiceID=03205590&SPURL=http://218.206.76.18:8080/dlWap/countDownload.do?	channelId=732&columnId=11769&colResId=265240&resourceFileId=711778&ws=7248343&ctype=100"));
	//*/
}


void CWapBrowserAppUi::RequestPage(const TDesC8& aUrl)
{
	//if(!iIsRequesting)
	{
		UtilityTools::WriteLogsL(_L("CWapBrowserAppUi::RequestPage"));
		//UtilityTools::WriteLogsL(aUrl);
		//HBufC8* buf = HBufC8::NewLC(255);
		HBufC8* buf = HBufC8::NewLC(aUrl.Length());
		//buf->Des().Append(_L8("http://wap.cocobox.cn/"));	
		//buf->Des().Append(_L8("http://wap.gd.monternet.com/"));
		buf->Des().Append(aUrl);
		HTTPEngine().IssueHTTPGetL(*buf);
		CleanupStack::PopAndDestroy();
		//HTTPEngine().IssueHTTPGetL(aUrl);
		iRequestType = ERequestPage;
		iAppView->ShowWaiting();
		iIsRequesting = ETrue;
	}
}

void CWapBrowserAppUi::RequestConfig()
{
	//http://218.16.120.168/miniwap/g.txt
	HTTPEngine().IssueHTTPGetL(_L8("http://59.36.98.140/g.txt"));
	//HTTPEngine().IssueHTTPGetL(_L8("http://59.36.98.140/g.txt"));
	iRequestType = ERequestConfig;
	iAppView->ShowWaiting();
}

void CWapBrowserAppUi::ParserConfig(HBufC8* aBuf)
{

	int firstPos = 0;
	int lastPos = 0;

	TPtrC8 ptr;
	ptr.Set(*aBuf);

	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));	
	TPtrC8 mobile_url = ptr.Mid(firstPos + 1,lastPos - firstPos - 1);
	// 	if(lastPos > firstPos && lastPos > 0 && firstPos > 0)
	// 	{
	// 	}
	TRACE(mobile_url);

	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));		
	TPtrC8 mobile_pre_str = ptr.Mid(firstPos + 1,lastPos - firstPos - 1);
	TRACE(mobile_pre_str);

	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));		
	TPtrC8 mobile_len = ptr.Mid(firstPos + 1,lastPos - firstPos - 1);
	TRACE(mobile_len);

	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	//lastPos = ptr.Find(_L8("0X0A"));	
	TPtrC8 service_url = ptr.Mid(firstPos + 1,ptr.Length() - firstPos - 1);
	TRACE(service_url);
}

CHTTPEngine& CWapBrowserAppUi::HTTPEngine()
{
	if(NULL == iHTTPEngine)
	{
		iHTTPEngine = CHTTPEngine::NewL(*this);
	}
	return *iHTTPEngine;
}

//////////////////////////////////////////////////////////////////////////
/*
#mobile_url=http://wap.gd.monternet.com/portal/wap/menu.do?menuid=212134
mobile_url=http://218.16.120.168/miniwap/m.wml
mobile_pre_str=mobileID=
mobile_len=11
service_url=http://218.16.120.168/miniwap/go.wml
*/

