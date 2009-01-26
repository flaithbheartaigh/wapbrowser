/*
============================================================================
 Name        : ListBoxItem.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CListBoxItem declaration

 说明		 : 目前只适用于一行或三行的情况,新增需求需要添加新的CListBoxItem
			 : 派生类且修改ListBox
============================================================================
*/

#ifndef LISTBOXITEM_H
#define LISTBOXITEM_H

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>

#include "NotifyTimer.h"

class CFont;
class CFbsBitmap;

class CGraphic;
class CListBox;
class CSlideText;
class MDrawEventObserver;

class CListBoxItem //: public CBase
{
public:
	virtual	~CListBoxItem();
	void	SetParent(CListBox* aParent);
	TInt	Active() const;

	virtual void	Init(const TSize& aLineSize,const CFont* aFont) = 0;		//初始化,Append以后被CListBox调用
	virtual	void	Draw(CGraphic& aGraphic,TPoint& aPoint) const = 0;							//绘制完成要修改坐标
	//virtual	CListBoxItem*	Clone() const = 0;
	virtual	void	SetActive(TBool aActive) = 0;
	virtual	void	SetTextColor(const TRgb& aColor) = 0;
	virtual	TInt	LineNum() const = 0;
	virtual	TInt	LineNumWhenFocus() const = 0;

protected:
	CListBoxItem();

	void	RequestDraw();

protected:
	CListBox*	iParent;
	TBool		iActive;
};
//////////////////////////////////////////////////////////////////////////
//CNormalListBoxItem
//标准的单行列表
//////////////////////////////////////////////////////////////////////////
class CNormalListBoxItem : public CListBoxItem
{
public:
	static	CNormalListBoxItem*	NewL(const TDesC& aDes,
									 const CFbsBitmap* aIcon = NULL,
									 const CFbsBitmap* aIconMask = NULL,
									 TBool aLimitText = EFalse);
	~CNormalListBoxItem();

private:
	CNormalListBoxItem(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void	ConstructL(const TDesC& aDes,TBool aLimitText);

public://From CListBoxItem
	virtual void	Init(const TSize& aLineSize,const CFont* aFont);
	virtual	void	Draw(CGraphic& aGraphic,TPoint& aPoint) const;		//绘制完成要修改坐标
	//virtual CListBoxItem*	Clone() const;
	virtual	void	SetActive(TBool aActive);
	virtual	void	SetTextColor(const TRgb& aColor);
	virtual	TInt	LineNum() const;
	virtual	TInt	LineNumWhenFocus() const;

public:
	void	ChangeIcon(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void	ChangeText(const TDesC& aText);

	const TDesC&	GetText() const;

private:
	CSlideText*			iSlideText;
	HBufC*				iText;

	TSize				iItemSize;
	mutable	TRect		iRect;
	//HBufC*				iText;

	const CFbsBitmap*	iIcon;
	const CFbsBitmap*	iIconMask;

	TBool				iLimitText;
};

//////////////////////////////////////////////////////////////////////////
//CCurThreeListBoxItem
//未被选中时显示单行,选中时显示三行
//////////////////////////////////////////////////////////////////////////
class CCurThreeListBoxItem : public CListBoxItem
{
public:
	static	CCurThreeListBoxItem*	NewL(const TDesC& aDes,
										 const TDesC& aSecondLine = KNullDesC,
										 const TDesC& aThirdLine = KNullDesC,
										 const CFbsBitmap* aIcon = NULL,
										 const CFbsBitmap* aIconMask = NULL);
	~CCurThreeListBoxItem();

private:
	CCurThreeListBoxItem(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void	ConstructL(const TDesC& aDes,const TDesC& aSecondLine,const TDesC& aThirdLine);

public://From CListBoxItem
	virtual void	Init(const TSize& aLineSize,const CFont* aFont);
	virtual	void	Draw(CGraphic& aGraphic,TPoint& aPoint) const;		//绘制完成要修改坐标
	virtual	void	SetActive(TBool aActive);
	virtual	void	SetTextColor(const TRgb& aColor);
	virtual	TInt	LineNum() const;
	virtual	TInt	LineNumWhenFocus() const;

public:
	void	ChangeIcon(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void	ChangeText(const TDesC& aText,const TDesC& aSecondLine = KNullDesC,const TDesC& aThirdLine = KNullDesC);

	const TDesC&	GetText() const;

private:
	CSlideText*			iSlideText;
	HBufC*				iText;
	HBufC*				iSecondText;
	HBufC*				iThirdText;

	TSize				iItemSize;
	mutable	TRect		iRect;

	const CFbsBitmap*	iIcon;
	const CFbsBitmap*	iIconMask;

	TRgb				iTextColor;

};
#endif // LISTBOXITEM_H
