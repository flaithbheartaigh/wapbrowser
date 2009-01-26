/*
============================================================================
 Name        : ResultView.h
 Author      : hsany
 Version     :
 Copyright   : Your copyright notice
 Description : CResultView declaration
============================================================================
*/

#ifndef RESULTVIEW_H
#define RESULTVIEW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
#include "MainEngine.h"
#include "Window.h"
#include "TypeDefine.h"
#include "NaviPane.h"
#include "ControlFactory.h"
#include "e32def.h"
#include"ListBox.h"
#include "PopUpMenu.h"
#include "Dialog.h"
#include "DialogObserver.h"
/**
*  CResultView
* 
*/
#include "PopUpMenuObserver.h"
#include "MControlObserver.h"
#include "NaviPaneObserver.h"
class CHandleSynchronization;

class CResultView : public CWindow,public MPopUpMenuObserver,public MNaviPaneObserver,public MHandleEventObserver,public MDialogObserver //public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CResultView();

        /**
        * Two-phased constructor.
        */
	static CResultView* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CResultView* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

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
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	
public://From MHandleEventObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);

	void AddListITemL();//�����б�Ԫ��

	void InitMenu(TInt atype);//��ʼ���˵�.

	TBool HandleIconKeyL(TInt aKeyCode);//��֪��ʲô�á�

	void EnterModel(TInt aType);//�����Ӵ���

	void ChangListItem(TInt aItem);
	void ChangTitle();//���������.
    void InitWaitDialog(const TDesC& aText);	
	TBool UpData();
	void CreateTileAndList();
public://From MNaviPaneObserver
	virtual void NaviPaneIndexChanged(TInt aNewIndex);
	TBuf<35>temptitle;//��������Ӵ��ڵı��⡣
private:

		CListBox* ilistbox;//�б�
		CNaviPane* naviPane;
		TInt panitem;//��ǰ��������ֵ
		CHandleSynchronization& iSyschr;//ͬ��
		CDialog* iDialog;//��ʾ��Ϣ�Ի���.
		TInt iTitlenumber;
	//	HBufC* iTitle;
		
	/**
        * Constructor for performing 1st stage construction
        */
	CResultView(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // RESULTVIEW_H
