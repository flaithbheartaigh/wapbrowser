/*
============================================================================
 Name        : TraficStatisticWindow.h
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CTraficStatisticWindow declaration
============================================================================
*/

#ifndef TRAFICSTATISTICWINDOW_H
#define TRAFICSTATISTICWINDOW_H

// INCLUDES
#include "Window.h"

#include "NotifyTimer.h"

class CMultiText;;
class CMainEngine;
class CFileReadByRow;
// CLASS DECLARATION

/**
*  CTraficStatisticWindow
* 
*/
class CTraficStatisticWindow : public CWindow,public MTimerNotifier
{
public: // Constructors and destructor

	~CTraficStatisticWindow();
	static CTraficStatisticWindow* NewL(CWindow* aParent,CMainEngine& aMainEngine);
	static CTraficStatisticWindow* NewLC(CWindow* aParent,CMainEngine& aMainEngine);

private:

	CTraficStatisticWindow(CWindow* aParent,CMainEngine& aMainEngine);
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

public://From MTimerNotifier
	virtual TBool DoPeriodTask();
private:
	CNotifyTimer* iTimer;

private:
	void	InitSelectGroup();

private:
	CMainEngine& iMainEngine;
	CMultiText* iMultiText;//��ʾ�����ȵȵ�����
	TReal i;

	CFileReadByRow* iRes;
};

#endif // TRAFICSTATISTICWINDOW_H
