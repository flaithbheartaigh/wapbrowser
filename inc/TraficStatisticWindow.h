/*
============================================================================
 Name        : TraficStatisticWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CTraficStatisticWindow declaration
============================================================================
*/

#ifndef TRAFICSTATISTICWINDOW_H
#define TRAFICSTATISTICWINDOW_H

// INCLUDES
#include "Window.h"

#include "NotifyTimer.h"

class CMultiText;;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CTraficStatisticWindow
* 
*/
class CTraficStatisticWindow : public CWindow,public MTimerNotifier
{
public: // Constructors and destructor

	~CTraficStatisticWindow();
	static CTraficStatisticWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CTraficStatisticWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CTraficStatisticWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();
public://From CWindow
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
	virtual TBool DoPeriodTask();
private:
	CNotifyTimer* iTimer;

private:
	void	InitSelectGroup();

private:
	CMainEngine& iMainEngine;
	CMultiText* iMultiText;//显示流量等等的内容
	TReal i;

	CFileReadByRow* iRes;
};

#endif // TRAFICSTATISTICWINDOW_H
