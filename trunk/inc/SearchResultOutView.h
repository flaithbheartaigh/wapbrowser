/*
============================================================================
 Name        : SearchResultOutView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultOutView declaration
============================================================================
*/

#ifndef SEARCHRESULTOUTVIEW_H
#define SEARCHRESULTOUTVIEW_H

// INCLUDES
#include <eikedwin.h>
#include "Window.h"
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "DialogObserver.h"
#include "MControlObserver.h"
#include "NotifyTimer.h"
// CLASS DECLARATION

class CMainEngine;
class CHandleOutSearch;
class CDialog;
class CInputDialog;
class CLinkListBox;
class CTitleBar;
// CLASS DECLARATION

/**
*  CSearchResultOutView
* 
*/
class CSearchResultOutView : public CWindow ,  public MTimerNotifier,public MPopUpMenuObserver,public MHandleEventObserver,public MDialogObserver,public MInputObserver,public MLinkListObserver
{
public: // Constructors and destructor

	~CSearchResultOutView();
	static CSearchResultOutView* NewL(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle);
	static CSearchResultOutView* NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC8& aURL,const TDesC& aTitle);
private:
	CSearchResultOutView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex);
	void ConstructL(const TDesC8& aURL,const TDesC& aTitle);
public://From Dialog
	virtual void DialogEvent(TInt aCommand);
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
public:// From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);
public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
public:
	virtual void LinkListEvent(TInt aEvent,TInt aType);
public://From MTimerNotifier
	virtual TBool	DoPeriodTask();
private:

	void	InitEdwin();
	void	InitWaitDialog(const TDesC& aText);
	void	InitDialog(const TDesC& aText);
	void	InitPopUpMenu();
	void	InitInputDialog();
	void	InitListBox();
	void    StartSearch();
	const TDesC8& GetUrlEncode(const TDesC& aText);
	
	void   SetEdwinVisible(TBool aBool);

	void	DoOKKey();
	void DrawEditor(CGraphic& gc) const;
private:
	CNotifyTimer*	iTimer;
	CEikEdwin* iEdwin;
	CInputDialog* iInputDialog;
	CTitleBar* iTitleBar;
	CDialog* iDialog;

	CHandleOutSearch* iOutSearch;

	CLinkListBox* iLinkList;
	CMainEngine& iMainEngine;

	TInt iIndex;
	HBufC* iTitle;
	HBufC8* iUrl;

	HBufC8* iTempBuf;

	TBuf<20> iKeyWord;

	TBool iFlag;

	TInt iCurPage;
	TInt iAllPage;

	TInt iPopState;

	TPoint iPoint;
	TInt iDownLoadFlag;
};

#endif // SEARCHRESULTOUTVIEW_H
