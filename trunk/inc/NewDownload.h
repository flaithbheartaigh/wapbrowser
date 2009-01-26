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
#include "tinyxml.h"//��������XML��������ġ�
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
	//������ʵ�ֶ�����ͼ
	virtual void	DoDeactivate();
	//������ʵ�ֻ�ͼ
	virtual void	DoDraw(CGraphic& aGraphic)const;
	//������ʵ�ְ����¼���Ӧ
	virtual TBool	DoKeyEventL(TInt aKeyCode);
	//������ʵ�������¼���Ӧ
	virtual TBool	DoHandleCommandL(TInt aCommand);
	//������ʵ�ֽ���ߴ�ı�
	virtual void	DoSizeChanged();

	//������ʵ���������Ұ�ť,�Ա㸲�ǰ�ť�Ŀؼ�����ʱ�ظ�ԭ��
	virtual void	ChangeButton();
	//���ڵ����˵�.
	virtual void DoPopUpMenuCommand(TInt aCommand);
	
	void AddListITemL();
	//��ת���������
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);//From InputDialog;
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver
	virtual void GradeEvent(TInt aEvent,TInt aGrade);//From GradeDialog

public://From ContentInfoDialog
	virtual void ContentInfoEvent(TInt aLeftCommond);
	void	InitInfoDialog(const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool);

	void CheckDowndtype();//����ļ�����.

public:
	void InitMenu();//��ʼ���˵�.
	
	void ReadXML(const TDesC& aFileName);//���ļ��ж�XML�ļ���

	TBool HandleIconKeyL(TInt aKeyCode);//��֪��ʲô�á�

	void EnterModel(TInt aType);//�����Ӵ���

	void ChangListItem(TInt aItem);
	void Goto(TInt apage);//��ת
	void Changpage(TBool aflalgs);//��ҳ;
	void ShowList();//��ʾ�б�
	void GetKeyWord(TDes& desleft,TDes& desright);
	void Search();
	void DrawRightInfo();//�ڱ����������Ͻǣ���ҳ����

//	void InitInfoDialog(const TDesC& aTitle,const TDesC& aInfo);//��Ϣ���顣

	virtual void InfoResponseEvent(TInt aEvent);//From MInfoObserver
public:
	TBuf<40>titlebuf;//����
	TInt iSecondID;
	TInt iThirdID;
	TInt iFirstID;
	CDesCArray* iDesArray;//���ڴ�����б�������ݡ�
	void InputDialog();//�����������
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	//����
	void GetDownLoadInfo(TDes& aUrl,TDes& aFileName,TInt& iFileSize);
	void DownLoadFile(TInt aDownType);
	TInt AdvertisMent();//���.
	void InitDialog(const TDesC& aText);//������ʼ���Ի���
	void SendMyGrade();//��������
	void InitWaitDialog(const TDesC& aText);//�ȴ��Ի���
	void InitGradeDialog();//���ֽ��档.
	
	void CreateTileAndList();

	void GetInfo();

	void ConstructLater();
	void Changdrawpage(TBool aflages);//���ڷ�ҳ
	void SetViewType(TInt aType){ iViewType=aType;}
public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();
private:
	TBool iflalgs;
	const TDesC *itempdes;//add
	TBool iGradeFlage;//add
	CListBox* ilistbox;//�б�
	CNaviPane* naviPane;
	CTitleBar* iTitlebar;
	TInt iCurpage;//��ǰҳ.
	TInt iAllpage;//��ҳ
	HBufC* titletxt;
	CInputDialog* iInputDialog;// ����� 
	//ͼ��
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	//��ʾ�Ի���
	CDialog* iDialog;
	CHandleGrade* iShowGrade;//����
	TInt iMidpage;
	TInt iDwonType;//�ж��ظ�����
private:
	TInt iViewType;
	CCacheEngine* iCacheEngine;
	TInt iGrade;//���ּ���

	CHandleInfo* iShowInfo;
	CContentInfoDialog* iContentInfo;//��Ϣ����ĶԻ���.
	/**
        * Constructor for performing 1st stage construction
        */
	CNewDownload(CWindow* aParent,CMainEngine& aMainEngine);

	TInt iJumpPage;	//add�����ʾ����ҳ����
	TInt iJumpFlage;//��������ֵ:0��ʾû�д�����1��ʾ������
	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // NEWDOWNLOAD_H
