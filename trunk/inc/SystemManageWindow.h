/*
============================================================================
 Name        : SystemManageWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CSystemManageWindow declaration
============================================================================
*/

#ifndef SYSTEMMANAGEWINDOW_H
#define SYSTEMMANAGEWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

#include "window.h"
#include "NaviPaneObserver.h"
#include "PopUpMenuObserver.h"
//#include "FileReadByRow.h"
// CLASS DECLARATION

/**
*  CSystemManageWindow
* 
*/


class CListBox;
class CNaviPane;
class CFileReadByRow;

class CSystemManageWindow : public CWindow , public MNaviPaneObserver,public MPopUpMenuObserver
{
public: // Constructors and destructor
	~CSystemManageWindow();
	static CSystemManageWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CSystemManageWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CSystemManageWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	 	virtual void NaviPaneIndexChanged(TInt aNewIndex);

	//From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
private:

	void	InitPopUpMenu();
public:
	
private:

	CListBox* iListBox;
	CNaviPane* iNaviPane;
	
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	CFileReadByRow* iRes;
	
};

#endif // SYSTEMMANAGEWINDOW_H
