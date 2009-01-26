/*
============================================================================
 Name        : ResultView.h
 Author      : hsany
 Version     :
 Copyright   : Your copyright notice
 Description : CResultView declaration
============================================================================
*/

#ifndef RESULTVIEW_H
#define RESULTVIEW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
#include "MainEngine.h"
#include "Window.h"
#include "TypeDefine.h"
#include "NaviPane.h"
#include "ControlFactory.h"
#include "e32def.h"
#include"ListBox.h"
#include "PopUpMenu.h"
#include "Dialog.h"
#include "DialogObserver.h"
/**
*  CResultView
* 
*/
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "NaviPaneObserver.h"
class CHandleSynchronization;

class CResultView : public CWindow,public MPopUpMenuObserver,public MNaviPaneObserver,public MHandleEventObserver,public MDialogObserver //public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CResultView();

        /**
        * Two-phased constructor.
        */
	static CResultView* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CResultView* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

public:
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
	//用于弹出菜单.
	virtual void DoPopUpMenuCommand(TInt aCommand);
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);

	void AddListITemL();//增加列表元素

	void InitMenu(TInt atype);//初始化菜单.

	TBool HandleIconKeyL(TInt aKeyCode);//不知有什么用。

	void EnterModel(TInt aType);//创建子窗口

	void ChangListItem(TInt aItem);
	void ChangTitle();//加入标题项.
    void InitWaitDialog(const TDesC& aText);	
	TBool UpData();
	void CreateTileAndList();
public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);
	TBuf<35>temptitle;//用来存放子窗口的标题。
private:

		CListBox* ilistbox;//列表
		CNaviPane* naviPane;
		TInt panitem;//当前标题栏的值
		CHandleSynchronization& iSyschr;//同步
		CDialog* iDialog;//提示信息对话框.
		TInt iTitlenumber;
	//	HBufC* iTitle;
		
	/**
        * Constructor for performing 1st stage construction
        */
	CResultView(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // RESULTVIEW_H
