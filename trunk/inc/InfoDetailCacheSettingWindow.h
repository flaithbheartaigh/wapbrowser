/*
============================================================================
 Name        : InfoDetailCacheSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CInfoDetailCacheSettingWindow declaration
============================================================================
*/

#ifndef INFODETAILCACHESETTINGWINDOW_H
#define INFODETAILCACHESETTINGWINDOW_H

// INCLUDES
#include "Window.h"
#include "DialogObserver.h"

class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CInfoDetailCacheSettingWindow
* 
*/
class CInfoDetailCacheSettingWindow : public CWindow,public MDialogObserver
{
public: // Constructors and destructor

	~CInfoDetailCacheSettingWindow();
	static CInfoDetailCacheSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CInfoDetailCacheSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CInfoDetailCacheSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

private:
	void	InitSelectGroup();

private:
	CSelectGroup*	iSelectGroup;
	CMainEngine& iMainEngine;

	CFileReadByRow* iRes;
};

#endif // INFODETAILCACHESETTINGWINDOW_H
