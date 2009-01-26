/*
============================================================================
 Name        : UserAdvise.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CUserAdvise declaration
============================================================================
*/

#ifndef USERADVISE_H
#define USERADVISE_H

#include "Window.h"
#include "HttpObserver.h"
#include "DialogObserver.h"
#include "NotifyTimer.h"

class CEikGlobalTextEditor;
class CEditorsManager;
class CDialog;
class CTitleBar;
class CUserAdviseReply : public CBase	
{
public:
	CUserAdviseReply();
	~CUserAdviseReply();
	TInt	ParseXML(const TDesC& aDes);
public:
	TInt	iResult;
	HBufC*	iInfo;
};

class CUserAdviseWindow : public CWindow ,  public MTimerNotifier, public MOperationObserver , public MDialogObserver
{
public: // Constructors and destructor
	~CUserAdviseWindow();
	static CUserAdviseWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CUserAdviseWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CUserAdviseWindow(CWindow* aParent,CMainEngine& aMainEngine);
	void ConstructL();
public://From MTimerNotifier
	virtual TBool	DoPeriodTask();
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

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

private:
	void	InitMultiText();
	void	InitEditor();

	void	CancelConnect();

	void	ShowDialog(const TDesC& aDes,TInt aType);

	HBufC8*	MakeBodyL(const TDesC8& aMsg,const TDesC8& aUserID);
	
	void	SendAdviseL();

	void	DrawInputType(CGraphic& gc)const;

	TInt	ParseXML(const TDesC8& aDes);

	void	InitByScreenSize();

private:
	CEditorsManager&		iEditorsManager;
	
	CNotifyTimer*	iTimer;
	CTitleBar* iTitleBar;

	CEikGlobalTextEditor*	iContentEdit;

	CDialog*				iDialog;

	TBool					iRequest;	//已发起请求

	CUserAdviseReply*		iReply;	

	TInt					iDialogEvent;
	TRect					iNaviPaneRect;

	TInt					iLineHeight;
	TInt					iEditorHeight;
};

#endif // USERADVISE_H
