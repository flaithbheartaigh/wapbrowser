/*
============================================================================
 Name        : ImageEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CImageEngine implementation
============================================================================
*/
#include "ImageEngine.h"
#include "WapEngine.h"
#include "WapHttpEngine.h"
#include "Widgets.h"
#include "WapBrowserappui.h"
#include "WapBrowserappview.h"
#include "UtilityTools.h"

_LIT(KTempFileName,			"C:\\data\\temp.img");

CImageEngine::CImageEngine(CWapEngine& aWapEngine)
	: iWapEngine(aWapEngine)
{
}

CImageEngine::~CImageEngine()
{
	CancelAllSession();
	delete iWapHttpEngine;
}

CImageEngine* CImageEngine::NewLC(CWapEngine& aWapEngine)
{
	CImageEngine* self = new (ELeave)CImageEngine(aWapEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CImageEngine* CImageEngine::NewL(CWapEngine& aWapEngine)
{
	CImageEngine* self=CImageEngine::NewLC(aWapEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CImageEngine::ConstructL()
{
}
//////////////////////////////////////////////////////////////////////////
//MWapHttpEngineObserver
//////////////////////////////////////////////////////////////////////////
void CImageEngine::HttpOk(const TDesC8& aData)
{
	UtilityTools::WriteLogsL(_L("CImageEngine::HttpOk"));
	iIsRequesting = FALSE;
	UpdatePic(aData);
//	RequestNextPic();

// 	UtilityTools::WriteFileL(aData,0,KTempFileName);
// 
// 
	// 	UtilityTools::DeleteFile(KTempFileName);
	UtilityTools::WriteLogsL(_L("CImageEngine::HttpOk End"));
}

void CImageEngine::HttpEmpty()
{
	iIsRequesting = FALSE;
}

void CImageEngine::HttpFailed()
{
	iIsRequesting = FALSE;
}

//////////////////////////////////////////////////////////////////////////
//public:
//////////////////////////////////////////////////////////////////////////
void CImageEngine::AddPicUrl(const TDesC8& aPicUrl,CPictureWidget* aPictureWidget)
{
	_LIT8(KHttpPrefix8,		"http://");
	HBufC8* url = NULL;
	int pos = aPicUrl.Find(KHttpPrefix8);	//大小写敏感,
	TBool urlValid = TRUE;
	if(-1 == pos)
	{
		const TDesC8& httpPrefix8(KHttpPrefix8);
		const TDesC8& referer =  iWapEngine.Referer();
		int httpPrefixLength = httpPrefix8.Length();

		if(referer.Length() <= httpPrefixLength)
		{
			urlValid = FALSE;
		}
		else
		{
			TPtrC8 location;
			location.Set(referer.Mid(httpPrefixLength));
			int pos = location.LocateReverse(L'/');
			if(-1 != pos)
			{
				int len = httpPrefix8.Length() + pos;
				location.Set(referer.Left(len));
			}
			int len = aPicUrl.Length() + location.Length() + 1;
			url = HBufC8::NewL(len);
			url->Des().Append(location);
			url->Des().Append(_L("/"));
			url->Des().Append(aPicUrl);
		}
	}
	else
	{
		url = aPicUrl.Alloc();
	}
	if(urlValid)
	{
		CPicUrlPair* pair = new CPicUrlPair;
		pair->iUrl = url;
		pair->iWidget = aPictureWidget;
		iPicUrlPairs.Append(pair);
	}
	//iPictureUrls.Append(url);
}

#undef TRACE
#define TRACE	UtilityTools::WriteLogsL

void CImageEngine::Remove(CPictureWidget* aPictureWidget)
{
	TRACE(_L("CImageEngine::Remove aPictureWidget:%d"),aPictureWidget);
	if(iCurPicUrlPair && aPictureWidget == iCurPicUrlPair->iWidget)
	{
		iCurPicUrlPair->iWidget = NULL;
	}
	else
	{
		for (int i = 0 ; i < iPicUrlPairs.Count() ; ++i)
		{
			CPicUrlPair* pair = iPicUrlPairs[i];
			if(aPictureWidget == pair->iWidget)
			{
				TRACE(_L("CImageEngine::Remove pair->iWidget:%d"),pair->iWidget);
				pair->iWidget = NULL;
				//iPicUrlPairs.Remove(i);
				break;
			}
		}		
	}
}

void CImageEngine::CancelAllSession()
{
/*
	//取消联网
	//清空队列

	//iPictureUrls.ResetAndDestroy();
*/
	if(iWapHttpEngine)
	{
		iWapHttpEngine->CancelTransaction();
	}
	iPicUrlPairs.ResetAndDestroy();
}

void CImageEngine::StartPicSession()
{
	RequestNextPic();
}

void CImageEngine::RequestNextPic()
{
	//if(!iIsRequesting && iPictureUrls.Count())
	if(!iIsRequesting && iPicUrlPairs.Count())
	{
		//ASSERT(NULL == iCurPicUrlPair);		//TODO:不应该屏蔽
		iCurPicUrlPair = iPicUrlPairs[0];	
		iPicUrlPairs.Remove(0);	
		//HBufC8* url = iCurPicUrlPair->iUrl;
		//CleanupStack::PushL(url);
		RequestPic(*iCurPicUrlPair->iUrl);
		//CleanupStack::PopAndDestroy();
	}
}

void CImageEngine::RequestPic(const TDesC8& aPicUrl)
{
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	iIsRequesting = TRUE;
	iWapHttpEngine->IssueHTTPGetL(aPicUrl);
}

void CImageEngine::UpdatePic(const TDesC8& aData)
{
	//ASSERT(iCurPicUrlPair && iCurPicUrlPair->iWidget);
	ASSERT(iCurPicUrlPair);
	const TDesC8& url = *iCurPicUrlPair->iUrl;
	int pos = url.LocateReverse(L'/');
	TPtrC8 fileName;
	fileName.Set(url.Mid(pos + 1));
	const TDesC& path(_L("C:\\Data\\"));
	int fileFullLen = path.Length() + fileName.Length();
	HBufC* fileFullName = HBufC::NewLC(fileFullLen);
	fileFullName->Des().Copy(fileName);
	fileFullName->Des().Insert(0,path);

	if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),*fileFullName))
	{
		SavePic(aData,*fileFullName);
	}
	if(iCurPicUrlPair->iWidget)
	{
		TRACE(_L("CImageEngine::UpdatePic iCurPicUrlPair:%d iWidget:%d"),iCurPicUrlPair,iCurPicUrlPair->iWidget);
		iCurPicUrlPair->iWidget->SetImage(*fileFullName);
	}

	CleanupStack::PopAndDestroy(fileFullName);
	delete iCurPicUrlPair;
	iCurPicUrlPair = NULL;
	RequestNextPic();
	//通知请求图片的widget
	//UpdateWindow();
}

void CImageEngine::SavePic(const TDesC8& aData,const TDesC& aFileName)
{
	UtilityTools::WriteFileL(aData,0,aFileName);
}

void CImageEngine::UpdateWindow()
{
	iWapEngine.UpdateWindow();
}