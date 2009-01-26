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

public://add by hesanyuan
	virtual void DialogEvent(TInt aCommand);
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver
    virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
	void InitDialog(const TDesC& aText);//������ʼ���Ի���
	void InitGradeDialog();//���ֽ��档
	void InitWaitDialog(const TDesC& aText);
	void SendMyGrade();//�������֡�
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);
//��Ϣ����.
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();

public:
	//����
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt iFileSize);
	void DownLoadFile(TInt aDownType);
public:	
	void Changdrawpage(TBool aflages);//���ڷ�ҳ
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	CDialog* iDialog;
	//void ReadXML();//��XML�ļ��������

	void ReadXML(const TDesC& aFileName);

	void GetInfo();//�õ���Ϣ����
	void Search();
	void Goto(TInt apage);//��ת
	void Changpage(TBool aflalgs);//��ҳ;
	CDesCArray* iDesArray;//���ڴ�����б�������ݡ�
	void ShowList();//��ʾ�б�
	TInt AdvertisMent();//���.
	void InputDialog();//�����������
	void DrawRightInfo();
	TBool UpData();
	void CreateTileAndList();

	void CheckDowndtype();//����ļ�����.

private:
		
	TInt iSearchid;//add
	TBool iflalgs;//add
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CCacheEngine*	iCacheEngine;
	TInt iDownType;
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CHandleGrade* iShowGrade;//����
	TInt iGrade;
	TInt iPopState;
	CHandleInfo* iShowInfo;//��Ϣ����
	TInt iCurpage;//��ǰҳ.
	TInt iAllpage;//��ҳ
	//ͼ��
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	CInputDialog* iInputDialog;// �����
	
	CHandleSynchronization& iSyschr;//ͬ��
	TInt *iMenuArray;//����˵��������.
	TInt iArrayNo;
	TInt iJumpPage;	//add�����ʾ����ҳ����//add
	TInt iJumpFlage;//��������ֵ:0��ʾû�д�����1��ʾ������

	CContentInfoDialog* iContentInfo;//��Ϣ������ʾ��.
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
