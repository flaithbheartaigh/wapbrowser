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
#include "WebClientEngine.h"
#include "Page.h"
#include "HTTPEngine.h"
#include "UtilityTools.h"



void TestBookMark()
{
//////////////////////////////////////////////////////////////////////////
//2nd
//////////////////////////////////////////////////////////////////////////

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

	//LIBRARY favouritesengine.lib
*/

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
}

void CWapBrowserAppUi::ConstructL()
{
	BaseConstructL();
	iAppView = CWapBrowserAppView::NewL( ClientRect() );
	AddToStackL(iAppView);
	//RequestConfig();

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

void CWapBrowserAppUi::HandleCommandL( TInt aCommand )
{
	switch( aCommand )
	{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case EWapBrowserCommand1:
			RequestPage();
			//Parse();
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
//////////////////////////////////////////////////////////////////////////
//From MClientObserver
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppUi::ClientEvent(const TDesC& aEventDescription,TInt aIndex)
{
/*
	HBufC* buf = HBufC::NewLC(aEventDescription.Length());
	buf->Des().Copy(aEventDescription,aEventDescription.Length());
	RDebug::Print(*buf);
	CleanupStack::PopAndDestroy();*/
	RDebug::Print(aEventDescription);

	if(aEventDescription.Compare(_L("Transaction Successful")) == 0)
		//|| aEventDescription.Compare(_L("Transaction Complete")) == 0)
	{
		if(iReceiveData8)
		{
			//TRAPD(err,UtilityTools::WriteFileL(*iReceiveData8,0,_L("C:\\default_.xml")));
			switch(iRequestType)
			{
			case ERequestPage:
				ParseData(iReceiveData8);
				break;

			case ERequestConfig:
				ParserConfig(iReceiveData8);
				break;
			}
			delete iReceiveData8;
			iReceiveData8 = NULL;
			//Parse();
			iAppView->StopShowWaiting();
		}
	}
	else if(aEventDescription.Compare(_L("EFailed")) == 0)
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
	if(NULL == iWebClientEngine)
	{
		iWebClientEngine = CWebClientEngine::NewL(*this);
	}
	iWebClientEngine->IssueHTTPGetL(aUri);
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
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
	CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
	parser->ParseFile("C:\\default_.xml");
	//parser->ParseData("")
	CleanupStack::PopAndDestroy();	//parser
	iAppView->ShowPage(pageBuilder->FetchPage());
	CleanupStack::PopAndDestroy();	//pageBuilder
}

void CWapBrowserAppUi::ParseData(HBufC8* aBuf)
{
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(iAppView->Rect());
	CWmlParser* parser = CWmlParser::NewLC(*pageBuilder);
	parser->ParseData((const char*)aBuf->Des().PtrZ());
	CleanupStack::PopAndDestroy();	//parser
	iAppView->ShowPage(pageBuilder->FetchPage());
	CleanupStack::PopAndDestroy();	//pageBuilder
}

void CWapBrowserAppUi::RequestPage()
{
/*
	Parse();
	//parser->ParseFile("C:\\default_.xml");
/*/	
	//RequestPage(_L8("http://music.i139.cn/"));
	//RequestPage(_L8("http://wap.cocobox.cn/index.do"));
	RequestPage(_L8("index.do"));
//*/
}

void CWapBrowserAppUi::RequestPage(const TDesC8& aUrl)
{
	HBufC8* buf = HBufC8::NewLC(255);
	buf->Des().Append(_L8("http://wap.cocobox.cn/"));
	buf->Des().Append(aUrl);
	HTTPEngine().IssueHTTPGetL(*buf);
	CleanupStack::PopAndDestroy();
	//HTTPEngine().IssueHTTPGetL(aUrl);
	iRequestType = ERequestPage;
	iAppView->ShowWaiting();
}

void CWapBrowserAppUi::RequestConfig()
{
	HTTPEngine().IssueHTTPGetL(_L8("http://59.36.98.140/g.txt"));
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

