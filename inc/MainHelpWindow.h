/*
============================================================================
Name        : MainHelpWindow.h
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : CMainHelpWindow declaration
============================================================================
*/

#ifndef MAINHELPWINDOW_H
#define MAINHELPWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

#include "Window.h"
#include "NaviPaneObserver.h"
// CLASS DECLARATION
class CListBox;
class CNaviPane;
/**
*  CMainHelpWindow
* 
*/
class CMainHelpWindow : public CWindow ,public MNaviPaneObserver 
{
public: // Constructors and destructor
	~CMainHelpWindow();
	static CMainHelpWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CMainHelpWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CMainHelpWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	//pan的消息响应(响应tab键的切换)
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

private:
	void	InitPopUpMenu();
public:
	//自定义的函数，用于读取帮助的配置信息
	void  LoadHelpConfig(const TDesC& aFileName);

	//创建导航栏
	void CreatNaviPan();//利用iItem数组的内容

	//根据选中的tab的不同为列表添加不同的数据
	void AddListboxItem(TInt aCurrentTab);

	//从文件中加载内容到列表
	void LoadItem(const TDesC& aFileName);
private:
	CDesCArray*  iConfigItem;//存放帮住配置文件的内容

	CDesCArray*  iItem;//存放具体的列表项的内容

	CListBox* iHelpListbox;

	CNaviPane* iNaviPane; 

	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;


};

#endif // MAINHELPWINDOW_H
