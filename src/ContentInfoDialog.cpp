/*
============================================================================
 Name        : ContentInfoDialog.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CContentInfoDialog implementation
============================================================================
*/

//#include <coemain.h>
//#include <akniconutils.h>
#include "Graphic.h"
#include "TypeDefine.h"
#include "MainEngine.h"
#include "MControlObserver.h"
#include "ScreenLayout.h"
#include "CoCoPreDefine.h"
#include "ContentInfoDialog.h"
#include "ImageManipAdapter.h" 
#include <eikenv.h>
#include "ScrollBar.h"

CContentInfoDialog::CContentInfoDialog(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
:CControl(EContentInfoDialog)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iCommandType(aCommandType)
,iShowBmp(NULL)
,iTitle(NULL)
,iName(NULL)
,iSize(NULL)
,iType(NULL)
,iRemark(NULL)
,iBmpPath(NULL)
{
	// No implementation required
	SetCleanUpObserver(&aMainEngine);
	SetDrawEventObserver(&aMainEngine);
	iHasButton = ETrue;
}


CContentInfoDialog::~CContentInfoDialog()
{
	delete iScrollbar;
	iScrollbar = NULL;

	DELETE(iDesArray);
	DELETE(iName);
	DELETE(iSize);
	DELETE(iType);
	DELETE(iRemark);
	DELETE(iTitle);
	DELETE(iAdapter);
	DELETE(iBmpPath);

	//iAdapter->CancelDecode();
	//iAdapter->ResetImageL();
}

CContentInfoDialog* CContentInfoDialog::NewLC(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
{
	CContentInfoDialog* self = new (ELeave)CContentInfoDialog(aObserver,aMainEngine,aCommandType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CContentInfoDialog* CContentInfoDialog::NewL(MContentInfoObserver& aObserver,CMainEngine& aMainEngine,TInt aCommandType)
{
	CContentInfoDialog* self=CContentInfoDialog::NewLC(aObserver,aMainEngine,aCommandType);
	CleanupStack::Pop(); // self;
	return self;
}

void CContentInfoDialog::ConstructL()
{
	iBool=EFalse;
	iDesArray = new (ELeave) CDesCArrayFlat(10);
	this->InitBitmap();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CContentInfoDialog::Draw(CGraphic& gc)const
{
	gc.SetPenColor(TRgb(255,255,255));
	gc.SetBrushColor(TRgb(64,61,90));
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ESolidBrush);

	gc.DrawRoundRect(iBackRect);

	TPoint point=iStartPoint;

	if(iBool&&iShowBmp==NULL)
	{
		gc.DrawRoundRect(TRect(iStartPoint-TPoint(2,2),TSize(iIconSize.iWidth+4,iIconSize.iHeight+4)));
		gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_GetImage),TRect(point,iIconSize),CGraphicsContext::ELeft);
		point.iY=point.iY+iIconSize.iHeight;
	}

	if(iShowBmp)
	{
		/*TPoint iconPoint(0,0);
		TSize iconSize=iIconSize;

		if(iSelectIndex<iAllLine)
		{
			iconPoint.iY=iconPoint.iY+iTextSize.iHeight*iSelectIndex;
			
			iconSize.iHeight=iIconSize.iHeight-iTextSize.iHeight*iSelectIndex;

			if(iconPoint.iY>=0&&iconSize.iHeight>0)
				gc.BitBlt(point,iShowBmp,TRect(iconPoint,iconSize));
		}*/
		gc.BitBlt(point,iShowBmp);
		point.iY=point.iY+iIconSize.iHeight;
	}

	TInt index=0;
	for(TInt i=0;i<iMaxLine;i++)
	{
		index=i+iFirstIndex;

		if(index>=iDesArray->Count())
			break;

		gc.DrawText((*iDesArray)[index],TRect(point+TPoint(0,i*iTextSize.iHeight),iTextSize),CGraphicsContext::ELeft);
	}	
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);		
	
	if(iScrollbar)
		iScrollbar->Draw(gc);
}

TBool CContentInfoDialog::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		{
			/*if(iSelectIndex>0)
			{
				iSelectIndex--;*/
				if(iFirstIndex>0)
				{
					iFirstIndex--;

					if(iScrollbar)
						iScrollbar->Up();
				}
			//}
		}
		break;
	case KKeyCodeDown:
		{
			/*if(iMaxLine<(iAllLine+iDesArray->Count()-iSelectIndex))
			{
				iSelectIndex++;*/
				if(iMaxLine<(iDesArray->Count()-iFirstIndex))
				{
					iFirstIndex++;

					if(iScrollbar)
						iScrollbar->Down();
				}
			//}
		}
		break;
	case KLeftSoftKey:
	case KOKKey:
		{
			MContentInfoObserver& aObserver=iObserver;
			TInt command=iCommandType;
			CleanupSelf();
			aObserver.ContentInfoEvent(command);
		}
		break;
	case KRightSoftKey:
		{
			MContentInfoObserver& aObserver=iObserver;
			CleanupSelf();
			aObserver.ContentInfoEvent(-1);
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return ETrue;
}

TBool CContentInfoDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CContentInfoDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}
const TDesC& CContentInfoDialog::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_MainDown);
}

const TDesC& CContentInfoDialog::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CContentInfoDialog::SetTilte(const TDesC& aTitle)
{
	DELETE(iTitle);
	iTitle=aTitle.Alloc();
	this->AddTextItem(aTitle);
}
void CContentInfoDialog::SetName(const TDesC& aName)
{
	DELETE(iName);
	iName=aName.Alloc();
	this->AddTextItem(aName);
}
void CContentInfoDialog::SetSize(const TDesC& aSize)
{
	DELETE(iSize);
	iSize=aSize.Alloc();
	this->AddTextItem(aSize);
}
void CContentInfoDialog::SetType(const TDesC& aType)
{
	DELETE(iType);
	iType=aType.Alloc();
	this->AddTextItem(aType);
}
void CContentInfoDialog::SetRemark(const TDesC& aRemark)
{
	DELETE(iRemark);
	iRemark=aRemark.Alloc();
	this->AddTextItem(aRemark);
}
void CContentInfoDialog::InitBitmap()
{
	iBackRect=iMainEngine.ScreenLayout().GetClientRect();

	iTextHeight=iMainEngine.ScreenLayout().FontHeight();
	iMargin=iTextHeight/2;

	if(iShowBmp)
	{
		iIconSize=iShowBmp->SizeInPixels();
		if(iIconSize.iHeight>100||iIconSize.iWidth>100)
		{

			#ifdef __SERIES60_3X__
				AknIconUtils::SetSize(iShowBmp,TSize(100,100));
			#endif

			iIconSize=iShowBmp->SizeInPixels();
		}
	}
	else if(iBool)
	{
		iIconSize.iHeight=iTextHeight+iMargin;
		iIconSize.iWidth=iBackRect.Size().iWidth-iTextHeight;
	}

	iTextSize.iHeight=iTextHeight+iMargin;
	iTextSize.iWidth=iBackRect.Size().iWidth-iTextHeight-3;

	TInt num=iIconSize.iHeight/iTextSize.iHeight;

	if(iIconSize.iHeight%iTextSize.iHeight>0)
		num++;

	iAllLine=num;

	iLineWidth=iBackRect.Size().iWidth-iTextHeight;

	iMaxLine=(iBackRect.Height()-iIconSize.iHeight-iTextHeight)/(iTextHeight+iMargin);

	iStartPoint.iX=iBackRect.iTl.iX+iMargin;
	iStartPoint.iY=iBackRect.iTl.iY+iMargin;
	InitScrollBar();
}
void CContentInfoDialog::SetBmpShow(TBool aBool)
{
	iBool=aBool;
}
void CContentInfoDialog::AddTextItem(const TDesC& aText)
{
	TInt textPos = 0;
 	while(textPos < aText.Length())
	{	
		TInt charNum = iMainEngine.ScreenLayout().Font()->TextCount(aText.Mid(textPos),iLineWidth);
 		const TDesC& tmpDes = aText.Mid(textPos,charNum);

		iDesArray->AppendL(tmpDes);
		textPos += charNum;
	}
	InitScrollBar();
}
void CContentInfoDialog::SetBmpPath(const TDesC& aPath)
{
	InitImageManipAdapter();
	iAdapter->OpenImageByPath(aPath);
}
void CContentInfoDialog::RedrawView(TInt aType)
{
	if(aType==0)
	{
		DELETE(iShowBmp);
		iShowBmp=&(iAdapter->Image());
		InitBitmap();
		InitScrollBar();
		RequestDraw();
	}
}
void CContentInfoDialog::InitImageManipAdapter()
{
	CEikonEnv* env = CEikonEnv::Static();
	iAdapter = CImageManipAdapter::NewL(*this,env->DefaultDisplayMode());
}
void CContentInfoDialog::InitScrollBar()
{
	DELETE(iScrollbar);
	if(iDesArray->Count()>iMaxLine)
	{
		iScrollbar = new (ELeave)CScrollBar;
		iScrollbar->DefaultLayout();
		iScrollbar->SetVerticalSpace(iBackRect.Height());
		iScrollbar->SetScrollWidth(3);
		iScrollbar->SetBeginPostion(iBackRect.iTl+TPoint(iBackRect.Width()-3,0));
		iScrollbar->SetCursorColor(KScrollSpaceColor);
		iScrollbar->SetBackgroundColor(KScrollBackgroundColor);
		iScrollbar->SetMaxLinePerPage(iMaxLine+iAllLine);
		iScrollbar->SetTotalLineNum(iAllLine+iDesArray->Count());
		iScrollbar->Layout();
	}
}
	