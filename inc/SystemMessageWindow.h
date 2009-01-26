/*
============================================================================
 Name        : SystemMessageWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CSystemMessageWindow declaration
============================================================================
*/

#ifndef SYSTEMMESSAGEWINDOW_H
#define SYSTEMMESSAGEWINDOW_H

// INCLUDES
#include "Window.h"
#include "PopUpMenuObserver.h"
#include "DialogObserver.h"
#include "MControlObserver.h"
#include "CoCoPreDefine.h"

#define MSGLIST_NUM 50//最多系统消息数
#define ITEM_NUM_PER_PAGE 15//每页的信息数

typedef struct TSysMsg SYSMSG;
struct TSysMsg 
{
	TInt			IsUsed;			//是否使用
	TBuf<255>		nFileName;		//文件名称
	TBuf<100>	    nShowInfo;		//显示信息
	TBuf<100>		nTitle;			//标题
} ;

class CMainEngine;
class CListBox;
class CTitleBar;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CSystemMessageWindow
* 
*/
class CSystemMessageWindow : public CWindow,public MPopUpMenuObserver,public MDialogObserver,public MInfoObserver
{
public: // Constructors and destructor

	~CSystemMessageWindow();
	static CSystemMessageWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CSystemMessageWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CSystemMessageWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	//MPopUpMenuObserver
	void DoPopUpMenuCommand(TInt aCommand);

public://FromMInfoObserver
	virtual void InfoResponseEvent(TInt aEvent);

public:
	void InitPopUpMenu();

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);
public:
	void GetMsgInfo();

	void ShowSystemInfo(const TDesC& aFileName,const TDesC& aTitle);

	//读文件列表
	void ReadFileNameList(const TDesC& strPath, CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TUint8>* pDirArray);
	
	void DisplaySysMsg(TInt aCurPage);//显示系统消息

	void DisplayPageNum();//显示页码
private:
	CMainEngine& iMainEngine;
	
	CTitleBar* iTitle ;

	CListBox* iSysInfoListbox;
	TInt iPageNum;//列表的页数
	TInt iCurPage;//当前页

	TInt	iMsgNum;//实际的个数

	CArrayFixFlat<TCONTENTTEXT>*	iFileNameArray;//太大了把？？

	TSysMsg* iMsgList;

	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;

	CFileReadByRow* iRes;
	
};

#endif // SYSTEMMESSAGEWINDOW_H
