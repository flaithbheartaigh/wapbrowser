/*
============================================================================
Name        : TitleBar.h
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CTitleBar declaration
============================================================================
*/

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "Control.h"
#include "PreDefine.h"

class CMainEngine;
class CSlideText;

class CTitleBar : public CControl
{
public: // Constructors and destructor
	~CTitleBar();
	static CTitleBar* NewL(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont,const TDesC& aTitleDes = KNullDesC);
	static CTitleBar* NewLC(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont,const TDesC& aTitleDes = KNullDesC);

private:
	CTitleBar(CMainEngine& aMainEngine,const TRect& aRect,const CFont* aFont);
	void ConstructL(const TDesC& aTitleDes = KNullDesC);

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);	//待论证

public:
	//void	Draw(CBitmapContext& gc)const;

	void	SetTextL(const TDesC& aTitleDes);
	void	SetRightInfoL(const TDesC& aInfo);
	//void	SetRect(const TRect& aRect);
	void	SetLeftBitmap(const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask = NULL);
	void	SetRightBitmap(const CFbsBitmap* aBitmap,const CFbsBitmap* aBitmapMask = NULL);

	void	InitBitmap();

private:
	//HBufC*	iTitleText;
	CMainEngine&	iMainEngine;

	CSlideText*		iSlideText;
	TBuf<100>		iTitleText;
	HBufC*			iRightInfoText;
	const CFont*	iFont;
	const CFbsBitmap*	iLeftBitmap;
	const CFbsBitmap*	iLeftBitmapMask;
	const CFbsBitmap*	iRightBitmap;
	const CFbsBitmap*	iRightBitmapMask;

	TRect			iRect;
	TRect			iSlideRect;
	//HBufC*	
	CFbsBitmap*		iTitleBitmap;

};

#endif // TITLEBAR_H
