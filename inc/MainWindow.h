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
#include "ControlObserver.h"
#include "MControlObserver.h"

class CCoCoAppView;
class CWindowGc;
class CSlideText;
class CScrollBar;
class CBitmapFactory;
class CHandleWeather;
class CHandleAutoUpdate;
class CDialog;
class CHandleLoginOrExit;
class CHandleSystemInfo;
class CHandleSynchronization;

#include "CacheObserver.h"	//Test
//////////////////////////////////////////////////////////////////////////
//CMainWindow
//////////////////////////////////////////////////////////////////////////
class CMainWindow : public CWindow , public MTimerNotifier , public MDialogObserver , public MPopUpMenuObserver,public MInputObserver ,public MHandleEventObserver,public MInfoObserver
{
friend class CMainEngine;
friend class MCommondBase;

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
	virtual void DialogEvent(TInt aCommand);

public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);

public://From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);
public://From MInfoObserver
	virtual void InfoResponseEvent(TInt aEvent);

public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
public:
/*
	//设置当前有效界面
	void	SetCurWindow(CWindow* aWindow);
	//发起绘图请求
	void	RequestDraw();
	*/

	//被CCoCoAppView调用,绘制离屏位图
	void	DrawOffScreenBitmap(CWindowGc& gc)const;

	void	SelectView(TInt aIndex);
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
	void    ShowInfoDialog(const TDesC& aTitle,const TDesC& aText,TInt aType);

	void	InitPictureWhite();
	void	InitIconBmp();
	void	DrawIcon(CGraphic& aGraphic) const;
	void	DrawTitle(CGraphic& aGraphic)const;

	void	InitSlideText();
	void	InitScrollBar();
	void	InitTitleBar();

	void	DrawDownLoadIcon(CGraphic& aGraphic) const;

	void	ShowWaitDialog(const TDesC& aText);
	void	ShowDialog(const TDesC& aText,TInt aType);

	void	InitConnection();
	void	HandleLoginState();
	void	HandleSynchronState();
	void	HandleSystemInfo();

private:	
	const CCoCoAppView&	iAppView;

	TInt			iIconNumPerLine;
	TInt			iIconLineNumPerPage;
	TInt			iIconLineNum;

	TInt			iStartLineNum;		//需要用滚动条代替
	TInt			iCurIndex;

	CNotifyTimer*	iTimer;

	CFbsBitmap*			iTitleBitmap;
	CFbsBitmap*			iTitleLogoBmp;

	TRect iRect;		//跑马灯

	TSize iIconSize;
	TPoint iStartPoint;
	TSize iFullClientSize;
	TInt iLineH;
	TInt iLineW;
	TInt iFontHeight;

	TInt iSelectedIndex;
	TInt iFirstIndex;

	
	TInt iMaxLine;
	TInt iTotleLine;
	TInt iSelectedLine;

	TInt iColor;

	TInt iLineHeight;

	TSize iLogoSize;
	TInt iDownloadFlag;

	CFbsBitmap* iPictureWhite;
	
	CBitmapFactory& iBitmapFactory;
	TInt iMainIconCount;
	CSlideText* iSlideText;
	CScrollBar*	iScrollbar;

	CHandleWeather* iHandleWeather;
    CHandleAutoUpdate* iHandleAutoUpdate;
	CDialog* iDialog;

	TInt iDIconNum;
	TBool iSystemInfoBool;
	CHandleSynchronization& iSyschr;
	CHandleLoginOrExit* iLogin;
	
	CHandleSystemInfo* iSystemInfo;
	TInt iStatueFlag;
	TInt iSUpdateState;

	TInt iTimeCount;
	TBool iRequestFirstInfo;
};

#endif // MAINWINDOW_H
