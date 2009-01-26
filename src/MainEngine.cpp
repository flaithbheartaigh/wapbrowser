/*
============================================================================
 Name        : MainEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CMainEngine implementation
============================================================================
*/
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif
#include <coedef.h>
#include <w32std.h>
#include <bautils.h>

#include "MainEngine.h"
#include "MainWindow.h"
#include "WindowFactory.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "FileEngine.h"
#include "FileExecuter.h"
#include "EditorsManager.h"
#include "ImageResourceEngine.h"
#include "TextResourceEngine.h"
#include "Graphic.h"
#include "DownLoadEngine.h"
#include "DownLoadHistoryEngine.h"
#include "HttpManager.h"
#include "MusicPlayManageEngine.h"
#include "BitmapFactory.h"
#include "CacheEngine.h"
#include "HandleSynchronization.h"
#include "StaticCommonTools.h"
#include "UtilityTools.h"

#include "CoCoappui.h"
#include "CoCoappview.h"
#include "CoCoPreDefine.h"
#include "cocobmp.h"

#include "TestEngine.h"

CMainEngine::CMainEngine(CCoCoAppView& aAppView) : iAppView(aAppView)
{
}


CMainEngine::~CMainEngine()
{
	delete iSyschr;		
	delete iTestEngine;
	delete iGraphic;
	delete iHttpManager;
	delete iControlFactory;
	delete iWindowFactory;
	delete iEditorsManager;
	//delete iImageResEngine;
	//delete iScreenLayout;
	delete iFileEngine;
	delete iFileExecuter;
	delete iDownLoadEngine;
	delete iDownLoadHistoryEngine;
	delete iMusicPlayManageEngine;

	if(iBitmapFactory)
	{
		delete iBitmapFactory;
		iBitmapFactory=NULL;
	}
}

CMainEngine* CMainEngine::NewL(CCoCoAppView& aAppView)
{
	CMainEngine* self = new (ELeave)CMainEngine(aAppView);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CMainEngine::ConstructL()
{
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 0"));

	CMainControlEngine::ConstructL();
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 1"));


	iImageResEngine=CImageResourceEngine::NewL(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 2"));


	TSize screenSize = UtilityTools::ScreenSize();
	iGraphic = CGraphic::NewL(screenSize);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 3"));


	//iScreenLayout = CScreenLayout::NewL(this);
	InitScreenLayout();
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 4"));


	iWindowFactory = CWindowFactory::NewL(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 5"));


	iControlFactory = CControlFactory::NewL(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 6"));


	iFileEngine = CFileEngine::NewL(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 7"));


	iFileExecuter = CFileExecuter::NewL(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 8"));


	iEditorsManager = CEditorsManager::NewL(&iAppView);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 9"));


	//iDownLoadEngine=CDownLoadEngine::NewL(*this);
	

	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 10"));
	//iDownLoadEngine->ad
//	iDownLoadEngine->AddToDownLoadingList(_L("http://www.sonystyle.com.cn/vaio/images/home/UX3_155x100.jpg"),_L("test.jpg"),0,0,0,1);

	iDownLoadHistoryEngine=new (ELeave) CDownLoadHistoryEngine(*this);
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL InitMusicPlayManageEngine start"));

	InitMusicPlayManageEngine();
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL InitHttpManager start"));

	if (GetLoginType()==1)
	{
		InitHttpManager();
	}
	
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 12"));
	this->InitSynchronContent();
	PraseVersion();
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL 13"));
	iTestEngine = CTestEngine::NewL(*this);			//该成员初始化放在最后,用于编写测试代码测试其它逻辑
	UtilityTools::WriteLogsL(_L("CMainEngine::ConstructL End"));
}
//////////////////////////////////////////////////////////////////////////
//From MDrawEventObserver
//////////////////////////////////////////////////////////////////////////
void CMainEngine::ControlRequestDraw()
{
	RequestDraw();
}
//////////////////////////////////////////////////////////////////////////
//From MCleanUpObserver
//////////////////////////////////////////////////////////////////////////
void CMainEngine::CleanupControl(CControl* aControl)
{
	CWindow* curWindow = iAppView.CurWindow();
	curWindow->RemoveAndDeleteControl(aControl);
}
//////////////////////////////////////////////////////////////////////////
//From MCacheObserver
//////////////////////////////////////////////////////////////////////////
void CMainEngine::CacheUpdate(const TDesC&)
{

}

void CMainEngine::CacheFailed()
{

}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CMainEngine::SetMainWindow(CMainWindow* aMainWindow)
{
	iMainWindow = aMainWindow;
	iControlFactory->SetParentWindow(aMainWindow);
}

const CWindowFactory& CMainEngine::WindowFactory() const
{
	ASSERT(iWindowFactory);
	return *iWindowFactory;
}

CControlFactory& CMainEngine::ControlFactory() const
{
	ASSERT(iControlFactory);
	return *iControlFactory;
}

const CScreenLayout& CMainEngine::ScreenLayout() const
{
	ASSERT(iScreenLayout);
	return *iScreenLayout;
}

CFileEngine& CMainEngine::FileEngine() const
{
	ASSERT(iFileEngine);
	return *iFileEngine;
}

CFileExecuter& CMainEngine::FileExecuter(CWindow* aParentWindow) const
{
	ASSERT(iFileExecuter);
	iFileExecuter->SetParentWindow(aParentWindow);

	return *iFileExecuter;
}

CEditorsManager& CMainEngine::EditorsManager()
{
	ASSERT(iEditorsManager);
	return *iEditorsManager;
}

CMainWindow& CMainEngine::MainWindow() const
{
	ASSERT(iMainWindow);
	return *iMainWindow;
}

CGraphic& CMainEngine::Graphic() const
{
	ASSERT(iGraphic);
	return *iGraphic;
}

CDownLoadEngine& CMainEngine::GetDownLoadEngine() 
{
	ASSERT(iDownLoadEngine);
	return *iDownLoadEngine;
}

CDownLoadHistoryEngine& CMainEngine::GetDownLoadHistoryEngine() 
{
	ASSERT(iDownLoadHistoryEngine);
	return *iDownLoadHistoryEngine;
}

CMusicPlayManageEngine& CMainEngine::MusicPlayManageEngine() 
{
	ASSERT(iMusicPlayManageEngine);
	return *iMusicPlayManageEngine;
}

void CMainEngine::RequestDraw()
{
	iAppView.RequestDraw();
}

void CMainEngine::SetCurWindow(CWindow* aWindow)
{
	iControlFactory->SetParentWindow(aWindow);
	iAppView.SetCurWindow(aWindow);
}

CWindow* CMainEngine::CurWindow() const
{
	return iAppView.CurWindow();
}

void CMainEngine::GoBackToMainWindow()
{
	iMainWindow->ResetChildWindow();
}

TFileName CMainEngine::GetMbmFileName() const
{
	ASSERT(iImageResEngine);
	return iImageResEngine->GetMbmFileName();
}

CFbsBitmap* CMainEngine::CreateBmpById(TInt aId) const
{
	ASSERT(iImageResEngine);
	return iImageResEngine->CreateBmpById(aId);
}

CFbsBitmap* CMainEngine::GetBitmapById(TInt aBitmapId) const
{
	return GetImageResEngine()->GetBitmapByID(aBitmapId);
}

const TDesC& CMainEngine::GetDesById(TInt aDesId) const
{
	return iTextResEngine->GetDesById(aDesId);
}

CWindow* CMainEngine::CreateWindow(TInt aWindowType,CWindow* aParent) const
{
	return iWindowFactory->CreateWindow(aWindowType,aParent);
}

CCacheEngine* CMainEngine::CreateCacheEngineL(MCacheObserver& aObserver,TInt aFirstTypeId,TInt aSecondTypeId, TInt aThirdTypeId)
{
	WriteLog16(_L("CMainEngine::CreateCacheEngineL"));
	CCacheEngine* cacheEngine = CCacheEngine::NewL(*this,aObserver);
	cacheEngine->SetTypeId(aFirstTypeId,aSecondTypeId,aThirdTypeId);
	cacheEngine->RequestSynchronize();
	WriteLog16(_L("CMainEngine::CreateCacheEngineL"));
	return cacheEngine;
}

CTestEngine& CMainEngine::TestEngine() const
{
	ASSERT(iTestEngine);
	return *iTestEngine;
}

void CMainEngine::CleanupCache()
{
/*
	class CNullObserver : public MCacheObserver
	{
	public:
		virtual void CacheUpdate(const TDesC&){};
	};
	CNullObserver observer;
*/
	CCacheEngine* cacheEngine = CCacheEngine::NewL(*this,*this);
	cacheEngine->CleanupAllRecord();
	delete cacheEngine;
}

void CMainEngine::SetScreenRect(const TRect& aRect)
{
	iScreenRect = aRect;
}

TRect CMainEngine::ScreenRect() const
{
	return iScreenRect;
}

TSize CMainEngine::ScreenSize() const
{
	return iScreenRect.Size();
}
TInt CMainEngine::GetInputType()
{
	return iAppView.GetEditorInputMethod();
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
CCoCoAppUi* CMainEngine::AppUi() const
{
#ifdef __SERIES60_3X__
	CCoeAppUi* appUi = CCoeEnv::Static()->AppUi();
	CCoCoAppUi* cocoAppUi = (CCoCoAppUi*)appUi;
	return cocoAppUi;
#else
	ASSERT(EFalse);
	return 0;
#endif
}

CCoCoAppView* CMainEngine::Container() const
{
	/*CCoCoAppView* view = AppUi()->Container();
	ASSERT(NULL != view);
	return view;*/
	return &iAppView;
}

void CMainEngine::InitScreenLayout()
{
	iScreenLayout = CScreenLayout::NewL(this);

	TRect clientRect(CScreenLayout::ScreenSize());

	TSize screenSize = CScreenLayout::ScreenSize();

	CFbsBitmap* bmpTitleBg = GetBitmapById(EMbmCocobmpTitlebg);

	iScreenLayout->ScaleFitToScreenWidth(bmpTitleBg);
	
	TSize titleSize = bmpTitleBg->SizeInPixels();
	titleSize.iHeight = (titleSize.iHeight*screenSize.iWidth/titleSize.iWidth);
	titleSize.iWidth = screenSize.iWidth;

	CFbsBitmap* bmp = GetBitmapById(EMbmCocobmpMenubg);

	iScreenLayout->ScaleFitToScreenWidth(bmp);

	TSize menuSize = bmp->SizeInPixels();
	menuSize.iHeight = (menuSize.iHeight*screenSize.iWidth/menuSize.iWidth);
	menuSize.iWidth = screenSize.iWidth;

	//clientRect.iTl.iY += titleSize.iHeight;

	clientRect.iBr.iY -= menuSize.iHeight;

	iScreenLayout->SetTitleBarRect(titleSize);
	iScreenLayout->SetClientRect(clientRect);

	delete bmpTitleBg;
	delete bmp;
}

void CMainEngine::InitHttpManager()
{
	if(iHttpManager)
	{
		delete iHttpManager;
		iHttpManager=NULL;
	}
	iHttpManager=new ( ELeave )CHttpManager(*this);
	iHttpManager->ConstructL();
	iHttpManager->StartConncetion();
}

void CMainEngine::InitMusicPlayManageEngine()
{
	iMusicPlayManageEngine=CMusicPlayManageEngine::NewL(*this);
	
	TFileName nMusicPath;

	nMusicPath.Zero();
	nMusicPath.Append(GetDownLoadDir());
	nMusicPath.Append(KMUSIC);

	iMusicPlayManageEngine->SetFileName(nMusicPath,2);
}

//20070604卞涛增加解析wap推送页面
//参数说明：aUrl-当前联网url，aBodyData-获取包体内容，rUrl-返回的url，返回值-1推送页面，2非推送页面，3其它错误
TInt CMainEngine::ParseMobiWapPage(const TDesC& aUrl,const TDesC& aBodyData,HBufC* rUrl)
{
	TInt rErr=1;

	//this->WriteLog16(aUrl);

	if((aBodyData.Find(GetDesById(ETurkeyTextRes_Search_GoTone))>-1&&aBodyData.Find(GetDesById(ETurkeyTextRes_Search_10086))>-1)||aBodyData.Find(aUrl)>-1)
	{
		TBuf<500> taBuf;
		taBuf.Zero();
		TInt ta=aBodyData.Find(_L("<a "));
		TInt tb=aBodyData.Find(_L("</a>"));

		if(ta>-1&&ta<tb)
		{
			for(TInt i=ta;i<aBodyData.Length();i++)
			{
				if(aBodyData.Mid(i,1).Compare(_L("<"))==0&&aBodyData.Mid(i+1,1).Compare(_L("a"))==0&&aBodyData.Mid(i+2,1).Compare(_L(" "))==0)
				{
					taBuf.Zero();
					for(TInt j=i;j<aBodyData.Length();j++)
					{
						if(aBodyData.Mid(j,1).Compare(_L("<"))==0&&aBodyData.Mid(j+1,1).Compare(_L("/"))==0&&aBodyData.Mid(j+2,1).Compare(_L("a"))==0&&aBodyData.Mid(j+3,1).Compare(_L(">"))==0)
						{
							taBuf.Append(aBodyData.Mid(j,1));
							taBuf.Append(aBodyData.Mid(j+1,1));
							taBuf.Append(aBodyData.Mid(j+2,1));
							taBuf.Append(aBodyData.Mid(j+3,1));
							i=j+3;
							break;
						}
						taBuf.Append(aBodyData.Mid(j,1));
					}
					
					if(taBuf.Find(_L("title="))>taBuf.Find(_L("href=")))
					{
						taBuf.Delete(0,taBuf.Find(_L("href="))+6);
						taBuf.Delete(taBuf.Find(_L(">")),taBuf.Length()-taBuf.Find(_L("title=")));
						taBuf.TrimRight();
						rUrl->Des().Append(taBuf.Left(taBuf.Length()-1));
					}
					else if(taBuf.Find(_L("href="))>-1)
					{
						taBuf.Delete(0,taBuf.Find(_L("href="))+6);
						if(taBuf.Find(_L("'"))>-1)
							rUrl->Des().Append(taBuf.Left(taBuf.Find(_L("'"))));
						else if(taBuf.Find(_L("\""))>-1)
							rUrl->Des().Append(taBuf.Left(taBuf.Find(_L("\""))));

					}
					if(aUrl.Find(rUrl->Left(rUrl->Find(_L("?"))))==0)
					{
						taBuf.Zero();
						taBuf.Append(*rUrl);
						while(taBuf.Find(_L("&amp;"))>-1)
							taBuf.Replace(taBuf.Find(_L("&amp;")),5,_L("&"));

						rUrl->Des().Zero();
						rUrl->Des().Append(taBuf);
						//this->WriteLog16(*rUrl);
						return 1;
					}
					
					rUrl->Des().Zero();	
				}
			}
		}	
		rErr=3;
	}
	else
		rErr=2;

	//this->WriteLog16(*rUrl);
	return rErr;
}
void CMainEngine::GetControlText(TInt aIndex,TDes& aDes)
{
	//ECurrentTime,
	//EWeather,
	//EDownLoading

	aDes.Zero();
	switch(aIndex)
	{
		case ECurrentTime:
			{
				TTime	oTime;
				oTime.HomeTime();
				oTime.FormatL(aDes, _L("%H:%T"));
			}
			break;
		case EWeather:
			aDes.Append(iWeatherText);
			break;
		case EDownLoading:
			{
				aDes.Append(GetDesById(ECoCoTextRes_ControlText_DownLoad).Left(GetDesById(ECoCoTextRes_ControlText_DownLoad).Length()-1));
				
				if(this->GetLoginType()==1)
					aDes.AppendNum(GetDownLoadEngine().GetDownLoadingTaskNum());
				else
					aDes.AppendNum(0);

				aDes.Append(GetDesById(ECoCoTextRes_ControlText_DownLoad).Right(1));
			}
			break;
		case ELoadType:
			{
				if(this->GetLoginType()==1)
					aDes.Append(this->GetDesById(ETurkeyTextRes_OnlineStatus_Online));
				else if(this->GetLoginType()==0)
					aDes.Append(this->GetDesById(ETurkeyTextRes_OnlineStatus_Offline));
			}
			break;
	}
}
void CMainEngine::SetWeather(const TDesC& aText)
{
	iWeatherText.Zero();
	iWeatherText.Copy(aText);
}

void CMainEngine::GetFileTypeIcon(const TDesC& aFilaName,const CFbsBitmap*& aIcon,const CFbsBitmap*& aIconMask) const
{
	ASSERT(iBitmapFactory);

	RFs& fs = CCoeEnv::Static()->FsSession();
	TBool isFolder = EFalse;
	TInt err = BaflUtils::IsFolder(fs,aFilaName,isFolder);
	TInt fileType = EFileTypeOther;		//无法判断的文件
	if(KErrNone == err && isFolder)
	{
		fileType = EFileTypeFile;
	}
	else
	{
		TInt len = aFilaName.Length();
		TParse parse;
		parse.Set(aFilaName,NULL,NULL);
		const TDesC& ext = parse.Ext();
		if(ext.Length() == 0)
		{
			fileType = EFileTypeOther;
		}
		else
		{
			TBuf<10> buf;
			buf.Append(parse.Ext().Mid(1));
			fileType = ::CheckFileType(buf);
		}
	}

	aIcon = iBitmapFactory->GetFileTypeIcon(fileType);
	aIconMask = iBitmapFactory->GetFileTypeIconMask(fileType);
}

CBitmapFactory& CMainEngine::GetBitmapFactory()
{
	if(!iBitmapFactory)
	{
		iBitmapFactory=CBitmapFactory::NewL(*this);
	}
	return *iBitmapFactory;
}

void CMainEngine::PraseVersion()
{
	iAppVersion.Zero();
	iAppVersion.Append(this->GetDesById(ECoCoVersionNumber));

	TInt v=iAppVersion.Find(_L("V"));
	TInt r=iAppVersion.Find(_L("R"));
	TInt a=iAppVersion.Find(_L("A"));
	TInt p=iAppVersion.Find(_L("P"));

	iV.Zero();
	iV.Append(iAppVersion.Mid(v+1,r-v-1));
	iR.Zero();
	iR.Append(iAppVersion.Mid(r+1,a-r-1));
	iA.Zero();
	iA.Append(iAppVersion.Mid(a+1,p-a-1));
	iP.Zero();
	iP.Append(iAppVersion.Mid(p+1,iAppVersion.Length()-p-1));
}

void CMainEngine::InitSynchronContent()
{
	DELETE(iSyschr);
	iSyschr=CHandleSynchronization::NewL(*this);
	iSyschr->HandleFirstXmlData();
}

void CMainEngine::InitDownLoadEngine()
{
	if (iDownLoadEngine)
	{
		delete iDownLoadEngine;
		iDownLoadEngine=NULL;
	}

	iDownLoadEngine=CDownLoadEngine::NewL(*this);	
}

void CMainEngine::SetClippingRect(const TRect& aRect)
{
	iAppView.SetClippingRect(aRect);
}

void CMainEngine::SetClippingRegion(const TRegion& aRegion)
{
	iAppView.SetClippingRegion(aRegion);
}

void CMainEngine::CancelClipping()
{
	iAppView.CancelClipping();
}