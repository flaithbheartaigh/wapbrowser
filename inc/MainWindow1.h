/*
============================================================================
 Name        : MainWindow.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 程序主界面,也是界面树的顶结点,所有的界面均以此类为核心进行控制
============================================================================
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "window.h"
#include "NotifyTimer.h"
#include "ControlObserver.h"
#include "DialogObserver.h"
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"

class CCoCoAppView;
class CWindowGc;

//////////////////////////////////////////////////////////////////////////
//CModuleData
//////////////////////////////////////////////////////////////////////////
class CModuleData : public CBase , public MTimerNotifier
{
public:
	CModuleData(TInt aModuleType,CFbsBitmap* aBitmap,const TDesC& aName);
	~CModuleData();

	CModuleData* NewLC(TInt aModuleType,CFbsBitmap* aBitmap,const TDesC& aName);
	CModuleData* NewL(TInt aModuleType,CFbsBitmap* aBitmap,const TDesC& aName);
	void ConstructL();

public:
	void	SetRect(const TRect& aRect);
	void	Draw(CGraphic& aGraphic) const;

	TInt	GetModuleType() const;

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();	

private:
	CFbsBitmap*		iIconBitmap;
	const TDesC&	iName;
	TInt			iModuleType;	//TWindowType
	TRect			iRect;

	TPoint			iIconPoint;
	TPoint			iNamePoint;
};

//////////////////////////////////////////////////////////////////////////
//CMainWindow
//////////////////////////////////////////////////////////////////////////
class CMainWindow : public CWindow , public MTimerNotifier , public MDialogObserver , public MPopUpMenuObserver,public MInputObserver,public MInfoObserver
{
friend class CMainEngine;
friend class MCommondBase;

private://定义执行命令用的枚举常量
	enum TEventCommand
	{
		EEnterModule,
		EShowHelp,
		EHideApp,
		EExitApp,
		EInputDialog,
		EExitTip,
	};
private://MCommondBase
	class CExitAppCommand : public MCommondBase
	{
	public:
		CExitAppCommand(CMainWindow& aMainWindow);
		virtual void Execute();
	private:
		CMainWindow&	iMainWindow;
	};

public: // Constructors and destructor
	~CMainWindow();
	static CMainWindow* NewL(CMainEngine& aMainEngine,const CCoCoAppView& aAppView);

private:
	CMainWindow(CMainEngine& aMainEngine,const CCoCoAppView& aAppView);
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

	virtual void	ChangeButton();

public://From MTimerNotifier
	virtual TBool	DoPeriodTask();

public://From MDialogObserver
	virtual void DialogConfirm();
	virtual void DialogCancel();

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);

public://From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);
public://From MInfoObserver
	virtual void InfoResponseEvent(TInt aEvent);
public:
/*
	//设置当前有效界面
	void	SetCurWindow(CWindow* aWindow);
	//发起绘图请求
	void	RequestDraw();
	*/

	//被CCoCoAppView调用,绘制离屏位图
	void	DrawOffScreenBitmap(CWindowGc& gc)const;


private:
	TBool	HandleIconKeyL(TInt aKeyCode);
// 	//绘制顶部标题栏
// 	void	DrawTitle(CGraphic& aGraphic)const;
	//绘制功能模块的图标
	void	DrawAllIcons(CGraphic& aGraphic)const;
	//弹出菜单
	void	InitPopUpMenu();
	//隐藏
	void	HideApplication();
	//程序初始化
	void	PrepareStart();
	//退出
	void	PrepareExit();
	//进入子模块
	void	EnterModule();
	//显示帮助
	void	ShowHelp();
	//初始化模块项
	void	InitModuleArray();
	//弹出输入框
	void    ShowInputDialog();
	void    ShowInfoDialog();
private:	
	const CCoCoAppView&	iAppView;
// 	//当前界面
// 	CWindow*		iCurWindow;	
// 	//当设置iCurWindow时,将iChild保存于此处(是否有此必要?)
// 	CWindow*		iBackupWindow;	

//	TBool			iIsDraw;

	TInt			iIconNumPerLine;
	TInt			iIconLineNumPerPage;
	TInt			iIconLineNum;

	RPointerArray<CModuleData>	iModuleArray;

	TInt			iStartLineNum;		//需要用滚动条代替
	TInt			iCurIndex;

	CNotifyTimer*	iTimer;
};

#endif // MAINWINDOW_H
