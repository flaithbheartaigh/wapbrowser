/*
============================================================================
Name        : Dialog.h
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CDialog declaration
============================================================================
*/

#ifndef DIALOG_H
#define DIALOG_H

#include "Control.h"
// #include "Basesprite.h"
// #include "PreDefine.h"
#include "NotifyTimer.h"

class CMultiText;
class CMainEngine;
class CScreenLayout;
class CBitmapFactory;
class MDialogObserver;
class MCommondBase;

class CDialog : public CControl , public MTimerNotifier
{
public:
	enum TDialogType
	{
		EDialogTip,			//提示
		EDialogInquire,		//询问
		EDialogWaiting,		//等待
		EDialogWaitingWithoutCancel,		//等待界面不可取消
	};
public: // Constructors and destructor
	~CDialog();
	static CDialog* NewL(const TDesC& aText,MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType = EDialogTip);
	static CDialog* NewLC(const TDesC& aText,MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType = EDialogTip);

private:
	CDialog(MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType);
	void ConstructL(const TDesC& aText);

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();

public:
// 	void	Draw(CBitmapContext& gc)const;
// 	TBool	KeyEventL(TInt aKeyCode);
	//void	DoPeriodTask();

	void	SetLeftCommand(MCommondBase* aCommand);
	void	SetRightCommand(MCommondBase* aCommand);

	void	SetLeftCommandType(TInt aCommand);
	void	SetRightCommandType(TInt aCommand);

	void	SetLeftButtonL(const TDesC& aDes);
	void	SetRightButtonL(const TDesC& aDes);

	void	SetRect(const TRect& aRect);

	void	SetDismissTime(TInt aTime);

	void	SetIconType(TInt aIconType);			//临时使用

private:
	void	InitMultiTextL(const TDesC& aText);

	void	DrawBackground(CGraphic& gc)const;
	void	DrawIcon(CGraphic& gc)const;
	void	DrawButton(CGraphic& gc)const;
	void	DrawAnimation(CGraphic& gc)const;


	void	InitBitmaps();
	void	ReleaseBmps();

	void	InitAnimBitmapL();

private:
	MDialogObserver&		iObserver;
	CMainEngine&			iMainEngine;	//临时使用
	const CScreenLayout&	iScreenLayout;
	const CBitmapFactory&	iBitmapFactory; 

	CMultiText*		iMultiText;

	CNotifyTimer*	iNotifyTimer;

	TInt			iDialogType;

	MCommondBase*	iLeftCommand;
	MCommondBase*	iRightCommand;

	TInt			iLeftCommandType;
	TInt			iRightCommandType;

	HBufC*			iLeftButtonText;
	HBufC*			iRightButtonText;

	TRect			iRect;

	const CFont*	iFont;


	TInt			iIconType;

	TInt			iDismissTime;	//对话框多久后自动消失
	TInt			iTimer;			//计时
	TInt			iWaitingIndex;


	CFbsBitmap*		iWaitingBmp;
	CFbsBitmap*		iDialogBmp;
	CFbsBitmap*		iDialogIcon;
	CFbsBitmap*		iDialogIconMask;

	//TRect			iTextRect;

	TSize			iWaitingSize;
	TSize			iDialogSize;
	TSize			iIconSize;

	TPoint			iAnimPoint;
	TPoint			iStartPoint;
	TPoint			iIconPoint;

	TInt			iTextLengthPerLine;
	TInt			iMargin;
};

#endif // DIALOG_H
