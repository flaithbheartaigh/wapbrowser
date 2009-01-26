/*
============================================================================
 Name        : JoinAccountSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CJoinAccountSettingWindow declaration
============================================================================
*/

#ifndef JOINACCOUNTSETTINGWINDOW_H
#define JOINACCOUNTSETTINGWINDOW_H

// INCLUDES
#include "Window.h"
#include "DialogObserver.h"

class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CJoinAccountSettingWindow
* 
*/
#define KMaxIapNum 20
#define KMaxIapNameLength 100
struct TIapInfo 
{
	TInt		nIapID;		//iap的id
	TBuf<KMaxIapNameLength>	nIapName;	//iap的名字	
	TBool	    nIsUse;		//是否使用
} ;
class CJoinAccountSettingWindow : public CWindow,public MDialogObserver
{
public: // Constructors and destructor

	~CJoinAccountSettingWindow();
	static CJoinAccountSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CJoinAccountSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CJoinAccountSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
public://获取联网方式
	void  GetIapInfo();

private:
	void	InitSelectGroup();

private:
	CSelectGroup*	iSelectGroup;
	CMainEngine& iMainEngine;

	TIapInfo* iIapInfoList;//iap信息列表
	TInt iIapNum;//iap的个数

	CFileReadByRow* iRes;
};


#endif // JOINACCOUNTSETTINGWINDOW_H
