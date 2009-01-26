/*
============================================================================
 Name        : SearchListBox.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchListBox implementation
 更改了AppendListItem(...)
============================================================================
*/
#include <W32STD.H>
#include "SearchListBox.h"
#include "MainEngine.h"
#include "TypeDefine.h"
#include "Graphic.h"
#include "ScreenLayout.h"
#include "EditorsManager.h"
#include "ControlFactory.h"
#include "MControlObserver.h"
#include "ListBox.h"
#include "BitmapFactory.h"
#include "ListBoxItem.h"

CSearchListBox::CSearchListBox(MSearchListObserver& aObserver,CMainEngine& aMainEngine)
:CControl(ESearchListBox)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iEdwin(NULL)
{
	// No implementation required
}


CSearchListBox::~CSearchListBox()
{
	iMainEngine.EditorsManager().RemoveEdwinFromArray(iEdwin);
	if(iListBox)
	{
		delete iListBox;
		iListBox=NULL;
	}
}

CSearchListBox* CSearchListBox::NewLC(MSearchListObserver& aObserver,CMainEngine& aMainEngine)
{
	CSearchListBox* self = new (ELeave)CSearchListBox(aObserver,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSearchListBox* CSearchListBox::NewL(MSearchListObserver& aObserver,CMainEngine& aMainEngine)
{
	CSearchListBox* self=CSearchListBox::NewLC(aObserver,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchListBox::ConstructL()
{
	iFlag=ETrue;
	this->InitEditor();
	this->InitListBox();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CSearchListBox::Draw(CGraphic& gc)const
{

	if(iListBox)
		this->iListBox->Draw(gc);

	DrawEditor(gc);

	if(iEdwin->IsFocused())
	{
		iMainEngine.GetBitmapFactory().DrawInputIconBmpByID(gc,iPoint,iMainEngine.GetInputType()-1);
	}
}

TBool CSearchListBox::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		if(iListBox->Count()>0&&iListBox->CurItemIndex()==0)
		{
			iEdwin->SetFocus(ETrue);
			iEdwin->MakeVisible(ETrue);
		}
		else
		{
			keyResult =iListBox->KeyEventL(aKeyCode);
		}
		break;
	case KKeyCodeDown:
		if(iListBox->Count()>0&&iListBox->CurItemIndex()==0)
		{
			iEdwin->SetFocus(EFalse);
			iEdwin->MakeVisible(EFalse);
		}
		keyResult =iListBox->KeyEventL(aKeyCode);
		break;
	case KOKKey:
		{
			if(iEdwin->IsFocused())
			{
				TBuf<25> tText;
				iEdwin->GetText(tText);

				if(tText.Length()==0)
				{
					iObserver.SearchListResponseEvent(1,iMainEngine.GetDesById(ETurkeyTextRes_InputNull));
				}
				else
				{
					iObserver.SearchListResponseEvent(0,tText);
				}
			}
			else
			{
				iObserver.SearchListResponseEvent(999,_L(""));
			}
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CSearchListBox::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CSearchListBox::SetEdwinVisible(TBool aBool)
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
TBool CSearchListBox::GetEdwinVisible()
{
	return iEdwin->IsFocused();
}
void CSearchListBox::SizeChanged(const TRect& /*aScreenRect*/)
{

}
void CSearchListBox::GetText(TDes& aDes)
{
	iEdwin->GetText(aDes);
}
void CSearchListBox::InitListBox()
{
	if(NULL == iListBox)
	{
		TRect tRect=iMainEngine.ScreenLayout().WhiteBackgroundRect();
		
		TInt fontHeight=iMainEngine.ScreenLayout().FontHeight();

		TPoint tPoint=tRect.iTl;
		tPoint.iY=tPoint.iY+fontHeight+fontHeight/2;

		TSize tSize=tRect.Size();

		tSize.iHeight=tSize.iHeight-fontHeight-fontHeight/2; 
		
		//ASSERT(0);	//待修改
		const CFont* font = iMainEngine.ScreenLayout().Font();
		TInt scrollBarWidth = iMainEngine.ScreenLayout().ScrollBarWidth();

		//iListBox = CListBox::NewL(iMainEngine,TRect(tPoint,tSize),font,scrollBarWidth);
		/*rect.SetHeight(tSize.iHeight);
		rect.SetWidth(tSize.iWidth);
		rect.iTl.iX=tPoint.iX;
		rect.iTl.iY=tPoint.iY;*/

		iListBox = new (ELeave) CListBox(iMainEngine,TRect(tPoint,tSize),font,scrollBarWidth);
		iListBox->SetTextColor(KRgbWhite,KRgbWhite);
		iListBox->Layout();


/*
		const CFont* font = iMainEngine.ScreenLayout().Font();
		iListBox = new (ELeave) CListBox(iMainEngine);
		iListBox->DeployL(TRect(tPoint,tSize),font,iMainEngine.ScreenLayout().ScrollBarWidth());
*/
	}
	else
	{
		iListBox->Reset();
	}

}


void CSearchListBox::InitEditor()
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

void CSearchListBox::AppendListItem(const TDesC& aText)
{
//	ASSERT(0);	//待修改 
	iListBox->AppendL(CNormalListBoxItem::NewL(aText));//add by hesanyuan
//	iListBox->AppendL(aText);
}
void CSearchListBox::AppendListItem(const TDesC& aText,const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask)
{
	iListBox->AppendL(CNormalListBoxItem::NewL(aText,aIcon,aIconMask));
}

void CSearchListBox::AppendListItem(const TDesC& aOneText,const TDesC& aTwoText,const TDesC& aThreeText,const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask)
{
	iListBox->AppendL(CCurThreeListBoxItem::NewL(aOneText,aTwoText,aThreeText,aIcon,aIconMask));
}

TInt CSearchListBox::GetSelectedIndex()
{
	return iListBox->CurItemIndex();
}

void CSearchListBox::DeleteListItem(TInt aIndex)
{
	iListBox->Remove(aIndex);
}
void CSearchListBox::ListBoxDraw()
{
	iListBox->Layout();
}

void CSearchListBox::DrawEditor(CGraphic& gc) const
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
}

TInt CSearchListBox::GetListCount()
{
	return iListBox->Count();
}
void CSearchListBox::PageUp()
{
	if(iListBox)
	{
		if(iListBox->Count()>iListBox->GetScreenMaxLine())
			iListBox->KeyEventL(KKeyCodePageUp);
	}
}
void CSearchListBox::PageDown()
{
	if(iListBox)
	{
		if(iListBox->Count()>iListBox->GetScreenMaxLine())
			iListBox->KeyEventL(KKeyCodePageDown);
	}
}