/*
============================================================================
 Name        : NaviPane.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CNaviPane implementation
============================================================================
*/
#include <coemain.h>

#include "NaviPane.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "graphic.h"

#include "NaviPaneObserver.h"

#include "UtilityTools.h"

#include "TypeDefine.h"
#include "cocobmp.h"
#include "BitmapFactory.h"
// #include "PreDefine.h"

_LIT(KListSeparator,		"|");
#define KArrowImageWidth 8	//该值无法通过图片计算

CNaviPane::CNaviPane(const TRect& aRect,const CFont& aFont,CMainEngine& aMainEngine,MNaviPaneObserver* aNaviPaneObserver) : CControl(ENaviPane)
																														  , iNaviPaneRect(aRect)
																														  , iFont(aFont)
																														  , iMainEngine(aMainEngine)
																														  , iObserver(aNaviPaneObserver)
{
}

CNaviPane::~CNaviPane()
{
	iCountArray.Close();
	delete iNaviTexts;

	delete iTitleBitmap;

}

void CNaviPane::ConstructL()
{
	iNaviTexts = new(ELeave)CDesCArrayFlat(1);
	InitBitmap();
	SetArrowIcon();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CNaviPane::Draw(CGraphic& gc)const
{
	UtilityTools::WriteLogsL(_L("CNaviPane::Draw"));
	ASSERT(iNaviTexts);
	ASSERT(iPageIndex >= 0 && iPageIndex < iCountArray.Count());

	DrawBackground(gc);

	TPoint point = iTextBeginPos;
	for (TInt i = 0 ; i < iCountArray[iPageIndex] ; i++)
	{
		TInt index = iThisPageBeginIndex + i;

		DrawEachItem(gc,point,index);

		if(i < iCountArray[iPageIndex] - 1)
		{
			DrawDivision(gc,point);
		}
	}
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);

	DrawArrow(gc);
	UtilityTools::WriteLogsL(_L("CNaviPane::Draw End"));
}

TBool CNaviPane::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyLeftArrow:
		ArrowLeft();
		break;

	case EKeyRightArrow:
		ArrowRight();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	if(keyResult && iObserver)
	{
		iObserver->NaviPaneIndexChanged(iIndex);
	}
	return keyResult;
}

TBool CNaviPane::HandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CNaviPane::SizeChanged(const TRect& aScreenRect)
{
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CNaviPane::SetRect(const TRect& aRect)
{
	iNaviPaneRect = aRect;
}

void CNaviPane::AppendItem(const TDesC& aDes)
{
	ASSERT(iNaviTexts);
	iNaviTexts->AppendL(aDes);
	Layout();
}

void CNaviPane::Layout()
{	
	iIndex = 0;
	iPageIndex = 0;
	iThisPageBeginIndex = 0;
	//令文字上下居中
	iTextBeginPos = iNaviPaneRect.iTl;
	TSize naviPaneSize = iNaviPaneRect.Size();
	iFontHeight = iFont.HeightInPixels();
	//若所有项无法在一屏显示,则绘制箭头
	TInt divisionWidth = iFont.MeasureText(KListSeparator);
	TInt totalWidth = 0;
	for (TInt i = 0 ; i < iNaviTexts->Count() ; i++)
	{
		totalWidth += iFont.MeasureText((*iNaviTexts)[i]) ;
		totalWidth += divisionWidth;
	}
	totalWidth -= divisionWidth;

	iCountArray.Reset();
	//判断一屏是否能够显示所有的项
	if(totalWidth <= naviPaneSize.iWidth)				
	{
		iDividePage = EFalse;
		iCountArray.Append(iNaviTexts->Count());
	}
	else	
	{
		//计算每屏显示多少项
		iTextBeginPos.iX += iArrowImageWidth;
		TSize availableSize = naviPaneSize;
		availableSize.iWidth -= (iArrowImageWidth<<1);

		totalWidth = 0;
		TInt countPerPage = 0;		//每一页显示的项数
		for (TInt i = 0 ; i < iNaviTexts->Count() ; i++)
		{
			TInt textWidth = iFont.MeasureText((*iNaviTexts)[i]) ;
			totalWidth += textWidth;
			totalWidth += divisionWidth;
			if(totalWidth - divisionWidth > availableSize.iWidth)
			{
				totalWidth -= textWidth;
				totalWidth -= divisionWidth;
				totalWidth -= divisionWidth;
				iCountArray.Append(countPerPage);
				countPerPage = 0;
				totalWidth = 0;
			}
			countPerPage++;
		}
		iCountArray.Append(countPerPage);
		iDividePage = ETrue;
	}
}

TInt CNaviPane::CurIndex()const
{
	return iIndex;
}

void CNaviPane::SetIndex(TInt aIndex)
{
	ASSERT(iNaviTexts);
	ASSERT(aIndex >= 0 && aIndex <iNaviTexts->Count());
	iIndex = aIndex;
}

void CNaviPane::SetObserver(MNaviPaneObserver* aObserver)
{
	iObserver = aObserver;
}
//////////////////////////////////////////////////////////////////////////
//Private
//////////////////////////////////////////////////////////////////////////
void CNaviPane::ArrowLeft()
{
	if(iIndex > 0)
	{
		--iIndex;
	}	
	else	//循环到最右侧
	{
		iIndex = iNaviTexts->Count() - 1;
	}
	CalculatePage();
}

void CNaviPane::ArrowRight()
{
	if(iIndex < iNaviTexts->Count() - 1)
	{
		iIndex++;
	}
	else	//循环到最左侧
	{
		iIndex = 0;
	}
	CalculatePage();
}

void CNaviPane::CalculatePage()
{
	ASSERT(iIndex < iNaviTexts->Count());

	iThisPageBeginIndex = 0;

	for (TInt i = 0 ; i < iCountArray.Count() ; i++)
	{
		TInt thisPageItemNum = iCountArray[i];
		if(iThisPageBeginIndex + thisPageItemNum <= iIndex)
		{
			iThisPageBeginIndex += thisPageItemNum;
		}
		else
		{
			iPageIndex = i;
			break;
		}
	}
}

void CNaviPane::DrawBackground(CGraphic& gc)const
{
	ASSERT(iTitleBitmap);
	gc.BitBlt(TPoint(),iTitleBitmap);
/*
	gc.SetPenColor(KNaviPaneBackgroundColor);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetBrushColor(KNaviPaneBackgroundColor);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);

	gc.DrawRoundRect(iNaviPaneRect);
*/

}

void CNaviPane::DrawEachItem(CGraphic& gc,TPoint& aPoint,TInt aIndex)const
{
	ASSERT(aIndex < iNaviTexts->Count());
	TInt textWidth = iFont.MeasureText((*iNaviTexts)[aIndex]) ;
	if (aIndex == iIndex)		//当前项
	{
		gc.SetPenColor(KNaviPaneSelectedItemColor);
		gc.SetBrushColor(KNaviPaneSelectedItemColor);
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);

		TPoint point;
		point.iX = aPoint.iX;
		point.iY = iNaviPaneRect.iTl.iY;

		TSize size(textWidth,iNaviPaneRect.Height());

		TRect rect(point,size);

		gc.DrawRoundRect(rect);
		gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	}

	gc.SetPenColor(KRgbWhite);
	gc.DrawText(aPoint,(*iNaviTexts)[aIndex]);

	aPoint.iX += textWidth;
}

void CNaviPane::DrawDivision(CGraphic& gc,TPoint& aPoint)const
{
	gc.DrawText(aPoint,KListSeparator);
	aPoint.iX += iFont.MeasureText(KListSeparator);
}

void CNaviPane::DrawArrow(CGraphic& gc) const
{
	if(iCountArray.Count() > 1)	//只有在一屏无法显示所有的项时才绘制溢出箭头
	{

		if(iPageIndex != 0)	//不在第一页,画左箭头
		{
			TPoint point = iNaviPaneRect.iTl;
			point.iY += (iNaviPaneRect.Height() - iArrowImageSize.iHeight)/2;
			iMainEngine.GetBitmapFactory().DrawArrowIconBmpByID(gc,point,EArrawLeft);
		}
		if (iPageIndex < iCountArray.Count() - 1)	//不在最后一页,画右箭头
		{
			TPoint point = iNaviPaneRect.iTl;
			point.iX += iNaviPaneRect.Size().iWidth;
			point.iX -= iArrowImageSize.iWidth;
			point.iY += (iNaviPaneRect.Height() - iArrowImageSize.iHeight)/2;

			iMainEngine.GetBitmapFactory().DrawArrowIconBmpByID(gc,point,EArrawRight);
		}
	}
}

void CNaviPane::InitBitmap()
{
	iTitleBitmap = iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpTitlebg);
}

void CNaviPane::SetArrowIcon()
{
	iArrowImageSize = iMainEngine.GetBitmapFactory().GetArrowIconBmpSize(0);
	iArrowImageWidth = iArrowImageSize.iWidth;
}