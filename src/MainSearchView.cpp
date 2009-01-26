/*
============================================================================
 Name        : MainSearchView.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CMainSearchView implementation
============================================================================
*/

#include "MainSearchView.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "NaviPane.h"
//#include "PopUpMenu.h"
#include "ListBox.h"
#include "TypeDefine.h"
#include "EditorsManager.h"
#include "ListBoxItem.h"

CMainSearchView::CMainSearchView(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(EMainSearchView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
,iNaviPane(NULL)
,iListBox(NULL)
{
	// No implementation required
}


CMainSearchView::~CMainSearchView()
{

}

CMainSearchView* CMainSearchView::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMainSearchView* self = new (ELeave)CMainSearchView(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMainSearchView* CMainSearchView::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMainSearchView* self=CMainSearchView::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CMainSearchView::ConstructL()
{
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CMainSearchView::DoActivateL()
{
	if(iNaviPane==NULL)
	{
		iNaviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane();
		iNaviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_In));
		//iNaviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Out));
		iNaviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Local));

		iNaviPane->SetObserver(this);
	}

	if(iListBox==NULL)
		InitListBox(1);
}

void CMainSearchView::DoDeactivate()
{
	//iListBox=NULL;
	//DeleteAllControl();
}

void CMainSearchView::DoDraw(CGraphic& aGraphic) const
{

}

TBool CMainSearchView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
	case KOKKey:
		{
			TBuf<20> tempBuf;
			tempBuf.Zero();
			if(iNaviPane->CurIndex()==0)
			{
				tempBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_In));
				tempBuf.Append(_L("->"));
		
				CNormalListBoxItem& item = (CNormalListBoxItem&)iListBox->CurItem();
				tempBuf.Append(item.GetText());

				TBuf8<200> urlBuf;
				urlBuf.Append(_L8("http://59.36.96.182/3gserver/search?corpid=000000&m=1&username="));
				urlBuf.Append(iMainEngine.GetIMEI());
				urlBuf.Append(_L8("&typeid="));
				urlBuf.AppendNum(iListBox->CurItemIndex());
				urlBuf.Append(_L8("&keyword="));
			
				SetChildWindow(iMainEngine.WindowFactory().CreateSearchResultView(ESearchResultView,this,this->iListBox->CurItemIndex(),urlBuf,tempBuf));
			}
			//else if(iNaviPane->CurIndex()==1)
			//{
			//	TBuf8<200> urlBuf;
			//	urlBuf.Zero();
			//	switch(this->iListBox->CurItemIndex())
			//	{
			//		case 0:
			//			urlBuf.Append(_L8("http://wap.roboo.com/Simage.wml?q="));
			//			break;
			//		case 1:
			//			urlBuf.Append(_L8("http://wap.roboo.com/Sring.wml?q="));
			//			break;
			//		case 2:
			//			urlBuf.Append(_L8("http://wap.roboo.com/Sgame.wml?q="));
			//			break;
			//		case 3:
			//			urlBuf.Append(_L8("http://wap.roboo.com/Svideo.wml?q="));
			//			break;
			//	}

			//	tempBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Out));
			//	tempBuf.Append(_L("->"));
		
			//	CNormalListBoxItem& item = (CNormalListBoxItem&)iListBox->CurItem();
			//	tempBuf.Append(item.GetText());

			//	SetChildWindow(iMainEngine.WindowFactory().CreateSearchResultView(ESearchOutResultView,this,this->iListBox->CurItemIndex(),urlBuf,tempBuf));
			//}
			else if(iNaviPane->CurIndex()==1)
			{
				tempBuf.Append(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Local));
				tempBuf.Append(_L("->"));
		
				CNormalListBoxItem& item = (CNormalListBoxItem&)iListBox->CurItem();
				tempBuf.Append(item.GetText());
				
				SetChildWindow(iMainEngine.WindowFactory().CreateSearchLocalResultView(ESearchLocalResultView,this,iListBox->CurItemIndex(),tempBuf));
			}
		}
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

TBool CMainSearchView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CMainSearchView::DoSizeChanged()
{

}

void CMainSearchView::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

//////////////////////////////////////////////////////////////////////////
//From MNaviPaneObserver
//////////////////////////////////////////////////////////////////////////
void CMainSearchView::NaviPaneIndexChanged(TInt aNewIndex)
{
	InitListBox(aNewIndex+1);
}

//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CMainSearchView::InitPopUpMenu()
{

}

void CMainSearchView::InitListBox(TInt aIndex)
{
	if(NULL == iListBox)
	{
		iListBox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	}
	else
	{
		iListBox->Reset();
	}

	switch(aIndex)
	{
		case 1://Õ¾ÄÚËÑË÷
			iListBox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetDesById(ETurkeyTextRes_All)));
			for(TInt i=0;i<7;i++)
			{
				iListBox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Type1+i)));
			}
			break;
		//case 2://Õ¾ÍâËÑË÷
		//	for(TInt i=0;i<4;i++)
		//	{
		//		iListBox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Type1+i)));
		//	}
		//	break;
		case 2://±¾µØËÑË÷
			for(TInt i=0;i<7;i++)
			{
				iListBox->AppendL(CNormalListBoxItem::NewL(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_Type1+i)));
			}
			break;
	}
	iListBox->Layout();
}