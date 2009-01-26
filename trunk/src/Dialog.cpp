/*
============================================================================
Name        : Dialog.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CDialog implementation
============================================================================
*/
#include <coemain.h>
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif

#include "Dialog.h"
#include "MultiText.h"
#include "ScrollBar.h"
#include "MainEngine.h"
#include "Graphic.h"
//#include "picture.h"
#include "ScreenLayout.h"
#include "BitmapFactory.h"
#include "DialogObserver.h"
#include "ControlObserver.h"
#include "PreDefine.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "StaticCommonTools.h"

CDialog::CDialog(MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType) : CControl(EDialog)
																						, iObserver(aObserver)
																						, iMainEngine(aMainEngine)
																						, iDialogType(aDialogType)
																						, iScreenLayout(aMainEngine.ScreenLayout())
																						, iBitmapFactory(aMainEngine.GetBitmapFactory())
{
	ASSERT(aDialogType >= EDialogTip && aDialogType <= EDialogWaitingWithoutCancel);
	SetDrawEventObserver(&aMainEngine);
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}

CDialog::~CDialog()
{
	ReleaseBmps();
	delete iWaitingBmp;
	delete iMultiText;
	delete iLeftButtonText;
	delete iRightButtonText;
	delete iNotifyTimer;
}

CDialog* CDialog::NewLC(const TDesC& aText,MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType)
{
	CDialog* self = new (ELeave)CDialog(aObserver,aMainEngine,aDialogType);
	CleanupStack::PushL(self);
	self->ConstructL(aText);
	return self;
}

CDialog* CDialog::NewL(const TDesC& aText,MDialogObserver& aObserver, CMainEngine& aMainEngine,TInt aDialogType)
{
	CDialog* self=CDialog::NewLC(aText,aObserver,aMainEngine,aDialogType);
	CleanupStack::Pop(); // self;
	return self;
}

void CDialog::ConstructL(const TDesC& aText)
{
	iIconType = 1;
	InitBitmaps();
	InitMultiTextL(aText);

	if(EDialogWaiting == iDialogType || EDialogWaitingWithoutCancel == iDialogType)
	{
		InitAnimBitmapL();
		iNotifyTimer = CNotifyTimer::NewL(*this);
		iNotifyTimer->Start(KStandardTimerInterval);
	}
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CDialog::Draw(CGraphic& gc)const
{
	DrawBackground(gc);

	DrawIcon(gc);

	iMultiText->Draw(gc);

//	DrawButton(gc);

	DrawAnimation(gc);
}

TBool CDialog::KeyEventL(TInt aKeyCode)
{
	//Assert();
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
	case KOKKey:
		if(iDialogType == EDialogTip ||iDialogType == EDialogInquire)
		{
			MDialogObserver& observer = iObserver;
			TInt commandType = iLeftCommandType;
			CleanupSelf();
			observer.DialogEvent(commandType);
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	case KRightSoftKey:
		if (iDialogType == EDialogWaiting || iDialogType == EDialogInquire)
		{
			MDialogObserver& observer = iObserver;
			TInt commandType = iRightCommandType;
			CleanupSelf();
			observer.DialogEvent(commandType);
		}
		else
		{
			keyResult = EFalse;
		}
		break;

	default:
		keyResult = EFalse;
		break;
	}
	//必须返回ETrue以屏蔽下层界面的处理,但此处有个问题:返回ETrue将产生无意义的重绘事件
	return ETrue;
	//return keyResult;
}

TBool CDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}

const TDesC& CDialog::LeftButton() const
{
	if(iLeftButtonText)
	{
		return *iLeftButtonText;
	}
	if(iDialogType == EDialogTip ||iDialogType == EDialogInquire)
	{
		return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
	}
	return KNullDesC;
}

const TDesC& CDialog::RightButton() const
{
	if(iRightButtonText)
	{
		return *iRightButtonText;
	}
	if (iDialogType == EDialogWaiting || iDialogType == EDialogInquire)
	{
		return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
	}
	return KNullDesC;
}
//////////////////////////////////////////////////////////////////////////
//From CNotifyTimer
//////////////////////////////////////////////////////////////////////////
TBool CDialog::DoPeriodTask()
{
	if(iDialogType == EDialogWaiting || iDialogType == EDialogWaitingWithoutCancel)
	{
		if(++iTimer == 2)
		{
			iTimer = 0;
		}	
		UtilityTools::WriteLogsL(_L("CDialog::DoPeriodTask"));
		RequestDraw();
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
/*
void CDialog::SetLeftCommand(MCommondBase* aCommand)
{
	iLeftCommand = aCommand;
}

void CDialog::SetRightCommand(MCommondBase* aCommand)
{
	iRightCommand = aCommand;
}
*/

void CDialog::SetLeftCommandType(TInt aCommand)
{
	iLeftCommandType = aCommand;
}

void CDialog::SetRightCommandType(TInt aCommand)
{
	iRightCommandType = aCommand;
}

void CDialog::SetLeftButtonL(const TDesC& aDes)
{
	ASSERT(iLeftButtonText == NULL);
	iLeftButtonText = aDes.AllocL();
}

void CDialog::SetRightButtonL(const TDesC& aDes)
{
	ASSERT(iRightButtonText == NULL);
	iRightButtonText = aDes.AllocL();
}

void CDialog::SetRect(const TRect& aRect)
{
	iRect = aRect;
}

void CDialog::SetDismissTime(TInt aTime)
{
	iDismissTime = aTime;
}

void CDialog::SetIconType(TInt aIconType)
{
	iIconType = aIconType;
}

void CDialog::InitMultiTextL(const TDesC& aText)
{
	if(iMultiText)
	{
		delete iMultiText;
		iMultiText = NULL;
	}

	iMultiText = CMultiText::NewL();
	//TRect clientRect(TPoint(50,85),TSize(100,65));

	TRect textRect = TRect(iStartPoint,iDialogSize);	//dialog rect
	textRect.iTl.iX += iIconSize.iWidth;
	//textRect.Move(TPoint(iIconSize.iWidth,0));

	TInt innerMargin = iScreenLayout.InnerMargin();
	textRect.Shrink(innerMargin,innerMargin);

	//卞涛增加获取要分割的行数并处理排放对其
	TInt lineNum=::ParseString(aText,iMainEngine.Graphic().GetFont(),textRect.Width());
	TInt lineHeight = iMainEngine.Graphic().GetFont()->HeightInPixels();
	textRect.iTl.iY +=(iDialogSize.iHeight-lineNum*lineHeight)/2;

	iMultiText->Layout(textRect,iMainEngine.Graphic().GetFont(),1,0);
	iMultiText->SetTextColor(KRgbBlack);

	//WriteLogsL(_L("CDialog::InitMultiTextL AppendL iMultiText = %d"),iMultiText);

	iMultiText->AppendL(aText);

	iMultiText->Scrollbar()->SetShow(EFalse);

	//WriteLogsL(_L("CDialog::InitMultiTextL End"));

}

void CDialog::DrawBackground(CGraphic& gc)const
{

	gc.BitBlt(iStartPoint,iDialogBmp);
}

void CDialog::DrawIcon(CGraphic& gc)const
{
// 	TRect iconRect(0,0,iIconSize.iWidth/3,iIconSize.iHeight);
// 	iconRect.Move(TPoint(iIconSize.iWidth*iIconType/3,0));
// 	gc.BitBltMasked(iStartPoint + iIconPoint,iDialogIcon,iDialogIconMask,iconRect);
	iBitmapFactory.DrawDialogIconBmpByID(gc,iStartPoint + iIconPoint,iIconType);
}


void CDialog::DrawAnimation(CGraphic& gc)const
{
	if(iDialogType == EDialogWaiting || iDialogType == EDialogWaitingWithoutCancel)
	{
		TRect rect(TPoint(0,iWaitingSize.iHeight*iTimer),iWaitingSize);
		//此操作用在这里没有用,必须和全局的绘图流程结合起来使用
		//gc.SetClippingRect(rect);	
		gc.BitBlt(iAnimPoint,iWaitingBmp,rect);
		//gc.CancelClippingRect();
	}
}

void CDialog::InitBitmaps()
{
	iDialogBmp = iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpDialog);

	iDialogSize = iDialogBmp->SizeInPixels();

	iIconSize = iBitmapFactory.GetDialogIconBmpSize(0);


	//上下左右居中
	TSize screenSize = CScreenLayout::ScreenSize();

	iStartPoint.iX = (screenSize.iWidth - iDialogSize.iWidth)/2;
	iStartPoint.iY = (screenSize.iHeight - iDialogSize.iHeight)/2;

	iIconPoint.iX = 0;
	iIconPoint.iY = (iDialogSize.iHeight - iIconSize.iHeight)/2;

	iTextLengthPerLine = iDialogSize.iWidth - iIconSize.iWidth/3;

	iMargin = iScreenLayout.InnerMargin();
	iTextLengthPerLine -= iMargin*2;
}

void CDialog::ReleaseBmps()
{
	delete iDialogBmp;
	iDialogBmp = NULL;
/*
	delete iDialogIcon;
	iDialogIcon = NULL;
	delete iDialogIconMask;
	iDialogIconMask = NULL;
*/
}

void CDialog::InitAnimBitmapL()
{
	iWaitingBmp = iMainEngine.CreateBmpById(EMbmCocobmpWating);
	iWaitingSize = iScreenLayout.ScaleByDefaultRatio(iWaitingBmp);
	iWaitingSize.iHeight /= 3;
	iAnimPoint = iStartPoint;
	//iAnimPoint.iX += iDialogSize.iWidth - iWaitingSize.iWidth;
	TInt iIconWidth = iIconSize.iWidth/3;
	iAnimPoint.iX += iIconWidth;
	TInt offset = iDialogSize.iWidth - iIconWidth;
	offset -= iWaitingSize.iWidth;
	offset = offset>>1;
	iAnimPoint.iX += offset;
	iAnimPoint.iY += iDialogSize.iHeight - iWaitingSize.iHeight*2;
}