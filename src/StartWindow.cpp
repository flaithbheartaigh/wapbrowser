/*
============================================================================
 Name        : StartWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CStartWindow implementation
============================================================================
*/

#include "StartWindow.h"
#include "MainEngine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "UtilityTools.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "BitmapFactory.h"
#include "CoCoPreDefine.h"
#include "ControlFactory.h"
#include "HandleLoginOrExit.h"
#include "HandleSynchronization.h"
#include "HandleSystemInfo.h"
#include "SystemInfoDialog.h"
#include "Dialog.h"
const TInt KMaxTimeLimit = 15;		//10秒
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
CStartWindow::CStartWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(EStartWindow,aParent,aMainEngine)
,iMainEngine(aMainEngine)
,iSyschr(aMainEngine.GetSynchronContent())
,iFlag(ETrue)
,iInitDownLoadFlag(ETrue)
{
	iIsNormalWindow = EFalse;
	iSystemInfoBool=EFalse;
	iButtonFlag=EFalse;
}


CStartWindow::~CStartWindow()
{
	delete iTimer;
	DELETE(iSystemInfo);
	DELETE(iLogin);
	DELETE(iControlPaneBmp);
}

CStartWindow* CStartWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CStartWindow* self = new (ELeave)CStartWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CStartWindow::ConstructL()
{
	//ActivateL();

	iStatueFlag=-1;
	

	if(iMainEngine.GetLoginType()==0)
	{
		iStatueFlag=ELoginNone;
	}
	else
	{
		iLogin=CHandleLoginOrExit::NewL(*this,iMainEngine);
		iSystemInfo=CHandleSystemInfo::NewL(*this,iMainEngine);
	}


	iTimer = CNotifyTimer::NewL(*this);
	iTimer->Start(KStandardTimerInterval);

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;
}

//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CStartWindow::DoActivateL()
{
	this->InitBmpPoint();
}

void CStartWindow::DoDeactivate()
{

}
void CStartWindow::DrawControlButton(CGraphic& gc) const
{
	TSize screenSize =iMainEngine.ScreenSize();
	TSize size = iControlPaneBmp->SizeInPixels();
	
	TPoint point(0,0);
	point.iY += screenSize.iHeight;
	point.iY -= size.iHeight;
	gc.BitBlt(point,iControlPaneBmp);
	gc.SetPenColor(KTextColor);
	
	TRect controlPaneRect = iMainEngine.ScreenLayout().GetControlPaneRect();
	TInt margin = iMainEngine.ScreenLayout().InnerMargin();

	if(iLeftText.Length() > 0)
	{
		gc.DrawText(iLeftText,controlPaneRect,CGraphicsContext::ELeft,margin);
	}
	if(iRightText.Length() > 0)
	{
		gc.DrawText(iRightText,controlPaneRect,CGraphicsContext::ERight,margin);
	}
}
void CStartWindow::DoDraw(CGraphic& gc)const
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

	iMainEngine.GetBitmapFactory().DrawLogoIconBmpByID(gc,iIconPoint,iIndex);

	
	TInt width = gc.GetFont()->MeasureText(iMainEngine.GetDesById(ETurkeyTextRes_Conneting));
	point.iX=(screenSize.iWidth-width)/2;
	point.iY=iIconPoint.iY+iIconSize.iWidth+iMainEngine.ScreenLayout().FontHeight()/2;
	gc.DrawText(point,iMainEngine.GetDesById(ETurkeyTextRes_Conneting));
		
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

	if(iButtonFlag)
		this->DrawControlButton(gc);
}

TBool CStartWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	if(EKeyDevice3 == aKeyCode)
	{
		keyResult = ETrue;
	}
	return keyResult;
}

TBool CStartWindow::DoHandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CStartWindow::DoSizeChanged()
{
	TSize screenSize = iMainEngine.ScreenSize();
}

void CStartWindow::ChangeButton()
{

}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CStartWindow::DoPeriodTask()
{
	//iMainEngine.WriteLog16(_L("CStartWindow::DoPeriodTask Start"));
	if(iMainEngine.GetLoginType()==1&&iStatueFlag==-1&&iMainEngine.GetIMEI().Length()>0)
	{
		iMainEngine.WriteLog16(_L("IMEI-Loading:%S"),&iMainEngine.GetIMEI());
		iStatueFlag=ELoginGateWay;
		iLogin->SendLoginRequest();
	}

	if(iMainEngine.GetLoginType()==1&&iNum==8&&iInitDownLoadFlag)
	{
		iInitDownLoadFlag=EFalse;
		iMainEngine.InitDownLoadEngine();
	}
	
	iNum++;

	if(iNum>39)
		iNum=1;

	iIndex=iNum/10;
	
	TBool timerResult = ETrue;

	if(!iSystemInfoDialog&&!iDialog)
		iTimeLimit++;

	if(iTimeLimit>=KMaxTimeLimit&&iStatueFlag==ELoginNone)
	{
		//iMainEngine.WriteLog16(_L("CStartWindow::DoPeriodTask GoBackToParentWindow"));
		GoBackToParentWindow();
		timerResult = EFalse;
	}
	else if(iTimeLimit==250&&!iSystemInfoDialog&&!iDialog)
	{
		//这段代码可能会导致出现问题！
		//iMainEngine.SetLoginTypeL(0);
		GoBackToParentWindow();
		timerResult = EFalse;
	}
	else
	{
		iMainEngine.RequestDraw();
	}
	//iMainEngine.WriteLog16(_L("CStartWindow::DoPeriodTask End"));
	return timerResult;
}
void CStartWindow::InitBmpPoint()
{
	TSize size=iMainEngine.ScreenLayout().ScreenSize();

	iIconSize=iMainEngine.GetBitmapFactory().GetLogoIconBmpSize(0);

	iIconPoint.iY=size.iHeight/4-size.iHeight/16;
	iIconPoint.iX=(size.iWidth-iIconSize.iWidth)/2;

	iControlPaneBmp=iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpMenubg);
}
void CStartWindow::HandleResponseEvent(TInt aEvent,TInt aType)
{
	iTimeLimit=0;
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleResponseEvent aEvent = %d,aType = %d"),aEvent,aType);
	if(aEvent==EHttpNoError)
	{
		switch(aType)
		{
			case EHttpLogin:
				HandleLoginState();
				break;
			case EHttpFirstContentUpdate:
				HandleSynchronState();
				break;
			case EHttpSystemInfo:
				HandleSystemInfo();
				break;
		}
	}
	else
	{
		//iMainEngine.WriteLog16(_L("CStartWindow::HandleResponseEvent ELoginNone"));
		iMainEngine.SetLoginTypeL(0);
		iStatueFlag=ELoginNone;
	}
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleResponseEvent End"));
}
void CStartWindow::HandleLoginState()
{
	iSystemInfoBool=iLogin->IsNextSystemInfo();
	if(iLogin->IsSmallGateWay())
	{
		iMainEngine.SetSmallGWState(1);
	}
	else
	{
		iMainEngine.SetSmallGWState(0);
	}
	if(iLogin->IsAppUpdate())
	{
		iStatueFlag=ELoginAppUpdate;
		InitDialog(iMainEngine.GetDesById(ETurkeyTextRes_Upgrate));
	}
	else
	{
		HandleSynchronState();
	}
}
void CStartWindow::HandleSynchronState()
{
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleSynchronState"));
	if((iLogin->IsContentUpdate()&&iStatueFlag!=ELoginSyschron)||iSyschr.IsUpdate(0,-1,-1))
	{
		UtilityTools::WriteLogsL(_L("CStartWindow::HandleSynchronState SendFirstRequest"));
		iStatueFlag=ELoginSyschron;
		iSyschr.SetObserver(this);
		iSyschr.SendFirstRequest();
	}
	else
	{
		UtilityTools::WriteLogsL(_L("CStartWindow::HandleSynchronState HandleSystemInfo"));
		HandleSystemInfo();
	}
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleSynchronState End"));
}
void CStartWindow::HandleSystemInfo()
{
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleSystemInfo"));
	if(iLogin->IsSystemInfo()&&iStatueFlag!=ELoginSystemInfo)
	{
		UtilityTools::WriteLogsL(_L("CStartWindow::HandleSystemInfo iStatueFlag!=ELoginSystemInfo"));
		iStatueFlag=ELoginSystemInfo;
		//显示系统信息

		if(iSystemInfoBool)
			InitSystemInfo(iLogin->GetTitle(),iLogin->GetSystemInfo(),0);
		else
			InitSystemInfo(iLogin->GetTitle(),iLogin->GetSystemInfo(),1);
	}
	else if(iStatueFlag==ELoginSystemInfo)
	{
		UtilityTools::WriteLogsL(_L("CStartWindow::HandleSystemInfo iStatueFlag==ELoginSystemInfo"));

		iSystemInfoBool=iSystemInfo->IsNextSystemInfo();
		if(iSystemInfo->IsNextSystemInfo())
			InitSystemInfo(iSystemInfo->GetTitle(),iSystemInfo->GetSystemInfo(),0);
		else
			InitSystemInfo(iSystemInfo->GetTitle(),iSystemInfo->GetSystemInfo(),1);
	}
	else
	{
		iStatueFlag=ELoginNone;
	}
	UtilityTools::WriteLogsL(_L("CStartWindow::HandleSystemInfo End"));
}
void CStartWindow::InitDialog(const TDesC& aText)
{
	iButtonFlag=ETrue;
	iDialog=iMainEngine.ControlFactory().CreateInquireDialog(aText,*this,99,0,this);
	iLeftText.Zero();
	iLeftText.Append(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	iRightText.Zero();
	iRightText.Append(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
}
void CStartWindow::InitSystemInfo(const TDesC& aTitle,const TDesC& aText,TInt aType)
{
	UtilityTools::WriteLogsL(_L("CStartWindow::InitSystemInfo aType = %d"),aType);
	iButtonFlag=ETrue;
	if(iSystemInfoDialog)
		this->RemoveAndDeleteControl(iSystemInfoDialog);

	iSystemInfoDialog=iMainEngine.ControlFactory().CreateInfoDialog(*this,aTitle,aText,0);

	iLeftText.Zero();
	if(aType==0)
	{
		iLeftText.Append(iMainEngine.GetDesById(ETurkeyTextRes_NextStep));
	}

	iRightText.Zero();
	iRightText.Append(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	UtilityTools::WriteLogsL(_L("CStartWindow::InitSystemInfo End"));
}

void CStartWindow::DialogEvent(TInt aCommand)
{
	this->iDialog=NULL;
	iButtonFlag=EFalse;
	if(aCommand==99)
	{
		//调用升级
		TBuf<100> updateBuf;
		updateBuf.Zero();
		updateBuf.Append (_L("4 http://59.36.96.182/download/check.jsp?name=3Gest|"));
		updateBuf.Append (iMainEngine.GetDesById(0)); 
		iMainEngine.LaunchWapBrowswerL (updateBuf);
		User::Exit(0);
	}
	else
	{
		HandleSynchronState();
	}
}
void CStartWindow::InfoResponseEvent(TInt aEvent)
{
	iSystemInfoDialog=NULL;
	iButtonFlag=EFalse;
	if(aEvent==0)
	{
		if(iSystemInfoBool)
			iSystemInfo->SendRequest();
		else
			iStatueFlag=ELoginNone;
	}
	else
	{
		iStatueFlag=ELoginNone;
	}
}