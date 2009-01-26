/*
============================================================================
Name        : MainHelpWindow.h
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : CMainHelpWindow declaration
============================================================================
*/

#ifndef MAINHELPWINDOW_H
#define MAINHELPWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

#include "Window.h"
#include "NaviPaneObserver.h"
// CLASS DECLARATION
class CListBox;
class CNaviPane;
/**
*  CMainHelpWindow
* 
*/
class CMainHelpWindow : public CWindow ,public MNaviPaneObserver 
{
public: // Constructors and destructor
	~CMainHelpWindow();
	static CMainHelpWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CMainHelpWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CMainHelpWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	//pan����Ϣ��Ӧ(��Ӧtab�����л�)
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

private:
	void	InitPopUpMenu();
public:
	//�Զ���ĺ��������ڶ�ȡ������������Ϣ
	void  LoadHelpConfig(const TDesC& aFileName);

	//����������
	void CreatNaviPan();//����iItem���������

	//����ѡ�е�tab�Ĳ�ͬΪ�б���Ӳ�ͬ������
	void AddListboxItem(TInt aCurrentTab);

	//���ļ��м������ݵ��б�
	void LoadItem(const TDesC& aFileName);
private:
	CDesCArray*  iConfigItem;//��Ű�ס�����ļ�������

	CDesCArray*  iItem;//��ž�����б��������

	CListBox* iHelpListbox;

	CNaviPane* iNaviPane; 

	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;


};

#endif // MAINHELPWINDOW_H
