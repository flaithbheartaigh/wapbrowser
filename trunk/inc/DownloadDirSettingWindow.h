/*
============================================================================
 Name        : DownloadDirSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadDirSettingWindow declaration
============================================================================
*/

#ifndef DOWNLOADDIRSETTINGWINDOW_H
#define DOWNLOADDIRSETTINGWINDOW_H
// INCLUDES
#include "Window.h"
#include "DialogObserver.h"
#include "FileBrowserObserver.h"
class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CDownloadDirSettingWindow
* 
*/
class CDownloadDirSettingWindow : public CWindow,public MDialogObserver,public MFileBrowserObserver
{
public: // Constructors and destructor

	~CDownloadDirSettingWindow();
	static CDownloadDirSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDownloadDirSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CDownloadDirSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
public://From MFileBrowserObserver
	virtual void	DoFileEvent(const TDesC& aFileName,TInt aCommand);

private:
	void	InitMultiText();

private:
	
	CMainEngine& iMainEngine;
	TFileName iNewPathName;//用户选择的要更改成的路径

	CFileReadByRow* iRes;
};

#endif // DOWNLOADDIRSETTINGWINDOW_H
