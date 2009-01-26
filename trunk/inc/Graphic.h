//////////////////////////////////////////////////////////////////////////
//CGraphic
//以Atapter模式封装图形上下文
//////////////////////////////////////////////////////////////////////

#ifndef		__GRAPHIC_H__
#define		__GRAPHIC_H__


#include <e32base.h>    // CBase
#include <FBS.H>
#include <gdi.h>


class CFbsBitmap;
class CFbsBitGc;
class CFbsBitmapDevice;


class CScreenLayout;

class CGraphic : public CBase  
{
public:
	enum TAlign
	{

	};
public:
	static CGraphic* NewL(const TSize& aSize);
	virtual ~CGraphic();

private:	
	CGraphic();
	void	ConstructL(const TSize& aSize);

public:
	void			Clear();
	void			Clear(const TRect& aRect);

	void			SetClippingRect(const TRect& aRect);
	void			SetClippintRegion(const TRegion& aRegion);
	void			CancelClippingRect();


	void			SetPenColor(TRgb aColor);
	void			SetPenStyle(CGraphicsContext::TPenStyle	aPenStyle);
	void			SetBrushColor(TRgb aColor);
	void			SetBrushStyle(CGraphicsContext::TBrushStyle	aPenStyle);

	void			DrawText(const TPoint& aPoint,const TDesC& aText);

	//在一个指定的矩形中画文字,根据参数给出的值决定居中与留白,默认上下居中
	void			DrawText(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz = CGraphicsContext::ELeft,TInt aMargin=0);

	void			DrawRect(const TRect& aRect);

	void			DrawRoundRect(const TRect& aRect/*,const TSize& aCornerSize*/);

	void			BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap);

	void			BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aRect);

	void			BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask,TBool aInvertMask = ETrue);

	void			BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask,const TRect& aRect,TBool aInvertMask = ETrue);

	void			DrawTextWithBlueUnderline(const TPoint& aPos,const TDesC& aText);

	TInt			MeasureText(const TDesC& aDes) const;

	CFbsBitGc&		GetBmpGc() const;
	const CFont*	GetFont() const; 
	const CFbsBitmap*		GetOffScreenBitmap() const;

	TInt			FontHeight() const;

	void			DrawLine(TPoint aPos1,TPoint aPos2);
	void			DrawText_Bold(TPoint aPos,const TDesC& aText);

	void			DrawText_Blue(TPoint aPos,const TDesC& aText);
	void			DrawText_Red(TPoint aPos,const TDesC& aText);

	void			DrawText_Blue(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz = CGraphicsContext::ELeft,TInt aMargin=0);
	void			DrawText_Red(const TDesC& aText,const TRect& aBox,CGraphicsContext::TTextAlign aHrz = CGraphicsContext::ELeft,TInt aMargin=0);

	TBool AlphaBlend(TPoint pos, CFbsBitmap* image, const TRect& rect, TInt nAlpha);
	//TBool Shadow(TInt nAlpha, TRect* pRect = NULL);
	inline TBool GetScanLine(TDes8& aBuf, const TPoint& aPixel, TInt aLength,TDisplayMode aDispMode)
	{
		if (iBitmap)
		{
			iBitmap->GetScanLine(aBuf, aPixel, aLength, aDispMode);
			return	ETrue;
		}
		else
			return	EFalse;
	}

	inline TBool SetScanLine(TDes8& aBuf,TInt aY)
	{
		if (iBitmap)
		{
			iBitmap->SetScanLine(aBuf, aY);
			return	ETrue;
		}
		else
			return	EFalse;
	}
private:
	const CFont*			iFont;
	CFbsBitmap*				iBitmap;
	CFbsBitmapDevice*		iBitmapDevice;
	CFbsBitGc*				iBmpGc;

	TInt					iFontHeight;

	TSize iAllSize;
	TInt	iPos, iMaskPos;
//	CScreenLayout*			iScreenLayout;
};

#endif //__GRAPHIC_H__
