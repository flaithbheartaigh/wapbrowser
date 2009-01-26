/*
============================================================================
 Name        : GraphicsEngine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对于系统提供的绘制图片、文字和图画等API进行封装，方便后期的移植工作
============================================================================
*/

#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

// INCLUDES
#include <e32std.h>
#include <gdi.h>


class CFbsBitmap;
class CFbsBitGc;
class CFbsBitmapDevice;
class CBmpPicture;
class CBmpPictureArray;
class CMainControlEngine;
// CLASS DECLARATION

/**
*  CGraphicsEngine
* 
*/
class CGraphicsEngine : public CBase
{
public: // Constructors and destructor

	~CGraphicsEngine();
	static CGraphicsEngine* NewL(CMainControlEngine& aMainControlEngine);
	static CGraphicsEngine* NewLC(CMainControlEngine& aMainControlEngine);

private:

	CGraphicsEngine(CMainControlEngine& aMainControlEngine);
	void ConstructL();

public:
	void			Create(TInt nSizeWidth,TInt nSizeHeight);
	void			SetFont(const CFont*	aFont);
	const CFont*	GetFont(); 
	CBmpPicture*		GetImgBackground();
	void			SetPenColor(TRgb aColor);
	void			SetPenStyle(CGraphicsContext::TPenStyle	aPenStyle);
	void			SetBrushColor(TRgb aColor);
	void			SetBrushStyle(CGraphicsContext::TBrushStyle	aPenStyle);
	void			DrawText(TPoint aPos,const TDesC& aText);
	void			DrawText_Bold(TPoint aPos,const TDesC& aText);

	void			DrawText_Blue(TPoint aPos,const TDesC& aText);
	void			DrawText_Red(TPoint aPos,const TDesC& aText);

	void			BitBltBmp(TPoint aPos,CFbsBitmap* aBitmap);
public:
	void			BitBltMasked(TPoint aPos,CBmpPicture* aImage);
	void			BitBltMasked(TPoint aPos,CBmpPicture* aImage,TRect aRect);
	void			BitBltMasked(TPoint aPos,CBmpPictureArray* aImage,TInt aIndex);
	
public:
	void			DrawLine(TPoint aPos1,TPoint aPos2);
	void			DrawRect(TRect aRect);

public:
	void			Clear();
	void			ClearRect(TRect& aRect);

	void		SetClippingRect(TRect aRect);
	void		CancelClippingRect();
	CFbsBitGc*   GetBmpGc()
	{
	  return 	iBmpGc;
	}
	
private:
	const CFont		*iFont;
	CBmpPicture		*iBitmap;
	CFbsBitmapDevice	*iDevice;
	CFbsBitGc			*iBmpGc;
	CMainControlEngine& iMainControlEngine;
};

#endif // GRAPHICSENGINE_H
