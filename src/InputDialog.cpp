/*
============================================================================
 Name        : InputDialog.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 提示输入框
============================================================================
*/

#include "InputDialog.h"
#include "MainEngine.h"
#include "TypeDefine.h"

#include "eikon.hrh"

#include "MControlObserver.h"
#include "Graphic.h"
#include "ScreenLayout.h"
#include "cocobmp.h"
#include "EditorsManager.h"
#include "BitmapFactory.h"
CInputDialog::CInputDialog(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
:CControl(EInputDialog)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iCommandType(aCommandType)
,iBitmapFactory(aMainEngine.GetBitmapFactory())
{
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CInputDialog::~CInputDialog()
{
	if(iTitle)
	{
		delete iTitle;
		iTitle=NULL;
	}

	if(iLableText)
	{
		delete iLableText;
		iLableText=NULL;
	}

	if(iText)
	{
		delete iText;
		iText=NULL;
	}

	if(iInfo)
	{
		delete iInfo;
		iInfo=NULL;
	}

	if(iDialogBmp)
	{
		delete iDialogBmp;
		iDialogBmp=NULL;
	}

	iMainEngine.EditorsManager().RemoveEdwinFromArray(iEdwin);
}

CInputDialog* CInputDialog::NewLC(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
{
	CInputDialog* self = new (ELeave)CInputDialog(aObserver,aMainEngine,aCommandType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CInputDialog* CInputDialog::NewL(MInputObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
{
	CInputDialog* self=CInputDialog::NewLC(aObserver,aMainEngine,aCommandType);
	CleanupStack::Pop(); // self;
	return self;
}

void CInputDialog::ConstructL()
{
	iIsNull=EFalse;
	iType=1;
	InitBitmaps();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CInputDialog::Draw(CGraphic& gc)const
{
	gc.SetPenColor(TRgb(0,0,0));
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);

	gc.BitBlt(iStartPoint,iDialogBmp);

	//画标题
	gc.DrawText(iStartPoint+TPoint(iMargin,iScreenLayout.FontHeight()/3),*iTitle);

	iBitmapFactory.DrawInputIconBmpByID(gc,iStartPoint+iIconPoint,iMainEngine.GetInputType()-1);
	//gc.BitBltMasked(iStartPoint+iIconPoint,iInputTypeIcon,iInputTypeIconMask);
	//画提示信息

	gc.DrawTextWithBlueUnderline(iStartPoint+TPoint(iMargin,(iScreenLayout.FontHeight()+iScreenLayout.FontHeight()/2)*2+iScreenLayout.FontHeight()/3),*iInfo);
}

TBool CInputDialog::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
	case KOKKey:
		{
			TBuf<20> tBuf;
			this->iEdwin->GetText(tBuf);
			if(iIsNull)
			{
				MInputObserver& observer = iObserver;
				TInt commandType = iCommandType;
				CleanupSelf();
				observer.InputResponseEvent(commandType,tBuf);
			}
			else
			{
				if(tBuf.Length()==0)
				{
					this->SetInfo(iMainEngine.GetDesById(ETurkeyTextRes_InputNull));
				}
				else
				{
					MInputObserver& observer = iObserver;
					TInt commandType = iCommandType;
					CleanupSelf();
					observer.InputResponseEvent(commandType,tBuf);
				}
			}
		}
		break;

	case KRightSoftKey:
		{
			MInputObserver& observer = iObserver;
			CleanupSelf();
			observer.InputResponseEvent(-1,KNullDesC);
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return ETrue;
}

TBool CInputDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CInputDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}

const TDesC& CInputDialog::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CInputDialog::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//From CNotifyTimer
//////////////////////////////////////////////////////////////////////////
TBool CInputDialog::DoPeriodTask()
{
	/*++iTimer;
	if(iTimer == 3)
	{
		iTimer = 0;
	}	*/		
	return ETrue;
}

void CInputDialog::SetTitle(const TDesC& aTitle) 
{
	if(iTitle)
	{
		delete iTitle;
		iTitle=NULL;
	}

	iTitle=HBufC::NewL(aTitle.Length());
	iTitle->Des().Copy(aTitle);
}
void CInputDialog::SetInfo(const TDesC& aText)
{
	if(iInfo)
	{
		delete iInfo;
		iInfo=NULL;
	}
	iInfo=HBufC::NewL(aText.Length());
	iInfo->Des().Copy(aText);
}

void CInputDialog::SetEdwinText(const TDesC& aText)
{
	iEdwin->SetTextL(&aText);
	iEdwin->SetCursorPosL(aText.Length(),ETrue);
}

void CInputDialog::SetLableText(const TDesC& aText)
{
	if(iLableText)
	{
		delete iLableText;
		iLableText=NULL;
	}

	iLableText=HBufC::NewL(aText.Length());
	iLableText->Des().Copy(aText);

	this->SetInfo(aText);
}
void CInputDialog::SetIsNull(TBool aBool)
{
	iIsNull=aBool;
}
void CInputDialog::SetInputType(TInt aType)
{
	iType=aType;
	InitInput(aType);
}
void CInputDialog::InitBitmaps()
{
	iDialogBmp =iBitmapFactory.GetBitmap(EMbmCocobmpDialog);
	iDialogSize = iDialogBmp->SizeInPixels();
	
	iIconSize=iBitmapFactory.GetInputIconBmpSize(0);

	//上下左右居中
	TSize screenSize = CScreenLayout::ScreenSize();

	iStartPoint.iX = (screenSize.iWidth - iDialogSize.iWidth)/2;
	iStartPoint.iY = (screenSize.iHeight - iDialogSize.iHeight)/2;

	iIconPoint.iX = iDialogSize.iWidth-iIconSize.iWidth-iMargin;
	iIconPoint.iY = iScreenLayout.FontHeight()/3;

	iMargin = iScreenLayout.InnerMargin();

	iRect.SetRect(iStartPoint+TPoint(iMargin,iScreenLayout.FontHeight()/3*2+iScreenLayout.FontHeight()),TSize(iDialogSize.iWidth-2*iMargin,iScreenLayout.FontHeight()/2+iScreenLayout.FontHeight()));
}

void CInputDialog::InitInput(TInt aType)
{
	switch(aType)
	{
	case 1:
		{
			iEdwin=iMainEngine.EditorsManager().CreateEdwin(iRect,20,TRgb(0,0,0));
			break;
		}
	case 2:
		{
			iEdwin=iMainEngine.EditorsManager().CreateEdwin(iRect,20,TRgb(0,0,0));
			iEdwin->SetAknEditorInputMode(EAknEditorNumericInputMode);
			iEdwin->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
			break;
		}
	}

	if(iEdwin)
	{
		iMainEngine.EditorsManager().SetCurEdwin(iEdwin);
	}
}

void CInputDialog::SetInputVisible(TBool aBool)
{
	iEdwin->MakeVisible(aBool);
}