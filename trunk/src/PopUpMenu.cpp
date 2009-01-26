/*
============================================================================
 Name        : PopUpMenu.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPopUpMenu implementation
============================================================================
*/

#include "PopUpMenu.h"
#include "PopUpMenuObserver.h"
#include "ScrollBar.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "BitmapFactory.h"

const TInt KMaxLinePerPage = 6;

//////////////////////////////////////////////////////////////////////////
//CSubMenuItem
//////////////////////////////////////////////////////////////////////////
CSubMenuItem::CSubMenuItem(const TDesC& aText)
{
	ASSERT(aText.Length() < iText.MaxLength());
	iText.Zero();
	iText.Append(aText);
}

CSubMenuItem::~CSubMenuItem()
{
	iSubMenuItemArray.Close();
}

void CSubMenuItem::Append(const TDesC& aText,TInt aCommand)
{
	TPopUpMenuItem item;
	ASSERT(aText.Length() < iText.MaxLength());
	item.iText.Append(aText);
	item.iCommand = aCommand;
	iSubMenuItemArray.Append(item);
}
//////////////////////////////////////////////////////////////////////////
//CPopUpMenu
//////////////////////////////////////////////////////////////////////////
CPopUpMenu::CPopUpMenu(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu) : CControl(EPopUpMenu)
																								, iObserver(aObserver)
																								, iMainEngine(aMainEngine)
																								, iIsSubMenu(aIsSubMenu)
{
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}

CPopUpMenu::~CPopUpMenu()
{
	iItemArray.Close();
	iSubMenuItemArray.ResetAndDestroy();
	iSubMenuItemArray.Close();
	iItemTypeArray.Close();

	delete iScrollBar;
	delete iSubMenu;
}

CPopUpMenu* CPopUpMenu::NewLC(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu)
{
	CPopUpMenu* self = new (ELeave)CPopUpMenu(aObserver,aMainEngine,aIsSubMenu);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPopUpMenu* CPopUpMenu::NewL(MPopUpMenuObserver& aObserver,CMainEngine& aMainEngine,TBool aIsSubMenu)
{
	CPopUpMenu* self=CPopUpMenu::NewLC(aObserver,aMainEngine,aIsSubMenu);
	CleanupStack::Pop(); // self;
	return self;
}

void CPopUpMenu::ConstructL()
{
	iScrollBar = CScrollBar::CreateDefaultScrollbarL();
	iScrollBar->SetControlMode(CScrollBar::ECircleControl);
	iScrollBar->SetBackgroundColor(KScrollBackgroundColor);
	iScrollBar->SetCursorColor(KScrollSpaceColor);

	SetArrowIcon();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CPopUpMenu::Draw(CGraphic& aGraphic) const
{
	DrawBackground(aGraphic);
	DrawItem(aGraphic);
	if(iSubMenu)
	{
		iSubMenu->Draw(aGraphic);
	}
	iScrollBar->Draw(aGraphic);
}

TBool CPopUpMenu::KeyEventL(TInt aKeyCode)
{
	ASSERT(iScrollBar);

	TBool keyResult = EFalse;

	if(iSubMenu)
	{
		keyResult = iSubMenu->KeyEventL(aKeyCode);
	}
	else
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyUpArrow:
			if(iScreenIndex > 0)
			{
				iScreenIndex --;
			}
			else
			{
				if(!iScrollBar->Up())
				{
					iScrollBar->End();
					iScreenIndex = iVisibleLineNum - 1;
				}
			}
			break;

		case EKeyDownArrow:
			if(iScreenIndex < iVisibleLineNum - 1)
			{
				iScreenIndex ++;
			}
			else
			{
				if(!iScrollBar->Down())
				{
					iScrollBar->Home();
					iScreenIndex = 0;
				}
			}
			break;

		case EKeyLeftArrow:
			if(iParent)
			{
				iParent->ReleaseSubMenu();
			}
			break;

		case EKeyRightArrow:
			InitSubMenu();
			break;

		case EKeyDevice0:
		case EKeyDevice3:
			if(!InitSubMenu())			//当前项含有子菜单
			{
				TInt cmd = CurItem().iCommand;
				MPopUpMenuObserver& tmpObserver = iObserver;
				if(iParent)				//子菜单,清除父菜单
				{
					iParent->CleanupSelf();
				}
				else
				{
					CleanupSelf();
				}
				tmpObserver.PopUpDestried();
				tmpObserver.DoPopUpMenuCommand(cmd);
			}
			break;

		case EKeyDevice1:
			if(iParent)		//子菜单删除自己
			{
				iParent->ReleaseSubMenu();
			}
			else			//返回
			{
				MPopUpMenuObserver& tmpObserver = iObserver;
				CleanupSelf();
				tmpObserver.PopUpDestried();
			}
			break;

		default:
			keyResult = EFalse;
			break;
		}
	}
	return ETrue;
}

TBool CPopUpMenu::HandleCommandL(TInt aCommand)
{
	if(iSubMenu)
	{
		iSubMenu->HandleCommandL(aCommand);
	}
	return EFalse;
}

void CPopUpMenu::SizeChanged(const TRect& aScreenRect)
{
	if(iSubMenu)
	{
		iSubMenu->SizeChanged(aScreenRect);
	}
	Layout();
}

const TDesC& CPopUpMenu::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CPopUpMenu::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CPopUpMenu::AppendItem(const TDesC& aDes,TInt aCommand)
{
	TPopUpMenuItem item;
	TInt length = aDes.Length();
	ASSERT(length <= item.iText.MaxLength());
	item.iText.Append(aDes);
	item.iCommand = aCommand;
	iItemArray.Append(item);


	TItemType itemType;
	itemType.iType = EText;
	itemType.iNumber = iItemArray.Count() - 1;
	iItemTypeArray.Append(itemType);

	Layout();
}

void CPopUpMenu::AppendSubMenu(CSubMenuItem* aSubMenuItem)
{
	iSubMenuItemArray.Append(aSubMenuItem);
	TItemType itemType;
	itemType.iType = ESubMenu;
	itemType.iNumber = iSubMenuItemArray.Count() - 1;
	iItemTypeArray.Append(itemType);

	Layout();
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CPopUpMenu::DrawBackground(CGraphic& aGraphic) const
{
	aGraphic.SetBrushColor(KPopMenuBackgroundColor);
	aGraphic.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
	aGraphic.DrawRoundRect(iClientRect);
	aGraphic.SetBrushStyle(CGraphicsContext::ENullBrush);
}

void CPopUpMenu::DrawItem(CGraphic& aGraphic) const
{
	TRect itemRect(iClientRect.iTl,TSize(iClientRect.Width(),iLineHeight));
	itemRect.Shrink(1,1);
	TInt curIndex = iScreenIndex + iScrollBar->CurIndex();
	for(TInt i = 0 ; i < iVisibleLineNum ; i++)
	{
		TInt index = i + iScrollBar->CurIndex();
		ASSERT(index >= 0 && index < iItemTypeArray.Count());

		const TItemType& type = iItemTypeArray[index];

		TPtrC ptr;
		if(EText == type.iType)
		{
			ptr.Set(iItemArray[type.iNumber].iText);
		}
		else
		{
			const CSubMenuItem& subMenuItem = *iSubMenuItemArray[type.iNumber];
			ptr.Set(subMenuItem.iText);
		}

		//const TDesC& text = iItemArray[index].iText;

		if(curIndex == index)
		{
			aGraphic.SetPenStyle(CGraphicsContext::ENullPen);
			aGraphic.SetBrushColor(KPopMenuSelectedItemColor);
			aGraphic.SetBrushStyle(CGraphicsContext::ESolidBrush);

			aGraphic.DrawRoundRect(itemRect);
			aGraphic.SetBrushStyle(CGraphicsContext::ENullBrush);
			aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
		}

		aGraphic.SetPenColor(KRgbWhite);
		aGraphic.DrawText(ptr,itemRect,CGraphicsContext::ELeft);

		if(ESubMenu == type.iType)
		{
			DrawArrow(aGraphic,itemRect);
		}

		itemRect.Move(0,iLineHeight);
	}
}

void CPopUpMenu::DrawSubMenu(CGraphic& aGraphic) const
{
	if(iSubMenu)
	{
		iSubMenu->Draw(aGraphic);
	}
}

void CPopUpMenu::DrawArrow(CGraphic& gc,const TRect& aRect) const
{
	TPoint point = aRect.iTl;
	point.iX += aRect.Size().iWidth;
	point.iX -= iArrowIconSize.iWidth;
	point.iX -= iMainEngine.ScreenLayout().ScrollBarWidth();
	point.iY += (aRect.Height() - iArrowIconSize.iHeight)/2;

	iMainEngine.GetBitmapFactory().DrawArrowIconBmpByID(gc,point,EArrawRight);
}

void CPopUpMenu::Layout()
{
	iVisibleLineNum = iItemTypeArray.Count();
	iVisibleLineNum = iVisibleLineNum < KMaxLinePerPage ? iVisibleLineNum : KMaxLinePerPage;

	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
	TRect controlPane = screenLayout.GetControlPaneRect();

	TSize screenSize = iMainEngine.ScreenSize();
	iLineHeight = screenLayout.LineHeight();
	TSize itemSize;
	itemSize.iWidth = (screenSize.iWidth>>1);
	itemSize.iHeight = iLineHeight;
	TInt baseLine = controlPane.iTl.iY;


	TInt itemCount = iItemArray.Count();

	TInt clientHeight =0;
	clientHeight = itemSize.iHeight * iVisibleLineNum;

	//WriteLogsL(_L("itemCount = %d,iVisibleLineNum = %d"),itemCount,iVisibleLineNum);

	TPoint point;
	point.iX = 4;
	point.iY = baseLine;
	point.iY -= clientHeight;

	iClientRect.SetRect(point, TSize(itemSize.iWidth - 4,clientHeight));	

	if (iIsSubMenu)
	{
		TPoint point = iPoint;

		TSize screenSize = screenLayout.ScreenSize();
		TRect controlPane = screenLayout.GetControlPaneRect();
		
		TInt clientHeight = screenSize.iHeight - controlPane.Height();
		if(point.iY + iLineHeight*(iVisibleLineNum + 1) > clientHeight)
		{
			point.iY = clientHeight - iLineHeight*(iVisibleLineNum);
		}				

		iClientRect.Move(-iClientRect.iTl);
		iClientRect.Move(point);
	}

	delete iScrollBar;
	iScrollBar = NULL;
	TRect rect = iClientRect;
	rect.Shrink(1,1);
	iScrollBar = iMainEngine.ControlFactory().CreateScrollBar(rect);
	ASSERT(iScrollBar);

	iScrollBar->SetMaxLinePerPage(KMaxLinePerPage);
	iScrollBar->SetTotalLineNum(iItemTypeArray.Count());
	iScrollBar->Layout();
	iScrollBar->SetShow(iItemArray.Count() > KMaxLinePerPage);
}

void CPopUpMenu::SetPosition(const TPoint& aPoint)
{
	iPoint = aPoint;
}

void CPopUpMenu::SetParentPopUpMenu(CPopUpMenu* aParent)
{
	iParent = aParent;
}

TBool CPopUpMenu::InitSubMenu()		
{
	//当前项含有子菜单,弹出子菜单
	TInt index = iScreenIndex + iScrollBar->CurIndex();
	TItemType& type = iItemTypeArray[index];
	if(ESubMenu == type.iType)
	{
		iSubMenu = CPopUpMenu::NewL(iObserver,iMainEngine,ETrue);
		iSubMenu->SetParentPopUpMenu(this);
		const CSubMenuItem& subItem = *iSubMenuItemArray[type.iNumber];

		TPoint point = iClientRect.iTl;
		point.iX += iClientRect.Width();
		point.iY += iLineHeight*iScreenIndex;

		iSubMenu->SetPosition(point);

		for(TInt i = 0 ; i < subItem.iSubMenuItemArray.Count() ; i++)
		{
			const TPopUpMenuItem& item = subItem.iSubMenuItemArray[i];
			iSubMenu->AppendItem(item.iText,item.iCommand);
		}
		return ETrue;
	}

	return EFalse;
}

TBool CPopUpMenu::ReleaseSubMenu()
{
	if(iSubMenu)		//当前子菜单已弹出
	{
		delete iSubMenu;
		iSubMenu = NULL;
		return ETrue;
	}
	return EFalse;
}

const TPopUpMenuItem& CPopUpMenu::CurItem() const
{
	TInt index = iScreenIndex + iScrollBar->CurIndex();
	const TItemType& itemType = iItemTypeArray[index];
	ASSERT(EText == itemType.iType);
	ASSERT(itemType.iNumber >= 0 && itemType.iNumber < iItemArray.Count());
	return iItemArray[itemType.iNumber];
}

void CPopUpMenu::SetArrowIcon()
{
	iArrowIconSize=iMainEngine.GetBitmapFactory().GetArrowIconBmpSize(0);
}