/*
============================================================================
 Name        : SelectBox.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectBox implementation
============================================================================
*/
#include <coemain.h>
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif

#include "SelectBox.h"

#include "TypeDefine.h"
#include "cocobmp.h"
#include "Graphic.h"
#include "ScreenLayout.h"
#include "MainEngine.h"
#include "BitmapFactory.h"
#include "CoCoPreDefine.h"

CSelectBox::CSelectBox(CMainEngine& aMainEngine,TInt aType)
:CControl(ESelectBox)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iBitmapFactory(aMainEngine.GetBitmapFactory())
,iText(NULL)
,iName(NULL)
,iType(aType)
{
	// No implementation required
	
}


CSelectBox::~CSelectBox()
{
	if(iText)
	{
		delete iText;
		iText=NULL;
	}

	if(iName)
	{
		delete iName;
		iName=NULL;
	}
	
}

CSelectBox* CSelectBox::NewLC(CMainEngine& aMainEngine,TInt aType)
{
	CSelectBox* self = new (ELeave)CSelectBox(aMainEngine,aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSelectBox* CSelectBox::NewL(CMainEngine& aMainEngine,TInt aType)
{
	CSelectBox* self=CSelectBox::NewLC(aMainEngine,aType);
	CleanupStack::Pop(); // self;
	return self;
}

void CSelectBox::ConstructL()
{
	if(iType==ESelectRedio||iType==ESelectCheckBox)
	{
		InitBitmap();
	}

	iName=HBufC::NewL(0);
}

void CSelectBox::InitBitmap()
{
	iSelectIconSize = iBitmapFactory.GetOperationIconBmpSize(EOperationCheckBox);
}
void CSelectBox::DrawItem(CGraphic& gc,TRect aRect)const
{
	gc.SetPenColor(KTextColor);
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);

	TPoint point=aRect.iTl;
	TSize  size=aRect.Size();

	point.iX=point.iX+iSelectIconSize.iWidth+2;
	size.iWidth=size.iWidth-iSelectIconSize.iWidth-2;

	TPoint iconPoint=aRect.iTl;
	iconPoint.iY=iconPoint.iY+(size.iHeight-iSelectIconSize.iHeight)/2;

	switch(iType)
	{
		case ESelectText:
			break;
		case ESelectRedio:
			DrawSelectIcon(gc,iconPoint,EOperationRadio);
			break;
		case ESelectCheckBox:
			DrawSelectIcon(gc,iconPoint,EOperationCheckBox);
			break;
	}
	gc.DrawText(*iText,TRect(point,size));
}
void CSelectBox::DrawSelectIcon(CGraphic& gc,TPoint aPoint,TInt aID) const
{
	if(iBool)
	{
		iBitmapFactory.DrawOperationIconBmpByID(gc,aPoint,aID+1);
	}
	else
	{
		iBitmapFactory.DrawOperationIconBmpByID(gc,aPoint,aID);
	}
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CSelectBox::Draw(CGraphic& gc)const
{
	
}

TBool CSelectBox::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KOKKey:
		{
			iBool=!iBool;
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CSelectBox::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CSelectBox::SizeChanged(const TRect& /*aScreenRect*/)
{

}
void CSelectBox::SetSelected(TBool aBool)
{
	iBool=aBool;
}
void CSelectBox::SetText(const TDesC& aText)
{
	if(iText)
	{
		delete iText;
		iText=NULL;
	}

	iText=HBufC::NewL(aText.Length());
	iText->Des().Copy(aText);
}
void CSelectBox::SetGroupName(const TDesC& aName)
{
	if(iName)
	{
		delete iName;
		iName=NULL;
	}

	iName=HBufC::NewL(aName.Length());
	iName->Des().Copy(aName);
}
const TDesC& CSelectBox::GetGroupName()
{
	return *iName;
}