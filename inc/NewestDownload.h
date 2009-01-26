/*
============================================================================
 Name        : NewestDownload.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice 
 Description : NewestDownload declaration
============================================================================
*/

#ifndef NEWESTDOWNLOAD_H 
#define NEWESTDOWNLOAD_H
//上面两句很重要勒。
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

#include "BitmapFactory.h"
#include "PictureEngine.h"
//add by hesanyuan
#include "Dialog.h"

/**
*  NewestDownload
* 
*/
#include "CacheObserver.h"//cacheobserver.
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"
#include "DialogObserver.h"
#include "ContentInfoDialog.h"
class CHandleGrade;
class CInputDialog;
class CHandleSynchronization;
class CCacheEngine;
class CHandleInfo;

class NewestDownload : public CWindow
					,public MPopUpMenuObserver
					,public MGradeObserver
					,public MHandleEventObserver
					,public MDialogObserver
					,public MInputObserver// public CBase
					, public MCacheObserver
					, public MInfoObserver
					,public MContentInfoObserver
{
public: // Constructors and destructor
 
	/**  
        * Destructor.
        */
	~NewestDownload();

        /**
        * Two-phased constructor.
        */
	static NewestDownload* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static NewestDownload* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


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
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);

public:
	virtual void DialogEvent(TInt aCommand);
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver

	void InitDialog(const TDesC& aText);//用来初始化对话框
	void InitGradeDialog();//评分界面。
	void InitWaitDialog(const TDesC& aText);
	void SendMyGrade();//发送评分
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;

	virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
//	void InitInfoDialog(const TDesC& aTitle,const TDesC& aInfo);//信息详情。
	void GetInfo();
	
	void CheckDowndtype();//检查文件类型.

public:
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	void ReadXML(const TDesC& aFileName);

	void Goto(TInt apage);//跳转
	void Changpage(TBool aflalgs);//翻页;
	void ShowList();//显示列表
	CDesCArray* iDesArray;//用于存放数列表里的数据。
	void InputDialog();//跳到的输入框
	//下载
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize);
	void DownLoadFile(TInt adowntype);
	TInt AdvertisMent();//广告.
	
	void GetGradid(TInt& aFirstid,TInt& aSecondid,TInt& aInput);//ainput:你想要得到的一二级的值。
	void CreateTileAndList();
	
	TBool UpData();
	void DrawRightInfo();
	
	void Search();
	TInt iSearchid;
	void Changdrawpage(TBool aflages);//用于翻页
	
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();
private:

	TBool iflalgs;//add
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	
	TInt iCurpage;//当前页.
	TInt iAllpage;//总页
	CDialog* iDialog;
	CHandleGrade* iShowGrade;//评分
	CInputDialog* iInputDialog;
		//图标
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	TInt iGrade;
	CCacheEngine* iCacheEngine;
	CHandleSynchronization& iSyschr;//同步
	TInt iInputid;

	TInt *iMenuArray;
	TInt iArrayNo;
	
	TInt iJumpPage;	//add这个表示跳的页数。
	TInt iJumpFlage;//共有三个值:0表示没有触发，1表示跳动。
	TInt iDownType;
	CContentInfoDialog* iContentInfo;//信息详情的对话框.
	CHandleInfo* iShowInfo;//信息详情/
	/**
        * Constructor for performing 1st stage construction
        */
	NewestDownload(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // FRIENDLINK_H
