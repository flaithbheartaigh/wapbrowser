// ScrollBar.cpp: implementation of the CScrollBar class.
//
//////////////////////////////////////////////////////////////////////
#include "ScrollBar.h"

#include "ScrollbarObserver.h"
// #include "ScreenLayout.h"
// #include "Graphic.h"

//////////////////////////////////////////////////////////////////////
//CScrollBar
//////////////////////////////////////////////////////////////////////
CScrollBar::CScrollBar()
	//: CControl(EScrollBar)
	: iIsShow(ETrue)
	, iControlMode(EPageControl)
{
}

CScrollBar::~CScrollBar()
{

}

CScrollBar* CScrollBar::CreateDefaultScrollbarL()
{
	CScrollBar* scrollbar = new (ELeave) CScrollBar;
	scrollbar->DefaultLayout();
	return scrollbar;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//void CScrollBar::Draw(CGraphic& gc)const
void CScrollBar::Draw(CGraphicsContext& gc)const
{
	if(!iIsShow)return;

	//UtilityTools::WriteLogsL(_L("CScrollBar::Draw"));
	AsserClass();

	if(iGrads > 0)		// 所有内容能够显示在一屏幕内时,不用画滚动浮标
	{
		gc.SetPenStyle(CBitmapContext::ENullPen);
		gc.SetBrushStyle(CBitmapContext::ESolidBrush);
		TRect backgroundRect(iBeginPosition,TSize(iScrollWidth,iScrollVerticalSpace));
		gc.SetBrushColor(iBackgroundColor);
		gc.DrawRect(backgroundRect);
		//TPoint currentPosition = iBeginPosition;

		TRect cursorRect(iCursorPosition,TSize(iScrollWidth,iScrollHeight));
		gc.SetBrushColor(iCursorColor);
		gc.DrawRect(cursorRect);

		gc.SetPenStyle(CBitmapContext::ESolidPen);
		gc.SetBrushStyle(CBitmapContext::ENullBrush);
	}
	//UtilityTools::WriteLogsL(_L("CScrollBar::Draw End"));
}

TBool CScrollBar::KeyEventL(TInt aKeyCode)
{
	AsserClass();	
	TBool keyResult = EFalse;
	switch(iControlMode)
	{
	case EPageControl:
		keyResult = PageControlKeyEventL(aKeyCode);
		break;

	case ECircleControl:
		keyResult = CircleContolKeyEventL(aKeyCode);
		break;

	default:
		ASSERT(EFalse);
		break;
	}
	return keyResult;
}

TBool CScrollBar::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CScrollBar::SizeChanged(const TRect& /*aScreenRect*/)
{
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CScrollBar::SetObserver(MScrollbarObserver* aObserver)
{
	iObserver = aObserver;
}

void CScrollBar::DefaultLayout()
{
	iScrollWidth = 3;
	iScrollVerticalSpace = 125;
	iBeginPosition = TPoint(SCREEN_WIDTH - iScrollWidth - 3,59);

	iScrollHeight = 23;
	iMaxLinePerPage = 8;
	iTotalLineNum = 10;
	iBackgroundColor = KScrollBackgroundColor;
	iCursorColor = KScrollSpaceColor;

	Layout();
}

void CScrollBar::SetShow(TBool aShow)
{
	iIsShow = aShow;
}

void CScrollBar::SetSize(TInt aWidth,TInt aHeight)
{
	iScrollWidth = aWidth;
	iScrollHeight = aHeight;
}

void CScrollBar::SetVerticalSpace(TInt aSpace)
{
	iScrollVerticalSpace = aSpace;
}

void CScrollBar::SetScrollWidth(TInt aWidth)
{
	iScrollWidth = aWidth;
}

void CScrollBar::SetMaxLinePerPage(TInt aMaxLine)
{
	iMaxLinePerPage = aMaxLine;
}

void CScrollBar::SetTotalLineNum(TInt aLineNum)
{
	iTotalLineNum = aLineNum;
}

void CScrollBar::SetTopRightPoint(const TPoint& aTopRightPoint)
{

}

void CScrollBar::SetBeginPostion(const TPoint& aPoint)
{
	iBeginPosition = aPoint;
}

void CScrollBar::SetBackgroundColor(const TRgb& aColor)
{
	iBackgroundColor = aColor;
}

void CScrollBar::SetCursorColor(const TRgb& aColor)
{
	iCursorColor = aColor;
}

void CScrollBar::SetControlMode(TInt aControlMode)
{
	iControlMode = aControlMode;
}

void CScrollBar::Layout()
{
//	WriteLogsL(_L("CScrollbar::Layout"));
	CalculateCursorHeight();
	iGrads = iTotalLineNum - iMaxLinePerPage;
	iGrads = iGrads > 0 ? iGrads : 0;

	iIndex = iIndex < iTotalLineNum ? iIndex : iTotalLineNum - 1;
	CalculateCursorPosition();
//	WriteLogsL(_L("CScrollbar::Layout End"));

	iAlreadyLayout = ETrue;
}

TBool CScrollBar::Up()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::Up"));
	TBool result = EFalse;
	if(iIndex > 0)
	{
		iIndex --;
		CalculateCursorPosition();
		result = ETrue;
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	//UtilityTools::WriteLogsL(_L("CScrollBar::Up End"));
	return result;
}

TBool CScrollBar::Down()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::Down"));
	TBool result = EFalse;
	if(iGrads != 0)
	{
		if(iIndex < iGrads)
		{
			iIndex ++;
			CalculateCursorPosition();
			result = ETrue;
		}
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	//UtilityTools::WriteLogsL(_L("CScrollBar::Down End"));
	return result;
}

TBool CScrollBar::Home()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::"));
	TBool result = EFalse;
	if(iIndex != 0)
	{
		iIndex = 0;
		CalculateCursorPosition();
		result = ETrue;
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	return result;
}

TBool CScrollBar::End()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::"));
	TBool result = EFalse;
	if(iIndex != iGrads)
	{
		iIndex = iGrads;
		CalculateCursorPosition();
		result = ETrue;
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	return result;
}

TBool CScrollBar::PageUp()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::"));
	TBool result = EFalse;
	if(iIndex != 0)
	{
		iIndex -= iMaxLinePerPage;
		iIndex = iIndex < 0 ? 0 : iIndex;
		CalculateCursorPosition();
		result = ETrue;
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	return result;
}

TBool CScrollBar::PageDown()
{
	//UtilityTools::WriteLogsL(_L("CScrollBar::"));
	TBool result = EFalse;
	if(iIndex != iGrads)
	{
		iIndex += iMaxLinePerPage;
		iIndex = iIndex > iGrads ? iGrads : iIndex;
		CalculateCursorPosition();
		result = ETrue;
	}
	if(iObserver)
	{
		iObserver->ScrollBarIndexChanged(iIndex);
	}
	return result;
}

TInt CScrollBar::CurIndex()const
{
	AsserClass();
	ASSERT(iIndex >= 0 && iIndex <= iGrads);
	return iIndex;
}

void CScrollBar::SetIndex(TInt aIndex)
{
	ASSERT(aIndex >= 0 && aIndex <= iGrads);
//	WriteLogsL(_L("CScrollbar::SetIndex"));
	if(iIndex != aIndex)
	{
		iIndex = aIndex;
		CalculateCursorPosition();
	}
//	WriteLogsL(_L("CScrollbar::SetIndex"));
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
TBool CScrollBar::CircleContolKeyEventL(TInt aKeyCode)
{
	AsserClass();	
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		keyResult = Up();
		if(!keyResult)
		{
			End();
		}
		break;

	case KKeyCodeDown:	
		keyResult = Down();
		if(!keyResult)
		{
			Home();
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CScrollBar::PageControlKeyEventL(TInt aKeyCode)
{
	AsserClass();	
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		keyResult = Up();
		break;

	case KKeyCodeDown:	
		keyResult = Down();
		break;

	case KKeyCodeHome:
		keyResult = Home();
		break;

	case KKeyCodeEnd:
		keyResult = End();
		break;

	case KKeyCodePageUp:
		keyResult = PageUp();
		break;

	case KKeyCodePageDown:
		keyResult = PageDown();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

void CScrollBar::CalculateCursorHeight()
{
	//防止除0错误,没有项时按只有一项对待
	iTotalLineNum = iTotalLineNum == 0 ? 1 : iTotalLineNum;	
	iScrollHeight = iScrollVerticalSpace * iMaxLinePerPage;
	iScrollHeight = iScrollHeight / iTotalLineNum;
	iScrollHeight = iScrollHeight < 3 ? 3 : iScrollHeight;
}

void CScrollBar::CalculateCursorPosition()
{
	if(iGrads > 0)		// 所有内容能够显示在一屏幕内时,不用画滚动浮标,也不用计算
	{
		ASSERT(iGrads > 0);
		TInt moveSpace = iScrollVerticalSpace - iScrollHeight;
		TInt iIncrement = moveSpace/iGrads;
		TInt iRemainder = moveSpace%iGrads;

		iCursorPosition = iBeginPosition;
		iCursorPosition.iY += iIndex*iIncrement;

		//////////////////////////////////////////////////////////////////////////
		//将剩余的增量分散加至余下的长度中
		//////////////////////////////////////////////////////////////////////////
		if(iRemainder > 0)
		{
			iCursorPosition.iY += CalculateIncrement(iGrads,iRemainder,iIndex);
		}
	}
}

//该函数与预想的效果略有不同,大体上可以使用
int CScrollBar::CalculateIncrement(int grads,int remainder,int index)
{
	ASSERT(remainder > 0);
	ASSERT(grads > remainder);
	int lowShareTimes = grads/remainder;		//同一个值最少被共享多少次,最多共享次数加1
	int highShareTimes = lowShareTimes + 1;				//最多共享次数比最小共享次数多1
	int highShareNum = grads % remainder;				//有多少值被被共享的次数比其它值多
	int lowShareNum = remainder - highShareNum;		//多少值被共享较少的次数

	ASSERT(lowShareTimes > 0);
	ASSERT(highShareTimes > 0);

	int increment = 0;
	if(index <= (lowShareNum*lowShareTimes))
	{
		ASSERT(lowShareTimes > 0);
		increment = index/lowShareTimes;
	}
	else
	{
		ASSERT(highShareTimes > 0);
		int tmpValue = index - lowShareNum*lowShareTimes + 1;
		increment = lowShareNum + tmpValue/highShareTimes;
	}
	ASSERT(increment <= remainder);
	return increment;
}

void CScrollBar::AsserClass()const
{
	ASSERT(iAlreadyLayout);

	ASSERT(iScrollVerticalSpace != 0);
	ASSERT(iScrollHeight != 0);
	ASSERT(iScrollWidth != 0);
	ASSERT(iMaxLinePerPage != 0);
	//ASSERT(iTotalLineNum != 0);
	//ASSERT(iIncrement != 0);
	//iGrads可以为0,
	//iRemainder在大多数情况下不为0,但为0也是正常情况
}

#ifndef __SERIES60_3X__
	extern "C"	//屏蔽_chkstk编译错误
	{
		void _chkstk()
		{
		}
	}
#endif