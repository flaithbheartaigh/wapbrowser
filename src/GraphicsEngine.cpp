/*
============================================================================
 Name        : GraphicsEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对于系统提供的绘制图片、文字和图画等API进行封装，方便后期的移植工作
============================================================================
*/



#include <bitstd.h>
#include <bitdev.h>
#include <GDI.H>
#include "GraphicsEngine.h"
#include "PictureEngine.h"
#include "MainControlEngine.h"
#include <CoCoPreDefine.h>
#include "ScreenLayout.h"

CGraphicsEngine::CGraphicsEngine(CMainControlEngine& aMainControlEngine):iMainControlEngine(aMainControlEngine)
{
	CLEAR(iBitmap);
	CLEAR(iDevice);
	CLEAR(iBmpGc);
}


CGraphicsEngine::~CGraphicsEngine()
{
	DELETE(iBmpGc);
	DELETE(iDevice);
	DELETE(iBitmap);
}

CGraphicsEngine* CGraphicsEngine::NewLC(CMainControlEngine& aMainControlEngine)
{
	CGraphicsEngine* self = new (ELeave)CGraphicsEngine(aMainControlEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CGraphicsEngine* CGraphicsEngine::NewL(CMainControlEngine& aMainControlEngine)
{
	CGraphicsEngine* self=CGraphicsEngine::NewLC(aMainControlEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CGraphicsEngine::ConstructL()
{

}


void CGraphicsEngine::Clear()
{
	iBmpGc->Clear();
}

void CGraphicsEngine::ClearRect(TRect& aRect)
{
	iBmpGc->Clear(aRect);
}

void CGraphicsEngine::SetClippingRect(TRect aRect)
{
	iBmpGc->SetClippingRect(aRect);
}

void CGraphicsEngine::CancelClippingRect()
{
	iBmpGc->CancelClipping();
}
void CGraphicsEngine::Create(TInt nSizeWidth,TInt nSizeHeight)
{
	iBitmap = new (ELeave) CBmpPicture();
	iBitmap->Create(nSizeWidth, nSizeHeight);
	iDevice = CFbsBitmapDevice::NewL(iBitmap->GetBitmap());
	User::LeaveIfError(iDevice->CreateContext(iBmpGc));
}

void CGraphicsEngine::DrawLine(TPoint aPos1,TPoint aPos2)
{
	iBmpGc->DrawLine(aPos1,aPos2);
}

void CGraphicsEngine::DrawRect(TRect aRect)
{
	iBmpGc->DrawRect(aRect);
}

CBmpPicture* CGraphicsEngine::GetImgBackground()
{
	return iBitmap;
}

void CGraphicsEngine::SetBrushColor(TRgb aColor)
{
	iBmpGc->SetBrushColor(aColor);
}

void CGraphicsEngine::SetBrushStyle(CGraphicsContext::TBrushStyle aPenStyle)
{
	iBmpGc->SetBrushStyle(aPenStyle);
}

void CGraphicsEngine::SetFont(const CFont* aFont)
{
	iFont = aFont;
}

const CFont* CGraphicsEngine::GetFont()
{
	//Debug_My_Assert(iFont);
	return iFont;
}

void CGraphicsEngine::SetPenColor(TRgb aColor)
{
	iBmpGc->SetPenColor(aColor);
}

void CGraphicsEngine::SetPenStyle(CGraphicsContext::TPenStyle aPenStyle)
{
	iBmpGc->SetPenStyle(aPenStyle);
}

void CGraphicsEngine::BitBltMasked(TPoint aPos,CBmpPicture* aImage)
{
	TSize aSize = aImage->GetSize();
	BitBltMasked(aPos,aImage,TRect(TPoint(0,0),aSize));
}

void CGraphicsEngine::BitBltMasked(TPoint aPos,CBmpPicture* aImage,TRect aRect)
{
		if (aImage->HasMask())
	{
		iBmpGc->BitBltMasked(aPos,
								aImage->GetBitmap(),
								aRect,
								aImage->GetMaskBitmap(),
								EFalse);
	}
	else
	{
		iBmpGc->BitBlt(aPos, aImage->GetBitmap(), aRect);
	}
}

void CGraphicsEngine::BitBltMasked(TPoint aPos,CBmpPictureArray* aImage,TInt aIndex)
{
	BitBltMasked(aPos,aImage,aImage->IconRect(aIndex));
}

void CGraphicsEngine::DrawText(TPoint aPos,const TDesC& aText)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = 0;
#ifndef __SERIES60_3X__
	  aFontHeight = iFont->HeightInPixels();//HeightInPixels
#else
	  aFontHeight = iFont->HeightInPixels();//HeightInPixels
#endif
	aPos.iY+=aFontHeight;
	iBmpGc->DrawText(aText,aPos);
	iBmpGc->DiscardFont();
}

void CGraphicsEngine::DrawText_Bold(TPoint aPos,const TDesC& aText)
{
	TRgb argb(0,0,0);
	SetPenColor(argb);
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = 0;
#ifndef __SERIES60_3X__
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#else
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#endif
	aPos.iY+=aFontHeight;
	iBmpGc->DrawText(aText,aPos-TPoint(1,0));
	iBmpGc->DrawText(aText,aPos+TPoint(1,0));
	iBmpGc->DrawText(aText,aPos-TPoint(0,1));
	iBmpGc->DrawText(aText,aPos+TPoint(0,1));
	iBmpGc->DrawText(aText,aPos+TPoint(1,1));
	iBmpGc->DrawText(aText,aPos+TPoint(1,-1));
	iBmpGc->DrawText(aText,aPos+TPoint(-1,1));
	iBmpGc->DrawText(aText,aPos+TPoint(-1,1));
	TRgb argb1(255,255,255);
	SetPenColor(argb1);
	iBmpGc->DrawText(aText,aPos);
	iBmpGc->DiscardFont();
}

void CGraphicsEngine::BitBltBmp(TPoint aPos,CFbsBitmap* aBitmap)
{
	iBmpGc->BitBlt(aPos,aBitmap);
}

void CGraphicsEngine::DrawText_Blue(TPoint aPos,const TDesC& aText)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = 0;
#ifndef __SERIES60_3X__
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#else
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#endif
	aPos.iY+=aFontHeight;
	TRgb argb(33,33,255);
	SetPenColor(argb);
	iBmpGc->DrawText(aText,aPos);
	TInt width=iFont->MeasureText(aText);
	iBmpGc->DrawLine(aPos+TPoint(0,1),aPos+TPoint(width,1));
	iBmpGc->DiscardFont();

	TRgb argb1(0,0,0);
	SetPenColor(argb1);
}
void CGraphicsEngine::DrawText_Red(TPoint aPos,const TDesC& aText)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = 0;
#ifndef __SERIES60_3X__
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#else
	aFontHeight = iFont->HeightInPixels();//HeightInPixels
#endif
	aPos.iY+=aFontHeight;
	TInt width=iFont->MeasureText(aText);

	SetPenStyle(CGraphicsContext::ENullPen);
	SetBrushColor(TRgb(99,200,255));
	SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(aPos-TPoint(0,iMainControlEngine.GetScreenLayout()->FontHeight()), TSize(width,iMainControlEngine.GetScreenLayout()->FontHeight()));
	iBmpGc->DrawRect(rect);
	
	SetPenStyle(CGraphicsContext::ESolidPen);
	iBmpGc->DrawText(aText,aPos);
	
	iBmpGc->DrawLine(aPos+TPoint(0,1),aPos+TPoint(width,1));
	iBmpGc->DiscardFont();

	SetBrushStyle(CGraphicsContext::ENullBrush);
	SetPenColor(KBlackRgb);	
	
}