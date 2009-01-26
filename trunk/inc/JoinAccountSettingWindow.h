/*
============================================================================
 Name        : JoinAccountSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CJoinAccountSettingWindow declaration
============================================================================
*/

#ifndef JOINACCOUNTSETTINGWINDOW_H
#define JOINACCOUNTSETTINGWINDOW_H

// INCLUDES
#include "Window.h"
#include "DialogObserver.h"

class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CJoinAccountSettingWindow
* 
*/
#define KMaxIapNum 20
#define KMaxIapNameLength 100
struct TIapInfo 
{
	TInt		nIapID;		//iap��id
	TBuf<KMaxIapNameLength>	nIapName;	//iap������	
	TBool	    nIsUse;		//�Ƿ�ʹ��
} ;
class CJoinAccountSettingWindow : public CWindow,public MDialogObserver
{
public: // Constructors and destructor

	~CJoinAccountSettingWindow();
	static CJoinAccountSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CJoinAccountSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CJoinAccountSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);
public://��ȡ������ʽ
	void  GetIapInfo();

private:
	void	InitSelectGroup();

private:
	CSelectGroup*	iSelectGroup;
	CMainEngine& iMainEngine;

	TIapInfo* iIapInfoList;//iap��Ϣ�б�
	TInt iIapNum;//iap�ĸ���

	CFileReadByRow* iRes;
};


#endif // JOINACCOUNTSETTINGWINDOW_H
