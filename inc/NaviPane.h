/*
============================================================================
 Name        : NaviPane.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 导航面板,与listbox类似,但用于顶部导航项的水平移动选择

 待添加功能	:	左右箭头
				判断一屏无法显示的情况


 待改进		:	使数据与界面完全分离,即CNaviPane中不保存要显示的内容与每项的具体信息
============================================================================
*/

#ifndef NAVIPANE_H
#define NAVIPANE_H

#include "Control.h"
#include "PreDefine.h"
#include <badesca.h>

//class CDesCArray;

class CMainEngine;
class MNaviPaneObserver;

class CNaviItem
{
	friend class CNaviPane;
public:
	void	InitializeL(const TDesC& aText,TInt aCommand);

public:
	HBufC*		iItemText;		//用于显示
	TInt		iCommand;		//用于返回命令
};

typedef RPointerArray<CNaviItem> RItemArray;

class CNaviPane : public CControl
{
public: // Constructors and destructor
	CNaviPane(const TRect& aRect,const CFont& aFont,CMainEngine& aMainEngine,MNaviPaneObserver* aNaviPaneObserver = NULL);
	void ConstructL();
	static CNaviPane* NewL();
	static CNaviPane* NewLC();
	~CNaviPane();


public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void	SetFont(const CFont* aFont);

	//void	SetTotalItemNum(TInt aNum);

	void	SetPoint(const TPoint& aPoint);

	void	SetRect(const TRect& aRect);

	void	AppendItem(const TDesC& aDes);

	void	Layout();

	TInt	CurIndex()const;

	void	SetArrowIcon(const CFbsBitmap* aArrowBitmap,const CFbsBitmap* aArrowBitmapMask);

	void	SetIndex(TInt aIndex);

	void	SetObserver(MNaviPaneObserver* aNaviPaneObserver);
/*
	void	DefaultLayout();		//为懒人准备

	void	SetSize(TInt aWidth,TInt aHeight);

	void	SetVerticalSpace(TInt aSpace);

	void	SetMaxLinePerPage(TInt aMaxLine);

	void	SetTotalLineNum(TInt aLineNum);

	void	SetBeginPostion(const TPoint& aPoint);

	void	SetBackgroundColor(const TRgb& aColor);

	void	SetCursorColor(const TRgb& aColor);
*/

private:
	void	ArrowLeft();		//向左按键事件
	void	ArrowRight();		//向右按键事件
	void	CalculatePage();

	void	DrawBackground(CGraphic& gc)const;
	void	DrawEachItem(CGraphic& gc,TPoint& aPoint,TInt aIndex)const;
	void	DrawDivision(CGraphic& gc,TPoint& aPoint)const;
	void	DrawArrow(CGraphic& gc) const;

	void	InitBitmap();
	void	SetArrowIcon();

private:
	const CFont&	iFont;
	CMainEngine&	iMainEngine;
	CDesCArray*		iNaviTexts;		//保存导航项的文字

	MNaviPaneObserver*		iObserver;

	CFbsBitmap*		iTitleBitmap;

	TRect			iNaviPaneRect;
	//TPoint			iBeginPosition;		//绘制的左上角点

	TPoint			iTextBeginPos;		//开始绘制文本的坐标

	TInt			iIndex;			//导航项的索引
	TInt			iPageIndex;		//导航页的索引
	TInt			iThisPageBeginIndex;

	TInt			iFontHeight;

	TBool			iDividePage;		//是否分页

	TSize			iArrowImageSize;

	TInt			iArrowImageWidth;

	RArray<TInt>	iCountArray;		//每一组显示几项
};

#endif // NAVIPANE_H
