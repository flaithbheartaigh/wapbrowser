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

	if(iGrads > 0)		// ���������ܹ���ʾ��һ��Ļ��ʱ,���û���������
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
	//��ֹ��0����,û����ʱ��ֻ��һ��Դ�
	iTotalLineNum = iTotalLineNum == 0 ? 1 : iTotalLineNum;	
	iScrollHeight = iScrollVerticalSpace * iMaxLinePerPage;
	iScrollHeight = iScrollHeight / iTotalLineNum;
	iScrollHeight = iScrollHeight < 3 ? 3 : iScrollHeight;
}

void CScrollBar::CalculateCursorPosition()
{
	if(iGrads > 0)		// ���������ܹ���ʾ��һ��Ļ��ʱ,���û���������,Ҳ���ü���
	{
		ASSERT(iGrads > 0);
		TInt moveSpace = iScrollVerticalSpace - iScrollHeight;
		TInt iIncrement = moveSpace/iGrads;
		TInt iRemainder = moveSpace%iGrads;

		iCursorPosition = iBeginPosition;
		iCursorPosition.iY += iIndex*iIncrement;

		//////////////////////////////////////////////////////////////////////////
		//��ʣ���������ɢ�������µĳ�����
		//////////////////////////////////////////////////////////////////////////
		if(iRemainder > 0)
		{
			iCursorPosition.iY += CalculateIncrement(iGrads,iRemainder,iIndex);
		}
	}
}

//�ú�����Ԥ���Ч�����в�ͬ,�����Ͽ���ʹ��
int CScrollBar::CalculateIncrement(int grads,int remainder,int index)
{
	ASSERT(remainder > 0);
	ASSERT(grads > remainder);
	int lowShareTimes = grads/remainder;		//ͬһ��ֵ���ٱ�������ٴ�,��๲�������1
	int highShareTimes = lowShareTimes + 1;				//��๲���������С���������1
	int highShareNum = grads % remainder;				//�ж���ֵ��������Ĵ���������ֵ��
	int lowShareNum = remainder - highShareNum;		//����ֵ��������ٵĴ���

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
	//iGrads����Ϊ0,
	//iRemainder�ڴ��������²�Ϊ0,��Ϊ0Ҳ���������
}

#ifndef __SERIES60_3X__
	extern "C"	//����_chkstk�������
	{
		void _chkstk()
		{
		}
	}
#endif