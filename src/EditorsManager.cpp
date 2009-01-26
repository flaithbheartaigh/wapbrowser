/*
============================================================================
 Name        : EditorsManager.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CEditorsManager implementation
============================================================================
*/

#include <eikedwin.h>
#include <eikgted.h>
#include <aknnumedwin.h>
#include <barsread.h>  // for resource reader

#include <coedef.h>
#include <w32std.h>

#include "EditorsManager.h"
#include "PreDefine.h"

CEditorsManager::CEditorsManager(const CCoeControl* aParentControl) : iParentControl(aParentControl)
{
	// No implementation required
}


CEditorsManager::~CEditorsManager()
{
}

CEditorsManager* CEditorsManager::NewL(const CCoeControl* aParentControl)
{
	CEditorsManager* self = new (ELeave)CEditorsManager(aParentControl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CEditorsManager::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
TKeyResponse CEditorsManager::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
//////////////////////////////////////////////////////////////////////////
//每次只能有一个输入框被处理
//////////////////////////////////////////////////////////////////////////
	TKeyResponse keyResponse = EKeyWasNotConsumed;
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		CEikEdwin* edwin = iEdwinArray[i];
		//卞涛修改对有焦点的输入框输入
		if(edwin->IsFocused())
		{
			keyResponse = edwin->OfferKeyEventL(aKeyEvent,aType);
			if(EKeyWasConsumed == keyResponse)
			{
				break;
			}
		}
	}
	return keyResponse;
}

TInt CEditorsManager::CountComponentControls() const
{
	return iEdwinArray.Count(); // return nbr of controls inside this container
}

// ---------------------------------------------------------
// CsoarskyContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CEditorsManager::ComponentControl(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iEdwinArray.Count());
	CCoeControl* control = iEdwinArray[aIndex];
	ASSERT(control);
	return control;
}

CEikEdwin* CEditorsManager::CreateEdwin(const TRect& aRect,TInt aTextLimit,TRgb aColor)
{
	CEikEdwin* edwin = new (ELeave) CEikEdwin(TGulBorder::ESingleGray);
	edwin->SetContainerWindowL(*iParentControl);
	edwin->ConstructL(0,0,0,1);
	edwin->SetTextLimit(aTextLimit);
	//edwin->SetHeightForNumOfLinesL();
	edwin->SetRect(aRect);
	edwin->SetFocus(EFalse);
	// 	UtilityTools::SetEdwinTextColor(edwin,aColor);
	// 	UtilityTools::SetEdwinTextFont(edwin,KFont);
	SetupEdwinText(edwin,aColor,KFont);
	AddEdwinToArray(edwin);
	return edwin;
}

CEikGlobalTextEditor* CEditorsManager::CreateGlbEdwin(const TRect& aRect,TInt aTextLimit,TRgb aColor)
{
	CEikGlobalTextEditor* gTextEd = new (ELeave) CEikGlobalTextEditor;
	gTextEd->ConstructL(iParentControl,5,aTextLimit,0,0,0);
	gTextEd->SetRect(aRect);
	gTextEd->SetAvkonWrap(ETrue);
	gTextEd->SetFocus(EFalse);
	// 	UtilityTools::SetEdwinTextColor(gTextEd,aColor);
	// 	UtilityTools::SetEdwinTextFont(gTextEd,KFont);
	SetupEdwinText(gTextEd,aColor,KFont);
	AddEdwinToArray(gTextEd);
	return gTextEd;
}

CAknIntegerEdwin* CEditorsManager::CreateIntergerEdwin(const TRect& aRect,TInt aMaxNum,TInt aMaxLength/*,TInt aMinNum,TInt aInitNum*/,TRgb aColor)
{
	CAknIntegerEdwin* editor = new (ELeave) CAknIntegerEdwin;
	editor->ConstructL(0,aMaxNum,aMaxLength);
	editor->SetContainerWindowL(*iParentControl);

	editor->SetRect(aRect);
	editor->SetFocus(EFalse);
	// 	UtilityTools::SetEdwinTextColor(editor,aColor);
	// 	UtilityTools::SetEdwinTextFont(editor,KFont);
	SetupEdwinText(editor,aColor,KFont);
	AddEdwinToArray(editor);
	return editor;
}

void CEditorsManager::AddEdwinToArray(CEikEdwin* aEdwin)
{
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		if(aEdwin == iEdwinArray[i])
		{
			return;
		}
	}
	iEdwinArray.Append(aEdwin);
}

void CEditorsManager::RemoveEdwinFromArray(CEikEdwin* aEdwin)
{
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		if(aEdwin == iEdwinArray[i])
		{
			if(aEdwin == CurEdwin())
			{
				SetCurEdwin(NULL);
			}
			iEdwinArray.Remove(i);
			delete aEdwin;
			break;
		}
	}
}

CEikEdwin* CEditorsManager::CurEdwin()const
{
	return iCurEdwin;
}

void CEditorsManager::SetCurEdwin(CEikEdwin* aEdwin)
{
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		if(CurEdwin() == iEdwinArray[i])
		{
			CurEdwin()->SetFocus(EFalse);
			break;
		}
	}
	iCurEdwin = aEdwin;
	if(iCurEdwin)
	{
		iCurEdwin->SetFocus(ETrue);
	}
}

void CEditorsManager::ShowAllEdwins()
{
	ASSERT(iCurEdwin == NULL);
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		iEdwinArray[i]->MakeVisible(ETrue);
	}
	SetCurEdwin(iTmpEdwin);
}

void CEditorsManager::HideAllEdwins()
{
	//ASSERT(iCurEdwin);
//	::WriteLogsL(_L("HideAllEdwins iCurEdwin = %d"),iCurEdwin);
	for (TInt i = 0 ; i < iEdwinArray.Count() ; i++)
	{
		iEdwinArray[i]->MakeVisible(EFalse);
	}
	iTmpEdwin = iCurEdwin;
	// 	if(iCurEdwin)
	// 	{
	// 		::WriteLogsL(_L("HideAllEdwins 2"));
	// 	}
	// 	ASSERT(iTmpEdwin);
	SetCurEdwin(NULL);
//	::WriteLogsL(_L("HideAllEdwins 3"));
}



void CEditorsManager::SetEdwinTextColor(CEikEdwin* aEdwin,const TRgb& aColor)
{
	CCharFormatLayer* FormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor=aColor;
	charFormatMask.SetAttrib(EAttColor);
	FormatLayer->SetL(charFormat, charFormatMask);
	aEdwin->SetCharFormatLayer(FormatLayer);  // Edwin takes the ownership
}

void CEditorsManager::SetEdwinTextFont(CEikEdwin* aEdwin,const CFont* aFont)
{
	CCharFormatLayer* FormatLayer=CEikonEnv::NewDefaultCharFormatLayerL(); 
	TCharFormat charFormat; 
	TCharFormatMask charFormatMask; 
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight); 
	charFormat.iFontSpec = aFont->FontSpecInTwips();
	FormatLayer->SetL(charFormat, charFormatMask); 
	aEdwin->SetCharFormatLayer(FormatLayer);
}

void CEditorsManager::SetupEdwinText(CEikEdwin* aEdwin,const TRgb& aColor,const CFont* aFont)
{
	CCharFormatLayer* FormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor=aColor;
	charFormatMask.SetAttrib(EAttColor);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight); 
	charFormat.iFontSpec = aFont->FontSpecInTwips();
	FormatLayer->SetL(charFormat, charFormatMask);
	aEdwin->SetCharFormatLayer(FormatLayer);  // Edwin takes the ownership

}