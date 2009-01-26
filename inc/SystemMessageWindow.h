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

#define MSGLIST_NUM 50//���ϵͳ��Ϣ��
#define ITEM_NUM_PER_PAGE 15//ÿҳ����Ϣ��

typedef struct TSysMsg SYSMSG;
struct TSysMsg 
{
	TInt			IsUsed;			//�Ƿ�ʹ��
	TBuf<255>		nFileName;		//�ļ�����
	TBuf<100>	    nShowInfo;		//��ʾ��Ϣ
	TBuf<100>		nTitle;			//����
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
	//������ʵ�ּ�����ͼ
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

	//���ļ��б�
	void ReadFileNameList(const TDesC& strPath, CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TUint8>* pDirArray);
	
	void DisplaySysMsg(TInt aCurPage);//��ʾϵͳ��Ϣ

	void DisplayPageNum();//��ʾҳ��
private:
	CMainEngine& iMainEngine;
	
	CTitleBar* iTitle ;

	CListBox* iSysInfoListbox;
	TInt iPageNum;//�б��ҳ��
	TInt iCurPage;//��ǰҳ

	TInt	iMsgNum;//ʵ�ʵĸ���

	CArrayFixFlat<TCONTENTTEXT>*	iFileNameArray;//̫���˰ѣ���

	TSysMsg* iMsgList;

	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;

	CFileReadByRow* iRes;
	
};

#endif // SYSTEMMESSAGEWINDOW_H
