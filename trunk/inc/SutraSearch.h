/*
============================================================================
 Name        : SutraSearch.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSutraSearch declaration
============================================================================
*/

#ifndef SUTRASEARCH_H
#define SUTRASEARCH_H

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
#include "SearchListBox.h"
/**
*  CSutraSearch
* 
*/
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"


class CSutraSearch : public CWindow,public MPopUpMenuObserver,public MSearchListObserver// public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CSutraSearch();

        /**
        * Two-phased constructor.
        */
	static CSutraSearch* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CSutraSearch* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


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
	//form MSearchListObserver
	virtual void SearchListResponseEvent(TInt aEvent,const TDesC& aText);
public:
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	TBuf<60>titlebuf;
private:

	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CSearchListBox* iSearchList;//搜索控件。
	/**
        * Constructor for performing 1st stage construction
        */
	CSutraSearch(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // SutraSearch_H
