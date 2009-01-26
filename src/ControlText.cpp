/*
============================================================================
 Name        : ControlText.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CControlText implementation
============================================================================
*/

#include "ControlText.h"
#include "Graphic.h"
#include "NotifyTimer.h"
#include "TypeDefine.h"
#include "CoCoPreDefine.h"
#include "cocobmp.h"
#include "StaticCommonTools.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "BitmapFactory.h"

CControlText::CControlText(MDrawEventObserver& aObserver,CMainEngine& aMainEngine)
:CControl(EControlText)
,iMainEngine(aMainEngine)
,iBitmapFactory(aMainEngine.GetBitmapFactory())
{
	// No implementation required
	SetDrawEventObserver(&aObserver);
}


CControlText::~CControlText()
{
	delete iTimer;
}

CControlText* CControlText::NewLC(MDrawEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CControlText* self = new (ELeave)CControlText( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CControlText* CControlText::NewL(MDrawEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CControlText* self=CControlText::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CControlText::ConstructL()
{
	iBool=EFalse;
	iSpeed=50;
	iStopTime=0;	
	InitBtimap();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CControlText::Draw(CGraphic& gc) const
{
	gc.SetClippingRect(iClientRect);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetPenColor(KTextColor);
	
	if(iShowIndex==EWeather)
	{
		if(iIconIndex>-1&&iIconIndex<iBitmapFactory.GetWeatherIconBmpCount())
		{
			iBitmapFactory.DrawWeatherIconBmpByID(gc,iClientRect.iTl+iStartPoint,iIconIndex);
			//gc.BitBltMasked(iClientRect.iTl+iStartPoint,iIcon,iIconMask,TRect(TPoint(0,iShowIconSize.iHeight*iIconIndex),iShowIconSize),ETrue);
		}
			gc.DrawText(iClientRect.iTl+iStartPoint+TPoint((iShowIconSize.iWidth+iMainEngine.ScreenLayout().FontHeight()/3),0),iText);
	}
	else
	{
		if(!iBool)
		{
			gc.DrawText(iText,iClientRect,CGraphicsContext::ECenter,iStartPos);
		}
		else
		{
			gc.DrawText(iText,iClientRect,CGraphicsContext::ELeft,iStartPos);
		}
	}
	
	gc.CancelClippingRect();
}

TBool CControlText::KeyEventL(TInt /*aKeyCode*/)
{
	return EFalse;
}

TBool CControlText::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CControlText::SizeChanged(const TRect& /*aScreenRect*/)
{
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
TBool CControlText::DoPeriodTask()
{
	switch(iShowIndex)
	{
	case ECurrentTime:
		IninShowTime();
		break;
	case EWeather:
		InitShowWeather();
		break;
	case EDownLoading:
		IninShowDownLoading();
		break;
	case ELoadType:
		IninShowLoginType();
		break;
	}
	RequestDraw();

	return ETrue;
}

void CControlText::SetClientRect(const TRect& aRect)
{
	iClientRect = aRect;
}
void CControlText::InitBtimap()
{
	iShowIconSize=iBitmapFactory.GetWeatherIconBmpSize(0);
}
void CControlText::IninShowTime()
{
	iStopTime++;
	if(iStopTime==1)
	{
		iText.Zero();
		iMainEngine.GetControlText(ECurrentTime,iText);
		if(iText.Length()==0)
		{
			iShowIndex=EWeather;
			iStopTime=0;
		}
	}
	else if(iStopTime>iSpeed)
	{
		iText.Zero();
		iShowIndex=EWeather;
		iStopTime=0;
	}
}
void CControlText::IninShowLoginType()
{
	iStopTime++;
	if(iStopTime==1)
	{
		iText.Zero();
		iMainEngine.GetControlText(ELoadType,iText);
		if(iText.Length()==0)
		{
			iShowIndex=ECurrentTime;
			iStopTime=0;
		}
	}
	else if(iStopTime>iSpeed)
	{
		iText.Zero();
		iShowIndex=ECurrentTime;
		iStopTime=0;
	}
}
void CControlText::InitShowWeather()
{
	iStopTime++;

	if(iStopTime==1)
	{
		iText.Zero();
		iMainEngine.GetControlText(EWeather,iText);
		if(iText.Length()==0)
		{
			iIconIndex=-1;
			iText.Zero();
			iShowIndex=EDownLoading;
			iStopTime=0;
		}
		else
		{
			TInt point=iText.Find(_L("|"));
			if(point>0)
			{
				iIconIndex=::ConvertTDesCToTInt(iText.Left(point));
				iText.Delete(0,point+1);
			}
			else
			{
				iIconIndex=-1;
			}
			iTextWidth=iMainEngine.ScreenLayout().Font()->MeasureText(iText);

			iStartPoint.iX=(iClientRect.Width()-iShowIconSize.iWidth-iTextWidth-iMainEngine.ScreenLayout().FontHeight()/3)/2;
			iStartPoint.iY=(iClientRect.Height()-iMainEngine.ScreenLayout().FontHeight())/2;
		}
	}
	else if(iStopTime>iSpeed)
	{
		iIconIndex=-1;
		iText.Zero();
		iShowIndex=EDownLoading;
		iStopTime=0;
	}
	
}
void CControlText::IninShowDownLoading()
{
	iStopTime++;

	if(iStopTime==1)
	{
		iMainEngine.GetControlText(EDownLoading,iText);
		if(iText.Length()==0)
		{
			iText.Zero();
			iStopTime=0;
			iShowIndex=ELoadType;
			iStartPos=0;
			iBool=EFalse;
		}
		else
		{
			iTextWidth=iMainEngine.ScreenLayout().Font()->MeasureText(iText);
			if(iTextWidth>iClientRect.Width())
			{
				iStartPos=0;
				iBool=ETrue;
			}
		}
	}
	else if(iStopTime>iSpeed)
	{
		if(iBool&&(iText.Length()-iStartPos)>4)
		{
			iStartPos++;
		}
		else
		{
			iText.Zero();
			iStopTime=0;
			iShowIndex=ELoadType;
			iStartPos=0;
			iBool=EFalse;
		}
	}

	if(iBool)
	{
		iStartPos++;
	}
}

void CControlText::Begin()
{
	iStartPos = 0;
	if(NULL == iTimer)
	{
		iTimer = CNotifyTimer::NewL(*this);
	}
	iTimer->Cancel();
	iTimer->Start(KStandardTimerInterval);
}

void CControlText::Stop()
{
	ASSERT(iTimer);
	iStartPos = 0;
	iTimer->Stop();
}