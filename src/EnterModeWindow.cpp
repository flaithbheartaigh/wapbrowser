/*
============================================================================
 Name        : EnterModeWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CEnterModeWindow implementation
============================================================================
*/

#include "EnterModeWindow.h"

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

CEnterModeWindow::CEnterModeWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CEnterModeWindow::~CEnterModeWindow()
{
}

CEnterModeWindow* CEnterModeWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CEnterModeWindow* self = new (ELeave)CEnterModeWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CEnterModeWindow* CEnterModeWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CEnterModeWindow* self=CEnterModeWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CEnterModeWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iMainEngine.ControlFactory().SetParentWindow(this);
	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_EnterMode]);

	InitSelectGroup();

}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CEnterModeWindow::DoActivateL()
{
	

}

void CEnterModeWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CEnterModeWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CEnterModeWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
}



TBool CEnterModeWindow::DoKeyEventL(TInt aKeyCode)
{


	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			switch(iSelectGroup->GetSelectRedio(_L("a")))

			{
			case 0://在线
				if (iMainEngine.SetLoginType(1)==0)
				{
					saved=1;
				}
				break;
			case 1://离线
				if (iMainEngine.SetLoginType(0)==0)
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
					(*iRes->iItem)[ESystemManageRes_EnterMode_Success]\
					,*this,1);
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_EnterMode_Fail]\
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

TBool CEnterModeWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CEnterModeWindow::DoSizeChanged()
{

}

void CEnterModeWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CEnterModeWindow::InitSelectGroup()
{


	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_EnterMode0]);

	TInt index;

	switch(iMainEngine.GetSavedLoginType())//
	{
	case 1://在线
		index=0;
		break;

	default://离线,默认为离线
		index=1;
		break;
	}
	iSelectGroup->AddSelectItem(\
		(*iRes->iItem)[ESystemManageRes_EnterMode_Kind1]\
		,_L("a"),ESelectRedio);//在线方式
	iSelectGroup->AddSelectItem(\
		(*iRes->iItem)[ESystemManageRes_EnterMode_Kind2]\
		,_L("a"),ESelectRedio);//离线方式


	iSelectGroup->SetSelected(_L("a"),index);

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_EnterMode1]);
	
}