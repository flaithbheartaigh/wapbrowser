/*
============================================================================
 Name        : DownloadHistorySettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadHistorySettingWindow implementation
============================================================================
*/

#include "DownloadHistorySettingWindow.h"

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
CDownloadHistorySettingWindow::CDownloadHistorySettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CDownloadHistorySettingWindow::~CDownloadHistorySettingWindow()
{
}

CDownloadHistorySettingWindow* CDownloadHistorySettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadHistorySettingWindow* self = new (ELeave)CDownloadHistorySettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadHistorySettingWindow* CDownloadHistorySettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadHistorySettingWindow* self=CDownloadHistorySettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadHistorySettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_DownloadHistory_Setting]);

	InitSelectGroup();
		
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDownloadHistorySettingWindow::DoActivateL()
{
	
	
	
}

void CDownloadHistorySettingWindow::DoDeactivate()
{
	
}

void CDownloadHistorySettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadHistorySettingWindow::DialogEvent(TInt aCommand)
{
if (aCommand==1)
{
	GoBackToParentWindow();
}
}

TBool CDownloadHistorySettingWindow::DoKeyEventL(TInt aKeyCode)
{
	

	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			switch(iSelectGroup->GetSelectRedio(_L("a")))
	
			{
			case 0://50
				if (iMainEngine.SetDownLoadMaxHistoryNum(50)==0)
				{
					saved=1;
				}
				break;
			case 1://100
				if (iMainEngine.SetDownLoadMaxHistoryNum(100)==0)
				{
					saved=1;
				}
				break;
			case 2://200
				if (iMainEngine.SetDownLoadMaxHistoryNum(200)==0)
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
					(*iRes->iItem)[ESystemManageRes_DownloadHistory_SettingSuccess]\
					,*this,1);
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_DownloadHistory_SettingFail]\
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

TBool CDownloadHistorySettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDownloadHistorySettingWindow::DoSizeChanged()
{

}

void CDownloadHistorySettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CDownloadHistorySettingWindow::InitSelectGroup()
{
	
	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_DownloadHistory_Setting0]);

	TInt index;
	
	switch(iMainEngine.GetDownLoadMaxHistoryNum())//
	{
	case 50:
		index=0;
		break;
	case 100:
		index=1;
		break;
	case 200:
		index=2;
		break;
	default://如果maxNum有误的话，就设置默认的选中项为50
		index=0;
		break;
	}
	iSelectGroup->AddSelectItem(_L("50"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("100"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("200"),_L("a"),ESelectRedio);

	iSelectGroup->SetSelected(_L("a"),index);
	

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_DownloadHistory_Setting1]);
	
}