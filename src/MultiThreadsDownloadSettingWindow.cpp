/*
============================================================================
 Name        : MultiThreadsDownloadSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CMultiThreadsDownloadSettingWindow implementation
============================================================================
*/

#include "MultiThreadsDownloadSettingWindow.h"

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

CMultiThreadsDownloadSettingWindow::CMultiThreadsDownloadSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CMultiThreadsDownloadSettingWindow::~CMultiThreadsDownloadSettingWindow()
{
}

CMultiThreadsDownloadSettingWindow* CMultiThreadsDownloadSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMultiThreadsDownloadSettingWindow* self = new (ELeave)CMultiThreadsDownloadSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMultiThreadsDownloadSettingWindow* CMultiThreadsDownloadSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMultiThreadsDownloadSettingWindow* self=CMultiThreadsDownloadSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CMultiThreadsDownloadSettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSetting]);

	InitSelectGroup();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CMultiThreadsDownloadSettingWindow::DoActivateL()
{
}

void CMultiThreadsDownloadSettingWindow::DoDeactivate()
{
	
}

void CMultiThreadsDownloadSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CMultiThreadsDownloadSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
}

TBool CMultiThreadsDownloadSettingWindow::DoKeyEventL(TInt aKeyCode)
{

	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			switch(iSelectGroup->GetSelectRedio(_L("a")))

			{
			case 0://1
				if (iMainEngine.SetDownLoadMaxThreadNum(1)==0)
				{
					saved=1;
				}
				break;
			case 1://3
				if (iMainEngine.SetDownLoadMaxThreadNum(3)==0)
				{
					saved=1;
				}
				break;
			case 2://5
				if (iMainEngine.SetDownLoadMaxThreadNum(5)==0)
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
					(*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSettingSuccess]\
					,*this,1);
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSettingFail]\
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

TBool CMultiThreadsDownloadSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CMultiThreadsDownloadSettingWindow::DoSizeChanged()
{

}

void CMultiThreadsDownloadSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CMultiThreadsDownloadSettingWindow::InitSelectGroup()
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

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSetting0]);

	

	TInt index;

	switch(iMainEngine.GetDownLoadMaxThreadNum())//
	{
	case 1:
		index=0;
		break;
	case 3:
		index=1;
		break;
	case 5:
		index=2;
		break;
	default://如果有误的话，就设置默认的选中项为1
		index=0;
		break;
	}
	iSelectGroup->AddSelectItem(_L("1"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("3"),_L("a"),ESelectRedio);
	iSelectGroup->AddSelectItem(_L("5"),_L("a"),ESelectRedio);

	iSelectGroup->SetSelected(_L("a"),index);


	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_MultiThreadsDownloadSetting1]);
	

	//AddControl(iSelectGroup);
}