/*
============================================================================
 Name        : Page.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPage implementation
============================================================================
*/
#include "Define.h"
#include "Page.h"
#include "PageBuilder.h"
#include "Widgets.h"
#include "ScrollBar.h"

/*
//////////////////////////////////////////////////////////////////////////
CWidgetGroup
//////////////////////////////////////////////////////////////////////////
*/
class CWidgetGroup : public CBase
{
public:
	CWidgetGroup()
		: iFocusIndex(-1)
	{
		iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
	}

	~CWidgetGroup()
	{
		iWidgetArray.ResetAndDestroy();
	}

	void AddWidget(CWidget* aWidget)
	{
		if(-1 == iFocusIndex && aWidget->Link().Length() > 0)
		{
			iFocusIndex = iWidgetArray.Count();			 
		}
		iWidgetArray.Append(aWidget);
	}

	CWidget* FocusWidget() const
	{
		if(iFocusIndex >= 0 && iFocusIndex < iWidgetArray.Count())
		{
			return iWidgetArray[iFocusIndex];
		}
		return NULL;
	}

	void Draw(CGraphicsContext& aGc) const
	{
		TPoint point = iPoint;
		for ( int i = 0 ; i < iWidgetArray.Count() ; i++)
		{
			CWidget* widget = iWidgetArray[i];

			widget->SetPoint(point);
			widget->SetTextHeight(iTextHeight);

			if(iFocus && i == iFocusIndex)
			{
				aGc.SetBrushColor(CGraphicsContext::ESolidBrush);
				aGc.SetBrushColor(KRgbYellow);
				TRect rect(point - TPoint(0,iTextHeight),widget->Size());
				aGc.DrawRect(rect);
				aGc.SetBrushColor(CGraphicsContext::ENullBrush);
			}

			widget->Draw(aGc);
			widget->Move(point);
		}
	}

	void Left()		//向左移动焦点
	{
		for (int i = iFocusIndex - 1 ; i >= 0 ; i--)
		{
			CWidget* widget = iWidgetArray[i];
			if(widget->Link().Length() > 0)
			{
				iFocusIndex = i;
				break;
			}
		}
	}

	void Right()		//向右移动焦点
	{
		for (int i = iFocusIndex + 1 ; i < iWidgetArray.Count() ; i++)
		{
			CWidget* widget = iWidgetArray[i];
			if(widget->Link().Length() > 0)
			{
				iFocusIndex = i;
				break;
			}
		}
	}

	TBool HasFocus()	//是否有焦点项
	{
		return -1 != iFocusIndex;
	}

	void SetPoint(const TPoint& aPoint)
	{
		iPoint = aPoint;
	}

	TBool SetFocus(TBool aFocus)
	{
		if(-1 != iFocusIndex)
		{
			iFocus = aFocus;
		}
		return EFalse;
	}

	int Height()
	{
		return iTextHeight;	//TODO:高度不一定是相同的
	}

private:
	RPointerArray<CWidget> iWidgetArray;
	TPoint iPoint;
	int iFocusIndex;
	int iTextHeight;
	TBool iFocus;
};
/*
//////////////////////////////////////////////////////////////////////////
CPage
//////////////////////////////////////////////////////////////////////////
*/
CPage::CPage()
	: iFocusIndex(-1)
{
	iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
}

CPage::~CPage()
{
	ASSERT(NULL == iWidgetGroup);
	iWidgetGroupArray.ResetAndDestroy();
	delete iScrollBar;
}

void CPage::ConstructL()
{

}

void CPage::Draw(CGraphicsContext& aGc) const
{
	ASSERT(iLayout);

	TPoint point = iRect.iTl;
	point.iY += iTextHeight;

	aGc.UseFont(CCoeEnv::Static()->NormalFont());

	for (int i = iStartIndex ; i < iEndIndex && i < iWidgetGroupArray.Count() ; i++)
	{
		CWidgetGroup* wg = iWidgetGroupArray[i];
		wg->SetPoint(point);
		wg->Draw(aGc);
		point.iY += iTextHeight;
	}
	ASSERT(iScrollBar);
	iScrollBar->Draw(aGc);
}

TBool CPage::KeyEvent(int aKeyCode)
{
	ASSERT(iLayout);
	TBool result = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
		break;

	case EKeyLeftArrow:
		if(CurWidgetGroup())
		{
			CurWidgetGroup()->Left();
		}
		break;

	case EKeyRightArrow:
		if(CurWidgetGroup())
		{
			CurWidgetGroup()->Right();
		}
		break;

	case EKeyUpArrow:
		//TODO:目录无法适应多种控制方式
		{
			TBool focusChanged = EFalse;
			ASSERT(iEndIndex <= iWidgetGroupArray.Count());
			if(iFocusIndex > iStartIndex)
			{
				//TODO:查找算法应该换成更快捷的库算法
				for (int i = iFocusIndex - 1 ; i >= iStartIndex ; i--)
				{
					CWidgetGroup* wg = iWidgetGroupArray[i];
					if(wg->HasFocus())
					{
						focusChanged = ETrue;
						iWidgetGroupArray[iFocusIndex]->SetFocus(EFalse);
						iFocusIndex = i;
						iWidgetGroupArray[iFocusIndex]->SetFocus(ETrue);
						break;
					}
				}
			}
			if(!focusChanged)
			{
				if(iStartIndex > 0)
				{
					//TODO:等量增减不适合元素大小不同的情况
					iStartIndex--;
					iEndIndex--;
					iScrollBar->Up();
				}
			}
		}
		break;

	case EKeyDownArrow:
		//if(iFocusIndex < iEndIndex)
		{
			TBool focusChanged = EFalse;
			if(iFocusIndex < iEndIndex - 1)
			{
				ASSERT(iEndIndex <= iWidgetGroupArray.Count());
				//TODO:查找算法应该换成更快捷的库算法
				for (int i = iFocusIndex + 1 ; i < iEndIndex && i < iWidgetGroupArray.Count() ; i++)
				{
					CWidgetGroup* wg = iWidgetGroupArray[i];
					if(wg->HasFocus())
					{
						focusChanged = ETrue;
						iWidgetGroupArray[iFocusIndex]->SetFocus(EFalse);
						iFocusIndex = i;
						iWidgetGroupArray[iFocusIndex]->SetFocus(ETrue);
						break;
					}
				}
			}
			if(!focusChanged)
			{
				if(iEndIndex <= iWidgetGroupArray.Count())	//TODO:<=会导致越界，但<会导致最后一行画不全
				{
					//TODO:等量增减不适合元素大小不同的情况
					iStartIndex++;
					iEndIndex++;
					iScrollBar->Down();
				}
			}
		}
		break;

	default:
		result = EFalse;
		break;
	}
	return result;
}

void CPage::SetRect(const TRect& aRect)
{
	iRect = aRect;
}

void CPage::AddWidget(CWidget* aWidget)
{
	if(NULL == iWidgetGroup)
	{
		iWidgetGroup = new CWidgetGroup;
	}
	iWidgetGroup->AddWidget(aWidget);
}

void CPage::Br()
{
	AddGroup();
}

void CPage::Layout()
{
	ASSERT(!iLayout);
	AddGroup();

	//TODO:计算布局
	iLayout = ETrue;

	if(NULL == iScrollBar)
	{
		iScrollBar = new CScrollBar;
		iScrollBar->SetSize(iRect.Width(),iRect.Height());
		iScrollBar->SetScrollWidth(5);
		iScrollBar->SetBeginPostion(TPoint(iRect.Width()-10,0));
		iScrollBar->SetVerticalSpace(iRect.Height());
		iScrollBar->SetBackgroundColor(KScrollBackgroundColor);
		iScrollBar->SetCursorColor(KScrollSpaceColor);
	}
	//iScrollBar->SetMaxLinePerPage(iRect.Height());		//TODO:
	//iScrollBar->SetMaxLinePerPage(10);		//TODO:
	//iScrollBar->SetTotalLineNum(iTotalHeight);

 	iScrollBar->SetMaxLinePerPage(10);		//TODO:这个值应该通过计算得到
 	iScrollBar->SetTotalLineNum(iWidgetGroupArray.Count());

	iScrollBar->Layout();
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CPage::AddGroup()
{
	if(iWidgetGroup)
	{
		if(-1 == iFocusIndex && iWidgetGroup->HasFocus())
		{
			iWidgetGroup->SetFocus(ETrue);
			iFocusIndex = iWidgetGroupArray.Count();
		}
		if(-1 == iStartIndex)
		{
			iStartIndex = iWidgetGroupArray.Count();
		}
		iTotalHeight += iWidgetGroup->Height();

		iWidgetGroupArray.Append(iWidgetGroup);
		iWidgetGroup = NULL;

		if(iTotalHeight < iRect.Height())
		{
			iEndIndex = iWidgetGroupArray.Count() + 1;		//TODO:这里iEndIndex有可能超出iWidgetGroupArray.Count()
		}
	}
}

CWidgetGroup* CPage::CurWidgetGroup() const
{
	if(iFocusIndex >= 0 && iFocusIndex < iWidgetGroupArray.Count())
	{
		CWidgetGroup* wg = iWidgetGroupArray[iFocusIndex];
		return wg;
	}
	return NULL;
}