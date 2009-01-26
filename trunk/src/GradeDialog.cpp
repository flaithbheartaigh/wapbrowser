/*
============================================================================
 Name        : GradeDialog.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CGradeDialog implementation
============================================================================
*/


#include "BitmapFactory.h"

#include "MainEngine.h"
#include "TypeDefine.h"

#include "MControlObserver.h"
#include "Graphic.h"
#include "ScreenLayout.h"
#include "cocobmp.h"

#include "GradeDialog.h"

CGradeDialog::CGradeDialog(MGradeObserver& aObserver,CMainEngine& aMainEngine)
:CControl(EGradeDialog)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iBitmapFactory(aMainEngine.GetBitmapFactory())
{
	// No implementation required
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CGradeDialog::~CGradeDialog()
{
	if(iDialogBmp)
	{
		delete iDialogBmp;
		iDialogBmp=NULL;
	}
}

CGradeDialog* CGradeDialog::NewLC(MGradeObserver& aObserver,CMainEngine& aMainEngine)
{
	CGradeDialog* self = new (ELeave)CGradeDialog( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CGradeDialog* CGradeDialog::NewL(MGradeObserver& aObserver,CMainEngine& aMainEngine)
{
	CGradeDialog* self=CGradeDialog::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CGradeDialog::ConstructL()
{
	iGradeNum=5;
	InitBitmaps();
}

//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CGradeDialog::Draw(CGraphic& gc)const
{
	gc.SetPenColor(TRgb(0,0,0));
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ENullBrush);

	gc.BitBlt(iStartPoint,iDialogBmp);

	gc.DrawText(iStartPoint+TPoint(iIconPoint.iX,iScreenLayout.FontHeight()/3),iTitle);

	for(TInt i=0;i<5;i++)
	{
		if(i<iGradeNum/2)
		{
			iBitmapFactory.DrawGradeBmpByID(gc,iStartPoint+iIconPoint+TPoint((iIconSize.iWidth+iIconPoint.iX)*i,0),0);	
		}
		else
		{
			if((iGradeNum-i*2)==1)
			{
				iBitmapFactory.DrawGradeBmpByID(gc,iStartPoint+iIconPoint+TPoint((iIconSize.iWidth+iIconPoint.iX)*i,0),1);	
			}
			else
			{
				iBitmapFactory.DrawGradeBmpByID(gc,iStartPoint+iIconPoint+TPoint((iIconSize.iWidth+iIconPoint.iX)*i,0),2);	
			}
		}
	}

}

TBool CGradeDialog::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeLeft:
		if(iGradeNum>0)
			iGradeNum--;
		break;
	case KKeyCodeRight:
		if(iGradeNum<10)
			iGradeNum++;
		break;
	case KLeftSoftKey:
	case KOKKey:
		{
			MGradeObserver& observer = iObserver;
			TInt aGradeNum=iGradeNum;
			CleanupSelf();
			observer.GradeEvent(0,aGradeNum);

		}
		break;
	case KRightSoftKey:
		{
			MGradeObserver& observer = iObserver;
			CleanupSelf();
			observer.GradeEvent(1,0);
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return ETrue;
}

TBool CGradeDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CGradeDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}

const TDesC& CGradeDialog::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CGradeDialog::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CGradeDialog::SetTitle(const TDesC& aTitle)
{
	iTitle.Zero();
	iTitle.Append(aTitle);
}

void CGradeDialog::SetGradeNum(TInt aNum)
{
	iGradeNum=aNum;
}

void CGradeDialog::InitBitmaps()
{
	iDialogBmp = iBitmapFactory.GetBitmap(EMbmCocobmpDialog);

	iDialogSize = iDialogBmp->SizeInPixels();

	iIconSize = iBitmapFactory.GetGradeBmpSize(0);

	//…œœ¬◊Û”“æ”÷–
	TSize screenSize = CScreenLayout::ScreenSize();

	iStartPoint.iX = (screenSize.iWidth - iDialogSize.iWidth)/2;
	iStartPoint.iY = (screenSize.iHeight - iDialogSize.iHeight)/2;

	iIconPoint.iX = (iDialogSize.iWidth-iIconSize.iWidth*5)/6;
	iIconPoint.iY = iDialogSize.iHeight-iScreenLayout.FontHeight()-(iScreenLayout.FontHeight()/3)*2;
}