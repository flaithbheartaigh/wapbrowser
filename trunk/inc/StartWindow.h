/*
============================================================================
 Name        : StartWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CStartWindow declaration
============================================================================
*/

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "Window.h"
#include "NotifyTimer.h"
#include "MControlObserver.h"
#include "DialogObserver.h"

class CHandleLoginOrExit;
class CHandleSystemInfo;
class CHandleSynchronization;
class CMainEngine;
class CDialog;
class CSystemInfoDialog;
enum TLoginState
{
	ELoginNone,
	ELoginGateWay,
	ELoginSystemInfo,
	ELoginSyschron,
	ELoginAppUpdate
};
class CStartWindow : public CWindow , public MTimerNotifier,public MInfoObserver,public MHandleEventObserver,public MDialogObserver
{
public: // Constructors and destructor
	~CStartWindow();
	static CStartWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CStartWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();

public://From Dialog
	virtual void DialogEvent(TInt aCommand);
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
public://From InfoDialog
	virtual void InfoResponseEvent(TInt aEvent);
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

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();

private:
	void InitBmpPoint();
	void InitDialog(const TDesC& aText);
	void InitSystemInfo(const TDesC& aTitle,const TDesC& aText,TInt aType);

	void HandleLoginState();
	void HandleSynchronState();
	void HandleSystemInfo();

	void DrawControlButton(CGraphic& gc) const;
private:
	CFbsBitmap*			iControlPaneBmp;

	TBool iInitDownLoadFlag;
	CDialog* iDialog;
	CSystemInfoDialog* iSystemInfoDialog;
	CHandleSynchronization& iSyschr;
	CHandleLoginOrExit* iLogin;
	
	CHandleSystemInfo* iSystemInfo;

	CNotifyTimer*	iTimer;

	CMainEngine& iMainEngine;

	TBool iSystemInfoBool;
	TInt iStatueFlag;
	TInt iIndex;
	TPoint iIconPoint;
	TSize iIconSize;
	TInt iNum;
	//TInt			iMaxTimeLimit;
	TInt			iTimeLimit;
	TBuf<5> iLeftText;
	TBuf<5> iRightText;
	TBool iButtonFlag;
	TBool iFlag;
};

#endif // STARTWINDOW_H
