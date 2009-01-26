/*
============================================================================
 Name        : DownloadVoiceSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadVoiceSettingWindow declaration
============================================================================
*/

#ifndef DOWNLOADVOICESETTINGWINDOW_H
#define DOWNLOADVOICESETTINGWINDOW_H

// INCLUDES
#include "Window.h"
#include "DialogObserver.h"

class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CDownloadVoiceSettingWindow
* 
*/
class CDownloadVoiceSettingWindow : public CWindow,public MDialogObserver
{
public: // Constructors and destructor

	~CDownloadVoiceSettingWindow();
	static CDownloadVoiceSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDownloadVoiceSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CDownloadVoiceSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

#endif // DOWNLOADVOICESETTINGWINDOW_H
