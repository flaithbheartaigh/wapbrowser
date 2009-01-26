/*
============================================================================
 Name        : SutraDownload.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSutraDownload declaration
============================================================================
*/

#ifndef SUTRADOWNLOAD_H
#define SUTRADOWNLOAD_H

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
#include "NewDownload.h"
#include "Dialog.h"
#include "ListBoxItem.h"
#include "StaticCommonTools.h"

/**
*  CSutraDownload
* 
*/
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"
#include "DialogObserver.h"

#include "BitmapFactory.h"
#include "PictureEngine.h"
//#include "SutraSearch.h"
#include "ControlFactory.h"
#include "CacheObserver.h"
#include "ContentInfoDialog.h"
#include "tinyxml.h"//add by hesanyuan
class CHandleGrade;
class CHandleInfo;
class CInputDialog;
class CCacheEngine;
class CHandleSynchronization;

class CSutraDownload : public CWindow
					 , public MPopUpMenuObserver
					 , public MGradeObserver
					 , public MHandleEventObserver
					 , public MDialogObserver
					 , public MInfoObserver
					 , public MInputObserver// public CBase
					 , public MCacheObserver
					 ,public MContentInfoObserver
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CSutraDownload();

        /**
        * Two-phased constructor.
        */
	static CSutraDownload* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CSutraDownload* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


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

public://From ContentInfoDialog
	virtual void ContentInfoEvent(TInt aLeftCommond);

public://add by hesanyuan
	virtual void DialogEvent(TInt aCommand);
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver
    virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
	void InitDialog(const TDesC& aText);//用来初始化对话框
	void InitGradeDialog();//评分界面。
	void InitWaitDialog(const TDesC& aText);
	void SendMyGrade();//发送评分。
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);
//信息详情.
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();

public:
	//下载
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize);
	void DownLoadFile(TInt aDownType);
public:	
	void Changdrawpage(TBool aflages);//用于翻页
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	CDialog* iDialog;
	//void ReadXML();//读XML文件里的内容

	void ReadXML(const TDesC& aFileName);

	void GetInfo();//得到信息详情
	void Search();
	void Goto(TInt apage);//跳转
	void Changpage(TBool aflalgs);//翻页;
	CDesCArray* iDesArray;//用于存放数列表里的数据。
	void ShowList();//显示列表
	TInt AdvertisMent();//广告.
	void InputDialog();//跳到的输入框
	void DrawRightInfo();
	TBool UpData();
	void CreateTileAndList();

	void CheckDowndtype();//检查文件类型.

private:
		
	TInt iSearchid;//add
	TBool iflalgs;//add
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CCacheEngine*	iCacheEngine;
	TInt iDownType;
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CHandleGrade* iShowGrade;//评分
	TInt iGrade;
	TInt iPopState;
	CHandleInfo* iShowInfo;//信息详情
	TInt iCurpage;//当前页.
	TInt iAllpage;//总页
	//图标
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	CInputDialog* iInputDialog;// 输入框
	
	CHandleSynchronization& iSyschr;//同步
	TInt *iMenuArray;//保存菜单项的数组.
	TInt iArrayNo;
	TInt iJumpPage;	//add这个表示跳的页数。//add
	TInt iJumpFlage;//共有三个值:0表示没有触发，1表示跳动。

	CContentInfoDialog* iContentInfo;//信息详情显示框.
	/**
        * Constructor for performing 1st stage construction
        */
	CSutraDownload(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // SutraDownload_H
