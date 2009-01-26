/*
============================================================================
 Name        : MainSearchView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CMainSearchView declaration
============================================================================
*/

#ifndef MAINSEARCHVIEW_H
#define MAINSEARCHVIEW_H

// INCLUDES
#include "Window.h"
#include "NaviPaneObserver.h"
//#include "PopUpMenuObserver.h"

// CLASS DECLARATION
class CListBox;
class CMainEngine;
class CNaviPane;
/**
*  CMainSearchView
* 
*/
class CMainSearchView : public CWindow , public MNaviPaneObserver //, public MPopUpMenuObserver
{
public: // Constructors and destructor

	~CMainSearchView();
	static CMainSearchView* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CMainSearchView* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CMainSearchView(CWindow* aParent,CMainEngine& aMainEngine);
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

	
public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

//public://From MPopUpMenuObserver
//	virtual void DoPopUpMenuCommand(TInt aCommand);

private:
	void	InitNaviPane();
	void	InitPopUpMenu();

	void	InitListBox(TInt aIndex);
private:
	CNaviPane* iNaviPane;
	CListBox*	iListBox;
	CMainEngine& iMainEngine;
};

#endif // MAINSEARCHVIEW_H
