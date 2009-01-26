/*
============================================================================
 Name        : CopyToDialog.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CCopyToDialog implementation
============================================================================
*/


#include "MainEngine.h"
#include "TypeDefine.h"

#include "MControlObserver.h"
#include "Graphic.h"
#include "ScreenLayout.h"
#include "cocobmp.h"
#include "CopyToDialog.h"
#include "BitmapFactory.h"

CCopyToDialog::CCopyToDialog(MCopyToObserver& aObserver,CMainEngine& aMainEngine)
:CControl(ECopyToDialog)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iTitle(NULL)
,iPath(NULL)
,iDesNameArray(NULL)
,iDesPathArray(NULL)
,iDialogBmp(NULL)
,iMaxLinePerPage(4)
,iSelectedIndex(0)
,iFirstIndex(0)
{
	// No implementation required
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CCopyToDialog::~CCopyToDialog()
{
	if(iTitle)
	{
		delete iTitle;
		iTitle=NULL;
	}

	if(iPath)
	{
		delete iPath;
		iPath=NULL;
	}

	if(iDesNameArray)
	{
		delete iDesNameArray;
		iDesNameArray=NULL;
	}

	if(iDesPathArray)
	{
		delete iDesPathArray;
		iDesPathArray=NULL;
	}

	if(iDialogBmp)
	{
		delete iDialogBmp;
		iDialogBmp=NULL;
	}

}

CCopyToDialog* CCopyToDialog::NewLC(MCopyToObserver& aObserver,CMainEngine& aMainEngine,const TDesC& aTitle)
{
	CCopyToDialog* self = new (ELeave)CCopyToDialog(aObserver,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	return self;
}

CCopyToDialog* CCopyToDialog::NewL(MCopyToObserver& aObserver,CMainEngine& aMainEngine,const TDesC& aTitle)
{
	CCopyToDialog* self=CCopyToDialog::NewLC(aObserver,aMainEngine,aTitle);
	CleanupStack::Pop(); // self;
	return self;
}

void CCopyToDialog::ConstructL(const TDesC& aTitle)
{
	iTitle=HBufC::NewL(aTitle.Length());
	iTitle->Des().Copy(aTitle);

	iDesNameArray = new (ELeave) CDesCArrayFlat(10);
	iDesPathArray = new (ELeave) CDesCArrayFlat(10);
	InitBitmaps();
}

//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CCopyToDialog::Draw(CGraphic& gc)const
{
	gc.SetPenColor(TRgb(KRgbBlack));
	gc.SetPenStyle(CGraphicsContext::ESolidPen);

	gc.BitBlt(iStartPoint,iDialogBmp);

	gc.DrawText(iStartPoint+TPoint(iScreenLayout.FontHeight()/3,iScreenLayout.FontHeight()/3),*iTitle);

	if(!iDesNameArray || iDesNameArray->Count() == 0)
	{
		return;
	}

	for (int i = 0;i< iMaxLinePerPage;i++)
	{
		TInt index=i+iFirstIndex;
		TPoint point=iStartPoint+TPoint(0,iItemSize.iHeight*i+iItemSize.iHeight+iScreenLayout.FontHeight()/3);
		DrawItem(gc,point,index);
	}
}

TBool CCopyToDialog::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		if(iSelectedIndex>0)
		{
			if(iFirstIndex>0&&iSelectedIndex==iFirstIndex)
			{
				iFirstIndex--;
			}
			iSelectedIndex--;
		}
		break;

	case KKeyCodeDown:
		if(iSelectedIndex<iDesNameArray->Count()-1)
		{
			if(iSelectedIndex==(iFirstIndex+iMaxLinePerPage-1))
			{
				iFirstIndex++;
			}
			iSelectedIndex++;
		}
		break;
	case KLeftSoftKey:
	case KOKKey:
		{
			TFileName filePath;
			filePath.Zero();
			filePath.Append((*iDesPathArray)[iSelectedIndex]);

			MCopyToObserver& observer = iObserver;
			CleanupSelf();
			observer.CopyToEvent(0,filePath);
		}
		break;

	case KRightSoftKey:
		{
			MCopyToObserver& observer = iObserver;
			CleanupSelf();
			observer.CopyToEvent(1,_L(""));
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return ETrue;
}

TBool CCopyToDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CCopyToDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}

const TDesC& CCopyToDialog::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CCopyToDialog::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CCopyToDialog::InitBitmaps()
{
	iDialogBmp = iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpSaveas);

	iDialogSize = iDialogBmp->SizeInPixels();

	iItemSize.iWidth = iDialogSize.iWidth ;
	iItemSize.iHeight =iScreenLayout.FontHeight()+iScreenLayout.FontHeight()/2;

	iMaxLinePerPage=iDialogSize.iHeight/iItemSize.iHeight-1;

	//上下左右居中
	TSize screenSize = CScreenLayout::ScreenSize();

	iStartPoint.iX = (screenSize.iWidth - iDialogSize.iWidth)/2;
	iStartPoint.iY = (screenSize.iHeight - iDialogSize.iHeight)/2;
}

void CCopyToDialog::AppendItem(const TDesC& aName,const TDesC& aPath)
{
	iDesNameArray->AppendL(aName);
	iDesPathArray->AppendL(aPath);
}

void CCopyToDialog::DrawItem(CGraphic& gc,TPoint& aPoint,TInt aIndex) const
{
	if(iSelectedIndex == aIndex)		//若为当前项，不显示文字，用CSlideText画出
	{

		TRect rect(aPoint, iItemSize);
		gc.SetPenColor(TRgb(111,127,54));
		gc.SetPenStyle(CGraphicsContext::ESolidPen);
		gc.SetBrushColor(TRgb(154,192,47));
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.DrawRect(rect);
	}

	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.SetPenColor(KBlackRgb);	

	gc.DrawText(aPoint,(*iDesNameArray)[aIndex]);

}