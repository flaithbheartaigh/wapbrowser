/*
============================================================================
Name        : Scrollbar.h
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : 两个滚动条类
			: CScrollbarWithFocus
			: 滚动条,用于简单的情况,即没有焦点的情况,例如帮助

			 
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

	void	DefaultLayout();		//为懒人准备

	void	SetShow(TBool aShow);

	void	SetSize(TInt aWidth,TInt aHeight);

	void	SetVerticalSpace(TInt aSpace);

	void	SetScrollWidth(TInt	aWidth);

	void	SetMaxLinePerPage(TInt aMaxLine);

	void	SetTotalLineNum(TInt aLineNum);

	void	SetTopRightPoint(const TPoint& aTopRightPoint);		//设置右上角位置

	void	SetBeginPostion(const TPoint& aPoint);

	void	SetBackgroundColor(const TRgb& aColor);

	void	SetCursorColor(const TRgb& aColor);

	void	SetControlMode(TInt aControlMode);

	void	Layout();

	TBool	Up();

	TBool	Down();

	TBool	Home();

	TBool	End();

	TBool	PageUp();				//在有iMaxLinePerPage时才能使用,考虑用某种方式替换以去除对iMaxLinePerPage的使用

	TBool	PageDown();				//在有iMaxLinePerPage时才能使用,考虑用某种方式替换以去除对iMaxLinePerPage的使用

	TInt	CurIndex()const;

	void	SetIndex(TInt aIndex);

private:
	TBool	CircleContolKeyEventL(TInt aKeyCode);

	TBool	PageControlKeyEventL(TInt aKeyCode);

	void	CalculateCursorHeight();		//计算浮标长度,当浮标的长度小于3时,均等于3

	void	HandleNoEnoughSpace();			//若阶数比可移动的像素点还多,移动空间不够时的处理

	void	CalculateCursorPosition();

	TInt	GetIncrement(TInt aIndex);


	void	AsserClass()const;

private:
	static	int CalculateIncrement(int grads,int remainder,int index);

private://通过设置得到
	TPoint	iBeginPosition;		//绘制的左上角点
	TPoint	iCursorPosition;

	TRgb	iBackgroundColor;
	TRgb	iCursorColor;

	TInt	iScrollVerticalSpace;
	TInt	iScrollHeight;
	TInt	iScrollWidth;

	TInt	iMaxLinePerPage;	//当需要在不移动滚动条的情况下移动当前项目时使用
								//用于向上或向下翻页
								//考虑是否可以不在该类中保存该变量
	TInt	iTotalLineNum;		//总的文本行数,仅用于计算iGrads
								//考虑去掉该变量

	TBool	iIsShow;			//是否显示

// 	TBool	iCanCircle;			//是否循环滚动,默认不允许
// 
// 	TBool	iPageControl;		//是否允许翻页控制,默认允许
	TInt	iControlMode;		//控制方式:允许循环滚动,允许翻页控制等,默认为允许翻页,不允许滚动

private://通过计算得到
	TInt	iGrads;				//滚动条可移动的阶数

	TInt	iIndex;				//滚动条的索引
// 	TInt	iIncrement;			//滚动条移动时的增量
// 	TInt	iRemainder;			//用于计算滚动条的位置

	//对该观察者的使用为可选,若不使用该观察者,则用户代码可以使用CurIndex()取出当前索引自行计算
	MScrollbarObserver*	iObserver;

	TBool	iAlreadyLayout;
};

#endif
