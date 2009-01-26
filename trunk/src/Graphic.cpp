// Canvas.cpp: implementation of the CGraphics class.
//
//////////////////////////////////////////////////////////////////////

#include <bitstd.h>
#include <bitdev.h>
//#include <gdi.h>

#include <aknutils.h>
#include <fontids.hrh>

#include "graphic.h"
#include "CoCoPreDefine.h"

#define	ALPHA_COLOR_MASK	(0x07E0F81F)	//MASK:0000 0,111 11,10 000,0 1111 1,000 00,01 1111

#define	KMaxAlpha (0x20)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphic::CGraphic()
{
}

CGraphic::~CGraphic()
{
	delete iBmpGc;
	delete iBitmapDevice;
	delete iBitmap;
}

void CGraphic::ConstructL(const TSize& aSize)
{
	iAllSize=aSize;

	iFont = AknLayoutUtils::FontFromId(EFontCustom);

	iFontHeight = iFont->HeightInPixels();

	iBitmap = new (ELeave) CFbsBitmap;
	TInt err = iBitmap->Create(aSize,EColor64K);
	User::LeaveIfError(err);
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	iBitmapDevice->CreateContext(iBmpGc);
	User::LeaveIfError(err);
}

CGraphic* CGraphic::NewL(const TSize& aSize)
{

	CGraphic* self = new (ELeave) CGraphic();
	CleanupStack::PushL(self);
	self->ConstructL(aSize);
	CleanupStack::Pop(); // self;
	return self;
}

void CGraphic::Clear()
{
	iBmpGc->Clear();
}

void CGraphic::Clear(const TRect& aRect)
{
	iBmpGc->Clear(aRect);
}

void CGraphic::SetClippingRect(const TRect& aRect)
{
	iBmpGc->SetClippingRect(aRect);
}

void CGraphic::SetClippintRegion(const TRegion& aRegion)
{
	ASSERT(EFalse);

	//iBmpGc->SetClippingRegion(aRegion);
}

void CGraphic::CancelClippingRect()
{
	iBmpGc->CancelClipping();
}

void CGraphic::SetPenColor(TRgb aColor)
{
	iBmpGc->SetPenColor(aColor);
}

void CGraphic::SetPenStyle(CGraphicsContext::TPenStyle aPenStyle)
{
	iBmpGc->SetPenStyle(aPenStyle);
}

void CGraphic::SetBrushColor(TRgb aColor)
{
	iBmpGc->SetBrushColor(aColor);
}

void CGraphic::SetBrushStyle(CGraphicsContext::TBrushStyle aPenStyle)
{
	iBmpGc->SetBrushStyle(aPenStyle);
}

void CGraphic::DrawText(const TPoint& aPoint,const TDesC& aText )
{
	iBmpGc->UseFont(iFont);

	TPoint point = aPoint;
	point.iY += iFontHeight;
	iBmpGc->DrawText(aText,point);
	iBmpGc->DiscardFont();
}

void CGraphic::DrawText(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz,TInt aMargin)
{
	iBmpGc->UseFont(iFont);

	TInt baseLine = iFontHeight + ((aBox.Height() - iFontHeight) >> 2);
	iBmpGc->DrawText(aText,aBox,baseLine,aHrz,aMargin);
}

void CGraphic::DrawRect(const TRect& aRect )
{
	iBmpGc->DrawRect(aRect);
}

void CGraphic::DrawRoundRect(const TRect& aRect)
{
	TSize cornerSize(3,3);
	iBmpGc->DrawRoundRect(aRect,cornerSize);
}

void CGraphic::BitBlt( const TPoint& aPos,const CFbsBitmap* aBitmap)
{
	iBmpGc->BitBlt(aPos,aBitmap);
}

void CGraphic::BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aRect)
{
	iBmpGc->BitBlt(aPoint,aBitmap,aRect);
}

void CGraphic::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask,TBool aInvertMask)
{
	TSize size = aBitmap->SizeInPixels();
	iBmpGc->BitBltMasked(aPoint,aBitmap,size,aBitmapMask,aInvertMask);
}

void CGraphic::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask,const TRect& aRect,TBool aInvertMask)
{
	iBmpGc->BitBltMasked(aPoint,aBitmap,aRect,aBitmapMask,aInvertMask);
}

void CGraphic::DrawLine(TPoint aPos1,TPoint aPos2)
{
	iBmpGc->DrawLine(aPos1,aPos2);
}

void CGraphic::DrawText_Bold(TPoint aPos,const TDesC& aText)
{
	TRgb argb(0,0,0);
	SetPenColor(argb);
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = iFont->HeightInPixels();
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

void CGraphic::DrawText_Blue(TPoint aPos,const TDesC& aText)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = iFont->HeightInPixels();
	aPos.iY+=aFontHeight;
	TRgb argb(204,204,255);
	SetPenColor(argb);
	iBmpGc->DrawText(aText,aPos);
	TInt width=iFont->MeasureText(aText);
	iBmpGc->DrawLine(aPos+TPoint(0,1),aPos+TPoint(width,1));
	iBmpGc->DiscardFont();

	SetPenColor(KTextColor);
}
void CGraphic::DrawText_Blue(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz,TInt aMargin)
{
	iBmpGc->UseFont(iFont);

	TInt aFontHeight = iFont->HeightInPixels();
	TInt baseLine = aFontHeight + ((aBox.Height() - aFontHeight) >> 2);

	TRgb argb(204,204,255);
	SetPenColor(argb);
	iBmpGc->DrawText(aText,aBox,baseLine,aHrz,aMargin);

	TPoint point=aBox.iTl;
	point.iY+=aBox.Width()-1;

	TInt width=iFont->MeasureText(aText);
	iBmpGc->DrawLine(point+TPoint(0,1),point+TPoint(width,1));
	iBmpGc->DiscardFont();

	SetPenColor(KTextColor);
}
void CGraphic::DrawText_Red(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz,TInt aMargin)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = iFont->HeightInPixels();
	TInt baseLine = aFontHeight + ((aBox.Height() - aFontHeight) >> 2);

	SetPenStyle(CGraphicsContext::ENullPen);
	SetBrushColor(TRgb(99,200,255));
	SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBmpGc->DrawRect(aBox);
	
	SetPenStyle(CGraphicsContext::ESolidPen);
	iBmpGc->DrawText(aText,aBox,baseLine,aHrz,aMargin);
	
	
	TPoint point=aBox.iTl;
	point.iY+=aBox.Width()-1;
	TInt width=iFont->MeasureText(aText);

	iBmpGc->DrawLine(point+TPoint(0,1),point+TPoint(width,1));
	iBmpGc->DiscardFont();

	SetBrushStyle(CGraphicsContext::ENullBrush);
	SetPenColor(KTextColor);

}
void CGraphic::DrawText_Red(TPoint aPos,const TDesC& aText)
{
	iBmpGc->UseFont(iFont);
	TInt aFontHeight = iFont->HeightInPixels();
	aPos.iY+=aFontHeight;
	TInt width=iFont->MeasureText(aText);

	SetPenStyle(CGraphicsContext::ENullPen);
	SetBrushColor(TRgb(99,200,255));
	SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(aPos-TPoint(0,iFontHeight), TSize(width,iFontHeight));
	iBmpGc->DrawRect(rect);
	
	SetPenStyle(CGraphicsContext::ESolidPen);
	iBmpGc->DrawText(aText,aPos);
	
	iBmpGc->DrawLine(aPos+TPoint(0,1),aPos+TPoint(width,1));
	iBmpGc->DiscardFont();

	SetBrushStyle(CGraphicsContext::ENullBrush);
	SetPenColor(KTextColor);	
	
}

void CGraphic::DrawTextWithBlueUnderline( const TPoint& aPoint,const TDesC& aText )
{
	iBmpGc->UseFont(iFont);
	TPoint point = aPoint;
	point.iY += iFont->HeightInPixels();
	TRgb argb(204,204,255);
	SetPenColor(argb);
	iBmpGc->DrawText(aText,point);
	TInt i_w= iFont->MeasureText(aText);
	iBmpGc->DrawLine(point+TPoint(0,1),point+TPoint(i_w,1));
	iBmpGc->DiscardFont();
	SetPenColor(KTextColor);
}

TInt CGraphic::MeasureText(const TDesC& aDes) const
{
	ASSERT(iFont);
	return iFont->MeasureText(aDes);
}

CFbsBitGc& CGraphic::GetBmpGc() const
{
	ASSERT(iBmpGc);
	return *iBmpGc;
}

const CFont* CGraphic::GetFont() const
{
	ASSERT(iFont);
	return iFont;
}

const CFbsBitmap* CGraphic::GetOffScreenBitmap() const
{
	return iBitmap;
}

TInt CGraphic::FontHeight() const
{
	return iFontHeight;
}

TBool CGraphic::AlphaBlend(TPoint pos, CFbsBitmap* image, const TRect& rect, TInt nAlpha)
{
	union UColor
	{
		TUint16	wColor[2];
		TUint32	dwColor;
	};

	TUint16* uSrcBuf = new TUint16[iAllSize.iWidth*2];
	TUint8*  uSrcBufMask = new TUint8[iAllSize.iWidth*2];
	TPtr8	pSrcBuf((TUint8*)(uSrcBuf), sizeof(TUint16)*iAllSize.iWidth*2);
	TPtr8	pSrcBufMask(uSrcBufMask, sizeof(TUint8)*iAllSize.iWidth*2);


	TUint16* uDesBuf = new TUint16[iAllSize.iWidth*2];
	TUint8*  uDesBufMask = new TUint8[iAllSize.iWidth*2];
	TPtr8	pDesBuf((TUint8*)(uDesBuf), sizeof(TUint16)*iAllSize.iWidth*2);
	TPtr8	pDesBufMask(uDesBufMask, sizeof(TUint8)*iAllSize.iWidth*2);

	TBool	bHasSrcMask =EFalse;// image->HasMask();
	TBool	bHasDesMask =EFalse;// iBitmap->HasMask();

	UColor	uSrcColor, uDesColor;

	nAlpha = (nAlpha >> 3) & 0x1F;

	for (TInt nLine = 0; nLine < rect.Size().iHeight; nLine ++)
	{
		image->GetScanLine(pSrcBuf, rect.iTl + TPoint(0,nLine), rect.Width(), EColor64K);
		//if (bHasSrcMask)
			//image->GetScanLineMask(pSrcBufMask, rect.iTl + TPoint(0,nLine), rect.Width(), EColor256);

		TUint16*	pSrcColor = uSrcBuf;
		TUint8*		pSrcMask = uSrcBufMask;

		GetScanLine(pDesBuf, TPoint(0, pos.iY + nLine), iAllSize.iWidth, EColor64K);
		/*if (bHasDesMask)
			GetScanLineMask(pDesBufMask, TPoint(0, pos.iY + nLine),iAllSize.iWidth, EColor256);*/
		TUint16*	pDesColor = &uDesBuf[pos.iX];
		TUint8*		pDesMask = &uDesBufMask[pos.iX];

		for (TInt nCol = 0; nCol < rect.Width(); nCol ++, pSrcColor ++, pDesColor ++, pSrcMask ++, pDesMask++)
		{
			if (bHasSrcMask && !(*pSrcMask))
				continue;
			if (bHasDesMask && !(*pDesMask))
				continue;

			//5:6:5
			uSrcColor.wColor[0] = *pSrcColor;
			uSrcColor.wColor[1] = *pSrcColor;
			uSrcColor.dwColor &= ALPHA_COLOR_MASK;
			uDesColor.wColor[0] = *pDesColor;
			uDesColor.wColor[1] = *pDesColor;
			uDesColor.dwColor &= ALPHA_COLOR_MASK;

			uDesColor.dwColor = uDesColor.dwColor * (KMaxAlpha - nAlpha) + uSrcColor.dwColor * nAlpha;
			uDesColor.dwColor = (uDesColor.dwColor >> 5) & ALPHA_COLOR_MASK;
			*pDesColor = (TUint16)(uDesColor.wColor[0] | uDesColor.wColor[1]);
		}
		SetScanLine(pDesBuf, pos.iY + nLine);
	}

 	delete [] uSrcBuf;
 	delete [] uSrcBufMask;
 	delete [] uDesBuf;
 	delete [] uDesBufMask;

	return	ETrue;
}
//*/

/*TBool CGraphic::Shadow(TInt nAlpha, TRect* pRect)
{
	union UColor
	{
		TUint16	wColor[2];
		TUint32	dwColor;
	};
	TUint16	uBuf[iAllSize.iWidth];
	TUint8	uBufMask[iAllSize.iWidth];
	TPtr8	pBuf((TUint8*)(&uBuf[0]), sizeof(uBuf));
	TPtr8	pBufMask(uBufMask, sizeof(uBufMask));
	TBool	bHasMask = iBitmap->HasMask();
	UColor	uColor;
	TRect	rect;

	nAlpha = (nAlpha >> 3) & 0x1F;
	rect = pRect ? (*pRect) : TRect(TPoint(0,0), iAllSize);

	for (TInt nLine = 0; nLine < rect.Size().iHeight; nLine ++)
	{
		GetScanLine(pBuf, TPoint(0, rect.iTl.iY + nLine), iAllSize.iWidth, EColor64K);
		if (bHasMask)
			GetScanLineMask(pBufMask, TPoint(0, rect.iTl.iY + nLine), iAllSize.iWidth, EColor256);
		TUint16*	pColor = &uBuf[rect.iTl.iX];
		TUint8*		pMask = &uBufMask[rect.iTl.iX];

		for (TInt nCol = 0; nCol < rect.Width(); nCol ++, pColor ++, pMask++)
		{
			if (bHasMask && !(*pMask))
				continue;
			uColor.wColor[0] = *pColor;
			uColor.wColor[1] = *pColor;
			uColor.dwColor &= ALPHA_COLOR_MASK;

			uColor.dwColor = uColor.dwColor * nAlpha;
			uColor.dwColor = (uColor.dwColor >> 5) & ALPHA_COLOR_MASK;
			*pColor = (TUint16)(uColor.wColor[0] | uColor.wColor[1]);
		}
		SetScanLine(pBuf, rect.iTl.iY + nLine);
	}
	return	ETrue;
}*/