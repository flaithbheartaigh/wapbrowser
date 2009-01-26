/*
============================================================================
 Name        : NewDownload.h
 Author      : hsany
 Version     :
 Copyright   : Your copyright notice
 Description : CNewDownload declaration
============================================================================
*/

#ifndef NEWDOWNLOAD_H
#define NEWDOWNLOAD_H

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
#include "tinyxml.h"//用来加载XML解释引擎的。
#include "tinystr.h"
#include "ListBoxItem.h"
#include "Dialog.h"
/**
*  CNewDownload
* 
*/
#include "CacheObserver.h"//cacheobserver.
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "NaviPaneObserver.h"
#include "DialogObserver.h"

#include "BitmapFactory.h"
#include "PictureEngine.h"
#include "ContentInfoDialog.h"
class CInputDialog;
class CHandleGrade;
class CCacheEngine;
class CHandleInfo;

class CNewDownload : public CWindow,public MPopUpMenuObserver
					,public MInputObserver
					,public MDialogObserver
					,public MHandleEventObserver
					,public MGradeObserver//public CBase
					, public MCacheObserver
					, public MInfoObserver
					,public MContentInfoObserver

{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CNewDownload();

        /**
        * Two-phased constructor.
        */
	static CNewDownload* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CNewDownload* NewLC(CWindow* aParent,CMainEngine& aMainEngine);
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
	
	void AddListITemL();
	//跳转到，输入框。
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog

public://From ContentInfoDialog
	virtual void ContentInfoEvent(TInt aLeftCommond);
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);

	void CheckDowndtype();//检查文件类型.

public:
	void InitMenu();//初始化菜单.
	
	void ReadXML(const TDesC& aFileName);//从文件中读XML文件。

	TBool HandleIconKeyL(TInt aKeyCode);//不知有什么用。

	void EnterModel(TInt aType);//创建子窗口

	void ChangListItem(TInt aItem);
	void Goto(TInt apage);//跳转
	void Changpage(TBool aflalgs);//翻页;
	void ShowList();//显示列表
	void GetKeyWord(TDes& desleft,TDes& desright);
	void Search();
	void DrawRightInfo();//在标题栏的右上角，画页数。

//	void InitInfoDialog(const TDesC& aTitle,const TDesC& aInfo);//信息详情。

	virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
public:
	TBuf<40>titlebuf;//标题
	TInt iSecondID;
	TInt iThirdID;
	TInt iFirstID;
	CDesCArray* iDesArray;//用于存放数列表里的数据。
	void InputDialog();//跳到的输入框
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	//下载
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt& iFileSize);
	void DownLoadFile(TInt aDownType);
	TInt AdvertisMent();//广告.
	void InitDialog(const TDesC& aText);//用来初始化对话框
	void SendMyGrade();//发送评分
	void InitWaitDialog(const TDesC& aText);//等待对话框
	void InitGradeDialog();//评分界面。.
	
	void CreateTileAndList();

	void GetInfo();

	void ConstructLater();
	void Changdrawpage(TBool aflages);//用于翻页
	void SetViewType(TInt aType){ iViewType=aType;}
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();
private:
	TBool iflalgs;
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CListBox* ilistbox;//列表
	CNaviPane* naviPane;
	CTitleBar* iTitlebar;
	TInt iCurpage;//当前页.
	TInt iAllpage;//总页
	HBufC* titletxt;
	CInputDialog* iInputDialog;// 输入框 
	//图标
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	//提示对话框
	CDialog* iDialog;
	CHandleGrade* iShowGrade;//评分
	TInt iMidpage;
	TInt iDwonType;//判断重复下载
private:
	TInt iViewType;
	CCacheEngine* iCacheEngine;
	TInt iGrade;//评分级别

	CHandleInfo* iShowInfo;
	CContentInfoDialog* iContentInfo;//信息详情的对话框.
	/**
        * Constructor for performing 1st stage construction
        */
	CNewDownload(CWindow* aParent,CMainEngine& aMainEngine);

	TInt iJumpPage;	//add这个表示跳的页数。
	TInt iJumpFlage;//共有三个值:0表示没有触发，1表示跳动。
	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // NEWDOWNLOAD_H
