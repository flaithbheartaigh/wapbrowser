/*
============================================================================
 Name        : SearchLocalResultView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchLocalResultView declaration
============================================================================
*/

#ifndef SEARCHLOCALRESULTVIEW_H
#define SEARCHLOCALRESULTVIEW_H

// INCLUDES
#include <eikedwin.h>
#include "Window.h"
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "DialogObserver.h"
// CLASS DECLARATION

class CMainEngine;
class CSearchListBox;
class CHandleLocalSearch;
class CDialog;
class CInputDialog;
class CTitleBar;
/**
*  CSearchLocalResultView
* 
*/
class CSearchLocalResultView : public CWindow , public MPopUpMenuObserver,public MHandleEventObserver,public MSearchListObserver,public MDialogObserver,public MInputObserver,public MCopyToObserver
{
public: // Constructors and destructor

	~CSearchLocalResultView();
	static CSearchLocalResultView* NewL(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC& aTitle);
	static CSearchLocalResultView* NewLC(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex,const TDesC& aTitle);

private:

	CSearchLocalResultView(CWindow* aParent,CMainEngine& aMainEngine,TInt aIndex);
	void ConstructL(const TDesC& aTitle);
public://From Dialog
	virtual void DialogEvent(TInt aCommand);

public://From MCopyToDialog
	virtual void CopyToEvent(TInt aEvent,const TDesC& aPath);

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
public://From MSearchListObserver
	virtual void SearchListResponseEvent(TInt aEvent,const TDesC& aText);
public://From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);
private:
	
	void	GetCopyToPath(TInt aIndex,TDes& aPath);
	void	InitCopyToDialog(const TDesC& aText);

	void	OpenFile();

	void	InitWaitDialog(const TDesC& aText);
	void	InitDialog(const TDesC& aText);
	void	InitPopUpMenu();
	void	InitInputDialog();
	void	InitListBox();
	TInt	GetCurType();
private:

	CTitleBar* iTitleBar ;
	CInputDialog* iInputDialog;

	CDialog* iDialog;

	CHandleLocalSearch* iLocalSearch;

	CSearchListBox* iSearchList;

	CMainEngine& iMainEngine;

	TInt iIndex;
	HBufC* iTitle;

	TInt iCurPage;
	TInt iAllPage;

	TInt iPopState;
};

#endif // SEARCHLOCALRESULTVIEW_H
