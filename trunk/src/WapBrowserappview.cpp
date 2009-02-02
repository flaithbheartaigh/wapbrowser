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