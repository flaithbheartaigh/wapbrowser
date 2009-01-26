/*
============================================================================
 Name        : FriendLink.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFriendLink declaration
============================================================================
*/

#ifndef FRIENDLINK_H
#define FRIENDLINK_H

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
#include "Dialog.h"
#include "DialogObserver.h"
/**
*  CFriendLink
* 
*/
#include "MControlObserver.h"
#include "PopUpMenuObserver.h"
#include "tinyxml.h"
#include "tinystr.h"
#include "NaviPaneObserver.h"
#include "NewDownload.h"
class CHandleSynchronization;

class CFriendLink : public CWindow,public MPopUpMenuObserver,public MNaviPaneObserver,public MHandleEventObserver,public MDialogObserver // public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CFriendLink();

        /**
        * Two-phased constructor.
        */
	static CFriendLink* NewL(CWindow* aParent,CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CFriendLink* NewLC(CWindow* aParent,CMainEngine& aMainEngine);


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

	virtual void NaviPaneIndexChanged(TInt aNewIndex);//pan����Ϣ��Ӧ��
public:
	virtual void DialogEvent(TInt aCommand);//form MDialogObserver
	virtual void HandleResponseEvent(TInt aEvent,TInt aType);//From MHandleEventObserver


public:
	void ChangeFriendLinkList(TInt aType);
	void ChangListItem(TInt aNewIndex);
	void AddListITemL();
	TBool HandleIconKeyL(TInt aKeyCode);
	void InitMenu();
	void EnterModel(TInt atype);
	virtual void DoPopUpMenuCommand(TInt aCommand);
	void ReadXML();
	TBool UpData();//����.
	void InitWaitDialog(const TDesC& aText);
	void CreateTileAndList();
	void ChangTitle();//���ı�����.
	TBool FriendUpData();//����.
	void AddFriendList();
private:

	TInt iFLinkType;
	TInt iPIndex;
	CListBox* ilistbox;
	CTitleBar* iTitlebar;
	CNaviPane* naviPane;
	CHandleSynchronization& iSyschr;//ͬ��
	CDialog* iDialog;//��ʾ��Ϣ�Ի���.
	TBool iThirdflags;
	
	TInt ifriendflags;//������ֵ��������ֵ���أ�����ʾ����ȷ���£�����ʾ���������.
	TInt iTitlenumber;
	/**
        * Constructor for performing 1st stage construction
        */
	CFriendLink(CWindow* aParent,CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

};

#endif // FRIENDLINK_H
