/*
============================================================================
 Name        : TurkeyListBox.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CListBox implementation
============================================================================
*/
//#include <badesca.h>
//#include ""

#include "ListBox.h"
#include "Scrollbar.h"
#include "SlideText.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "MainEngine.h"
#include "PreDefine.h"

_LIT(KTEXTBACK,"...");



CListBox::CListBox(MDrawEventObserver& aObserver,const TRect& aRect,const CFont* aFont,TInt aScrollWidth)  : CControl(EListBox)
																										   , iClientRect(aRect)
																										   , iFont(aFont)
																										   , iScrollWidth(aScrollWidth)
																										   , iAlreadyLayout(EFalse)
																										   , iTextColor(KRgbWhite)
																										   , iTextColorWhenFocus(KRgbWhite)
																										   , iUseHotKey(ETrue)
{
	SetDrawEventObserver(&aObserver);
	CalculateSize();
}

CListBox::~CListBox()
{
	iItemArray.ResetAndDestroy();
	iItemArray.Close();
	delete iScrollBar;
	iScrollBar = NULL;
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CListBox::Draw(CGraphic& gc) const
{
	if(iAlreadyLayout)
	{
		Assert();

		TPoint point = iClientRect.iTl;

		gc.SetPenStyle(CGraphicsContext::ESolidPen);
		gc.SetPenColor(iTextColor);

		for (TInt i = 0 ; i < iMaxLinePerPage && (i + iScrollBar->CurIndex()) < iItemArray.Count(); i++)
		{
			TInt index = i + iScrollBar->CurIndex();
			ASSERT(index >= 0 && index < iItemArray.Count());
			const CListBoxItem* item = iItemArray[index];
			item->Draw(gc,point);
		}
		iScrollBar->Draw(gc);
	}
}

TBool CListBox::KeyEventL(TInt aKeyCode)
{
	if(!iAlreadyLayout)
	{
		return EFalse;
	}
	Assert();

	TInt keyResult = ETrue;
	switch(aKeyCode) 
	{
	case KKeyCodeUp:
		DeactivateCurItem();
		if(iCurScreenIndex > 0)
		{
			--iCurScreenIndex;
		}
		else
		{
			keyResult = iScrollBar->Up();
		}
		ActiveCurItem();
		break;

	case KKeyCodeDown:
		DeactivateCurItem();
		if(iCurScreenIndex < iMaxLinePerPage - 1 && iCurScreenIndex + iScrollBar->CurIndex() < iItemArray.Count() - 1)
		{
			++iCurScreenIndex;
		}
		else
		{
			keyResult = iScrollBar->Down();
		}
		ActiveCurItem();
		break;

	case KKeyCodePageUp:
		if(iUseHotKey)
		{
			DeactivateCurItem();
			iCurScreenIndex = 0;
			keyResult = iScrollBar->PageUp();
			ActiveCurItem();
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	case KKeyCodePageDown:
		if(iUseHotKey)
		{
			DeactivateCurItem();
			if(iItemArray.Count() > iMaxLinePerPage)
			{
				iCurScreenIndex = iMaxLinePerPage - 1;
			}
			else
			{
				iCurScreenIndex = iItemArray.Count() - 1;
			}
			keyResult = iScrollBar->PageDown();
			ActiveCurItem();
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	case KKeyCodeHome:
		if(iUseHotKey)
		{
			DeactivateCurItem();
			iCurScreenIndex = 0;
			keyResult = iScrollBar->Home();
			ActiveCurItem();
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	case KKeyCodeEnd:
		if(iUseHotKey)
		{
			DeactivateCurItem();
			if(iItemArray.Count() > iMaxLinePerPage)
			{
				iCurScreenIndex = iMaxLinePerPage - 1;
			}
			else
			{
				iCurScreenIndex = iItemArray.Count() - 1;
			}
			keyResult = iScrollBar->End();
			ActiveCurItem();
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return iUseHotKey ? keyResult : EFalse;
	//return keyResult;
}

TBool CListBox::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CListBox::SizeChanged(const TRect& /*aScreenRect*/)
{

}

void CListBox::ActivateL()
{
	if(0 < iItemArray.Count())
	{
		iItemArray[CurItemIndex()]->SetActive(ETrue);
	}
}

void CListBox::Deactivate()
{
	if(0 < iItemArray.Count())
	{
		iItemArray[CurItemIndex()]->SetActive(EFalse);
	}
}
//////////////////////////////////////////////////////////////////////////
//From MDrawEventObserver
//////////////////////////////////////////////////////////////////////////
void CListBox::ControlRequestDraw()
{
	RequestDraw();
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CListBox::AppendL(CListBoxItem* aItem)
{
	aItem->SetParent(this);
	aItem->Init(iItemSize,iFont);
	aItem->SetActive(EFalse);
	aItem->SetTextColor(iTextColor);
	iItemArray.Append(aItem);
	if(iItemArray.Count() == 1)
	{
		CListBoxItem* item = iItemArray[0];
		iMaxLinePerPage -= (item->LineNumWhenFocus() - item->LineNum());
	}
	Layout();
}

//////////////////////////////////////////////////////////////////////////
//CListBox::Remove
//删除一�?
//删除的项有可能不是当前项
//////////////////////////////////////////////////////////////////////////
void CListBox::Remove(TInt aIndex)
{
	ASSERT(aIndex >= 0 && aIndex < iItemArray.Count());
	//ASSERT(iCurScreenIndex + iScrollBar->CurIndex() == aIndex);

	//如果只有一项的时候删除应该怎么处理?
	if(iItemArray.Count() == 1)
	{
		iCurScreenIndex = 0;
		delete iItemArray[aIndex];
		iItemArray.Remove(aIndex);
	}
	else
	{
		TInt curIndex = CurItemIndex();
		if(aIndex == curIndex)		//删除当前�?
		{
			if(curIndex == iItemArray.Count() - 1)	//当前项为最后一�?
			{
				iScrollBar->Up();
				curIndex--;
				iCurScreenIndex = curIndex - iScrollBar->CurIndex();
				ASSERT(iCurScreenIndex>= 0 && iCurScreenIndex < iMaxLinePerPage);
			}
			else
			{
				//不用处理
			}
		}
		else if(aIndex > curIndex)
		{
			//不用处理
		}
		else	//aIndex < curIndex
		{
			iScrollBar->Up();
			curIndex--;
			iCurScreenIndex = curIndex - iScrollBar->CurIndex();
			ASSERT(iCurScreenIndex>= 0 && iCurScreenIndex < iMaxLinePerPage);
		}
		delete iItemArray[aIndex];
		iItemArray.Remove(aIndex);
		if (iMaxLinePerPage + iScrollBar->CurIndex() > iItemArray.Count())
		{
			if(iScrollBar->CurIndex() > 0)
			{
				iScrollBar->Up();
				iCurScreenIndex++;
			}
		}
		curIndex = CurItemIndex();
		if(curIndex >= 0 && curIndex < iItemArray.Count())
		{
			iItemArray[curIndex]->SetActive(ETrue);
		}		
	}

/*
	if (CurItemIndex() == aIndex)
	{
	}
	else if(CurItemIndex() < aIndex)
	{

	}
	else//CurItemIndex() < aIndex
	{

	}
	if(aIndex == iItemArray.Count() - 1)		//删除最后一�?上移一�?
	{
// 		if(iScrollBar->CurIndex() == 0)
// 		{
// 
// 		}
		iCurScreenIndex--;
	}
	else
	{

	}
*/

	//TInt curIndex = iCurScreenIndex + iScrollBar->CurIndex();

/*
	iScrollBar->Up();
	if(iItemArray.Count() - 1 == CurItemIndex())	//在最后一�?上移一�?
	{
		//curIndex--;
	}
	else									//否则保留原有索引
	{
		iScrollBar->Up();
		iCurScreenIndex++;
	}

*/
	//curIndex = iCurScreenIndex + iScrollBar->CurIndex();
/*
	if(iCurScreenIndex > 0)
	{
		iCurScreenIndex--;
	}
	else
	{
		iScrollBar->Up();	//这里可能有问�?
	}
*/

	//TInt curIndex = iCurScreenIndex + iScrollBar->CurIndex();


	LayoutScrollBar();
	RequestDraw();
	ASSERT(iCurScreenIndex >= 0);
}

/*
void CListBox::RemoveCurItem()
{

}
*/

void CListBox::Reset()
{
	CalculateSize();
	iItemArray.ResetAndDestroy();
	iCurScreenIndex = 0;
	iScrollBar->Home();
	LayoutScrollBar();
}

TInt CListBox::Count()const
{
	return iItemArray.Count();
}

TInt CListBox::CurItemIndex()const
{
	TInt curIndex = iCurScreenIndex + iScrollBar->CurIndex();
	ASSERT(curIndex >= 0 && curIndex < iItemArray.Count());
	return curIndex;
}

CListBoxItem& CListBox::GetItem(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iItemArray.Count());
	CListBoxItem* item = iItemArray[aIndex];
	ASSERT(item);
	return *item;
}

CListBoxItem& CListBox::CurItem() const
{
	CListBoxItem* item = iItemArray[CurItemIndex()];
	ASSERT(item);
	return *item;
}

CScrollBar* CListBox::ScrollBar()const
{
	ASSERT(iScrollBar);
	return iScrollBar;
}

void CListBox::SetTextColor(const TRgb& aTextColor,const TRgb& aTextColorWhenFocus)
{
	iTextColor = aTextColor;
	iTextColorWhenFocus = aTextColorWhenFocus;
}

void CListBox::Layout()
{

	if(iItemArray.Count() > 0 )
	{
		iItemArray[0]->SetActive(ETrue);
		iItemArray[0]->SetTextColor(iTextColorWhenFocus);
	}
	LayoutScrollBar();
	iAlreadyLayout = ETrue;
}

void CListBox::SetUseHotKey(TBool aUseHotKey)
{
	iUseHotKey = aUseHotKey;
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CListBox::CalculateSize()
{
	iLineHeight = iFont->HeightInPixels();
	iLineHeight += (iLineHeight>>1);		//行高定为字体高度�?.5�?

	iMaxLinePerPage = iClientRect.Height()/iLineHeight;
	iItemSize.iWidth = iClientRect.Width() - iScrollWidth;
	iItemSize.iHeight = iLineHeight;

}

void CListBox::ActiveCurItem()
{
	if(iItemArray.Count() > 0)
	{
		TInt curIndex = iScrollBar->CurIndex() + iCurScreenIndex;
		ASSERT(curIndex >= 0 && curIndex < iItemArray.Count());
		iItemArray[curIndex]->SetActive(ETrue);
		iItemArray[curIndex]->SetTextColor(iTextColorWhenFocus);
	}
}

void CListBox::DeactivateCurItem()
{
	if(iItemArray.Count() > 0)
	{
		TInt curIndex = iScrollBar->CurIndex() + iCurScreenIndex;
		ASSERT(curIndex >= 0 && curIndex < iItemArray.Count());
		iItemArray[curIndex]->SetActive(EFalse);
		iItemArray[curIndex]->SetTextColor(iTextColor);
	}
}

void CListBox::LayoutScrollBar()
{
	if(NULL == iScrollBar)
	{
		iScrollBar = new (ELeave)CScrollBar;

		TPoint scrollPoint = iClientRect.iTl;
		scrollPoint.iX += iClientRect.Width();
		scrollPoint.iX -= iScrollWidth;
		iScrollBar->SetBeginPostion(scrollPoint);
		iScrollBar->SetVerticalSpace(iClientRect.Height());
		iScrollBar->SetScrollWidth(iScrollWidth);
		iScrollBar->SetBackgroundColor(KScrollBackgroundColor);
		iScrollBar->SetCursorColor(KScrollSpaceColor);
		iScrollBar->SetMaxLinePerPage(iMaxLinePerPage);
	}
	iScrollBar->SetTotalLineNum(iItemArray.Count());
	iScrollBar->Layout();
}

void CListBox::Assert() const
{
	//ASSERT(iAlreadyLayout);
	ASSERT(iScrollBar);
}