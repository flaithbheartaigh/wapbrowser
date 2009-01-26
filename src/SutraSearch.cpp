/*
============================================================================
 Name        : SutraSearch.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSutraSearch implementation
============================================================================
*/

#include "SutraSearch.h"

enum TEventCommand
{
	ENULL,
	EStart,
	Edownload,
	EFileGrade,
	EcontentDetial
};
CSutraSearch::CSutraSearch(CWindow* aParent,CMainEngine& aMainEngine):CWindow(ESutraSearchView,aParent,aMainEngine)
{
	this->ilistbox = NULL;
	this->iTitlebar = NULL;
	// No implementation required
}


CSutraSearch::~CSutraSearch()
{
}

CSutraSearch* CSutraSearch::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSutraSearch* self = new (ELeave)CSutraSearch(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSutraSearch* CSutraSearch::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSutraSearch* self=CSutraSearch::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSutraSearch::ConstructL()
{

}
TBool CSutraSearch::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	keyResult = HandleIconKeyL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:	//×óÈí¼ü,µ¯³ö²Ëµ¥
			this->InitMenu();
			break;

		case EKeyDevice1:	//ÓÒÈí¼ü,Òþ²Ø»òÍË³ö³ÌÐò
			//	PrepareExit();
			GoBackToParentWindow();
			//HideApplication();
			break; 

		case EKeyDevice3:	//OK¼ü,½øÈëÄ£¿é		
			{
				TInt type = 0;
				this->EnterModel(type);
			}
			break;
		default:
			keyResult = EFalse;
			break;
		}
	}
	return keyResult;
}
void CSutraSearch::DoDraw(CGraphic& aGraphic)const
{
	
}
void CSutraSearch::DoDeactivate()
{
	iTitlebar = NULL;
	ilistbox=NULL;
	DeleteAllControl();
}
void CSutraSearch::DoActivateL()
{
	//if (!this->iTitlebar)
	//{
	//	_LIT(name,"SutraSearch");
	//	/*	CResultView* ResultView = (CResultView*)this->iParentWindow;
	//	ResultView->temptitle.*/
	//	this->iTitlebar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(name);
	//}
	if (!this->iSearchList)
	{
		this->iSearchList = iMainEngine.ControlFactory().CreateSearchListBox(*this);
		iSearchList->InitListBox();
		iSearchList->ListBoxDraw();
	}
	
	this->AddListITemL();
}
TBool CSutraSearch::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}
void CSutraSearch::DoSizeChanged()
{
	
}
void CSutraSearch::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = this->iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
void CSutraSearch::AddListITemL()
{
	
}
TBool CSutraSearch::HandleIconKeyL(TInt aKeyCode)
{
	return EFalse;

}
void CSutraSearch::InitMenu()
{

	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);//CreatePopUpMenu(*this);
	popUpMenu->AppendItem(_L("SutraSearch"),EStart);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Search_SDownLoad),Edownload);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade),EFileGrade);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ContentDetail),EcontentDetial);
}
void CSutraSearch::EnterModel(TInt atype)
{

}
void CSutraSearch::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EStart:
		{
			this->GoBackToParentWindow();
		}
		break;
	case Edownload:
		{
			break;
		}
	case EFileGrade:
		{
			break;
		}
	case EcontentDetial:
		{
			break;
		}
	default:
		break;
	}
}
void CSutraSearch::SearchListResponseEvent(TInt aEvent,const TDesC& aText)
{
	
}