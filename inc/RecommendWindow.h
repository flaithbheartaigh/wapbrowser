/*
============================================================================
 Name        : RecommendWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CRecommendWindow declaration
============================================================================
*/

#ifndef RECOMMENDWINDOW_H
#define RECOMMENDWINDOW_H

#include "Window.h"
#include "PopUpMenuObserver.h"
#include "HttpObserver.h"
#include "DialogObserver.h"
#include "NotifyTimer.h"
#include "CoCoPreDefine.h"

class CPbkContactEngine;
class MObjectProvider;
class CEikGlobalTextEditor;
class CEditorsManager;
class CScreenLayout;
class CControl;
class CFbsBitmap;
class CTitleBar;

#ifndef __SERIES60_3X__
	class CPbkContactEngine;
#endif

class CRecommendWindow : public CWindow  ,  public MTimerNotifier, public MPopUpMenuObserver , public MOperationObserver , public MDialogObserver
{
public: // Constructors and destructor
	~CRecommendWindow();
	static CRecommendWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CRecommendWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CRecommendWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();

private://From CWindow
	//派生类实现激活视图
	virtual void	DoActivateL();
	//派生类实现冻结视图
	virtual void	DoDeactivate();
	//派生类实现绘图
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//派生类实现按键事件响应
	virtual TBool	DoKeyEventL(TInt aKeyCode);
	//派生类实现命令事件响应
	virtual TBool	DoHandleCommandL(TInt aCommand);
	//派生类实现界面尺寸改变
	virtual void	DoSizeChanged();

	//派生类实现设置左右按钮,以便覆盖按钮的控件返回时回复原样
	virtual void	ChangeButton();

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
	virtual	void PopUpDestried();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

private:
	void	InitPopUpMenu();

	void	InitMultiText();

	void	InitEdwin();

	void	ResetEdwin();

	void	DrawInputType(CGraphic& aGraphic) const;

	void	RequestRecommend();

	TInt	HandelMobile(const TDesC& aDes);

	void	InitBitmaps();

	TInt	Check(const TDesC& aDes,TInt aDepth = 0);	//多于5个号码或号码长度不正确时返回EFalse
	TBool	IsValidPhoneNumber(const TDesC& aDes) const;

	#ifndef __SERIES60_3X__
		void	DisplayPhoneNumbersL(MObjectProvider* aMopParent);
		void	CheckPhoneNumber();
	#endif

	TInt	GetEdwinHeight() const;

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();
private:

	#ifndef __SERIES60_3X__
		CPbkContactEngine* iPbkContactEngine;
		TInt iNum;
		TFileName iPhoneText;
	#endif

	CNotifyTimer*	iTimer;
	CTitleBar* iTitleBar;

	CEditorsManager&		iEditorsManager;
	const CScreenLayout&	iScreenLayout;

	CEikGlobalTextEditor*	iContentEdit;

	CControl*				iDialog;

	TPoint					iIconPoint;
};

#endif // RECOMMENDWINDOW_H
