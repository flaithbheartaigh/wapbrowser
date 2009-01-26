/*
============================================================================
 Name        : MainSearchView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CMainSearchView declaration
============================================================================
*/

#ifndef MAINSEARCHVIEW_H
#define MAINSEARCHVIEW_H

// INCLUDES
#include "Window.h"
#include "NaviPaneObserver.h"
//#include "PopUpMenuObserver.h"

// CLASS DECLARATION
class CListBox;
class CMainEngine;
class CNaviPane;
/**
*  CMainSearchView
* 
*/
class CMainSearchView : public CWindow , public MNaviPaneObserver //, public MPopUpMenuObserver
{
public: // Constructors and destructor

	~CMainSearchView();
	static CMainSearchView* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CMainSearchView* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CMainSearchView(CWindow* aParent,CMainEngine& aMainEngine);
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

	
public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);

//public://From MPopUpMenuObserver
//	virtual void DoPopUpMenuCommand(TInt aCommand);

private:
	void	InitNaviPane();
	void	InitPopUpMenu();

	void	InitListBox(TInt aIndex);
private:
	CNaviPane* iNaviPane;
	CListBox*	iListBox;
	CMainEngine& iMainEngine;
};

#endif // MAINSEARCHVIEW_H
