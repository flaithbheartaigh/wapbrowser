/*
============================================================================
 Name        : ContentListCacheSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CContentListCacheSettingWindow implementation
============================================================================
*/

#include "ContentListCacheSettingWindow.h"

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

CContentListCacheSettingWindow::CContentListCacheSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CContentListCacheSettingWindow::~CContentListCacheSettingWindow()
{
}

CContentListCacheSettingWindow* CContentListCacheSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CContentListCacheSettingWindow* self = new (ELeave)CContentListCacheSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CContentListCacheSettingWindow* CContentListCacheSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CContentListCacheSettingWindow* self=CContentListCacheSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CContentListCacheSettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_ContentListCacheSetting]);

	InitSelectGroup();



}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CContentListCacheSettingWindow::DoActivateL()
{
	

}

void CContentListCacheSettingWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CContentListCacheSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CContentListCacheSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
	/*TBuf<20>d;
	d.Num(200);*/
}


TBool CContentListCacheSettingWindow::DoKeyEventL(TInt aKeyCode)
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
				if (iMainEngine.SetContentListMaxNum(20)==0)
				{
					saved=1;
				}
				break;
			case 1://50
				if (iMainEngine.SetContentListMaxNum(50)==0)
				{
					saved=1;
				}
				break;
			case 2://100
				if (iMainEngine.SetContentListMaxNum(100)==0)
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
					(*iRes->iItem)[ESystemManageRes_ContentListCacheSettingSuccess]\
					,*this,1);
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_ContentListCacheSettingFail]\
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

TBool CContentListCacheSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CContentListCacheSettingWindow::DoSizeChanged()
{

}

void CContentListCacheSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CContentListCacheSettingWindow::InitSelectGroup()
{


	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_ContentListCacheSetting0]);
	


	TInt index;

	switch(iMainEngine.GetContentListMaxNum())//
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

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_ContentListCacheSetting1]);
	
}