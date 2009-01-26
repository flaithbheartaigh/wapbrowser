/*
============================================================================
Name        : TitleBar.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CTitleBar implementation
============================================================================
*/

#include "TitleBar.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "SlideText.h"
#include "TypeDefine.h"
#include "cocobmp.h"

CTitleBar::CTitleBar(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont) : CControl(ETitleBar) 
																					 , iMainEngine(aMainEngine)
																					 , iRect(aRect)
																					 , iFont(aFont)
{
	ASSERT(iFont);
}

CTitleBar::~CTitleBar()
{
	delete iSlideText;
	delete iTitleBitmap;
	//delete iTitleText;
}

CTitleBar* CTitleBar::NewLC(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont,const TDesC& aTitleDes)
{
	CTitleBar* self = new (ELeave)CTitleBar(aMainEngine,aRect,aFont);
	CleanupStack::PushL(self);
	self->ConstructL(aTitleDes);
	return self;
}

CTitleBar* CTitleBar::NewL(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont,const TDesC& aTitleDes)
{
	CTitleBar* self=CTitleBar::NewLC(aMainEngine,aRect,aFont,aTitleDes);
	CleanupStack::Pop(); // self;
	return self;
}

void CTitleBar::ConstructL(const TDesC& aTitleDes)
{
// 	iTitleText.Zero();
// 	iTitleText.Append(aTitleDes.Left(iTitleText.MaxLength()));

	InitBitmap();
	/*
	WriteLogsL(_L("CTitleBar::ConstructL"));
	WriteLogsL(aTitleDes);
	iTitleText = aTitleDes.AllocL();
	WriteLogsL(_L("CTitleBar::ConstructL iTitleText = %d"),iTitleText);
	const TDesC& testDes = *iTitleText;
	WriteLogsL(_L("CTitleBar::ConstructL iTitleText len = %d"),iTitleText->Length());
	WriteLogsL(testDes);
	WriteLogsL(*iTitleText);
	*/

	iSlideRect = iRect;
	iSlideText = CSlideText::NewL(iMainEngine,CSlideText::ELeftWithPause);
	iSlideText->SetFont(iFont);
	iSlideText->SetTextColor(KRgbWhite);
	iSlideText->SetMoveSpeed(5);
	iSlideText->SetStopTime(20);
	iSlideText->Layout(ECenterAlign);
	iSlideText->SetClientRect(iSlideRect);
	iSlideText->SetText(aTitleDes);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CTitleBar::Draw(CGraphic& gc)const
{
	UtilityTools::WriteLogsL(_L("CTitleBar::Draw"));
	//ASSERT(iTitleText);

	ASSERT(iTitleBitmap);
	ASSERT(iSlideText);

	gc.BitBlt(TPoint(),iTitleBitmap);

	iSlideText->Draw(gc);
/*
	gc.SetPenColor(KNaviPaneBackgroundColor);
	gc.SetBrushColor(KNaviPaneBackgroundColor);
	gc.SetPenStyle(CBitmapContext::ENullPen);
	gc.SetBrushStyle(CBitmapContext::ESolidBrush);
	//gc.DrawRect(iRect);	
	gc.DrawRoundRect(iRect);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);
	gc.SetPenColor(KBlackRgb);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
*/

	//	if(iTitleText)
	{
		/*
		WriteLogsL(_L("CTitleBar::Draw iFont = %d"),iFont);
		TInt fontHeight = iFont->HeightInPixels();
		WriteLogsL(_L("CTitleBar::Draw fontHeight = %d"),fontHeight);
		WriteLogsL(_L("CTitleBar::Draw iTitleText = %d"),iTitleText);
		const TDesC& testDes = *iTitleText;
		WriteLogsL(_L("CTitleBar::Draw iTitleText len = %d"),iTitleText->Length());
		WriteLogsL(testDes);
		WriteLogsL(*iTitleText);
		*/


/*
		TPoint point = iRect.iTl;
		point.iY += iFont->HeightInPixels();
		point.iY += (iRect.Height() - iFont->HeightInPixels())/2;
		//gc.DrawText(*iTitleText,point);
*/

		//gc.DrawText(iRect.iTl,iTitleText);

		//WriteLogsL(_L("CTitleBar::Draw End"));
// 		gc.SetPenColor(KRgbWhite);
// 		gc.DrawText(iTitleText,iRect,CGraphicsContext::ELeft,iLeftBitmap->SizeInPixels().iWidth);

	}



	if(iLeftBitmap)
	{
		TPoint point = iRect.iTl;
		point.iY += ((iRect.Height() - iRightBitmap->SizeInPixels().iHeight)>>1);
		if(NULL == iRightBitmapMask)
		{
			gc.BitBlt(point,iRightBitmap);
		}
		else
		{
			gc.BitBltMasked(point,iRightBitmap,iRightBitmapMask);
		}
	}
	if(iRightBitmap)
	{
		TPoint point = iRect.iTl;
		TSize size = iRightBitmap->SizeInPixels();
		point.iX += iRect.Width();
		point.iX -= size.iWidth;
		point.iY += ((iRect.Height() - size.iHeight)>>1);

		if(NULL == iRightBitmapMask)
		{
			gc.BitBlt(point,iRightBitmap);
		}
		else
		{
			gc.BitBltMasked(point,iRightBitmap,iRightBitmapMask);
		}
	}
	else if(iRightInfoText)
	{
		gc.DrawText(*iRightInfoText,iRect,CGraphicsContext::ERight);
	}
	UtilityTools::WriteLogsL(_L("CTitleBar::Draw End"));
}

TBool CTitleBar::KeyEventL(TInt /*aKeyCode*/)
{
	return EFalse;
}

TBool CTitleBar::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CTitleBar::SizeChanged(const TRect& /*aScreenRect*/)
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CTitleBar::SetTextL(const TDesC& aTitleDes)
{
	ASSERT(iSlideText);

	iSlideText->SetText(aTitleDes);
// 	iTitleText.Zero();
// 	iTitleText.Append(aTitleDes.Left(iTitleText.MaxLength()));


	/*
	WriteLogsL(_L("CTitleBar::SetTextL"));
	WriteLogsL(aTitleDes);
	delete iTitleText;
	iTitleText = NULL;
	iTitleText = aTitleDes.AllocL();
	WriteLogsL(_L("CTitleBar::SetTextL iTitleText = %d"),iTitleText);
	const TDesC& testDes = *iTitleText;
	WriteLogsL(_L("CTitleBar::SetTextL iTitleText len = %d"),iTitleText->Length());
	WriteLogsL(testDes);
	WriteLogsL(*iTitleText);
	*/

}

void CTitleBar::SetRightInfoL(const TDesC& aInfo)
{
	delete iRightInfoText;
	iRightInfoText = NULL;
	iRightInfoText = aInfo.AllocL();
}

void CTitleBar::SetLeftBitmap(const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask)
{
	if(iLeftBitmap!=aBitmap&&iLeftBitmapMask!=aBitmapMask)
	{
		iLeftBitmap = aBitmap;
		iLeftBitmapMask = aBitmapMask;
		iSlideRect.iTl = iRect.iTl;
		iSlideRect.iTl.iX += aBitmap->SizeInPixels().iWidth;
		iSlideText->SetClientRect(iSlideRect);
	}
}

void CTitleBar::SetRightBitmap(const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask)
{
	if(iRightBitmap!=aBitmap&&iRightBitmapMask!=aBitmapMask)
	{
		iRightBitmap = aBitmap;
		iRightBitmapMask = aBitmapMask;
		iSlideRect.iBr = iRect.iBr;
		iSlideRect.iBr.iX -= aBitmap->SizeInPixels().iWidth;
		iSlideText->SetClientRect(iSlideRect);
	}
}

void CTitleBar::InitBitmap()
{
	iTitleBitmap = iMainEngine.CreateBmpById(EMbmCocobmpTitlebg);

	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
	TSize screenSize = iMainEngine.ScreenLayout().ScreenSize();

	screenLayout.ScaleFitToScreenWidth(iTitleBitmap);

	TInt multiple = screenLayout.Multiple();
	TInt divisor = screenLayout.Divisor();

	iRect = iTitleBitmap->SizeInPixels();
}