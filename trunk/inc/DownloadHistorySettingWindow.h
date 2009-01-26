/*
============================================================================
 Name        : DownloadHistorySettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadHistoryRecordSettingWindow declaration
============================================================================
*/

#ifndef DOWNLOADHISTORYSETTINGWINDOW_H
#define DOWNLOADHISTORYSETTINGWINDOW_H

// INCLUDES
#include "Window.h"

#include "DialogObserver.h"

class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CDownloadHistorySettingWindow
* 
*/
class CDownloadHistorySettingWindow : public CWindow,public MDialogObserver
{
public: // Constructors and destructor

	~CDownloadHistorySettingWindow();
	static CDownloadHistorySettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDownloadHistorySettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CDownloadHistorySettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

	
private:
	void	InitSelectGroup();

private:
	CSelectGroup*	iSelectGroup;
	CMainEngine& iMainEngine;

	CFileReadByRow* iRes;
};

#endif // DOWNLOADHISTORYSETTINGWINDOW_H
