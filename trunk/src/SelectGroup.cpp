/*
============================================================================
 Name        : SelectGroup.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectGroup implementation
============================================================================
*/

#include "SelectGroup.h"

#include "TypeDefine.h"
#include "MainEngine.h"

#include "Graphic.h"
#include "ScreenLayout.h"
#include "CoCoPreDefine.h"

#include "SelectBox.h"
#include "Scrollbar.h"

CSelectGroup::CSelectGroup(CMainEngine& aMainEngine)
:CControl(ESelectBoxGroup)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iLineWidth(0)
,iIndex(-1)
,iFirstIndex(0)
,iSelectedIndex(0)
,iLineHeight(0)
,iScrollbar(NULL)
{
	// No implementation required
	iHasButton = ETrue;
}


CSelectGroup::~CSelectGroup()
{
	DELETE(iScrollbar);
	iSelectArray.Close();
	//delete iSelected;
}

CSelectGroup* CSelectGroup::NewLC(CMainEngine& aMainEngine)
{
	CSelectGroup* self = new (ELeave)CSelectGroup(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSelectGroup* CSelectGroup::NewL(CMainEngine& aMainEngine)
{
	CSelectGroup* self=CSelectGroup::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSelectGroup::ConstructL()
{

}
void CSelectGroup::SelectItemReset()
{
	iSelectArray.ResetAndDestroy();
	iFirstIndex=0;
	iSelectedIndex=0;
}
void CSelectGroup::InitLayout(TRect aRect)
{
	iRect=aRect;
	iStartPoint=aRect.iTl;

	iLineWidth = aRect.Width();

	iLineHeight=iMainEngine.ScreenLayout().FontHeight();

	iLineHeight+=iLineHeight>>1;

	iItemSize.iHeight=iLineHeight;
	iItemSize.iWidth=iLineWidth;

	iMaxLinePerPage = iMainEngine.ScreenLayout().GetClientRect().Height()/iLineHeight-1;
}
void CSelectGroup::AddSelectItem(const TDesC& aText,const TDesC& aName,TInt aType,TBool aBool)
{
	CSelectBox* selectBox=CSelectBox::NewL(iMainEngine,aType);
	selectBox->SetGroupName(aName);
	selectBox->SetText(aText);
	selectBox->SetSelected(aBool);
	iSelectArray.Append(selectBox);
	for(TInt i=0;i<iSelectArray.Count();i++)
	{
		if(iSelectArray[i]->GetType()!=ESelectText)
		{
			iSelectedIndex=i;
			break;
		}
	}
	LayoutScrollBar();
}
void CSelectGroup::AddTextItem(const TDesC& aText)
{
	TInt textPos = 0;
 	while(textPos < aText.Length())
	{	
		TInt charNum = iMainEngine.ScreenLayout().Font()->TextCount(aText.Mid(textPos),iLineWidth);
 		const TDesC& tmpDes = aText.Mid(textPos,charNum);

		CSelectBox* selectBox=CSelectBox::NewL(iMainEngine,ESelectText);
		selectBox->SetText(tmpDes);
		iSelectArray.Append(selectBox);

		textPos += charNum;
	}
	LayoutScrollBar();
}

TInt CSelectGroup::GetCount(const TDesC& aName)
{
	TInt num=0;
	for(TInt i=0;i<iSelectArray.Count();i++)
	{
		if(iSelectArray[i]->GetGroupName().Compare(aName)==0)
			num++;
	}
	return num;
}

TInt CSelectGroup::GetSelectRedio(const TDesC& aName)
{
	TInt index=-1;
	for(TInt i=0;i<iSelectArray.Count();i++)
	{
		if(iSelectArray[i]->GetGroupName().Compare(aName)==0)
		{
			index++;
			if(iSelectArray[i]->IsSelected())
			{
				break;
			}
		}
	}
	return index;
}

TBool CSelectGroup::GetSelectCheckBox(const TDesC& aName,TInt aIndex)
{
	TInt index=-1;

	TBool tBool=EFalse;

	for(TInt i=0;i<iSelectArray.Count();i++)
	{
		if(iSelectArray[i]->GetType()!=ESelectCheckBox)
			continue;

		if(iSelectArray[i]->GetGroupName().Compare(aName)==0)
		{
			index++;
			if(index==aIndex)
			{
				tBool=iSelectArray[i]->IsSelected();
				break;
			}
		}
	}
	return tBool;
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CSelectGroup::Draw(CGraphic& gc)const
{
	TInt index=0;
	for(TInt i=0;i<iMaxLinePerPage;i++)
	{
		index=i+iFirstIndex;
		if(index>=iSelectArray.Count())
			break;

		if(index==iSelectedIndex)
		{
			TRect rect(iStartPoint+TPoint(0,i*iLineHeight),iItemSize);
			gc.SetPenColor(KPopMenuRectColor);
			gc.SetPenStyle(CGraphicsContext::ESolidPen);
			gc.SetBrushColor(KNaviPaneSelectedItemColor);
			gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
			gc.DrawRect(rect);

			gc.SetBrushStyle(CGraphicsContext::ENullBrush);
			gc.SetPenColor(KRgbWhite);	
		}
		iSelectArray[index]->DrawItem(gc,TRect(iStartPoint+TPoint(0,i*iLineHeight),iItemSize));
	}
	if(iScrollbar)
		iScrollbar->Draw(gc);
}

TBool CSelectGroup::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		{
			ChangeUpSelectedIndex();
			break;
		}
	case KKeyCodeDown:
		{
			ChangeDownSelectedIndex();
			break;
		}
	case KOKKey:
		{
			CheckSelect();
			//iSelectArray[iSelectedIndex]->KeyEventL(aKeyCode);
			break;
		}
	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}
void CSelectGroup::CheckSelect()
{
	if(iSelectArray[iSelectedIndex]->GetType()==ESelectRedio)
	{
		for(TInt i=0;i<iSelectArray.Count()-1;i++)
		{
			if(iSelectArray[i]->GetGroupName().Compare(iSelectArray[iSelectedIndex]->GetGroupName())==0)
			{
				iSelectArray[i]->SetSelected(EFalse);
			}
		}
	}
	iSelectArray[iSelectedIndex]->KeyEventL(KOKKey);
}
TBool CSelectGroup::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CSelectGroup::SizeChanged(const TRect& /*aScreenRect*/)
{

}
const TDesC& CSelectGroup::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CSelectGroup::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSelectGroup::ChangeUpSelectedIndex()
{
	if(iSelectedIndex<=0)
		return;

	TInt index=iSelectedIndex;

	if(iSelectArray[index-1]->GetType()==ESelectText)
	{
		for(TInt i=0;i<index-iFirstIndex;i++)
		{
			if(iSelectArray[index-1-i]->GetType()!=ESelectText)
			{
				index=index-1-i;
				break;
			}
		}
	}
	else
	{
		index--;
	}

	
	if((index<=iFirstIndex||index==iSelectedIndex)&&iFirstIndex>0)
	{
		iFirstIndex--;

		if(iScrollbar)
		{
			iScrollbar->Up();
		}
	}	

	if(index>=iFirstIndex)
	{
		iSelectedIndex=index;
	}

}
void CSelectGroup::ChangeDownSelectedIndex()
{
	if(iSelectedIndex>=iSelectArray.Count()-1||iFirstIndex>=iSelectArray.Count()-1)
		return;

	TInt index=iSelectedIndex;

	if(iSelectArray[index+1]->GetType()==ESelectText)
	{
		for(TInt i=0;i<iSelectArray.Count()-index-1;i++)
		{
			if(iSelectArray[index+1+i]->GetType()!=ESelectText)
			{
				index=index+1+i;
				break;
			}
		}
	}
	else
	{
		index++;
	}

	if((index>=iFirstIndex+iMaxLinePerPage||iSelectedIndex==index)&&iFirstIndex<iSelectArray.Count()-iMaxLinePerPage)
	{
		iFirstIndex++;
		if(iScrollbar)
		{
			iScrollbar->Down();
		}
	}	

	if(index<=iFirstIndex+iMaxLinePerPage)
	{
		iSelectedIndex=index;
	}
}

//没有考虑组中已经有选中项的情况，使用时注意
TInt CSelectGroup::SetSelected(const TDesC& aGroupName,TInt aIndex)
{
	TInt index=-1;
	
	for(TInt i=0;i<iSelectArray.Count();i++)
	{
		if(iSelectArray[i]->GetGroupName().Compare(aGroupName)==0)
		{
			index++;
			if(index==aIndex)
			{
				iSelectArray[i]->SetSelected(true);
				return 0;//成功
			}
		}
	}

	return -1;//表示没有对应的index
}
void CSelectGroup::LayoutScrollBar()
{
	DELETE(iScrollbar);
	if(iSelectArray.Count()>iMaxLinePerPage)
	{
		iScrollbar = new (ELeave)CScrollBar;
		iScrollbar->DefaultLayout();
		iScrollbar->SetVerticalSpace(iRect.Height());
		iScrollbar->SetBeginPostion(iStartPoint+TPoint(iRect.Width()-3,0));
		iScrollbar->SetCursorColor(KScrollSpaceColor);
		iScrollbar->SetBackgroundColor(KScrollBackgroundColor);

		iScrollbar->SetMaxLinePerPage(iMaxLinePerPage);
		iScrollbar->SetTotalLineNum(iSelectArray.Count());
		iScrollbar->Layout();
	}
}