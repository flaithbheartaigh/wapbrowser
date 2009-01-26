/*
============================================================================
 Name	: WapBrowserView.cpp
 Author	  : 浮生若茶
 Version	 :
 Copyright   : Your copyright notice
 Description : Application view
============================================================================
*/
#include "Define.h"
#include "WapBrowserAppView.h"
#include "PageBuilder.h"

CWapBrowserAppView* CWapBrowserAppView::NewL( const TRect& aRect )
{
	CWapBrowserAppView* self = CWapBrowserAppView::NewLC( aRect );
	CleanupStack::Pop( self );
	return self;
}

CWapBrowserAppView* CWapBrowserAppView::NewLC( const TRect& aRect )
{
	CWapBrowserAppView* self = new ( ELeave ) CWapBrowserAppView;
	CleanupStack::PushL( self );
	self->ConstructL( aRect );
	return self;
}

void CWapBrowserAppView::ConstructL( const TRect& aRect )
{
	CreateWindowL();
	SetRect( aRect );
	ActivateL();
}

CWapBrowserAppView::CWapBrowserAppView()
{
}

CWapBrowserAppView::~CWapBrowserAppView()
{
}


void CWapBrowserAppView::Draw( const TRect& /*aRect*/ ) const
{
	static const int KTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
	CWindowGc& gc = SystemGc();
	TRect drawRect( Rect());
	gc.Clear( drawRect );

	if(iPageBuilder)
	{
		TPoint point = drawRect.iTl;
		point.iY += KTextHeight;
		point.iY -= startYPos*KTextHeight;

		gc.UseFont(CCoeEnv::Static()->NormalFont());
		for ( int i = 0 ; i < iPageBuilder->ElementAmount() ; i++)
		{
			TBool inRegion = point.iY < drawRect.iBr.iY + KTextHeight;
			if(!inRegion)	//已经跑出屏幕下方，直接返回，不再绘制
			{
				return;
			}
			inRegion &= point.iY /*- KTextHeight*/ > drawRect.iTl.iY /*- KTextHeight*/;
/*
			if()	
			{

			}
*/

			const CElement& element = iPageBuilder->Element(i);

			switch(element.Type())
			{
			case CElement::EText:
				{
					const TDesC& text = ((CTextElement&)element).Text();

					int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);
					if(inRegion)
					{
						//TRect rect(point,TSize(width,textHeight));
						//gc.DrawRect(rect);

						if(element.Link().Length())		//带链接
						{
							gc.SetPenColor(KRgbBlue);

							//TODO：添加焦点的显示
							TPoint point1 = point;
							TPoint point2 = point1;
							point2.iX += width;
							gc.SetPenColor(KRgbBlue);
							gc.DrawLine(point1,point2);
						}
						else							//不带链接
						{
							gc.SetPenColor(KRgbBlack);
						}
						gc.DrawText(text,point);
					}
					point.iX += width;
				}
				break;

			case CElement::EPicture:
				{
					const TDesC& text = ((CPictureElement&)element).Alt();

					int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);
					if(inRegion)
					{
						TRect rect(point,TSize(width,KTextHeight));
						rect.Move(TPoint(0,-KTextHeight));

						gc.SetPenColor(KRgbYellow);
						gc.DrawRect(rect);

						gc.SetPenColor(KRgbBlue);
						gc.DrawText(text,point);
					}

					point.iX += width;
				}
				//point.iX += gc.
				break;

			case CElement::EBr:
				point.iX = 0;
				point.iY += KTextHeight;
				break;

			default:
				ASSERT(FALSE);
				break;
			}
		}
	}
}

void CWapBrowserAppView::SizeChanged()
{  
	DrawNow();
}

TKeyResponse CWapBrowserAppView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse response = EKeyWasNotConsumed;
	if(EEventKey == aType)
	{
		response =  EKeyWasConsumed;
		switch(aKeyEvent.iCode)
		{
		case EKeyDevice0:
			break;

		case EKeyUpArrow:
			if(startYPos > 0)
			{
				startYPos--;
			}
			break;

		case EKeyDownArrow:
			if(1)
			{
				startYPos++;
			}
			break;

		default:
			response = EKeyWasNotConsumed;
			break;
		}

		if(EKeyWasConsumed == response)
		{
			DrawNow();
		}
	}
	return response;
}

void CWapBrowserAppView::ShowPage(CPageBuilder* aPageBuilder)
{
	iPageBuilder = aPageBuilder;
	DrawNow();
}