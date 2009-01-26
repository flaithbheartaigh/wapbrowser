/*
============================================================================
 Name        : EndWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CEndWindow implementation
============================================================================
*/

#include "EndWindow.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "HandleLoginOrExit.h"
#include "CoCoPreDefine.h"
#include "BitmapFactory.h"

const TInt KMaxTimeLimit = 20;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CEndWindow::CEndWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EEndWindow,aParent,aMainEngine)
{
	iIsNormalWindow = EFalse;
	iCanPopUpWindow = EFalse;
}

CEndWindow::~CEndWindow()
{
	//Deactivate();
	delete iTimer;
	DELETE(iExit);
}

CEndWindow* CEndWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CEndWindow* self = new (ELeave)CEndWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CEndWindow::ConstructL()
{
	iExit=CHandleLoginOrExit::NewL(*this,iMainEngine);

	if(iMainEngine.GetLoginType()==1)
	{
		iExit->SendExitRequest();

		//保存流量统计
		iMainEngine.SaveNetTrafficInfo();
	}

	//ActivateL();
	iTimer = CNotifyTimer::NewL(*this);
	iTimer->Start(KStandardTimerInterval);

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;
}
//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CEndWindow::DoActivateL()
{

	//该调用不规范,暂时如此书写
	DoSizeChanged();
}

void CEndWindow::DoDeactivate()
{

}

void CEndWindow::DoDraw(CGraphic& gc)const
{
	TPoint point(0,0);
	TSize screenSize=iMainEngine.ScreenSize();

	gc.SetPenColor(TRgb(0,0,0));
	gc.SetBrushColor(TRgb(0,0,0));
	gc.SetPenStyle(CBitmapContext::ENullPen);
	gc.SetBrushStyle(CBitmapContext::ESolidBrush);
	gc.DrawRect(TRect(point,screenSize));

	gc.SetPenColor(KTextColor);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);	


	TSize iconSize=iMainEngine.GetBitmapFactory().GetLogoIconBmpSize(0);
	TPoint iconPoint(0,0);
	
	iconPoint.iY=screenSize.iHeight/4-screenSize.iHeight/16;
	iconPoint.iX=(screenSize.iWidth-iconSize.iWidth)/2;

	iMainEngine.GetBitmapFactory().DrawLogoIconBmpByID(gc,iconPoint,0);

	
	TInt width = gc.GetFont()->MeasureText(iMainEngine.GetDesById(ETurkeyTextRes_Exiting));
	point.iX=(screenSize.iWidth-width)/2;
	point.iY=iconPoint.iY+iconSize.iWidth+iMainEngine.ScreenLayout().FontHeight()/2;
	gc.DrawText(point,iMainEngine.GetDesById(ETurkeyTextRes_Exiting));
		
	width = gc.GetFont()->MeasureText(iMainEngine.GetDesById(ECoCoTextRes_CoCo_DownLoad));
	point.iX=(screenSize.iWidth-width)/2;
	point.iY=point.iY+iMainEngine.ScreenLayout().FontHeight()+iMainEngine.ScreenLayout().FontHeight()/2;
	gc.DrawText_Bold(point,iMainEngine.GetDesById(ECoCoTextRes_CoCo_DownLoad));

	width = gc.GetFont()->MeasureText(iMainEngine.GetDesById(ECoCoTextRes_Web_Url));
	point.iX=(screenSize.iWidth-width)/2;
	point.iY=point.iY+iMainEngine.ScreenLayout().FontHeight()+iMainEngine.ScreenLayout().FontHeight()/2;
	gc.DrawText_Bold(point,iMainEngine.GetDesById(ECoCoTextRes_Web_Url));

	width = gc.GetFont()->MeasureText(iMainEngine.GetDesById(ETurkeyTextRes_LoadingTitle));
	point.iX=(screenSize.iWidth-width)/2;
	point.iY=point.iY+iMainEngine.ScreenLayout().FontHeight()*2;
	gc.DrawText(point,iMainEngine.GetDesById(ETurkeyTextRes_LoadingTitle));

}

TBool CEndWindow::DoKeyEventL(TInt /*aKeyCode*/)
{
	return EFalse;
}

TBool CEndWindow::DoHandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CEndWindow::DoSizeChanged()
{

}

void CEndWindow::ChangeButton()
{

}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CEndWindow::DoPeriodTask()
{
	iTimeLimit++;
	if(KMaxTimeLimit == iTimeLimit)	//超时
	{
		User::Exit(0);
	}
	return ETrue;
}
void CEndWindow::HandleResponseEvent(TInt aEvent,TInt aType)
{
	
}