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

CPage::CPage()
{
	iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
}

CPage::~CPage()
{
}

void CPage::ConstructL()
{

}

void CPage::Draw(CGraphicsContext& aGc) const
{
	TPoint point = iRect.iTl;
	point.iY += iTextHeight;
	point.iY -= iStartYPos*iTextHeight;

	aGc.UseFont(CCoeEnv::Static()->NormalFont());

	for ( int i = 0 ; i < iWidgetArray.Count() ; i++)
	{
		//////////////////////////////////////////////////////////////////////////
		//范围控制
		TBool inRegion = point.iY < iRect.iBr.iY + iTextHeight;
		if(!inRegion)	//已经跑出屏幕下方，直接返回，不再绘制
		{
			return;
		}
		inRegion &= point.iY /*- KTextHeight*/ > iRect.iTl.iY /*- KTextHeight*/;
		//////////////////////////////////////////////////////////////////////////

		CWidget* widget = iWidgetArray[i];

		widget->SetPoint(point);
		widget->SetInRegion(inRegion);
		widget->SetTextHeight(iTextHeight);
		switch(widget->Type())
		{
		case CWidget::EText:
			//DrawText(*widget,inRegion,aGc,point);
			{
				CTextWidget* w = (CTextWidget*)widget;
				w->Draw(inRegion, aGc, point);
			}
			
			break;

		case CWidget::EPicture:
			//DrawPicture(*widget,inRegion,point,aGc);
			{
				CPictureWidget* w = (CPictureWidget*)widget;
				//w->SetTextHeight(iTextHeight);
				w->Draw(inRegion, point, aGc);
			}

			break;

		case CWidget::EBr:
// 			point.iX = 0;
// 			point.iY += iTextHeight;
			break;

		default:
			ASSERT(FALSE);
			break;
		}


		widget->Move(point);
// 		point.iX += widget->Size().iWidth;
// 		point.iY += widget->Size().iHeight;
// 
// 		widget

	}
}

TBool CPage::KeyEvent(int aKeyCode)
{
	TBool result = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
		break;

	case EKeyLeftArrow:
		break;

	case EKeyRightArrow:
		break;

	case EKeyUpArrow:
		if(iStartYPos > 0)
		{
			iStartYPos--;
		}
		break;

	case EKeyDownArrow:
		if(1)
		{
			iStartYPos++;
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
	iWidgetArray.Append(aWidget);
}
/*
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CPage::DrawText( const CWidget&element, TBool inRegion, CGraphicsContext &aGc, TPoint &point ) const
{
	const TDesC& text = ((CTextWidget&)element).Text();

	int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);
	if(inRegion)
	{
		//TRect rect(point,TSize(width,textHeight));
		//gc.DrawRect(rect);

		if(element.Link().Length())		//带链接
		{
			aGc.SetPenColor(KRgbBlue);

			//TODO：添加焦点的显示
			TPoint point1 = point;
			TPoint point2 = point1;
			point2.iX += width;
			aGc.SetPenColor(KRgbBlue);
			aGc.DrawLine(point1,point2);
		}
		else							//不带链接
		{
			aGc.SetPenColor(KRgbBlack);
		}
		aGc.DrawText(text,point);
	}
	point.iX += width;
}

void CPage::DrawPicture( const CWidget&element, TBool inRegion, TPoint &point, CGraphicsContext &aGc ) const
{
	const TDesC& text = ((CPictureWidget&)element).Alt();

	int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);
	if(inRegion)
	{
		TRect rect(point,TSize(width,iTextHeight));
		rect.Move(TPoint(0,-iTextHeight));

		aGc.SetPenColor(KRgbYellow);
		aGc.DrawRect(rect);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(text,point);
	}

	point.iX += width;
	}
	*/