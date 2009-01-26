/*
============================================================================
 Name        : SearchResultOutView.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultOutView implementation
============================================================================
*/

#include "SearchResultOutView.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "TitleBar.h"
#include "PopUpMenu.h"
#include "LinkListBox.h"
#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "HandleOutSearch.h"

#include "InputDialog.h"
#include "HttpManager.h"
#include <EscapeUtils.h>
#include <utf.h>
#include "Dialog.h"
#include "HttpObserver.h"
#include "StaticCommonTools.h"

#include "CoCoPreDefine.h"
#include "BitmapFactory.h"
#include "SearchResultItem.h"
#include "SearchResultTempItem.h"
#include "EditorsManager.h"

//定义执行命令用的枚举常量
enum TEventPopCommand
{
	EOutSearchResultPopBack,
	EOutSearchResultPopLastPage,
	EOutSearchResultPopHomePage,
	EOutSearchResultPopPrePage,
	EOutSearchResultPopNextPage,
	EOutSearchResultPopOk,
	EOutSearchResultPopGoto,
	EOutSearchResultPopDownLoad,
	EOutSearchResultPopMainWindows,
	EOutSearchResultPopNone
};
enum TDialogEvent
{
	EOutSearchResultDialogInfo=800,
	EOutSearchResultDialogWait,
	EOutSearchResultDialogDownLoad,
	EOutSearchResultDialogNone
};

CSearchResultOutView::CSearchResultOutView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex)
:CWindow(ESearchOutResultView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
,iIndex(aIndex)
,iOutSearch(NULL)
,iUrl(NULL)
,iTempBuf(NULL)
,iCurPage(1)
,iAllPage(0)
,iDownLoadFlag(0)
{
	// No implementation required
}


CSearchResultOutView::~CSearchResultOutView()
{
	iMainEngine.EditorsManager().RemoveEdwinFromArray(iEdwin);
	DELETE(iOutSearch);
	DELETE(iUrl);
	DELETE(iTempBuf);
}

CSearchResultOutView* CSearchResultOutView::NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle)
{
	CSearchResultOutView* self = new (ELeave)CSearchResultOutView( aParent, aMainEngine, aIndex);
	CleanupStack::PushL(self);
	self->ConstructL(aURL,  aTitle);
	return self;
}

CSearchResultOutView* CSearchResultOutView::NewL(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle)
{
	CSearchResultOutView* self=CSearchResultOutView::NewLC(aParent, aMainEngine, aIndex,  aURL,  aTitle);
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchResultOutView::ConstructL(const TDesC8& aURL,const TDesC& aTitle)
{
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();
	iTitle=HBufC::NewL(aTitle.Length());
	iTitle->Des().Copy(aTitle);

	iFlag=ETrue;
	iUrl=aURL.Alloc();

	iOutSearch=CHandleOutSearch::NewL(*this,iMainEngine);
	iTitleBar= iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle,this);
}
void CSearchResultOutView::DialogEvent(TInt aCommand)
{
	if(aCommand==EOutSearchResultDialogWait)
	{
		if(this->iOutSearch)
		{
			iOutSearch->CancelSendRequest();
		}
	}
	SetEdwinVisible(ETrue);
}

void CSearchResultOutView::InitWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,EOutSearchResultDialogWait,this);
	SetEdwinVisible(EFalse);
}
void CSearchResultOutView::InitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,EOutSearchResultDialogInfo,this);
	SetEdwinVisible(EFalse);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSearchResultOutView::DoActivateL()
{
	this->InitEdwin();
	InitListBox();
	iTimer = CNotifyTimer::NewL(*this);
 	iTimer->Start(100000);
}

void CSearchResultOutView::DoDeactivate()
{
	//DeleteAllControl();
	delete iTimer;
	iTimer = NULL;
}

void CSearchResultOutView::DoDraw(CGraphic& gc) const
{
		DrawEditor(gc);
}

TBool CSearchResultOutView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KOKKey:
		this->DoOKKey();
		break;
	case KKeyCodeLeft:
		if(iCurPage>1)
		{
			iCurPage--;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendPrePageRequest();
		}
		break;
	case KKeyCodeRight:
		if(iCurPage<iAllPage)
		{
			iCurPage++;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendNextPageRequest();
		}
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

TBool CSearchResultOutView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSearchResultOutView::DoSizeChanged()
{

}

void CSearchResultOutView::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CSearchResultOutView::DoPopUpMenuCommand(TInt aCommand)
{
	iPopState=aCommand;
	switch(aCommand)
	{
	case EOutSearchResultPopMainWindows:
		GoBackToMainWindow();
		break;
	case EOutSearchResultPopBack:
		GoBackToParentWindow();
		break;
	case EOutSearchResultPopLastPage:
		{
			iCurPage=iAllPage;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendEndPageRequest();
		}
		break;
	case EOutSearchResultPopHomePage:
		{
			iCurPage=1;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendHomePageRequest();
		}
		break;
	case EOutSearchResultPopPrePage:
		if(iCurPage>1)
		{
			iCurPage--;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendPrePageRequest();
		}
		break;
	case EOutSearchResultPopNextPage:
		if(iCurPage<iAllPage)
		{
			iCurPage++;
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
			iOutSearch->SendNextPageRequest();
		}
		break;
	case EOutSearchResultPopOk:
		StartSearch();
		break;
	case EOutSearchResultPopGoto:
		InitInputDialog();
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//From MHandleEventObserver
//////////////////////////////////////////////////////////////////////////
void CSearchResultOutView::HandleResponseEvent(TInt aEvent,TInt aType)
{
	SetEdwinVisible(ETrue);

	if(iDialog)
			RemoveAndDeleteControl(iDialog);

	if(aEvent==EHttpNoError)
	{
		if(aType==EHttpOutSearch)
		{
			CSearchResultTempItem& item=iOutSearch->GetCurItem();
			iLinkList->ResetList();
			for(TInt i=0;i<item.GetItemCount();i++)
			{
				CSearchResultItem& cItem=item.GetItem(i);
				if(!cItem.IsNull())
				{
					if(cItem.IsLink())
						iLinkList->AppendItem(cItem.GetName(),cItem.GetUrl(),cItem.IsLink());
					else
						iLinkList->AppendItem(cItem.GetName(),_L8(""),cItem.IsLink());
				}
			}
			iCurPage=item.GetCurPage();
			iAllPage=item.GetAllPage();
			iLinkList->LayoutList();
		}
		else if(aType==EHttpOutSearchError)
		{
			InitDialog(iOutSearch->GetErrorInfo());
		}	
	}
	else
	{

		InitDialog(iOutSearch->GetInfo());
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CSearchResultOutView::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Ok),EOutSearchResultPopOk);
	if(iCurPage>1)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),EOutSearchResultPopHomePage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),EOutSearchResultPopPrePage);
	}
	if(iCurPage<iAllPage)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),EOutSearchResultPopNextPage);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),EOutSearchResultPopLastPage);
	}
	if(iAllPage>1)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),EOutSearchResultPopGoto);
	}
	
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_PopUpMenu_GoBackMainView),EOutSearchResultPopMainWindows);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),EOutSearchResultPopBack);
}

void CSearchResultOutView::InitListBox()
{
	if(NULL == iLinkList)
	{
		TRect tRect=iMainEngine.ScreenLayout().WhiteBackgroundRect();
		
		TInt fontHeight=iMainEngine.ScreenLayout().FontHeight();

		TPoint tPoint=tRect.iTl;
		tPoint.iY=tPoint.iY+fontHeight+fontHeight/3;

		TSize tSize=tRect.Size();

		tSize.iHeight=tSize.iHeight-fontHeight+fontHeight/2; 

		iLinkList=iMainEngine.ControlFactory().CreateLinkListBox(*this,TRect(tPoint,tSize),this);
	}
	else
	{
		iLinkList->ResetList();
	}
}


void CSearchResultOutView::StartSearch()
{
	iKeyWord.Zero();
	iEdwin->GetText(iKeyWord);
	if(iKeyWord.Length()>0)
	{
		InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));

		TBuf8<300> tempUrl;
		tempUrl.Zero();
		tempUrl.Append(*iUrl);
		tempUrl.Append(GetUrlEncode(iKeyWord));

		iOutSearch->ClearTempBuf();
		iOutSearch->SendRequest(tempUrl);
		iOutSearch->SendKeyWordRequest(GetUrlEncode(iKeyWord));
		
	}
	else
	{
		InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_InputNull));
	}
}

const TDesC8& CSearchResultOutView::GetUrlEncode(const TDesC& aText)
{
	if(iTempBuf)
	{
		delete iTempBuf;
		iTempBuf=NULL;
	}

	TBuf8<20> tBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(tBuf,aText);
	iTempBuf=EscapeUtils::EscapeEncodeL(tBuf,EscapeUtils::EEscapeUrlEncoded);
	return *iTempBuf;
}
 
void CSearchResultOutView::InitInputDialog()
{
	iInputDialog=iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),_L(""),1,this);
	SetEdwinVisible(EFalse);
}
void CSearchResultOutView::InputResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent==1)
	{
		TInt page=0;
		if(::CheckIsTInt(aText)==KErrNone)
		{
			page=::ConvertTDesCToTInt(aText);
			if(page<iAllPage)
			{
				iCurPage=page;
			}
			else
			{
				iCurPage=iAllPage;
			}
			iOutSearch->SendGoToPageRequest(iCurPage);
		}
	}
	SetEdwinVisible(ETrue);
}

void CSearchResultOutView::SetEdwinVisible(TBool aBool)
{
	if(iEdwin->IsFocused()&&!aBool)
	{
		iFlag=EFalse;
		iEdwin->SetFocus(EFalse);
		iEdwin->MakeVisible(EFalse);
	}
	else if(!iFlag&&aBool)
	{
		iFlag=ETrue;
		iEdwin->SetFocus(ETrue);
		iEdwin->MakeVisible(ETrue);
	}
}
void CSearchResultOutView::InitEdwin()
{
	if(!iEdwin)
	{
		TRect tRect=iMainEngine.ScreenLayout().WhiteBackgroundRect();
		
		TInt fontHeight=iMainEngine.ScreenLayout().FontHeight();

		TSize tSize=tRect.Size();

		tSize.iHeight=fontHeight+fontHeight/4;

		iEdwin=iMainEngine.EditorsManager().CreateEdwin(TRect(tRect.iTl,tSize),20,TRgb(0,0,0));
		iMainEngine.EditorsManager().SetCurEdwin(iEdwin);

		
		TPoint point=iMainEngine.ScreenLayout().GetNaviPaneRect().iTl;
		TSize size=iMainEngine.ScreenLayout().GetNaviPaneRect().Size();

		TSize iconSize=iMainEngine.GetBitmapFactory().GetInputIconBmpSize(0);

		iPoint.iX=point.iX+size.iWidth-iconSize.iWidth-iMainEngine.ScreenLayout().FontHeight()/3;
		iPoint.iY=point.iY+(size.iHeight-iconSize.iHeight)/2;
	}
}
void CSearchResultOutView::LinkListEvent(TInt aEvent,TInt aType)
{
	if(aEvent==KKeyCodeDown)
	{
		this->SetEdwinVisible(EFalse);
	}
	else if(aEvent==KKeyCodeUp)
	{
		this->SetEdwinVisible(ETrue);
	}
}
void CSearchResultOutView::DoOKKey()
{
	if(iEdwin->IsFocused())
	{
		this->StartSearch();
	}
	else
	{	
		TBuf8<300> tempUrl;
		tempUrl.Zero();
		tempUrl.Append(iLinkList->GetUrl());

		if(iOutSearch->CheckUrl(tempUrl)==2)
		{
			if(tempUrl.Find(_L8("http://"))!=0)
			{
				tempUrl.Zero();
				tempUrl.Append(_L8("http://wap.roboo.com/"));
				tempUrl.Append(iLinkList->GetUrl());
			}
			TBuf<300> url;
			url.Copy(tempUrl);

			iDownLoadFlag=this->AddDownLoadList(*this,EOutSearchResultDialogDownLoad,url,iLinkList->GetName(),0,0,1,0);
		}
		else if(iOutSearch->CheckUrl(tempUrl)==1||iOutSearch->CheckUrl(tempUrl)==3)
		{
			if(tempUrl.Find(_L8("http://"))!=0)
			{
				tempUrl.Zero();
				tempUrl.Append(_L8("http://wap.roboo.com/"));
				tempUrl.Append(iLinkList->GetUrl());
			}
			this->iOutSearch->SendRequest(tempUrl);
			InitWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_DialogInfo_Connecting));
		}
	}
}
void CSearchResultOutView::DrawEditor(CGraphic& gc) const
{
	if(!iEdwin->IsFocused())
	{
		TBuf<25> tText;
		iEdwin->GetText(tText);

		TRect tRect=iMainEngine.ScreenLayout().WhiteBackgroundRect();
		
		TInt fontHeight=iMainEngine.ScreenLayout().FontHeight();

		TSize tSize=tRect.Size();

		tSize.iHeight=fontHeight+fontHeight/4;

		gc.SetPenColor(TRgb(255,255,255));
		gc.SetBrushColor(TRgb(255,255,255));
		gc.SetPenStyle(CBitmapContext::ENullPen);
		gc.SetBrushStyle(CBitmapContext::ESolidBrush);
		gc.DrawRect(TRect(tRect.iTl,tSize));

		gc.SetPenColor(KRgbBlack);
		gc.SetPenStyle(CGraphicsContext::ESolidPen);

		gc.DrawText(tRect.iTl,tText);
		gc.SetBrushStyle(CGraphicsContext::ENullBrush);		

	}
	else
		iMainEngine.GetBitmapFactory().DrawInputIconBmpByID(gc,iPoint,iMainEngine.GetInputType()-1);
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CSearchResultOutView::DoPeriodTask()
{
	if(iTitleBar)
	{
		iTitleBar->SetRightBitmap(iMainEngine.GetBitmapFactory().GetInputIconBmp(iMainEngine.GetInputType()-1),iMainEngine.GetBitmapFactory().GetInputIconBmpMask(iMainEngine.GetInputType()-1));
	}
	return ETrue;
}