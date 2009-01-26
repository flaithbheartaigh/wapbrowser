/*
============================================================================
 Name        : DownloadDirSettingWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadDirSettingWindow declaration
============================================================================
*/

#ifndef DOWNLOADDIRSETTINGWINDOW_H
#define DOWNLOADDIRSETTINGWINDOW_H
// INCLUDES
#include "Window.h"
#include "DialogObserver.h"
#include "FileBrowserObserver.h"
class CSelectGroup;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CDownloadDirSettingWindow
* 
*/
class CDownloadDirSettingWindow : public CWindow,public MDialogObserver,public MFileBrowserObserver
{
public: // Constructors and destructor

	~CDownloadDirSettingWindow();
	static CDownloadDirSettingWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDownloadDirSettingWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CDownloadDirSettingWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
public://From MFileBrowserObserver
	virtual void	DoFileEvent(const TDesC& aFileName,TInt aCommand);

private:
	void	InitMultiText();

private:
	
	CMainEngine& iMainEngine;
	TFileName iNewPathName;//�û�ѡ���Ҫ���ĳɵ�·��

	CFileReadByRow* iRes;
};

#endif // DOWNLOADDIRSETTINGWINDOW_H
