/*
============================================================================
 Name        : CopyToDialog.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CCopyToDialog declaration
============================================================================
*/

#ifndef COPYTODIALOG_H
#define COPYTODIALOG_H

// INCLUDES
#include "Control.h"
#include <BADESCA.H>

class CMainEngine;
class MCopyToObserver;
class CScreenLayout;

// CLASS DECLARATION

/**
*  CCopyToDialog
* 
*/
class CCopyToDialog : public CControl 
{
public: // Constructors and destructor

	~CCopyToDialog();
	static CCopyToDialog* NewL(MCopyToObserver& aObserver,CMainEngine& aMainEngine,const TDesC& aTitle);
	static CCopyToDialog* NewLC(MCopyToObserver& aObserver,CMainEngine& aMainEngine,const TDesC& aTitle);

private:

	CCopyToDialog(MCopyToObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL(const TDesC& aTitle);

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public:
	void AppendItem(const TDesC& aName,const TDesC& aPath); 

private:
	void InitBitmaps();
	void DrawItem(CGraphic& gc,TPoint& aPoint,TInt aIndex) const;
private:
	CFbsBitmap*		iDialogBmp;

	MCopyToObserver& iObserver;
	CMainEngine& iMainEngine;
	const CScreenLayout& iScreenLayout;

	HBufC* iTitle;
	HBufC* iPath;

	CDesCArray*	iDesNameArray;
	CDesCArray*	iDesPathArray;

	TSize iItemSize;
	TSize iDialogSize;

	TPoint iStartPoint;

	TInt iMaxLinePerPage;
	TInt iSelectedIndex;
	TInt iFirstIndex;
};

#endif // COPYTODIALOG_H
