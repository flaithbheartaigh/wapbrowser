/*
============================================================================
 Name        : SutraSearch.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSutraSearch declaration
============================================================================
*/

#ifndef SUTRASEARCH_H
#define SUTRASEARCH_H

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
#include "ListBoxItem.h"
#include "SearchListBox.h"
/**
*  CSutraSearch
* 
*/
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"


class CSutraSearch : public CWindow,public MPopUpMenuObserver,public MSearchListObserver// public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CSutraSearch();

        /**
        * Two-phased constructor.
        */
	static CSutraSearch* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CSutraSearch* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


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
	//form MSearchListObserver
	virtual void SearchListResponseEvent(TInt aEvent,const TDesC& aText);
public:
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	TBuf<60>titlebuf;
private:

	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CSearchListBox* iSearchList;//�����ؼ���
	/**
        * Constructor for performing 1st stage construction
        */
	CSutraSearch(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // SutraSearch_H
