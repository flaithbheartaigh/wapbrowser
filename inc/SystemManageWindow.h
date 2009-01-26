/*
============================================================================
 Name        : SystemManageWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CSystemManageWindow declaration
============================================================================
*/

#ifndef SYSTEMMANAGEWINDOW_H
#define SYSTEMMANAGEWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

#include "window.h"
#include "NaviPaneObserver.h"
#include "PopUpMenuObserver.h"
//#include "FileReadByRow.h"
// CLASS DECLARATION

/**
*  CSystemManageWindow
* 
*/


class CListBox;
class CNaviPane;
class CFileReadByRow;

class CSystemManageWindow : public CWindow , public MNaviPaneObserver,public MPopUpMenuObserver
{
public: // Constructors and destructor
	~CSystemManageWindow();
	static CSystemManageWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CSystemManageWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CSystemManageWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

	// public:
	 	virtual void NaviPaneIndexChanged(TInt aNewIndex);

	//From MPopUpMenuObserver
	virtual void DoPopUpMenuCommand(TInt aCommand);
private:

	void	InitPopUpMenu();
public:
	
private:

	CListBox* iListBox;
	CNaviPane* iNaviPane;
	
	const CFbsBitmap*	iIconBitmap;
	const CFbsBitmap*	iIconBitmapMask;
	CFileReadByRow* iRes;
	
};

#endif // SYSTEMMANAGEWINDOW_H
