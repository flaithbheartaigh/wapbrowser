/*
============================================================================
 Name        : InputDialog.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : soarsky
 Description : 提示输入框控件
============================================================================
*/

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

// INCLUDES
#include <eikedwin.h>

#include "Control.h"
#include "NotifyTimer.h"

class CMainEngine;
class MInputObserver;
class CScreenLayout;
class CBitmapFactory;
// CLASS DECLARATION

/**
*  CInputDialog
* 
*/
class CInputDialog : public CControl , public MTimerNotifier 
{
public: // Constructors and destructor

	~CInputDialog();
	static CInputDialog* NewL(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);
	static CInputDialog* NewLC(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);

private:
	CInputDialog(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public://From MTimerNotifier
	virtual TBool DoPeriodTask();

public:
	void SetTitle(const TDesC& aTitle); 
	void SetLableText(const TDesC& aText);

	void SetIsNull(TBool aBool);
	void SetInputType(TInt aType);
	void SetInputVisible(TBool aBool);

	void SetInfo(const TDesC& aText);

	void SetEdwinText(const TDesC& aText);

private:

	void InitBitmaps();
	void InitInput(TInt aType);
private:

	CFbsBitmap*		iDialogBmp;
	
	CBitmapFactory& iBitmapFactory;

	MInputObserver& iObserver;
	CMainEngine& iMainEngine;
	const CScreenLayout& iScreenLayout;

	HBufC* iTitle;
	HBufC* iLableText;
	HBufC* iText;
	HBufC* iInfo;

	TBool iIsNull;

	TInt iType; //输入类型 1任何字符,2数字

	TRect iRect;
	TSize iDialogSize;
	TSize iIconSize;

	TPoint iStartPoint;
	TPoint iIconPoint;

	TInt iMargin;

	CEikEdwin* iEdwin;

	TInt		iCommandType;
};

#endif // INPUTDIALOG_H
