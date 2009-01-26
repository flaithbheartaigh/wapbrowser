/*
============================================================================
 Name        : HelpWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CHelpWindow implementation
============================================================================
*/

#include "HelpWindow.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "TypeDefine.h"
#include "MultiText.h"
CHelpWindow::CHelpWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EHelpWindow,aParent,aMainEngine)
{
}


CHelpWindow::~CHelpWindow()
{
}

CHelpWindow* CHelpWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CHelpWindow* self = new (ELeave)CHelpWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHelpWindow* CHelpWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CHelpWindow* self=CHelpWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHelpWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	TFileName helpFileName;
	helpFileName.Append(iMainEngine.GetAppPath());
	helpFileName.Append(_L("SoarskyHelp.txt"));

	iMultiText=iMainEngine.ControlFactory().CreateDefaultMutliTextFromFile(helpFileName,_L(""));

	iMainEngine.ControlFactory().CreateDefaultTitleBarL(iMainEngine.GetDesById(ETurkeyTextRes_MainMenuHelp));
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CHelpWindow::DoActivateL()
{
}

void CHelpWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CHelpWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CHelpWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice3:	//OK键,进入模块
		iMultiText->GetEndFlag();
		break;
	case EKeyDevice1:
		GoBackToParentWindow();
		break;
	}
	return keyResult;
}

TBool CHelpWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CHelpWindow::DoSizeChanged()
{

}

void CHelpWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CHelpWindow::InitPopUpMenu()
{
/*
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	popUpMenu->AppendItem(_L("Enter"),EEnterModule);
	popUpMenu->AppendItem(_L("Help"),EShowHelp);
	popUpMenu->AppendItem(_L("Hide"),EHideApp);
	popUpMenu->AppendItem(_L("Enter"),EEnterModule);
	popUpMenu->AppendItem(_L("Help"),EShowHelp);
	popUpMenu->AppendItem(_L("Hide"),EHideApp);
	popUpMenu->AppendItem(_L("Back"),EExitApp);
*/

}