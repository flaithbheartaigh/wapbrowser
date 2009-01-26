/*
============================================================================
Name        : FileBrowser.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CSelectAppWindow implementation
============================================================================
*/
#include <coemain.h>
#include <bautils.h>
#include <APGCLI.H>
#ifdef __SERIES60_3X__
#include <akniconutils.h>
#endif

#include "SelectAppWindow.h"
#include "SelectAppObserver.h"
#include "Window.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "BitmapFactory.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "ListBox.h"
#include "PreDefine.h"
#include "TypeDefine.h"
#include "cocobmp.h"

CSelectAppWindow::CSelectAppWindow(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver) 
: CControl(EFileBrowser)
, iMainEngine(aMainEngine)
, iObserver(aSelectAppObserver)
{
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CSelectAppWindow::~CSelectAppWindow()
{
	delete iListBox;
	delete iSaveAsBmp;
	delete iTitleText;

	delete iCaptionArray;
	delete iPathArray;
}

CSelectAppWindow* CSelectAppWindow::NewLC(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver)
{
	CSelectAppWindow* self = new (ELeave)CSelectAppWindow(aMainEngine,aSelectAppObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSelectAppWindow* CSelectAppWindow::NewL(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver)
{
	CSelectAppWindow* self=CSelectAppWindow::NewLC(aMainEngine,aSelectAppObserver);
	CleanupStack::Pop(); // self;
	return self;
}

void CSelectAppWindow::ConstructL()
{
	iTitleText = iMainEngine.GetDesById(ETurkeyTextRes_SelectApp).AllocL();

	InitBitmaps();

	InitAppInfo();

	InitListBox();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CSelectAppWindow::Draw(CGraphic& aGraphic)const
{
	ASSERT(iListBox);
	ASSERT(iTitleText);

	aGraphic.BitBlt(iStartPoint,iSaveAsBmp);
	aGraphic.SetPenColor(KRgbBlack);
	aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
	aGraphic.DrawText(iStartPoint + iTextPoint,*iTitleText);

	iListBox->Draw(aGraphic);
}

TBool CSelectAppWindow::KeyEventL(TInt aKeyCode)
{
	ASSERT(iListBox);

	TBool keyResult = iListBox->KeyEventL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case KOKKey:	
		case EKeyDevice0:
			DoConfirm();
			break;

		case EKeyLeftArrow:
			
			break;

		case EKeyRightArrow:
		//case EKeyDevice3:
			
			break;

		case EKeyDevice1:
			DoCancel();
			break;

		default:
			keyResult = EFalse;
			break;
		}
	}
	return ETrue;	//屏蔽底层操作
}

TBool CSelectAppWindow::HandleCommandL(TInt aCommand)
{
	ASSERT(iListBox);

	return iListBox->HandleCommandL(aCommand);
}

void CSelectAppWindow::SizeChanged(const TRect& aScreenRect)
{
	ASSERT(iListBox);

	iListBox->SizeChanged(aScreenRect);
}

const TDesC& CSelectAppWindow::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CSelectAppWindow::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CSelectAppWindow::DoConfirm()
{
	CNormalListBoxItem& curListItem = (CNormalListBoxItem&)(iListBox->CurItem());
	
	TInt i=iListBox->CurItemIndex() ;

	TInt nLen=iPathArray->At(i).Length();

	HBufC* tempFilePath = HBufC::NewLC(nLen);
	tempFilePath->Des().Append(iPathArray->At(i));

	MSelectAppObserver& observer = iObserver;
	CleanupSelf();

	observer.DoSelectEvent(*tempFilePath,1);

	CleanupStack::PopAndDestroy();	//tempFilePath
}

void CSelectAppWindow::DoCancel()
{
	MSelectAppObserver& observer = iObserver;
	CleanupSelf();
	observer.DoSelectEvent(_L(""),0);
}

void CSelectAppWindow::InitListBox()
{
	if(iListBox)
	{
		delete iListBox;
		iListBox = NULL;
	}
	
	iListBox = iMainEngine.ControlFactory().CreateListBox(iListBoxRect);
	iListBox->SetTextColor(KRgbBlack,KRgbBlack);

	iMainEngine.CurWindow()->RemoveControl(iListBox);
	for (TInt i = 0 ; i < iCaptionArray->Count() ; i++)
	{
		const CFbsBitmap* icon = NULL;
		const CFbsBitmap* iconMask = NULL;
		
		icon = iMainEngine.GetBitmapFactory().GetFileTypeIcon(0);
		iconMask = iMainEngine.GetBitmapFactory().GetFileTypeIconMask(0);
		
		iListBox->AppendL(CNormalListBoxItem::NewL((*iCaptionArray)[i],icon,iconMask));
	}
	iListBox->Layout();
}

void CSelectAppWindow::InitBitmaps()
{
	iSaveAsBmp = iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpSaveas);

	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout(); 

	iBmpSize = iSaveAsBmp->SizeInPixels();

	//上下左右居中
	TRect clientRect = screenLayout.GetClientRect();

	iStartPoint = clientRect.iTl;
	iStartPoint.iX += (clientRect.Width() - iBmpSize.iWidth)/2;
	iStartPoint.iY += (clientRect.Height() - iBmpSize.iHeight)/2;

	iLineHeight = screenLayout.LineHeight();

	TPoint listPoint;
	screenLayout.GetSaveAsPos(iTextPoint,listPoint);

	iListBoxRect = iSaveAsBmp->SizeInPixels();
	iListBoxRect.Move(iStartPoint);
	iListBoxRect.iTl.iY += listPoint.iY;
	TInt innerMargin = screenLayout.InnerMargin();
	iListBoxRect.Shrink(innerMargin,innerMargin);
}

void CSelectAppWindow::InitAppInfo()
{
	iPathArray = new (ELeave) CArrayFixFlat<TFileName>(20);
	iCaptionArray = new (ELeave) CArrayFixFlat<TFileName>(20);
	//iIconList = new (ELeave) CArrayFixFlat<CApaMaskedBitmap>(20);

	TBuf<128> buf;

	//第一个选项为系统默认程序
	iCaptionArray->AppendL( iMainEngine.GetDesById(ECoCoTextRes_DefaultAppName));

	buf.Copy(KDefaultAppPath);
	iPathArray->AppendL(buf);

	//第二个选项为内部打开
	iCaptionArray->AppendL(_L("CoCo"));

	buf.Copy(KCoCoAppPath);
	iPathArray->AppendL(buf);

	RApaLsSession rls;

	User::LeaveIfError( rls.Connect() );
	User::LeaveIfError( rls.GetAllApps() );

	TApaAppInfo appInfo;

	//	All apps that should be ignored
	//	About 50 ignored apps
	//	TBuf<50> size is only 32
	//	so all ignored apps are devided into 13 groups
	TBuf<50> aAppGroup1 = iMainEngine.GetDesById(ETurkeyTextRes_IgnoreAppGroup1);
	TBuf<50> aAppGroup2 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup2 );
	TBuf<50> aAppGroup3 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup3 );
	TBuf<50> aAppGroup4 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup4 );
	TBuf<50> aAppGroup5 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup5 );
	TBuf<50> aAppGroup6 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup6 );
	TBuf<50> aAppGroup7 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup7 );
	TBuf<50> aAppGroup8 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup8 );
	TBuf<50> aAppGroup9 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup9 );
	TBuf<50> aAppGroup10 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup10 );
	TBuf<50> aAppGroup11 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup11 );
	TBuf<50> aAppGroup12 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup12 );
	TBuf<50> aAppGroup13 = iMainEngine.GetDesById( ETurkeyTextRes_IgnoreAppGroup13 );

	TBuf<50> aRecommendedAppGroup = iMainEngine.GetDesById( ETurkeyTextRes_RecommenedAppGroup );

	while( rls.GetNextApp( appInfo ) == KErrNone )
	{
		//	app caption
		buf.SetLength( 0 );
		buf.Append( appInfo.iCaption );
		buf.ZeroTerminate();

		//	Filer app
		if ( KErrNotFound != aAppGroup1.FindF( buf ) || 
			KErrNotFound != aAppGroup2.FindF( buf ) || 
			KErrNotFound != aAppGroup3.FindF( buf ) || 
			KErrNotFound != aAppGroup4.FindF( buf ) || 
			KErrNotFound != aAppGroup5.FindF( buf ) || 
			KErrNotFound != aAppGroup6.FindF( buf ) || 
			KErrNotFound != aAppGroup7.FindF( buf ) || 
			KErrNotFound != aAppGroup8.FindF( buf ) || 
			KErrNotFound != aAppGroup9.FindF( buf ) || 
			KErrNotFound != aAppGroup10.FindF( buf ) ||
			KErrNotFound != aAppGroup11.FindF( buf ) ||
			KErrNotFound != aAppGroup12.FindF( buf ) ||
			KErrNotFound != aAppGroup13.FindF( buf ) ||
			buf.Size() <= 2 )	//	No caption
			continue;

		if ( KErrNotFound != aRecommendedAppGroup.FindF( buf ) )
		{
			iCaptionArray->AppendL( buf );
			iRecommendedAppCount ++;

			//app full path
			buf.SetLength( 0 );
			buf.Append( appInfo.iFullName );
			buf.ZeroTerminate();
			iPathArray->AppendL( buf );

			//app icon
			/*CApaMaskedBitmap*	aMaskedBitmap = CApaMaskedBitmap::NewLC();
			TInt aError = rls.GetAppIcon( appInfo.iUid, 0, *aMaskedBitmap );
			if ( KErrNone != aError )
			{
				aError = rls.GetAppIcon( appInfo.iUid, 1, *aMaskedBitmap );
				if ( KErrNone != aError )
				{
					aError = rls.GetAppIcon( appInfo.iUid, 2, *aMaskedBitmap );
				}
			}

			iIconList->AppendL( *aMaskedBitmap );
			CleanupStack::PopAndDestroy(aMaskedBitmap );*/
		}
	}

	rls.Close();

	User::LeaveIfError( rls.Connect() );
	User::LeaveIfError( rls.GetAllApps() );

	while( rls.GetNextApp( appInfo ) == KErrNone )
	{
		//	app caption
		buf.SetLength( 0 );
		buf.Append( appInfo.iCaption );
		buf.ZeroTerminate();

		//	Filer app
		//
		if ( KErrNotFound != aAppGroup1.FindF( buf ) || 
			KErrNotFound != aAppGroup2.FindF( buf ) || 
			KErrNotFound != aAppGroup3.FindF( buf ) || 
			KErrNotFound != aAppGroup4.FindF( buf ) || 
			KErrNotFound != aAppGroup5.FindF( buf ) || 
			KErrNotFound != aAppGroup6.FindF( buf ) || 
			KErrNotFound != aAppGroup7.FindF( buf ) || 
			KErrNotFound != aAppGroup8.FindF( buf ) || 
			KErrNotFound != aAppGroup9.FindF( buf ) || 
			KErrNotFound != aAppGroup10.FindF( buf ) ||
			KErrNotFound != aAppGroup11.FindF( buf ) ||
			KErrNotFound != aAppGroup12.FindF( buf ) ||
			KErrNotFound != aAppGroup13.FindF( buf ) ||
			buf.Size() <= 2 )	//	No caption
			continue;

		if ( KErrNotFound == aRecommendedAppGroup.FindF( buf ) )
		{
			iCaptionArray->AppendL( buf );
			//nRecommendedAppCount ++;

			//	app full path
			buf.SetLength( 0 );
			buf.Append( appInfo.iFullName );
			buf.ZeroTerminate();
			iPathArray->AppendL( buf );
		}
	}

	rls.Close();
}

