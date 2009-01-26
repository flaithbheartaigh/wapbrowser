/*
============================================================================
 Name        : ControlFactory.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CControlFactory declaration
 说明		 : 控件工厂,提供一系列方法用于创建各种公共控件
			 : 需将当前界面设置为父界面,在创建控件成功时会直接将控件推入当前
			 : 界面的控件栈中
			 : 创建菜单、对话框等控件同时改变父界面的左右按钮,以体现控件的功
			 : 能
 注意		 : 此类中仅处理可能在多处使用的公共控件,只有某一个界面独自使用的
			 : 控件不应在此类中初始化,而应在使用到的地方独立地初始化,也不应
			 : 该修改该类
			 : 创建函数中的参数CWindow* aWindow将指定控件添加到哪个窗口,这个
			 : 参数带有默认参数NULL,如果没有指定该参数,控件将添加到默认的窗
			 : 口中
============================================================================
*/

#ifndef CONTROLFACTORY_H
#define CONTROLFACTORY_H

#include "PreDefine.h"

//#include "MultiTextObserver.h"

class CWindow;
class CMainEngine;
class CScreenLayout;

class CScrollBar;
class CMultiText;
class CNaviPane;
class CListBox;
class CTitleBar;
class CDialog;
class CPopUpMenu;
class CFileBrowser;
class CSlideText;
class CInputDialog;
class CSystemInfoDialog;
class CSearchListBox;
class CCopyToDialog;
class CControlText;
class CGradeDialog;
class CContentInfoDialog;
class CSelectAppWindow;
class CLinkListBox;
class CSelectGroup;

class MListBoxLayout;
class MNaviPaneObserver;
class MDialogObserver;
class MPopUpMenuObserver;
class MFileBrowserObserver;
class MCommondBase;
class MInputObserver;
class MInfoObserver;
class MSearchListObserver;
class MCopyToObserver;
class MGradeObserver;
class MContentInfoObserver;
class MSelectAppObserver;
class MLinkListObserver;

class CControlFactory : public CBase //, public MMultiTextObserver
{
public: // Constructors and destructor
	~CControlFactory();
	static CControlFactory* NewL(CMainEngine& aMainEngine);
	static CControlFactory* NewLC(CMainEngine& aMainEngine);

private:
	CControlFactory(CMainEngine& aMainEngine);
	void ConstructL();

public:
	void	SetParentWindow(CWindow* aParentWindow);	

public:
	CScrollBar*	CreateDefaultScrollbarL()const;
	CNaviPane*	CreateDefaultNaviPane(MNaviPaneObserver* aObserver = NULL,CWindow* aWindow = NULL)const;
	CTitleBar*	CreateDefaultTitleBarL(const TDesC& aTitleText,CWindow* aWindow = NULL)const;

	CListBox*	CreateListBox(const TRect& aRect,CWindow* aWindow = NULL) const;
	CListBox*	CreateDefaultListBoxL(CWindow* aWindow = NULL)const;

	CMultiText* CreateMultiText(const TRect& aRect,CWindow* aWindow = NULL)const;
	CMultiText* CreateMultiTextFromFile(const TRect& aRect,const TDesC& aFileName,CWindow* aWindow = NULL) const;
	CMultiText* CreateDefaultMutliText(CWindow* aWindow = NULL)const;
	CMultiText* CreateDefaultMutliTextFromFile(const TDesC& aFileName,CWindow* aWindow = NULL) const;
	CMultiText* CreateDefaultMutliTextFromFile(const TDesC& aFileName,const TDesC& aText,CWindow* aWindow = NULL) const; //对于多行文字显示,最后一行增加蓝色字体(这里只是测试,可进行修改)

	CScrollBar*	CreateScrollBar(const TRect& aRect)const;


	CDialog*	CreateTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow = NULL) const;
	CDialog*	CreateSuccessTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow = NULL) const;
	CDialog*	CreateFailTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow = NULL) const;
	CDialog*	CreateInquireDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,TInt aRightCommand,CWindow* aWindow = NULL) const;
	CDialog*	CreateWaitDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aRightCommand,CWindow* aWindow = NULL) const;
	CDialog*	CreateWaitDialogWithoutCancel(const TDesC& aDes,MDialogObserver& aObserver,CWindow* aWindow = NULL) const;


	//CDialog*	CreateInquireDialog(const TDesC& aDes,MDialogObserver& aObserver,MCommondBase* aLeftCommand,MCommondBase* aRightCommand = NULL) const;

	CPopUpMenu*	CreatePopUpMenu(MPopUpMenuObserver& aObserver,CWindow* aWindow = NULL) const;

	CFileBrowser*	CreateFileBrowser(MFileBrowserObserver& aObserver,TInt aLeftCommand = KCmdEventNull,TInt aRightCommand = KCmdEventNull,CWindow* aWindow = NULL) const;
	CFileBrowser*	CreateFolderBrowser(MFileBrowserObserver& aObserver,TInt aLeftCommand = KCmdEventNull,TInt aRightCommand = KCmdEventNull,CWindow* aWindow = NULL) const;

	CSlideText*	CreateSlideText(const TRect& aRect,const TDesC& aDefaultText,TInt aStyle,CWindow* aWindow = NULL) const;
	CSlideText*	CreateLeftCycleSlideText(const TRect aRect, const TDesC& aDefaultText,CWindow* aWindow = NULL) const;


	CInputDialog* CreateInputTextDialog(MInputObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aCommandType,CWindow* aWindow = NULL) const;
	CInputDialog* CreateInputNumberDialog(MInputObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aCommandType,CWindow* aWindow = NULL) const;

	CSystemInfoDialog* CreateInfoDialog(MInfoObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aType,CWindow* aWindow=NULL) const;
	
	CSearchListBox* CreateSearchListBox(MSearchListObserver& aObserver,CWindow* aWindow = NULL) const;
	CCopyToDialog* CreateCopyToDialog(MCopyToObserver& aObserver,const TDesC& aTitle,CWindow* aWindow = NULL) const;

	CControlText* CreateControlText(const TRect& aRect,CWindow* aWindow = NULL) const;
	CControlText* CreateDefaultControlText(CWindow* aWindow = NULL) const;

	CGradeDialog* CreateGradeDialog(MGradeObserver& aObserver,CWindow* aWindow = NULL) const;

	CContentInfoDialog* CreateContentInfoDialog(MContentInfoObserver& aObserver,const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool,CWindow* aWindow = NULL) const;
	
	CContentInfoDialog* CreateContentInfoDialog(MContentInfoObserver& aObserver,const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,const TDesC& aPath,TInt aCommand,TBool aBool,CWindow* aWindow = NULL) const;
	
	CSelectAppWindow* CreateSelectAppWindow(MSelectAppObserver& aObserver,CWindow* aWindow = NULL) const;

	CLinkListBox* CreateLinkListBox(MLinkListObserver& aObserver,const TRect& aRect,CWindow* aWindow = NULL) const;

	CSelectGroup* CreateSelectBox(const TRect& aRect,CWindow* aWindow = NULL) const;
	CSelectGroup* CreateDefaultSelectBox(CWindow* aWindow = NULL) const;

	void		AssertClass() const;

private:
// public://From MMultiTextObserver
// 	//CScrollbar* CreateScrollbarL()const;

public:
	CMainEngine&			iMainEngine;
	const CScreenLayout&	iScreenLayout;
	CWindow*				iParentWindow;		//为哪个界面创建控件

	mutable RPointerArray<CFbsBitmap>	iBitmapCleanArray;
};

#endif // CONTROLFACTORY_H
