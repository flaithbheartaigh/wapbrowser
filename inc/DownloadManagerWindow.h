/*
============================================================================
 Name        : DownloadManagerWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadManagerWindow declaration
============================================================================
*/

#ifndef DOWNLOADMANAGERWINDOW_H
#define DOWNLOADMANAGERWINDOW_H

#include "Window.h"
#include "NaviPaneObserver.h"
#include "PopUpMenuObserver.h"

class CControl;
class CListBox;

class CDownloadManagerWindow : public CWindow , public MNaviPaneObserver //, public MPopUpMenuObserver
{
friend class CDownloadHistoryList;
friend class CDownloadingList;
public: // Constructors and destructor
	~CDownloadManagerWindow();
	static CDownloadManagerWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDownloadManagerWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CDownloadManagerWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

private:
	void	InitNaviPane();
	void	InitPopUpMenu();
	void	InitDownloadingList();
	void	InitDownloadedList();
	void	InitDownLoadHistoryList();

private:
	CControl*	iTempControl;
	CListBox*	iTempListBox;
};

#endif // DOWNLOADMANAGERWINDOW_H
