/*
============================================================================
 Name        : SearchLocalResultView.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchLocalResultView implementation
============================================================================
*/

#include "SearchLocalResultView.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "TitleBar.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "SearchListBox.h"
#include "HandleLocalSearch.h"

#include "InputDialog.h"
#include "Dialog.h"
#include "StaticCommonTools.h"
#include "FileExecuter.h"
#include "CopyToDialog.h"
#include "CoCoPreDefine.h"
#include "BitmapFactory.h"

//定义执行命令用的枚举常量
enum TEventPopCommand
{
	ESearchLocalPopOpen,
	ESearchLocalPopCut,
	ESearchLocalPopCopy,
	ESearchLocalPopBack,
	ESearchLocalPopLastPage,
	ESearchLocalPopHomePage,
	ESearchLocalPopPrePage,
	ESearchLocalPopNextPage,
	ESearchLocalPopOk,
	ESearchLocalPopGoto,
	ESearchLocalMainWindow,
	ESearchLocalPopNone
};

CSearchLocalResultView::CSearchLocalResultView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex)
:CWindow(ESearchLocalResultView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
,iIndex(aIndex)
,iLocalSearch(NULL)
,iCurPage(1)
,iAllPage(0)
{
	// No implementation required
}


CSearchLocalResultView::~CSearchLocalResultView()
{
	DELETE(iTitle);
	DELETE(iLocalSearch);	
}

CSearchLocalResultView* CSearchLocalResultView::NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC& aTitle)
{
	CSearchLocalResultView* self = new (ELeave)CSearchLocalResultView( aParent, aMainEngine,aIndex);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	return self;
}

CSearchLocalResultView* CSearchLocalResultView::NewL( CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC& aTitle)
{
	CSearchLocalResultView* self=CSearchLocalResultView::NewLC(aParent, aMainEngine,aIndex,aTitle);
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchLocalResultView::ConstructL(const TDesC& aTitle)
{
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();
	iTitle=HBufC::NewL(aTitle.Length());
	iTitle->Des().Copy(aTitle);
	iLocalSearch=CHandleLocalSearch::NewL(*this,iMainEngine,iIndex);
}
void CSearchLocalResultView::DialogEvent(TInt aCommand)
{
	this->iSearchList->SetEdwinVisible(ETrue);
}

void CSearchLocalResultView::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,1,this);
	this->iSearchList->SetEdwinVisible(EFalse);
}
void CSearchLocalResultView::InitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,2,this);
	this->iSearchList->SetEdwinVisible(EFalse);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSearchLocalResultView::DoActivateL()
{
	iTitleBar= iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle);
	this->InitListBox();
}

void CSearchLocalResultView::DoDeactivate()
{
	//DeleteAllControl();
}

void CSearchLocalResultView::DoDraw(CGraphic& aGraphic) const
{
	
}

TBool CSearchLocalResultView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
	case KKeyCodeDown:
		iSearchList->KeyEventL(aKeyCode);
		break;
	case KKeyCodeLeft:
		iSearchList->PageUp();
		break;
	case KKeyCodeRight:
		iSearchList->PageDown();
		break;
	case KLeftSoftKey:
		InitPopUpMenu();
		break;
	case KRightSoftKey:
		GoBackToParentWindow();
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CSearchLocalResultView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSearchLocalResultView::DoSizeChanged()
{

}

void CSearchLocalResultView::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CSearchLocalResultView::DoPopUpMenuCommand(TInt aCommand)
{
	iPopState=aCommand;
	switch(aCommand)
	{
	case ESearchLocalPopOpen:
		{
			iSearchList->SetEdwinVisible(EFalse);
			this->OpenFile();
		}
		break;
	case ESearchLocalPopCut:
		InitCopyToDialog(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_FileCut));
		break;
	case ESearchLocalPopCopy:
		InitCopyToDialog(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_FileCopy));
		break;
	case ESearchLocalPopBack:
		GoBackToParentWindow();
		break;
	case ESearchLocalMainWindow:
		GoBackToMainWindow();
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MHandleEventObserver
//////////////////////////////////////////////////////////////////////////
void CSearchLocalResultView::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if(aEvent==EHttpNoError)
	{
		this->iSearchList->SetEdwinVisible(ETrue);
		iSearchList->InitListBox();
		RemoveAndDeleteControl(iDialog);
		if(iLocalSearch->GetCount()==0)
		{
			this->InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_ListNull));
		}
		else
		{
			iSearchList->SetEdwinVisible(EFalse);
			for(TInt i=0;i<iLocalSearch->GetCount();i++)
			{
				const CFbsBitmap* icon = NULL;
				const CFbsBitmap* iconMask = NULL;
				icon = iMainEngine.GetBitmapFactory().GetFileTypeIcon(GetCurType());
				iconMask = iMainEngine.GetBitmapFactory().GetFileTypeIconMask(GetCurType());
				iSearchList->AppendListItem(iLocalSearch->GetOneItemName(i),icon,iconMask);
			}

			TBuf<50> titleBuf;
			titleBuf.Zero();
			titleBuf.Append(*iTitle);
			titleBuf.Append(_L("["));
			titleBuf.AppendNum(iLocalSearch->GetCount());
			titleBuf.Append(_L("]"));
			iTitleBar->SetTextL(titleBuf);

			iSearchList->ListBoxDraw();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CSearchLocalResultView::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	if(iLocalSearch->GetCount()>0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_OpenFile),ESearchLocalPopOpen);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_FileCut),ESearchLocalPopCut);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_FileCopy),ESearchLocalPopCopy);
	}
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_PopUpMenu_GoBackMainView),ESearchLocalMainWindow);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ESearchLocalPopBack);
}

void CSearchLocalResultView::InitListBox()
{
	if(!iSearchList)
	{
		iSearchList = iMainEngine.ControlFactory().CreateSearchListBox(*this);
		iSearchList->InitListBox();
		iSearchList->ListBoxDraw();
	}
}
void CSearchLocalResultView::SearchListResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent==0)
	{
		if(aText.Length()>0)
		{
			this->InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_FileSearching));
			iLocalSearch->SetKeyWord(aText);
		}
		else
		{
			InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_InputNull));
		}
	}
	else if(aEvent==999)
	{
		if(iLocalSearch->GetCount()>0)
			this->OpenFile();
	}
}

void CSearchLocalResultView::InitInputDialog()
{
	
}
void CSearchLocalResultView::InputResponseEvent(TInt aEvent,const TDesC& aText)
{

}
void CSearchLocalResultView::OpenFile()
{
	TFileName fileName;
	fileName.Zero();
	fileName.Append(iLocalSearch->GetOneItemPath(this->iSearchList->GetSelectedIndex()));
	fileName.Append(iLocalSearch->GetOneItemName(this->iSearchList->GetSelectedIndex()));

	iMainEngine.FileExecuter((CWindow*)this).OpenFileByDefault(fileName);
}

void CSearchLocalResultView::CopyToEvent(TInt aEvent,const TDesC& aPath)
{
	if(aEvent==0)
	{
		switch(iPopState)
		{
			case ESearchLocalPopCut:
				this->iLocalSearch->MoveFile(iSearchList->GetSelectedIndex(),aPath);
				this->iSearchList->SetEdwinVisible(ETrue);
				break;
			case ESearchLocalPopCopy:
				this->iLocalSearch->CopyFile(iSearchList->GetSelectedIndex(),aPath);
				this->iSearchList->SetEdwinVisible(ETrue);
				break;
		}
	}
	this->iSearchList->SetEdwinVisible(ETrue);
}
void CSearchLocalResultView::GetCopyToPath(TInt aIndex,TDes& aPath)
{
	aPath.Append(iMainEngine.GetAppDrive());
	aPath.Append(_L("\\Data\\CoCo\\"));
	switch(aIndex)
	{
		case 0:
			aPath.Append(KIMAGES);
			break;
		case 1:
			aPath.Append(KMUSIC);
			break;
		case 2:
			aPath.Append(KAPPS);
			break;
		case 3:
			aPath.Append(KVIDEO);
			break;
		case 4:
			aPath.Append(KAPPS);
			break;
		case 5:
			aPath.Append(KBOOKS);
			break;
		case 6:
			aPath.Append(KTOPS);
			break;
	}
}
void CSearchLocalResultView::InitCopyToDialog(const TDesC& aText)
{
	TFileName path;
	
	CCopyToDialog* copyToDialog=iMainEngine.ControlFactory().CreateCopyToDialog(*this,aText,this);
	for(TInt i=0;i<7;i++)
	{
		path.Zero();
		this->GetCopyToPath(i,path);
		copyToDialog->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Type1+i),path);
	}
	this->iSearchList->SetEdwinVisible(EFalse);
}
TInt CSearchLocalResultView::GetCurType()
{
	TInt index=EFileTypeOther;
	switch(iIndex)
	{
		case 0:
			index=EFileTypeImage;
			break;
		case 1:
			index=EFileTypeMusic;
			break;
		case 2:
			index=EFileTypeAppOrGame;
			break;
		case 3:
			index=EFileTypeVideo;
			break;
		case 4:
			index=EFileTypeAppOrGame;
			break;
		case 5:
			index=EFileTypeEBook;
			break;
		case 6:
			index=EFileTypeTheme;
			break;
	}
	return index;
}