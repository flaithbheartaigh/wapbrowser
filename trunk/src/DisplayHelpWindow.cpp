/*
============================================================================
Name        : CDisplayHelpWindow.cpp
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : CBarCodeHelpWindow implementation
============================================================================
*/

#include "DisplayHelpWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "TitleBar.h"
#include "TypeDefine.h"

CDisplayHelpWindow::CDisplayHelpWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EHelpWindow,aParent,aMainEngine)
{
}


CDisplayHelpWindow::~CDisplayHelpWindow()
{

	//DELETE(iDisplayText);

}

CDisplayHelpWindow* CDisplayHelpWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDisplayHelpWindow* self = new (ELeave)CDisplayHelpWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CDisplayHelpWindow* CDisplayHelpWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDisplayHelpWindow* self=CDisplayHelpWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDisplayHelpWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);
	//SetParent表示要为哪个界面创建控件



}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDisplayHelpWindow::DoActivateL()
{
}

void CDisplayHelpWindow::DoDeactivate()
{
	//DeleteAllControl();

}

void CDisplayHelpWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CDisplayHelpWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	//if (iMultiText->GetEndFlag())//当连接文字出现的时候，显示“确定”
	//{
	//	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	//	SetLeftButtonL(textRes[ETurkeyTextRes_Ok]);
	//}
	switch(aKeyCode)
	{
	case KRightSoftKey:
		GoBackToParentWindow();
		break;
	case KLeftSoftKey:
		
	case EKeyDevice3://ok键
		//根据用户进入的帮助栏目的不同，进入不同的模块
		{
			if (iMultiText->GetEndFlag())//有链接且被选中才跳到相应的模块
			{
				switch(iModelNumber)
				{
				case EDownloadManagerWindow:
					CreateChildWindow(EDownloadManagerWindow);
					break;
				case EMainSearchView:
					CreateChildWindow(EMainSearchView);
					break;
				case ERecommendWindow:
					CreateChildWindow( ERecommendWindow);
					break;
					
				case EDownloadedWindow://已下载，
					CreateChildWindow(EDownloadedWindow);
					break;
				case EResultWindow://分类下载
					CreateChildWindow( EResultWindow);
					break;

				case EFriendLinkView://友链
					CreateChildWindow( EFriendLinkView);
					break;
				case ENewestdownView://最新下载
					CreateChildWindow( ENewestdownView);
					break;
				case ESutraDownloadView://精品下载.
					CreateChildWindow( ESutraDownloadView);
					break;
				case ESystemManageWindow:
					CreateChildWindow( ESystemManageWindow);
					break;
				case EUserAdviseWindow:
					CreateChildWindow( EUserAdviseWindow);
					break;
				default:
					keyResult = EFalse;
					break;				
				}
			}

		}

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CDisplayHelpWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDisplayHelpWindow::DoSizeChanged()
{

}

void CDisplayHelpWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	//SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CDisplayHelpWindow::InitPopUpMenu()
{

}

void  CDisplayHelpWindow::LoadFileToMultiText(const TDesC& aFileName)
{

	//文件的加载：可以在帮助主界视图调用的时候传入参数（路径＋文件名）

	iMultiText=iMainEngine.ControlFactory().CreateDefaultMutliTextFromFile(aFileName);
	
	iMultiText->SetObserver(this);//设置连接文字是否出现的观察器

}
void CDisplayHelpWindow::LoadTitle(const TDesC& aTitleText)
{
	//_LIT(KName,"help");

	iMainEngine.ControlFactory().CreateDefaultTitleBarL(aTitleText);
}
void CDisplayHelpWindow::LoadEnterModel(TInt aModelNumber)
{
	iModelNumber=aModelNumber;
}

void CDisplayHelpWindow::LoadLinkText(const TDesC& aLinkText)
{
	if(aLinkText.CompareC(_L("no"))!=0)//当没有链接的时候连接文字是“no”
	{
		iMultiText->AppendEndItem(aLinkText);//链接文字
	}
}

void CDisplayHelpWindow::MultiEvent(TInt aEvent)
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	
	if (aEvent==1)//出现“确定”左软件
	{
		SetLeftButtonL(textRes[ETurkeyTextRes_Ok]);
	} 
	if (aEvent==0)//使左软件“确定”消失
	{
		SetLeftButtonL(_L(""));
	} 
}