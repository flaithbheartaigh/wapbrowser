/*
============================================================================
 Name        : HelpWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CHelpWindow declaration
============================================================================
*/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include "Window.h"
class CMultiText;
//#include "NaviPaneObserver.h"

class CHelpWindow : public CWindow //, public MNaviPaneObserver
{
public: // Constructors and destructor
	~CHelpWindow();
	static CHelpWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CHelpWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CHelpWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

// public:
// 	virtual void NaviPaneIndexChanged(TInt aNewIndex);
private:
	void	InitPopUpMenu();
	CMultiText* iMultiText;
};

#endif // HELPWINDOW_H
