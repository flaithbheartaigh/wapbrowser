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
//�����������Ҫ�ա�
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
	//������ʵ�ֶ�����ͼ
	virtual void	DoDeactivate();
	//������ʵ�ֻ�ͼ
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//������ʵ�ְ����¼���Ӧ
	virtual TBool	DoKeyEventL(TInt aKeyCode);

	virtual TBool	DoHandleCommandL(TInt aCommand);
	//������ʵ�ֽ���ߴ�ı�
	virtual void	DoSizeChanged();

	//������ʵ���������Ұ�ť,�Ա㸲�ǰ�ť�Ŀؼ�����ʱ�ظ�ԭ��
	virtual void	ChangeButton();
public://From ContentInfoDialog
	virtual void ContentInfoEvent(TInt aLeftCommond);
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);

public:
	virtual void DialogEvent(TInt aCommand);
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver

	void InitDialog(const TDesC& aText);//������ʼ���Ի���
	void InitGradeDialog();//���ֽ��档
	void InitWaitDialog(const TDesC& aText);
	void SendMyGrade();//��������
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;

	virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
//	void InitInfoDialog(const TDesC& aTitle,const TDesC& aInfo);//��Ϣ���顣
	void GetInfo();
	
	void CheckDowndtype();//����ļ�����.

public:
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	void ReadXML(const TDesC& aFileName);

	void Goto(TInt apage);//��ת
	void Changpage(TBool aflalgs);//��ҳ;
	void ShowList();//��ʾ�б�
	CDesCArray* iDesArray;//���ڴ�����б�������ݡ�
	void InputDialog();//�����������
	//����
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize);
	void DownLoadFile(TInt adowntype);
	TInt AdvertisMent();//���.
	
	void GetGradid(TInt& aFirstid,TInt& aSecondid,TInt& aInput);//ainput:����Ҫ�õ���һ������ֵ��
	void CreateTileAndList();
	
	TBool UpData();
	void DrawRightInfo();
	
	void Search();
	TInt iSearchid;
	void Changdrawpage(TBool aflages);//���ڷ�ҳ
	
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();
private:

	TBool iflalgs;//add
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	
	TInt iCurpage;//��ǰҳ.
	TInt iAllpage;//��ҳ
	CDialog* iDialog;
	CHandleGrade* iShowGrade;//����
	CInputDialog* iInputDialog;
		//ͼ��
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	TInt iGrade;
	CCacheEngine* iCacheEngine;
	CHandleSynchronization& iSyschr;//ͬ��
	TInt iInputid;

	TInt *iMenuArray;
	TInt iArrayNo;
	
	TInt iJumpPage;	//add�����ʾ����ҳ����
	TInt iJumpFlage;//��������ֵ:0��ʾû�д�����1��ʾ������
	TInt iDownType;
	CContentInfoDialog* iContentInfo;//��Ϣ����ĶԻ���.
	CHandleInfo* iShowInfo;//��Ϣ����/
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
