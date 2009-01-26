/*
============================================================================
 Name        : InfoDetailCacheSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CInfoDetailCacheSettingWindow implementation
============================================================================
*/

#include "InfoDetailCacheSettingWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "SelectGroup.h"
#include "TypeDefine.h"
#include "SelectBox.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "FileReadByRow.h"
#include "Dialog.h"

CInfoDetailCacheSettingWindow::CInfoDetailCacheSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CInfoDetailCacheSettingWindow::~CInfoDetailCacheSettingWindow()
{
}

CInfoDetailCacheSettingWindow* CInfoDetailCacheSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CInfoDetailCacheSettingWindow* self = new (ELeave)CInfoDetailCacheSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CInfoDetailCacheSettingWindow* CInfoDetailCacheSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CInfoDetailCacheSettingWindow* self=CInfoDetailCacheSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CInfoDetailCacheSettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_InfoDetailCacheSetting]);

	InitSelectGroup();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CInfoDetailCacheSettingWindow::DoActivateL()
{
	


}

void CInfoDetailCacheSettingWindow::DoDeactivate()
{
	
}

void CInfoDetailCacheSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CInfoDetailCacheSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
}

TBool CInfoDetailCacheSettingWindow::DoKeyEventL(TInt aKeyCode)
{


	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			switch(iSelectGroup->GetSelectRedio(_L("a")))

			{
			case 0://20
				if (iMainEngine.SetContentInfoMaxNum(20)==0)
				{
					saved=1;
				}
				break;
			case 1://50
				if (iMainEngine.SetContentInfoMaxNum(50)==0)
				{
					saved=1;
				}
				break;
			case 2://100
				if (iMainEngine.SetContentInfoMaxNum(100)==0)
				{
					saved=1;
				}
				break;

			default:
				break;
			}
			if (saved==1)//提示并回到上层界面
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_InfoDetailCacheSettingSuccess]\
					,*this,1);
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_InfoDetailCacheSettingfail]\
					,*this,2);
				break;
			}

		}
	case KRightSoftKey:
		GoBackToParentWindow();
		break;
	default:
		keyResult = EFalse;
		break;
	}

	return keyResult;
}

TBool CInfoDetailCacheSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CInfoDetailCacheSettingWindow::DoSizeChanged()
{

}

void CInfoDetailCacheSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CInfoDetailCacheSettingWindow::InitSelectGroup()
{


	//iSelectGroup = CSelectGroup::NewL(iMainEngine);

	//TInt topX;//SelectBox左上角x坐标
	//TInt topY;//SelectBox左上角y坐标
	//TInt height;//SelectBox的高度
	//TInt width;//SelectBox的宽度

	//TRect naviRect=iMainEngine.ScreenLayout().GetNaviPaneRect();
	//TRect clientRect=iMainEngine.ScreenLayout().GetClientRect();

	//topX=(naviRect.iTl).iX;
	//topY=(naviRect.iTl).iY+naviRect.Height();

	//height=clientRect.Height()-naviRect.Height();
	//width=naviRect.Width();

	//iSelectGroup->InitLayout( TRect( TPoint(topX,topY),TSize(width,height) ) );
	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();
	
	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_InfoDetailCacheSetting0]);



	TInt index;

	switch(iMainEngine.GetContentInfoMaxNum())//
	{
	case 20:
		index=0;
		break;
	case 50:
		index=1;
		break;
	case 100:
		index=2;
		break;
	default://如果有误的话，就设置默认的选中项为20
		index=0;
		break;
	}
	iSelectGroup->AddSelectItem(_L("20"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("50"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("100"),_L("a"),ESelectRedio);

	iSelectGroup->SetSelected(_L("a"),index);


	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_InfoDetailCacheSetting1]);

	//AddControl(iSelectGroup);
}