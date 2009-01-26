/*
============================================================================
Name        : CDisplayHelpWindow.h
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : ������ʾ�����б���ѡ�еľ���������Ӧ�İ�����Ϣ��^_^
��
============================================================================
*/

#ifndef CDISPLAYHELPWINDOW_H
#define CDISPLAYHELPWINDOW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


#include "Window.h"

#include "MultiText.h"
#include "MControlObserver.h"
// CLASS DECLARATION

/**
*  CDisplayHelpWindow
* 
*/
class CMultiText;

class CDisplayHelpWindow : public CWindow ,public MMultiObserver
{
public: 
	// Constructors and destructor
	~CDisplayHelpWindow();
	static CDisplayHelpWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CDisplayHelpWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:
	CDisplayHelpWindow(CWindow* aParent,CMainEngine& aMainEngine);
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
	void	        InitPopUpMenu();//����û��ʲô��
	//from MMultiObserver
	virtual void MultiEvent(TInt aEvent);
	//�������Զ���Ķ���
private:
	//���ڽ�������ģ��

	TInt iModelNumber;//�����ڹ�������ʱ����ʶģ�������

	CMultiText* iMultiText;
public:
	//��ʾMultiText�����title�����ݰ������ݵĲ�ͬ��̬�ı仯
	void LoadTitle(const TDesC& aTitleText);
	//�����ļ��ĺ�������ʵ�ָ��ݰ�ס���ݵĲ�ͬ����̬��ʾ���ݵĹ���
	void LoadFileToMultiText(const TDesC& aFileName);
	//���ر�ʶģ�������
	void LoadEnterModel(TInt aModelNumber);
	//����Ҫ��ʾ����������
	void LoadLinkText(const TDesC& aLinkText);

};
#endif // BARCODEHELPWINDOW_H
