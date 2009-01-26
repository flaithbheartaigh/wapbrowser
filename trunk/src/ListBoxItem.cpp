/*
============================================================================
 Name        : ListBoxItem.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CListBoxItem implementation
============================================================================
*/

#include "ListBoxItem.h"
#include "ListBox.h"
#include "Graphic.h"
#include "SlideText.h"
#include "PreDefine.h"
#include "CoCoPreDefine.h"

//////////////////////////////////////////////////////////////////////////
//CListBoxItem
//////////////////////////////////////////////////////////////////////////
CListBoxItem::CListBoxItem() : iParent(NULL)
							 , iActive(EFalse)
{

}

CListBoxItem::~CListBoxItem()
{

}

void CListBoxItem::SetParent(CListBox* aParent)
{
	ASSERT(aParent);
	iParent = aParent;
}

TInt CListBoxItem::Active() const
{
	return iActive;
}

void CListBoxItem::SetActive(TBool aActive)
{
	iActive = aActive;
}

void CListBoxItem::RequestDraw()
{
	iParent->RequestDraw();
}
//////////////////////////////////////////////////////////////////////////
//CNormalListBoxItem
//////////////////////////////////////////////////////////////////////////
CNormalListBoxItem::CNormalListBoxItem(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask) : iIcon(aIcon)
, iIconMask(aIconMask)
{
}

CNormalListBoxItem::~CNormalListBoxItem()
{
	delete iSlideText;
	delete iText;
}

CNormalListBoxItem* CNormalListBoxItem::NewL(const TDesC& aDes,
											 const CFbsBitmap* aIcon,
											 const CFbsBitmap* aIconMask,
											 TBool aLimitText)
{
	CNormalListBoxItem* self = new (ELeave) CNormalListBoxItem(aIcon,aIconMask);
	CleanupStack::PushL(self);
	self->ConstructL(aDes,aLimitText);
	CleanupStack::Pop();
	return self;
}

void CNormalListBoxItem::ConstructL(const TDesC& aDes,TBool aLimitText)
{
	iLimitText = aLimitText;
	iText = aDes.AllocL();
}
//////////////////////////////////////////////////////////////////////////
//From CListBoxItem
//////////////////////////////////////////////////////////////////////////
void CNormalListBoxItem::Init(const TSize& aLineSize,const CFont* aFont)
{
	ASSERT(iParent);
	ASSERT(iText);

	iItemSize = aLineSize;
	TRect rect = aLineSize;

	iSlideText = CSlideText::NewL(*iParent,CSlideText::ELeftWithPause);
	iSlideText->SetFont(aFont);
	iSlideText->SetTextColor(KListTextColor);
	iSlideText->SetMoveSpeed(5);
	iSlideText->SetStopTime(20);
	iSlideText->Layout(ECenterAlign);
	iSlideText->SetClientRect(rect);
	iSlideText->SetText(*iText);
	delete iText;
	iText = NULL;
}

void CNormalListBoxItem::Draw(CGraphic& aGraphic,TPoint& aPoint) const
{
	UtilityTools::WriteLogsL(_L("CNormalListBoxItem::Draw"));
	ASSERT(iSlideText);
	TRect rect(iItemSize);
	rect.Move(aPoint);
	if(iActive)
	{
		aGraphic.SetPenColor(KTextColor);
		aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
		aGraphic.SetBrushColor(KListSelectedColor);
		aGraphic.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGraphic.DrawRect(rect);
		aGraphic.SetBrushStyle(CGraphicsContext::ENullBrush);
	}
	if(iIcon)
	{
		rect.iTl.iX += iIcon->SizeInPixels().iWidth;
	}
	if(iIcon)
	{
		ASSERT(iIconMask);
		aGraphic.BitBltMasked(aPoint,iIcon,iIconMask);
	}
	iSlideText->SetClientRect(rect);
	iSlideText->Draw(aGraphic);

	aPoint.iY += iItemSize.iHeight;
	UtilityTools::WriteLogsL(_L("CNormalListBoxItem::Draw End"));
}

void CNormalListBoxItem::SetActive(TBool aActive)
{
	ASSERT(iSlideText);
	iActive = aActive;
	if(iLimitText)
	{
		iSlideText->Stop();
		return;
	}
	if(aActive)
	{
		iSlideText->Begin();
	}
	else
	{
		iSlideText->Stop();
	}
}

void CNormalListBoxItem::SetTextColor(const TRgb& aColor)
{
	iSlideText->SetTextColor(aColor);
}

TInt CNormalListBoxItem::LineNum() const
{
	return 1;
}

TInt CNormalListBoxItem::LineNumWhenFocus() const
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CNormalListBoxItem::ChangeIcon(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask)
{
	iIcon = aIcon;
	iIconMask = aIconMask;
	RequestDraw();
}

void CNormalListBoxItem::ChangeText(const TDesC& aText)
{
	UtilityTools::WriteLogsL(_L("CNormalListBoxItem::ChangeText"));
	UtilityTools::WriteLogsL(aText);
	ASSERT(iSlideText);
	iSlideText->SetText(aText);
	RequestDraw();
}

const TDesC& CNormalListBoxItem::GetText() const
{
	ASSERT(iSlideText);
	return iSlideText->GetText();
}
//////////////////////////////////////////////////////////////////////////
//CNormalListBoxItem
//////////////////////////////////////////////////////////////////////////
CCurThreeListBoxItem::CCurThreeListBoxItem(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask) : iIcon(aIcon)
																								, iIconMask(aIconMask)
{
}

CCurThreeListBoxItem::~CCurThreeListBoxItem()
{
	delete iSlideText;
	delete iText;
	delete iSecondText;
	delete iThirdText;
}

CCurThreeListBoxItem* CCurThreeListBoxItem::NewL(const TDesC& aDes,
												 const TDesC& aSecondLine,
												 const TDesC& aThirdLine,
												 const CFbsBitmap* aIcon,
												 const CFbsBitmap* aIconMask)
{
	CCurThreeListBoxItem* self = new (ELeave) CCurThreeListBoxItem(aIcon,aIconMask);
	CleanupStack::PushL(self);
	self->ConstructL(aDes,aSecondLine,aThirdLine);
	CleanupStack::Pop();
	return self;
}

void CCurThreeListBoxItem::ConstructL(const TDesC& aDes,const TDesC& aSecondLine,const TDesC& aThirdLine)
{
	iText = aDes.AllocL();
	iSecondText = aSecondLine.AllocL();
	iThirdText = aThirdLine.AllocL();
}
//////////////////////////////////////////////////////////////////////////
//From CListBoxItem
//////////////////////////////////////////////////////////////////////////
void CCurThreeListBoxItem::Init(const TSize& aLineSize,const CFont* aFont)
{
	ASSERT(iParent);
	ASSERT(iText);

	iItemSize = aLineSize;
	TRect rect = aLineSize;

	iSlideText = CSlideText::NewL(*iParent,CSlideText::ELeftWithPause);
	iSlideText->SetFont(aFont);
	iSlideText->SetTextColor(KListTextColor);
	iSlideText->SetMoveSpeed(5);
	iSlideText->SetStopTime(20);
	iSlideText->Layout(ECenterAlign);
	iSlideText->SetClientRect(rect);
	iSlideText->SetText(*iText);
	delete iText;
	iText = NULL;
}

void CCurThreeListBoxItem::Draw(CGraphic& aGraphic,TPoint& aPoint) const
{
	TPoint point = aPoint;

	ASSERT(iSlideText);
	TRect rect;
	if(iActive)
	{
		TSize size = iItemSize;
		size.iHeight *= 3;
		rect = size;
		rect.Move(point);

		aGraphic.SetPenColor(KTextColor);
		aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
		aGraphic.SetBrushColor(KListSelectedColor);
		aGraphic.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGraphic.DrawRect(rect);
		aGraphic.SetBrushStyle(CGraphicsContext::ENullBrush);
	}
	if(iIcon)
	{
		ASSERT(iIconMask);
		aGraphic.BitBltMasked(point,iIcon,iIconMask);
	}
	rect = iItemSize;
	if(iIcon)
	{
		rect.iTl.iX += iIcon->SizeInPixels().iWidth;
	}
	rect.Move(point);
	iSlideText->SetClientRect(rect);
	iSlideText->Draw(aGraphic);

	if(iActive)
	{	
		aGraphic.SetPenColor(iTextColor);
		rect.Move(0,iItemSize.iHeight);
		if(iSecondText)
		{
			aGraphic.DrawText(*iSecondText,rect);
		}
		rect.Move(0,iItemSize.iHeight);
		if(iThirdText)
		{
			aGraphic.DrawText(*iThirdText,rect);
		}

		aPoint.iY += iItemSize.iHeight*3;
	}
	else
	{
		aPoint.iY += iItemSize.iHeight;
	}
}

void CCurThreeListBoxItem::SetActive(TBool aActive)
{
	ASSERT(iSlideText);
	iActive = aActive;
	if(aActive)
	{
		iSlideText->Begin();
	}
	else
	{
		iSlideText->Stop();
	}
}

void CCurThreeListBoxItem::SetTextColor(const TRgb& aColor)
{
	iTextColor = aColor;
	iSlideText->SetTextColor(aColor);
}

TInt CCurThreeListBoxItem::LineNum() const
{
	return 1;
}

TInt CCurThreeListBoxItem::LineNumWhenFocus() const
{
	return 3;
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CCurThreeListBoxItem::ChangeIcon(const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask)
{
	iIcon = aIcon;
	iIconMask = aIconMask;
	RequestDraw();
}

void CCurThreeListBoxItem::ChangeText(const TDesC& aText,const TDesC& aSecondLine,const TDesC& aThirdLine)
{
	ASSERT(iSlideText);
	iSlideText->SetText(aText);

	delete iSecondText;
	iSecondText = NULL;
	iSecondText = aSecondLine.AllocL();

	delete iThirdText;
	iThirdText = NULL;
	iThirdText = aThirdLine.AllocL();
	RequestDraw();
}

const TDesC& CCurThreeListBoxItem::GetText() const
{
	ASSERT(iSlideText);
	return iSlideText->GetText();
}