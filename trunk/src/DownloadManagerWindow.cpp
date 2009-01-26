/*
============================================================================
 Name        : DownloadManagerWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadManagerWindow implementation
============================================================================
*/

#include "DownloadManagerWindow.h"
#include "DownloadHistoryList.h"
#include "DownloadingList.h"
#include "MainEngine.h"
#include "ListBox.h"
#include "NaviPane.h"
#include "PopUpMenu.h"
#include "ControlFactory.h"
#include "TypeDefine.h"


CDownloadManagerWindow::CDownloadManagerWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EDownloadManagerWindow,aParent,aMainEngine)
{
	iCanPopUpWindow = EFalse;
}

CDownloadManagerWindow::~CDownloadManagerWindow()
{
}

CDownloadManagerWindow* CDownloadManagerWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadManagerWindow* self = new (ELeave)CDownloadManagerWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadManagerWindow* CDownloadManagerWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadManagerWindow* self=CDownloadManagerWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadManagerWindow::ConstructL()
{
	ChangeButton();

	iMainEngine.ControlFactory().SetParentWindow(this);
	InitNaviPane();
	InitDownloadingList();
}
//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CDownloadManagerWindow::DoActivateL()
{
}

void CDownloadManagerWindow::DoDeactivate()
{
}

void CDownloadManagerWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CDownloadManagerWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice1:
		GoBackToParentWindow();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CDownloadManagerWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDownloadManagerWindow::DoSizeChanged()
{

}

void CDownloadManagerWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//////////////////////////////////////////////////////////////////////////
//From MNaviPaneObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadManagerWindow::NaviPaneIndexChanged(TInt aNewIndex)
{
	switch(aNewIndex)
	{
	case 0:
		InitDownloadingList();
		break;

/*
	case 1:
		InitDownloadedList();
		break;
*/

	case 1:
		InitDownLoadHistoryList();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CDownloadManagerWindow::InitNaviPane()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	CNaviPane* naviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane(this);
	naviPane->AppendItem(textRes[ETurkeyTextRes_DownLoadingList]);
	//naviPane->AppendItem(textRes[ETurkeyTextRes_DownLoadedList]);
	naviPane->AppendItem(textRes[ETurkeyTextRes_DownLoad_History]);
}

void CDownloadManagerWindow::InitDownloadingList()
{
	RemoveAndDeleteControl(iTempControl);
	iTempControl = NULL;
	iTempControl = CDownloadingList::NewL(iMainEngine,*this);
	AddControl(iTempControl);
	//User::InfoPrint(_L("InitDownloadingList"));
}

void CDownloadManagerWindow::InitDownloadedList()
{
	RemoveAndDeleteControl(iTempControl);
	iTempListBox = NULL;

}

void CDownloadManagerWindow::InitDownLoadHistoryList()
{
	RemoveAndDeleteControl(iTempControl);
	iTempControl = NULL;
	iTempControl = CDownloadHistoryList::NewL(iMainEngine,*this);
	AddControl(iTempControl);
	//User::InfoPrint(_L("InitDownLoadHistoryList"));
}