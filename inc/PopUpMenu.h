/*
============================================================================
 Name        : PopUpMenu.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 菜单,实现为CControl的派生类
============================================================================
*/

#ifndef POPUPMENU_H
#define POPUPMENU_H

#include "Control.h"
#include "NotifyTimer.h"

class MPopUpMenuObserver;
class CScrollBar;
class CSlideText;
class CMainEngine;
class CFbsBitmap;

//////////////////////////////////////////////////////////////////////////
//TPopUpMenuItem
//////////////////////////////////////////////////////////////////////////
struct TPopUpMenuItem
{
	TBuf<12>	iText;
	TInt		iCommand;
};

//////////////////////////////////////////////////////////////////////////
//CSubMenuItem
//////////////////////////////////////////////////////////////////////////
class CSubMenuItem : public CBase
{
friend class CPopUpMenu;
public:
	CSubMenuItem(const TDesC& aText);
	~CSubMenuItem();

	void	Append(const TDesC& aText,TInt aCommand);

private:
	TBuf<12>	iText;
	RArray<TPopUpMenuItem>	iSubMenuItemArray;
};
//////////////////////////////////////////////////////////////////////////
//CPopUpMenu
//////////////////////////////////////////////////////////////////////////
class CPopUpMenu : public CControl
{
private:
	enum{EText,ESubMenu};
	struct TItemType 
	{
		TInt	iType;			//是否子菜单
		TInt	iNumber;		//不是子菜单,在iItemArray中的序号,是子菜单,在iSubMenuItemArray里的序号
	};
public: // Constructors and destructor
	~CPopUpMenu();
	static CPopUpMenu* NewL(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu = EFalse);
	static CPopUpMenu* NewLC(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu = EFalse);

private:
	CPopUpMenu(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu = EFalse);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public:
	void	AppendItem(const TDesC& aDes,TInt aCommand);

	void	AppendSubMenu(CSubMenuItem* aSubMenuItem);		//在外部初始化CSubMenuItem实例,并将拥有权传进来

private:
	void	DrawBackground(CGraphic& aGraphic) const;
	void	DrawItem(CGraphic& aGraphic) const;

	void	DrawSubMenu(CGraphic& aGraphic) const;

	void	DrawArrow(CGraphic& gc,const TRect& aRect) const;

	void	Layout();

	void	SetPosition(const TPoint& aPoint);		//用于设置子菜单位置

	void	SetParentPopUpMenu(CPopUpMenu* aParent);

	TBool	InitSubMenu();

	TBool	ReleaseSubMenu();

	const TPopUpMenuItem&	CurItem() const;

	void	SetArrowIcon();

private:
	MPopUpMenuObserver&		iObserver;
	CMainEngine&			iMainEngine;
	CScrollBar*				iScrollBar;

	CPopUpMenu*				iParent;		//当前为子菜单时有效
	CPopUpMenu*				iSubMenu;

	TRect					iClientRect;

	TPoint					iPoint;

	TSize					iArrowIconSize;

	TInt					iVisibleLineNum;
	TInt					iScreenIndex;
	TInt					iLineHeight;

	RArray<TPopUpMenuItem>	iItemArray;

	RPointerArray<CSubMenuItem>	iSubMenuItemArray;

	RArray<TItemType>		iItemTypeArray;

	TBool					iIsSubMenu;
};

#endif // POPUPMENU_H
