/*
============================================================================
Name        : CDisplayHelpWindow.h
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : 用于显示帮助列表中选中的具体项所对应的帮助信息奥^_^
：
============================================================================
*/

#ifndef CDISPLAYHELPWINDOW_H
#define CDISPLAYHELPWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


#include "Window.h"

#include "MultiText.h"
#include "MControlObserver.h"
// CLASS DECLARATION

/**
*  CDisplayHelpWindow
* 
*/
class CMultiText;

class CDisplayHelpWindow : public CWindow ,public MMultiObserver
{
public: 
	// Constructors and destructor
	~CDisplayHelpWindow();
	static CDisplayHelpWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDisplayHelpWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CDisplayHelpWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

private:
	void	        InitPopUpMenu();//好像没有什么用
	//from MMultiObserver
	virtual void MultiEvent(TInt aEvent);
	//以下是自定义的东东
private:
	//用于进入具体的模块

	TInt iModelNumber;//就是在工厂生产时，标识模块的数字

	CMultiText* iMultiText;
public:
	//显示MultiText上面的title，根据帮助内容的不同动态的变化
	void LoadTitle(const TDesC& aTitleText);
	//加载文件的函数，以实现根据帮住内容的不同，动态显示数据的功能
	void LoadFileToMultiText(const TDesC& aFileName);
	//加载标识模块的数字
	void LoadEnterModel(TInt aModelNumber);
	//加载要显示的连接文字
	void LoadLinkText(const TDesC& aLinkText);

};
#endif // BARCODEHELPWINDOW_H
