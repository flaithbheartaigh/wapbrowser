/*
============================================================================
 Name        : SlideText.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CSlide declaration

 描述		:	实现文字跑马灯的效果,即定时的滑动显示
				目前只做保存一行文本的功能,以后可实现多条字符串滚动的情况
				
 使用范例	:	使用前需要调用以下函数初始化
				SetFont(const CFont* aFont);
				SetText(const TDesC& aDes);
				SetTextColor(TRgb aColor);
				SetMoveSpeed(TInt aSpeed);
				SetClientRect(const TRect& aRect);
				Layout(TVerticalAlign aAlign);
============================================================================
*/

#ifndef SLIDE_H
#define SLIDE_H

//#include <gdi.h>

#include "Control.h"
#include "NotifyTimer.h"
#include "PreDefine.h"

class MDrawEventObserver;

enum TVerticalAlign
{
	EUpAlign,
	ECenterAlign,
    EDownAlign
};

class CSlideText : public CControl , public MTimerNotifier
{
public:
	enum TStyle
	{
		ELeftWithPause,		//向左循环,到最左边后恢复原状,停留数秒
		ELeftCycle,			//向左循环,然后再从右向左循环
		EGoOrBack,			//来回往复,先左后右
	};
public: // Constructors and destructor
	~CSlideText();
	static CSlideText* NewL(MDrawEventObserver& aObserver,TInt aStyle = ELeftCycle);
	static CSlideText* NewLC(MDrawEventObserver& aObserver,TInt aStyle = ELeftCycle);

private:
	CSlideText(MDrawEventObserver& aObserver,TInt aStyle = ELeftCycle);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();

public:
	void	SetFont(const CFont* aFont);
	void	SetText(const TDesC& aDes);
	void	SetClientRect(const TRect& aRect);
	void	SetStopTime(TInt aStopTime);		//滚动的停止时间,动态的控制滚动
	void	SetTextColor(TRgb aColor);
	void	SetMoveSpeed(TInt aSpeed);
	void	SetStyle(TInt aStyle,TInt aTimeInterval,TInt aStopInterval = 0);			//设置类型,默认为 ELeftWithPause
	void	Layout(TVerticalAlign aAlign);
	void	Layout(const TDesC& aDes,const TRect& aRect,TVerticalAlign aAlign);

	void	Begin();
	void	Stop();

	const TDesC& GetText() const;

private:
	TBool	LeftWithPause();
	TBool	LeftCycle();
	TBool	GoOrBack();

private:
	void	Assert()const;

private:
	const CFont*	iFont;
	CNotifyTimer*	iTimer;

	HBufC*			iText;

	TRect	iClientRect;			//限制字符串只能绘制在这个矩形以内

	TRgb	iTextColor;

	TInt	iStyle;

	TInt	iStartPos;
	TInt	iTextWidth;
	TInt	iBaseLine;
	TInt	iSpeed;

	TInt	iStopTime;			//保存停止时间
	TInt	iStopTimer;			//

	TBool	iAlreadyLayout;
};

#endif // SLIDE_H
