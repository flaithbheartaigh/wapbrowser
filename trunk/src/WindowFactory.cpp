/*
============================================================================
 Name        : WindowFactory.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWindowFactory implementation
============================================================================
*/

#include "WindowFactory.h"
#include "Window.h"
//#include "MainWindow.h"
#include "StartWindow.h"
#include "EndWindow.h"
#include "HelpWindow.h"
#include "DownloadManagerWindow.h"
#include "FileManagerWindow.h"
#include "RecommendWindow.h"
#include "UserAdviseWindow.h"

#include "MainSearchView.h"
#include "SearchResultView.h"
#include "SearchLocalResultView.h"
#include "SearchResultOutView.h"
#include "PreDefine.h"
#include "TypeDefine.h"

//add by hesanyuan
#include "FriendLink.h"
#include "ResultView.h"
#include "NewestDownload.h"
#include "SutraDownload.h"

//add by wangxiaohua
#include "SystemManageWindow.h"
#include "MainHelpWindow.h"
#include "DisplayHelpWindow.h"

#include "DownloadHistorySettingWindow.h"
#include "DownloadDirSettingWindow.h"
#include "InfoDetailCacheSettingWindow.h"
#include "ContentListCacheSettingWindow.h"
#include "CacheCleanWindow.h"
#include "DownloadVoiceSettingWindow.h"
#include "EnterModeWindow.h"
#include "JoinAccountSettingWindow.h"
#include "MultiThreadsDownloadSettingWindow.h"
#include "SystemMessageWindow.h"
#include "TraficStatisticWindow.h"

//add by zhanghui
#include "BrowserPicWindow.h"
#include "BrowserBookWindow.h"
#include "BrowserVideoWindow.h"
#include "BrowserMusicWindow.h"

CWindowFactory::CWindowFactory(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
{
}


CWindowFactory::~CWindowFactory()
{ 
}

CWindowFactory* CWindowFactory::NewL(CMainEngine& aMainEngine)
{
	CWindowFactory* self = new (ELeave)CWindowFactory(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CWindowFactory::ConstructL()
{

}

CWindow* CWindowFactory::CreateWindow(TInt aWindowType,CWindow* aParent) const
{
	CWindow* window = NULL;
	switch(aWindowType)
	{
	case EMainWindow:
		//CMainWindow在view中创建
		//window = CMainWindow::NewL()
		break;

	case EStartWindow:
		window = CStartWindow::NewL(aParent,iMainEngine);
		break;

	case EEndWindow:
		window = CEndWindow::NewL(aParent,iMainEngine);
		break;

	case EHelpWindow:
		window = CHelpWindow::NewL(aParent,iMainEngine);
		break;
	case EMainSearchView:
		window=CMainSearchView::NewL(aParent,iMainEngine);
		break;

	case EDownloadManagerWindow:
		window = CDownloadManagerWindow::NewL(aParent,iMainEngine);
		break;

	case EDownloadedWindow:
		window = CFileManagerWindow::NewL(aParent,iMainEngine);
		break;

	case ERecommendWindow:
		window = CRecommendWindow::NewL(aParent,iMainEngine);
		break;

	case EUserAdviseWindow:
		window = CUserAdviseWindow::NewL(aParent,iMainEngine);
		break;

	case EResultWindow://分类下载
		window = CResultView::NewL(aParent,iMainEngine);
		break;
	case EFriendLinkView://友链
		window = CFriendLink::NewL(aParent,iMainEngine);
		break;
	case ENewestdownView://最新下载
		window = NewestDownload::NewL(aParent,iMainEngine);
		break;
	case ESutraDownloadView://精品下载.
		window = CSutraDownload::NewL(aParent,iMainEngine);
		break;

	case ESystemManageWindow://系统管理
		window = CSystemManageWindow::NewL(aParent,iMainEngine);
		break;
	case EDownloadHistorySettingWindow://下载历史记录设置
		window = CDownloadHistorySettingWindow::NewL(aParent,iMainEngine);
		break;
	case EDownloadDirSettingWindow://下载保存目录设置
		window = CDownloadDirSettingWindow::NewL(aParent,iMainEngine);
		break;
	case EInfoDetailCacheSettingWindow://信息详情缓存设置
		window = CInfoDetailCacheSettingWindow::NewL(aParent,iMainEngine);
		break;
	case EContentListCacheSettingWindow://内容列表缓存设置
		window = CContentListCacheSettingWindow::NewL(aParent,iMainEngine);
		break;
	case ECacheCleanWindow://清除缓存
		window = CCacheCleanWindow::NewL(aParent,iMainEngine);
		break;

	case EDownloadVoiceSettingWindow://下载提示音设置
		window = CDownloadVoiceSettingWindow::NewL(aParent,iMainEngine);
		break;
	case EEnterModeWindow://登陆方式
		window = CEnterModeWindow::NewL(aParent,iMainEngine);
		break;
	case EJoinAccountSettingWindow://联网帐号设置
		window = CJoinAccountSettingWindow::NewL(aParent,iMainEngine);
		break;
	case EMultiThreadsDownloadSettingWindow://多线程下载设置
		window = CMultiThreadsDownloadSettingWindow::NewL(aParent,iMainEngine);
		break;
	case ESystemMessageWindow://系统消息
		window = CSystemMessageWindow::NewL(aParent,iMainEngine);
		break;
	case ETraficStatisticWindow://联网流量统计
		window = CTraficStatisticWindow::NewL(aParent,iMainEngine);
		break;
	case EMainHelpWindow://帮助主界面
		window = CMainHelpWindow::NewL(aParent,iMainEngine);
		break;
	case EDisplayHelpWindow://显示具体帮助信息的界面
		window = CDisplayHelpWindow::NewL(aParent,iMainEngine);
		break;

	case EOpenFileWindow_Pic:
		window=CBrowserPicWindow::NewL(aParent,iMainEngine);
		break;

	case EOpenFileWindow_Book:
		window=CBrowserBookWindow::NewL(aParent,iMainEngine);
		break;

	case EOpenFileWindow_Music:
		window=CBrowserMusicWindow::NewL(aParent,iMainEngine);
		break;

	case EOpenFileWindow_Video:
		window=CBrowserVideoWindow::NewL(aParent,iMainEngine);
		break;
	}
	ASSERT(NULL != window);
	return window;
}
CWindow* CWindowFactory::CreateSearchLocalResultView(TInt aWindowType,CWindow* aParent,TInt aIndex,const TDesC& aTitle) const
{
	CWindow* window = NULL;
	switch(aWindowType)
	{
	case ESearchLocalResultView:
		window=CSearchLocalResultView::NewL(aParent,iMainEngine,aIndex,aTitle);
		break;
	}
	ASSERT(NULL != window);
	return window;
}
CWindow* CWindowFactory::CreateSearchResultView(TInt aWindowType,CWindow* aParent,TInt aIndex,const TDesC8& aUrl,const TDesC& aTitle) const
{
	CWindow* window = NULL;
	switch(aWindowType)
	{
	case ESearchResultView:
		window=CSearchResultView::NewL(aParent,iMainEngine,aIndex,aUrl,aTitle);
		break;
	case ESearchOutResultView:
		window=CSearchResultOutView::NewL(aParent,iMainEngine,aIndex,aUrl,aTitle);
		break;
	}
	ASSERT(NULL != window);
	return window;
}