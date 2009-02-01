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

CPage::CPage(CPageBuilder* aPageBuilder,const TRect& aRect)
	: iPageBuilder(aPageBuilder)
	, iRect(aRect)
{
	iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
}

CPage::~CPage()
{
}

/*
CPage* CPage::NewLC()
{
	CPage* self = new (ELeave)CPage();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPage* CPage::NewL()
{
	CPage* self=CPage::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}
*/

void CPage::ConstructL()
{

}

void CPage::Draw(CGraphicsContext& aGc) const
{
	if(iPageBuilder)
	{
		TPoint point = iRect.iTl;
		point.iY += iTextHeight;
		point.iY -= iStartYPos*iTextHeight;

		aGc.UseFont(CCoeEnv::Static()->NormalFont());
		for ( int i = 0 ; i < iPageBuilder->ElementAmount() ; i++)
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
			/*
			if()	
			{

			}
			*/

			const CWidget& element = iPageBuilder->Element(i);

			switch(element.Type())
			{
			case CWidget::EText:
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
				break;

			case CWidget::EPicture:
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
				//point.iX += gc.
				break;

			case CWidget::EBr:
				point.iX = 0;
				point.iY += iTextHeight;
				break;

			default:
				ASSERT(FALSE);
				break;
			}
		}
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