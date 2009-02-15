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
#include "Page.h"
#include "NotifyTimer.h"

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
	delete iPage;
	delete iNotifyTimer;
}

void CWapBrowserAppView::Draw( const TRect& /*aRect*/ ) const
{
	static const int KTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
	CWindowGc& gc = SystemGc();
	TRect drawRect( Rect());
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbGray);
	gc.Clear( drawRect );

	if(iPage)
	{
		iPage->Draw(gc);
	}
	DrawWaiting(gc);
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
		if(iPage && iPage->KeyEvent(aKeyEvent.iCode))
		{
			response = EKeyWasConsumed;
			DrawNow();
		}
	}
	return response;
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CWapBrowserAppView::DoPeriodTask()
{
	if(3 == ++iWaitingPos)
	{
		iWaitingPos = 0;
	}
	DrawNow();
	return ETrue;
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CWapBrowserAppView::ShowPage(CPage* aPage)
{
	if(aPage)
	{
		delete iPage; 
		iPage = aPage;
		iPage->SetRect(Rect());
		DrawNow();
	}
}

void CWapBrowserAppView::ShowWaiting()
{
	if(NULL == iNotifyTimer)
	{
		iNotifyTimer = CNotifyTimer::NewL(*this);
	}
	iNotifyTimer->Start(100*1000);
	iShowWaiting = ETrue;
	DrawNow();
}

void CWapBrowserAppView::StopShowWaiting()
{
	if(iShowWaiting)
	{
		ASSERT(iNotifyTimer);
		iNotifyTimer->Stop();
		iShowWaiting = EFalse;
		DrawNow();
	}
}

void CWapBrowserAppView::DrawWaiting(CGraphicsContext& aGc) const
{
	if(iShowWaiting)
	{
		TRect rect = Rect();
		rect.iTl.iY = rect.iBr.iY - 20;
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetBrushColor(KRgbYellow);
		aGc.SetPenStyle(CGraphicsContext::ENullPen);
		aGc.DrawRect(rect);
		rect.iTl.iX = 10*iWaitingPos;
		rect.iBr.iX = rect.iTl.iX + 10;
		aGc.SetBrushColor(KRgbGreen);
		for ( int i = 0 ; i < 20 ; i++)
		{
			aGc.DrawRect(rect);
			rect.Move(TPoint(30,0));
		}
	}
}