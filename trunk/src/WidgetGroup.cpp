/*
============================================================================
 Name        : WidgetGroup.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWidgetGroup implementation
============================================================================
*/

#include "WidgetGroup.h"

CWidgetGroup::CWidgetGroup()
	: iFocusIndex(-1)
{
	iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
}

CWidgetGroup::~CWidgetGroup()
{
	iWidgetArray.ResetAndDestroy();
}

void CWidgetGroup::AddWidget(CWidget* aWidget)
{
	if(-1 == iFocusIndex && aWidget->Link().Length() > 0)
	{
		iFocusIndex = iWidgetArray.Count();		
	}
	iWidgetArray.Append(aWidget);
}

/*
CWidget* FocusWidget() const
{
if(iFocusIndex >= 0 && iFocusIndex < iWidgetArray.Count())
{
return iWidgetArray[iFocusIndex];
}
return NULL;
}*/


void CWidgetGroup::Draw(CGraphicsContext& aGc) const
{
	TPoint point = iPoint;
	for ( int i = 0 ; i < iWidgetArray.Count() ; i++)
	{
		CWidget* widget = iWidgetArray[i];

		widget->Draw(aGc);


		/*
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
		*/

	}
}

void CWidgetGroup::Execute()
{
	const TDesC8& link = FocusWidget()->Link();
	if(link.Length())
	{
		CWapBrowserAppUi::Static()->RequestPageL(link);
	}
}

void CWidgetGroup::Left()			//向左移动焦点
{
	for (int i = iFocusIndex - 1 ; i >= 0 ; i--)
	{
		CWidget* widget = iWidgetArray[i];
		if(widget->SetActive(ETrue))
		{
			iFocusWidget->SetActive(EFalse);
			iFocusWidget = widget;
			iFocusIndex = i;
			break;
		}
		/*
		if(widget->Link().Length() > 0)
		{
		//CurWidget()->SetActive(FALSE);
		iFocusIndex = i;	
		//CurWidget()->SetActive(ETrue);
		break;
		}
		*/
	}
}

void CWidgetGroup::Right()		//向右移动焦点
{
	for (int i = iFocusIndex + 1 ; i < iWidgetArray.Count() ; i++)
	{
		CWidget* widget = iWidgetArray[i];
		if(widget->SetActive(ETrue))
		{
			iFocusWidget->SetActive(EFalse);
			iFocusWidget = widget;
			iFocusIndex = i;
			break;
		}
		/*
		if(widget->Link().Length() > 0)
		{
		//CurWidget()->SetActive(FALSE);
		iFocusIndex = i;	
		//CurWidget()->SetActive(ETrue);
		break;
		}
		*/

	}
}

TBool CWidgetGroup::HasFocus()	//是否有焦点项
{
	return -1 != iFocusIndex;
}

void CWidgetGroup::SetPoint(const TPoint& aPoint)
{
	iPoint = aPoint;	//TODO:iPoint可以不用
	TPoint point = aPoint;
	for ( int i = 0 ; i < iWidgetArray.Count() ; i++)
	{
		CWidget* widget = iWidgetArray[i];

		widget->SetPoint(point);
		widget->Move(point);
	}
}

/*
TBool SetFocus(TBool aFocus)
{
if(-1 != iFocusIndex)
{
iFocus = aFocus;
}
return EFalse;
}
*/

int CWidgetGroup::Height()
{
	int height = 0;
	for (int i = 0 ; i < iWidgetArray.Count() ; i++)
	{
		CWidget* w = iWidgetArray[i];
		int wHeight = w->Size().iHeight;
		if(wHeight > height)
		{
			height = wHeight;
		}
	}
	return height;
	//return iTextHeight;	//TODO:高度不一定是相同的
}

CWidget* CWidgetGroup::FocusWidget()
{
	return iFocusWidget;
	/*
	if(iFocusIndex >= 0 && iFocusIndex < iWidgetArray.Count())
	{
	return iWidgetArray[iFocusIndex];
	}
	return NULL;
	*/
}

const CWidget* CWidgetGroup::FocusWidget() const
{
	return iFocusWidget;
	/*
	if(iFocusIndex >= 0 && iFocusIndex < iWidgetArray.Count())
	{
	return iWidgetArray[iFocusIndex];
	}
	return NULL;
	*/

}

TBool CWidgetGroup::SetActive(TBool aActive)
{
	if(!iFocusWidget)
	{
		for (int i = 0 ; i < iWidgetArray.Count() ; i++)
		{
			CWidget* widget = iWidgetArray[i];
			if(widget->SetActive(aActive))
			{
				iFocusWidget = widget;
				iFocusIndex = i;
				return ETrue;
			}
		}
		return EFalse;
	}
	else
	{
		if(FocusWidget())
		{
			return FocusWidget()->SetActive(aActive);
		}
		return ETrue;
	}
}