/*
============================================================================
 Name        : SlideText.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CSlide implementation
============================================================================
*/

#include "SlideText.h"
#include "Graphic.h"
#include "NotifyTimer.h"
#include "TypeDefine.h"

CSlideText::CSlideText(MDrawEventObserver& aObserver,TInt aStyle) : CControl(ESlideText)
																  , iStyle(aStyle)
{
	SetDrawEventObserver(&aObserver);
	iIsActive = ETrue;
}


CSlideText::~CSlideText()
{
	delete iText;
	delete iTimer;
}

CSlideText* CSlideText::NewLC(MDrawEventObserver& aObserver,TInt aStyle)
{
	CSlideText* self = new (ELeave)CSlideText(aObserver,aStyle);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSlideText* CSlideText::NewL(MDrawEventObserver& aObserver,TInt aStyle)
{
	CSlideText* self=CSlideText::NewLC(aObserver,aStyle);
	CleanupStack::Pop(); // self;
	return self;
}

void CSlideText::ConstructL()
{
	iText = HBufC::NewL(10);
	iTimer = CNotifyTimer::NewL(*this);
	iTimer->Start(KStandardTimerInterval);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSlideText::Draw(CGraphic& gc) const
{
	UtilityTools::WriteLogsL(_L("CSlideText::Draw"));
	UtilityTools::WriteLogsL(*iText);
	ASSERT(iAlreadyLayout);
	Assert();
	gc.SetClippingRect(iClientRect);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetPenColor(iTextColor);
	gc.DrawText(*iText,iClientRect,CGraphicsContext::ELeft,iStartPos);
	gc.CancelClippingRect();
	//gc.DrawText(*iText,iClientRect,iBaseLine,CGraphicsContext::ELeft,iStartPos);
	//如果上面的函数中aLeftMargin可以为负值的话,就可以用它来实现
	//gc.DiscardFont();
}

TBool CSlideText::KeyEventL(TInt /*aKeyCode*/)
{
	return EFalse;
}

TBool CSlideText::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CSlideText::SizeChanged(const TRect& /*aScreenRect*/)
{
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
TBool CSlideText::DoPeriodTask()
{
	UtilityTools::WriteLogsL(_L("CSlideText::DoPeriodTask"));
	if(iIsActive)
	{
		ASSERT(iAlreadyLayout);
		TBool result = ETrue;
		switch(iStyle)
		{
		case ELeftWithPause:
			result = LeftWithPause();
			break;

		case ELeftCycle:
			result = LeftCycle();
			break;

		case EGoOrBack:
			result = GoOrBack();
			break;

		default:
			ASSERT(EFalse);
			break;
		}
		if(result)
		{
			RequestDraw();
		}
	}
	UtilityTools::WriteLogsL(_L("CSlideText::DoPeriodTask End"));
	return iIsActive;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSlideText::SetFont(const CFont* aFont)
{
	iFont = aFont;
}

void CSlideText::SetText(const TDesC& aDes)
{
	UtilityTools::WriteLogsL(aDes);
	ASSERT(iFont);
	delete iText;
	iText = NULL;
	iText = aDes.Alloc();
	iTextWidth = iFont->MeasureText(*iText);
	iStartPos = 0;
}

void CSlideText::SetTextColor(TRgb aColor)
{
	iTextColor = aColor;
}

void CSlideText::SetMoveSpeed(TInt aSpeed)
{
	iSpeed = aSpeed;
}

void CSlideText::SetClientRect(const TRect& aRect)
{
	iClientRect = aRect;
	//iClientRect.iTl.iX -= 1;
}

void CSlideText::SetStopTime(TInt aStopTime)
{
	iStopTime = aStopTime;
}


//该函数暂时不使用
// void CSlide::SetStyle(TInt aStyle,TInt aTimeInterval,TInt aStopInterval /* = 0 */)
// {
// 	iStyle = aStyle;
// }

void CSlideText::Layout(TVerticalAlign aAlign)
{
	iStartPos = 0;
	TInt fontHeight = iFont->HeightInPixels();
	TInt clientHeight = iClientRect.Height();
	switch(aAlign)
	{
	case EUpAlign:
		iBaseLine = fontHeight;
		break;

	case ECenterAlign:
		iBaseLine = clientHeight - fontHeight;
		iBaseLine = (iBaseLine>>1);
		iBaseLine = clientHeight - iBaseLine;
		iBaseLine -= 1;
		break;

	case EDownAlign:
		iBaseLine = clientHeight;
		break;

	default:
		ASSERT(EFalse);
		break;
	}
	iAlreadyLayout = ETrue;
}

/*
void CSlideText::Layout(const TDesC& aDes,const TRect& aRect,TVerticalAlign aAlign)
{
}
*/

void CSlideText::Begin()
{
	iStartPos = 0;
	if(NULL == iTimer)
	{
		iTimer = CNotifyTimer::NewL(*this);
	}
	iTimer->Cancel();
	iTimer->Start(KStandardTimerInterval);
}

void CSlideText::Stop()
{
	ASSERT(iTimer);
	iStartPos = 0;
	iTimer->Stop();
}
const TDesC& CSlideText::GetText() const
{
	return *iText;
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
TBool CSlideText::LeftWithPause()
{
	//if(0)return;	//未超时
	TBool result = EFalse;

	if (iTextWidth > iClientRect.Width())
	{
		if(iStopTimer > 0)
		{
			--iStopTimer;
		}
		else
		{
			TInt showLength = iTextWidth + iStartPos;

			if(showLength < iClientRect.Width())	//已到最左边
			{
				iStartPos = 0;
				iStopTimer = iStopTime;				//停留数秒	
			}
			else	//向左移动
			{
				iStartPos -= iSpeed;
			}
			result = ETrue;
		}
	}
	return result;
}

TBool CSlideText::LeftCycle()
{
	//if(0)return;	//未超时

	TInt showLength = iTextWidth + iStartPos;
	if(showLength < 0)
	{
		iStartPos = iClientRect.Width();
	}
	else
	{
		iStartPos -= iSpeed;
	}
	return ETrue;
}

TBool CSlideText::GoOrBack()
{
	ASSERT(EFalse);	//此处逻辑未对长度不足的字符串做处理,有待调整
	//if(0)return;	//未超时

	TInt showLength = iTextWidth + iStartPos;

	if(iSpeed > 0)	//向左移动
	{
		if(showLength < iClientRect.Width())	//已到最左边
		{
			//iStartPos = 0;
			iSpeed = -iSpeed;
		}
	}
	else			//向右移动
	{
		if(iStartPos > 0)		//已到最右边
		{
			iSpeed = -iSpeed;
		}
	}
	return ETrue;
}

void CSlideText::Assert()const
{
	ASSERT(iFont);
	ASSERT(iText);
}