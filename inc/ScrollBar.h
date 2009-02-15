/*
============================================================================
Name        : Scrollbar.h
Author      : ��������
Version     :
Copyright   : Your copyright notice
Description : ������������
			: CScrollbarWithFocus
			: ������,���ڼ򵥵����,��û�н�������,�������

			 
============================================================================
*/

#ifndef	__SCROLLBAR_H__
#define __SCROLLBAR_H__
// 
// #include "Control.h"
// #include "PreDefine.h"
#include "Define.h"

class MScrollbarObserver;
//////////////////////////////////////////////////////////////////////////
//CScrollbar
//////////////////////////////////////////////////////////////////////////
class CScrollBar 
	: public CBase
	//: public CControl  
{
public:
	enum TControlMode
	{
		EPageControl,
		ECircleControl,
	};
public:
	CScrollBar();
	virtual ~CScrollBar();

	static	CScrollBar*	CreateDefaultScrollbarL();

public://From CControl
	//virtual void	Draw(CGraphic& aGraphic)const;
	virtual void	Draw(CGraphicsContext& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void	SetObserver(MScrollbarObserver* aObserver);

	void	DefaultLayout();		//Ϊ����׼��

	void	SetShow(TBool aShow);

	void	SetSize(TInt aWidth,TInt aHeight);

	void	SetVerticalSpace(TInt aSpace);

	void	SetScrollWidth(TInt	aWidth);

	void	SetMaxLinePerPage(TInt aMaxLine);

	void	SetTotalLineNum(TInt aLineNum);

	void	SetTopRightPoint(const TPoint& aTopRightPoint);		//�������Ͻ�λ��

	void	SetBeginPostion(const TPoint& aPoint);

	void	SetBackgroundColor(const TRgb& aColor);

	void	SetCursorColor(const TRgb& aColor);

	void	SetControlMode(TInt aControlMode);

	void	Layout();

	TBool	Up();

	TBool	Down();

	TBool	Home();

	TBool	End();

	TBool	PageUp();				//����iMaxLinePerPageʱ����ʹ��,������ĳ�ַ�ʽ�滻��ȥ����iMaxLinePerPage��ʹ��

	TBool	PageDown();				//����iMaxLinePerPageʱ����ʹ��,������ĳ�ַ�ʽ�滻��ȥ����iMaxLinePerPage��ʹ��

	TInt	CurIndex()const;

	void	SetIndex(TInt aIndex);

private:
	TBool	CircleContolKeyEventL(TInt aKeyCode);

	TBool	PageControlKeyEventL(TInt aKeyCode);

	void	CalculateCursorHeight();		//���㸡�곤��,������ĳ���С��3ʱ,������3

	void	HandleNoEnoughSpace();			//�������ȿ��ƶ������ص㻹��,�ƶ��ռ䲻��ʱ�Ĵ���

	void	CalculateCursorPosition();

	TInt	GetIncrement(TInt aIndex);


	void	AsserClass()const;

private:
	static	int CalculateIncrement(int grads,int remainder,int index);

private://ͨ�����õõ�
	TPoint	iBeginPosition;		//���Ƶ����Ͻǵ�
	TPoint	iCursorPosition;

	TRgb	iBackgroundColor;
	TRgb	iCursorColor;

	TInt	iScrollVerticalSpace;
	TInt	iScrollHeight;
	TInt	iScrollWidth;

	TInt	iMaxLinePerPage;	//����Ҫ�ڲ��ƶ���������������ƶ���ǰ��Ŀʱʹ��
								//�������ϻ����·�ҳ
								//�����Ƿ���Բ��ڸ����б���ñ���
	TInt	iTotalLineNum;		//�ܵ��ı�����,�����ڼ���iGrads
								//����ȥ���ñ���

	TBool	iIsShow;			//�Ƿ���ʾ

// 	TBool	iCanCircle;			//�Ƿ�ѭ������,Ĭ�ϲ�����
// 
// 	TBool	iPageControl;		//�Ƿ�����ҳ����,Ĭ������
	TInt	iControlMode;		//���Ʒ�ʽ:����ѭ������,����ҳ���Ƶ�,Ĭ��Ϊ����ҳ,���������

private://ͨ������õ�
	TInt	iGrads;				//���������ƶ��Ľ���

	TInt	iIndex;				//������������
// 	TInt	iIncrement;			//�������ƶ�ʱ������
// 	TInt	iRemainder;			//���ڼ����������λ��

	//�Ըù۲��ߵ�ʹ��Ϊ��ѡ,����ʹ�øù۲���,���û��������ʹ��CurIndex()ȡ����ǰ�������м���
	MScrollbarObserver*	iObserver;

	TBool	iAlreadyLayout;
};

#endif
