/*
============================================================================
 Name        : ControlText.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CControlText declaration
============================================================================
*/

#ifndef CONTROLTEXT_H
#define CONTROLTEXT_H

// INCLUDES
#include "Control.h"
#include "NotifyTimer.h"
#include "PreDefine.h"

class MDrawEventObserver;
class CMainEngine;
class CBitmapFactory;
// CLASS DECLARATION

/**
*  CControlText
* 
*/
class CControlText : public CControl , public MTimerNotifier
{
public: // Constructors and destructor

	~CControlText();
	static CControlText* NewL(MDrawEventObserver& aObserver,CMainEngine& aMainEngine);
	static CControlText* NewLC(MDrawEventObserver& aObserver,CMainEngine& aMainEngine);

private:
	CControlText(MDrawEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();
public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();

public:
	void	SetClientRect(const TRect& aRect);
	//void	Layout();
	void	SetSpeed(TInt aSpeed){iSpeed=aSpeed;}
	void	Begin();
	void	Stop();
private:
	void	InitBtimap();
	void	IninShowTime();
	void	IninShowLoginType();
	void	InitShowWeather();
	void	IninShowDownLoading();
private:
	CMainEngine& iMainEngine;
	CBitmapFactory& iBitmapFactory;
	
	TSize iIconSize;
	TPoint iStartPoint;
	
	TSize iShowIconSize;

	CNotifyTimer*	iTimer;

	TBuf<50> iText;

	TRect	iClientRect;			//限制字符串只能绘制在这个矩形以内

	TInt	iStartPos;
	TInt	iTextWidth;

	TInt	iSpeed;

	TInt	iStopTime;			//保存停止时间

	TInt iIconIndex;

	TInt iShowIndex;
	TBool iBool;

};

#endif // CONTROLTEXT_H
