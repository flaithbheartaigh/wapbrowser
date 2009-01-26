/*
============================================================================
 Name        : ContentInfoDialog.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CContentInfoDialog declaration
============================================================================
*/

#ifndef CONTENTINFODIALOG_H
#define CONTENTINFODIALOG_H

// INCLUDES
#include "Control.h"
#include <BADESCA.H>
#include "MultimediaController.h"

class CMainEngine;
class CScreenLayout;
class MContentInfoObserver;
class CImageManipAdapter;
class CScrollBar;
// CLASS DECLARATION

/**
*  CContentInfoDialog
* 
*/
class CContentInfoDialog : public CControl,public MMultimediaController
{
public: // Constructors and destructor

	~CContentInfoDialog();
	static CContentInfoDialog* NewL(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);
	static CContentInfoDialog* NewLC(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);

private:

	CContentInfoDialog(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public: // from MMultimediaController
    virtual void RedrawView(TInt aType); // used by engine to redraw view
public:
	void SetTilte(const TDesC& aTitle);
	void SetName(const TDesC& aName);
	void SetSize(const TDesC& aSize);
	void SetType(const TDesC& aType);
	void SetRemark(const TDesC& aRemark);
	void SetBmpPath(const TDesC& aPath);

	void InitBitmap();
	void SetBmpShow(TBool aBool);
	void InitScrollBar();
private:
	void AddTextItem(const TDesC& aText);
	void InitImageManipAdapter();
private:

	CFbsBitmap*		iShowBmp;

	CImageManipAdapter* iAdapter;
	MContentInfoObserver& iObserver;
	CMainEngine& iMainEngine;
	const CScreenLayout& iScreenLayout;

	HBufC* iTitle;

	HBufC* iName;
	HBufC* iSize;
	HBufC* iType;
	HBufC* iRemark;
	HBufC* iBmpPath;

	TSize iTextSize;
	TSize iIconSize;
	TRect iBackRect;

	TPoint iStartPoint;
	TPoint iIconPoint;

	TInt iCommandType;

	TInt iMaxLine;
	TInt iAllLine;

	TInt iTextHeight;
	TInt iMargin;

	TInt iFirstIndex;
	TInt iSelectIndex;

	TInt iLineWidth;
	TBool iBool;
	CDesCArray* iDesArray;
	CScrollBar*	iScrollbar;
};

#endif // CONTENTINFODIALOG_H
