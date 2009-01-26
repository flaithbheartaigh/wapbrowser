/*
============================================================================
 Name        : FriendLink.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFriendLink declaration
============================================================================
*/

#ifndef FRIENDLINK_H
#define FRIENDLINK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
#include "ControlFactory.h"
#include "MainEngine.h"
#include "Window.h"	
#include "TypeDefine.h"
#include"ListBox.h"
#include "PopUpMenu.h"
#include "NaviPane.h"
#include "TitleBar.h"
#include "ListBoxItem.h"
#include "Dialog.h"
#include "DialogObserver.h"
/**
*  CFriendLink
* 
*/
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "NaviPaneObserver.h"
#include "NewDownload.h"
class CHandleSynchronization;

class CFriendLink : public CWindow,public MPopUpMenuObserver,public MNaviPaneObserver,public MHandleEventObserver,public MDialogObserver // public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CFriendLink();

        /**
        * Two-phased constructor.
        */
	static CFriendLink* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CFriendLink* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


public:
	virtual void	DoActivateL();
	//派生类实现冻结视图
	virtual void	DoDeactivate();
	//派生类实现绘图
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//派生类实现按键事件响应
	virtual TBool	DoKeyEventL(TInt aKeyCode);

	virtual TBool	DoHandleCommandL(TInt aCommand);
	//派生类实现界面尺寸改变
	virtual void	DoSizeChanged();

	//派生类实现设置左右按钮,以便覆盖按钮的控件返回时回复原样
	virtual void	ChangeButton();

	virtual void NaviPaneIndexChanged(TInt aNewIndex);//pan的消息响应。
public:
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver


public:
	void ChangeFriendLinkList(TInt aType);
	void ChangListItem(TInt aNewIndex);
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	void ReadXML();
	TBool UpData();//更新.
	void InitWaitDialog(const TDesC& aText);
	void CreateTileAndList();
	void ChangTitle();//更改标题栏.
	TBool FriendUpData();//友链.
	void AddFriendList();
private:

	TInt iFLinkType;
	TInt iPIndex;
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CNaviPane* naviPane;
	CHandleSynchronization& iSyschr;//同步
	CDialog* iDialog;//提示信息对话框.
	TBool iThirdflags;
	
	TInt ifriendflags;//共三个值，０：无值返回，１表示，正确更新，２表示，错误更新.
	TInt iTitlenumber;
	/**
        * Constructor for performing 1st stage construction
        */
	CFriendLink(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // FRIENDLINK_H
