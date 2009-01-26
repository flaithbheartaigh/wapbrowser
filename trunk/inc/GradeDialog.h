/*
============================================================================
 Name        : GradeDialog.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CGradeDialog declaration
============================================================================
*/

#ifndef GRADEDIALOG_H
#define GRADEDIALOG_H

// INCLUDES
#include "Control.h"

class CMainEngine;
class MGradeObserver;
class CScreenLayout;
class CBitmapFactory;

// CLASS DECLARATION

/**
*  CGradeDialog
* 
*/
class CGradeDialog : public CControl
{
public: // Constructors and destructor


	~CGradeDialog();
	static CGradeDialog* NewL(MGradeObserver& aObserver,CMainEngine& aMainEngine);
	static CGradeDialog* NewLC(MGradeObserver& aObserver,CMainEngine& aMainEngine);

private:

	CGradeDialog(MGradeObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public:
	void SetTitle(const TDesC& aTitle); 
	void SetGradeNum(TInt aNum);
private:
	void InitBitmaps();
private:
	CFbsBitmap*		iDialogBmp;

	MGradeObserver& iObserver;
	CMainEngine& iMainEngine;
	const CScreenLayout& iScreenLayout;
	CBitmapFactory& iBitmapFactory;

	TInt iGradeNum;
	TBuf<20> iTitle;

	TSize iDialogSize;
	TSize iIconSize;

	TPoint iStartPoint;
	TPoint iIconPoint;
};

#endif // GRADEDIALOG_H
