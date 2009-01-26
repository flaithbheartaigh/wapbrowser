/*
============================================================================
 Name        : SelectBoxView.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectBoxView declaration
============================================================================
*/

#ifndef SELECTBOXVIEW_H
#define SELECTBOXVIEW_H

// INCLUDES
#include "Window.h"

class CSelectGroup;
class CMainEngine;
// CLASS DECLARATION

/**
*  CSelectBoxView
* 
*/
class CSelectBoxView : public CWindow
{
public: // Constructors and destructor

	~CSelectBoxView();
	static CSelectBoxView* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CSelectBoxView* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CSelectBoxView(CWindow* aParent,CMainEngine& aMainEngine);
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
private:
	void	InitSelectGroup();

private:
	CSelectGroup*	iSelectGroup;
	CMainEngine& iMainEngine;
};

#endif // SELECTBOXVIEW_H
