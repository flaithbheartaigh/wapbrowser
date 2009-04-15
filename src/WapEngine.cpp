/*
============================================================================
 Name        : WapEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWapEngine implementation
============================================================================
*/
#include "WapEngine.h"
#include "ImageEngine.h"
#include "PageBuilder.h"
#include "WmlParser.h"
#include "WapBrowserappui.h"
#include "WapBrowserappview.h"
#include "WapHttpEngine.h"
#include "UtilityTools.h"

_LIT(KTempFileName,			"C:\\data\\temp.xml");
_LIT8(KTempFileName8,		"C:\\data\\temp.xml");

CWapEngine::CWapEngine()
{
	// No implementation required
}

CWapEngine::~CWapEngine()
{	
	delete iWapHttpEngine;
	delete iImageEngine;
}

CWapEngine* CWapEngine::NewLC()
{
	CWapEngine* self = new (ELeave)CWapEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CWapEngine* CWapEngine::NewL()
{
	CWapEngine* self=CWapEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CWapEngine::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//MWapHttpEngineObserver
//////////////////////////////////////////////////////////////////////////
void CWapEngine::HttpOk(const TDesC8& aData)
{
	ASSERT(iAppView);
	iIsRequesting = FALSE;
	iAppView->StopShowWaiting();

	//TODO:test
	UtilityTools::DeleteFile(KTempFileName);

	UtilityTools::WriteFileL(aData,0,KTempFileName);
	ParseFile(KTempFileName8);
	//UtilityTools::DeleteFile(KTempFileName);
}

void CWapEngine::HttpEmpty()
{
	ASSERT(iAppView);
	iIsRequesting = FALSE;
	iAppView->StopShowWaiting();
}

void CWapEngine::HttpFailed()
{
	ASSERT(iAppView);
	iIsRequesting = FALSE;
	iAppView->StopShowWaiting();
}
//////////////////////////////////////////////////////////////////////////
//public:
//////////////////////////////////////////////////////////////////////////
void CWapEngine::RequestPageL(const TDesC8& aUrl)
{
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	if(!iIsRequesting)
	{
		SetReferer(aUrl);
		Reset();
		if(iAppView)
		{
			iAppView->ShowWaiting();
		}
		iIsRequesting = TRUE;
		iWapHttpEngine->IssueHTTPGetL(aUrl);
	}
}

void CWapEngine::RequestPageL(const TDesC8& aUrl,const TDesC8& aBody)
{
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	if(!iIsRequesting)
	{
		HBufC8* referer = iReferer;
		iReferer = NULL;
		CleanupStack::PushL(referer);
		SetReferer(aUrl);
		Reset();
		if(iAppView)
		{
			iAppView->ShowWaiting();
		}
		iIsRequesting = TRUE;
		iWapHttpEngine->IssueHTTPPostL(aUrl,aBody,referer?*referer:_L8(""));
		CleanupStack::PopAndDestroy(referer);
	}
}

void CWapEngine::SetReferer(const TDesC8& aReferer)
{
	HBufC8* referer = aReferer.AllocL();	
	if(iReferer)
	{
		delete iReferer;
	}
	iReferer = referer;
}

const TDesC8& CWapEngine::Referer() const
{
	if(iReferer)
	{
		return *iReferer;
	}
	return KNullDesC8;
}

void CWapEngine::Reset()
{
	if(iImageEngine)
	{
		iImageEngine->CancelAllSession();
	}
}

void CWapEngine::UpdateWindow()
{
	if(iAppUi)
	{
		iAppUi->UpdateWindow();
	}
	//CWapBrowserAppUi::Static()->UpdateWindow();
}

void CWapEngine::SetAppUi(CWapBrowserAppUi* aAppUi)
{
	iAppUi = aAppUi;
}

void CWapEngine::SetAppView(CWapBrowserAppView* aAppView)
{
	iAppView = aAppView;
}

void CWapEngine::ParseData(HBufC8* aBuf)
{
	ASSERT(iAppView);
	ASSERT(aBuf);
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(*this,iAppView->Rect());
	CWmlParser* parser = CWmlParser::NewLC(*this,*pageBuilder);
	TPtr8 ptr(aBuf->Des());
	const char* data = (const char*)ptr.PtrZ();
	if(parser->ParseData(data))
	{
		iAppView->ShowPage(pageBuilder->FetchPage());
		ImageEngine().StartPicSession();
	}
	CleanupStack::PopAndDestroy();	//parser
	CleanupStack::PopAndDestroy();	//pageBuilder
}

void CWapEngine::ParseFile(const TDesC8& aFileName)
{
	ASSERT(iAppView);
	CPageBuilder* pageBuilder = CPageBuilder::NewLC(*this,iAppView->Rect());
	CWmlParser* parser = CWmlParser::NewLC(*this,*pageBuilder);
	TBuf8<100> fileName = aFileName;
	if(parser->ParseFile((const char*)fileName.PtrZ()))
	{
		iAppView->ShowPage(pageBuilder->FetchPage());
		ImageEngine().StartPicSession();
	}
	CleanupStack::PopAndDestroy();	//parser
	CleanupStack::PopAndDestroy();	//pageBuilder
}

void CWapEngine::TestParse()
{
	ParseFile(_L8("C:\\Data\\Default.xml"));
}

CImageEngine& CWapEngine::ImageEngine()
{
	if(NULL == iImageEngine)
	{
		iImageEngine = CImageEngine::NewL(*this);
	}
	return *iImageEngine;
}