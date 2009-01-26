/*
============================================================================
 Name        : EndWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CEndWindow declaration
============================================================================
*/

#ifndef ENDWINDOW_H
#define ENDWINDOW_H

#include "Window.h"
#include "NotifyTimer.h"
#include "MControlObserver.h"
class CHandleLoginOrExit;

class CEndWindow : public CWindow , public MTimerNotifier,public MHandleEventObserver
{
public: // Constructors and destructor
	~CEndWindow();
	static CEndWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CEndWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
private:
	CNotifyTimer*	iTimer;

	TInt			iTimeLimit;
	CHandleLoginOrExit* iExit;
};

#endif // ENDWINDOW_H
