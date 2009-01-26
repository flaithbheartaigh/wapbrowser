/*
============================================================================
 Name        : SystemManageWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CSystemManageWindow implementation
============================================================================
*/

#include "SystemManageWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "TypeDefine.h"
#include "PopUpMenu.h"
#include "BitmapFactory.h"
#include "PictureEngine.h"
#include "ListBoxItem.h"
#include "ListBox.h"
#include "StaticCommonTools.h"


#include "FileReadByRow.h"
enum TEventCommand
{
	EMenuHelp,
	EMenuOk
};

CSystemManageWindow::CSystemManageWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EHelpWindow,aParent,aMainEngine)
{
}


CSystemManageWindow::~CSystemManageWindow()
{
}

CSystemManageWindow* CSystemManageWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSystemManageWindow* self = new (ELeave)CSystemManageWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSystemManageWindow* CSystemManageWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSystemManageWindow* self=CSystemManageWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSystemManageWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iNaviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane();
	iNaviPane->AppendItem((*iRes->iItem)[ESystemManageRes_SoftwareSetting]);//软件设置
	iNaviPane->AppendItem((*iRes->iItem)[ESystemManageRes_NetworkSetting]);//网络设置
	iNaviPane->AppendItem((*iRes->iItem)[ESystemManageRes_SystemInfo]);//系统信息
	iNaviPane->SetObserver(this);//设置观察器

	iListBox=iMainEngine.ControlFactory().CreateDefaultListBoxL();

	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(EFileTypeOther);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(EFileTypeOther);

	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_DownloadHistory_Setting],iIconBitmap,iIconBitmapMask));
	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_DownloadDir_Setting],iIconBitmap,iIconBitmapMask));
	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_InfoDetailCacheSetting],iIconBitmap,iIconBitmapMask));
	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_ContentListCacheSetting],iIconBitmap,iIconBitmapMask));
	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_CacheClean],iIconBitmap,iIconBitmapMask));
	iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_Download_VoiceSetting],iIconBitmap,iIconBitmapMask));

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSystemManageWindow::DoActivateL()
{
	
}

void CSystemManageWindow::DoDeactivate()
{
	
}

void CSystemManageWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CSystemManageWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice1:
		GoBackToParentWindow();
		break;
	case EKeyDevice0://左软件,弹出菜单	
		InitPopUpMenu();
		break;
		
	case EKeyDevice3://ok键	，进入具体的模块
		switch(iNaviPane->CurIndex())
		{
		case 0://软件设置
			switch(iListBox->CurItemIndex())
			{

			case 0://下载历史记录设置:
				CreateChildWindow(EDownloadHistorySettingWindow);
				break;
			case 1://下载保存目录设置
				CreateChildWindow(EDownloadDirSettingWindow);
				break;
			case 2://信息详情缓存设置
				CreateChildWindow(EInfoDetailCacheSettingWindow);
				break;
			case 3://内容列表缓存设置
				CreateChildWindow(EContentListCacheSettingWindow);
				break;

			case 4://清除缓存设置
				CreateChildWindow(ECacheCleanWindow);
				break;
			case 5://下载提示音设置
				CreateChildWindow(EDownloadVoiceSettingWindow);
				break;
			default:
				break;
			}
			break;
		case 1://网络设置
			switch(iListBox->CurItemIndex())
			{
			case 0://登陆方式
				CreateChildWindow(EEnterModeWindow);
				break;
			case 1://联网帐号设置
				CreateChildWindow(EJoinAccountSettingWindow);
				break;
			case 2://多线程下载设置
				CreateChildWindow(EMultiThreadsDownloadSettingWindow);
				break;
			default:
				break;
			}

			break;
		case 2://系统信息
			switch(iListBox->CurItemIndex())
			{
			case 0://系统消息
				CreateChildWindow(ESystemMessageWindow);
				break;
			case 1://联网流量统计
				CreateChildWindow(ETraficStatisticWindow);
				break;
			default :
				break;
			}
			break;
		}
		break;
	default :
		keyResult=EFalse;
		break;
	}
	return keyResult;
}

TBool CSystemManageWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSystemManageWindow::DoSizeChanged()
{

}

void CSystemManageWindow::ChangeButton()
{
	
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CSystemManageWindow::InitPopUpMenu()
{
	
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EMenuOk);//待改
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuHelp),EMenuHelp);//待改
	
	

}
void CSystemManageWindow::NaviPaneIndexChanged(TInt aNewIndex)
{

	//ASSERT(NULL==iListBox);
	
	iListBox->Reset();//没有内容会出问题?
	switch(aNewIndex)
	{
	case 0:
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_DownloadHistory_Setting],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_DownloadDir_Setting],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_InfoDetailCacheSetting],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_ContentListCacheSetting],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_CacheClean],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_Download_VoiceSetting],iIconBitmap,iIconBitmapMask));

		break;
	case 1:
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_EnterMode],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_JoinAccountSetting],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSetting],iIconBitmap,iIconBitmapMask));

		break;
	case 2:
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_SystemMessage],iIconBitmap,iIconBitmapMask));
		iListBox->AppendL(CNormalListBoxItem::NewL((*iRes->iItem)[ESystemManageRes_Connecttion_Traffic],iIconBitmap,iIconBitmapMask));
		break;
	default:
	    break;
	}
}
void CSystemManageWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EMenuOk://进入具体的模块
		switch(iNaviPane->CurIndex())
		{
		case 0://软件设置
			switch(iListBox->CurItemIndex())
			{

			case 0://下载历史记录设置:
				CreateChildWindow(EDownloadHistorySettingWindow);
				break;
			case 1://下载保存目录设置
				CreateChildWindow(EDownloadDirSettingWindow);
				break;
			case 2://信息详情缓存设置
				CreateChildWindow(EInfoDetailCacheSettingWindow);
				break;
			case 3://内容列表缓存设置
				CreateChildWindow(EContentListCacheSettingWindow);
				break;

			case 4://清除缓存设置
				CreateChildWindow(ECacheCleanWindow);
				break;
			case 5://下载提示音设置
				CreateChildWindow(EDownloadVoiceSettingWindow);
				break;
			default:
				break;
			}
			break;
		case 1://网络设置
			switch(iListBox->CurItemIndex())
			{
			case 0://登陆方式
				CreateChildWindow(EEnterModeWindow);
				break;
			case 1://联网帐号设置
				CreateChildWindow(EJoinAccountSettingWindow);
				break;
			case 2://多线程下载设置
				CreateChildWindow(EMultiThreadsDownloadSettingWindow);
				break;
			default:
				break;
			}

			break;
		case 2://系统信息
			switch(iListBox->CurItemIndex())
			{
			case 0://系统消息
				CreateChildWindow(ESystemMessageWindow);
				break;
			case 1://联网流量统计
				CreateChildWindow(ETraficStatisticWindow);
				break;
			default :
				break;
			}
			break;
		}

		break;
	case EMenuHelp:
		//进入帮助模块
		CreateChildWindow(EMainHelpWindow);
	    break;
	default:
	    break;
	}
}

