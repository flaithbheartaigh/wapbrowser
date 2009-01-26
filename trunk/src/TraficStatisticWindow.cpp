/*
============================================================================
 Name        : TraficStatisticWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CTraficStatisticWindow implementation
============================================================================
*/

#include "TraficStatisticWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "TypeDefine.h"
#include "MultiText.h"
#include "FileReadByRow.h"
CTraficStatisticWindow::CTraficStatisticWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CTraficStatisticWindow::~CTraficStatisticWindow()
{
}

CTraficStatisticWindow* CTraficStatisticWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CTraficStatisticWindow* self = new (ELeave)CTraficStatisticWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTraficStatisticWindow* CTraficStatisticWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CTraficStatisticWindow* self=CTraficStatisticWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CTraficStatisticWindow::ConstructL()
{
	ChangeButton();
	iTimer=CNotifyTimer::NewL(*this);
	
	i=0;

	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iMainEngine.ControlFactory().SetParentWindow(this);
	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_Connecttion_Traffic]);

	InitSelectGroup();

	
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CTraficStatisticWindow::DoActivateL()
{
	iTimer->Start(10*KStandardTimerInterval);//1秒，流量显示刷新一下便于测试观察；稍后改为10秒
}

void CTraficStatisticWindow::DoDeactivate()
{
	iTimer->Stop();
}

void CTraficStatisticWindow::DoDraw(CGraphic& aGraphic) const
{

}


TBool CTraficStatisticWindow::DoKeyEventL(TInt aKeyCode)
{


	TBool keyResult = ETrue;
	
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			
		}
		break;
	case KRightSoftKey:
		///iTimer->Stop();
		GoBackToParentWindow();
		break;
	default:
		keyResult = EFalse;
		break;
	}

	return keyResult;
}

TBool CTraficStatisticWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CTraficStatisticWindow::DoSizeChanged()
{

}

void CTraficStatisticWindow::ChangeButton()
{
	//SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CTraficStatisticWindow::InitSelectGroup()
{
	TBuf<100>content;//存放文字提示以及流量,以便一行显示
	TBuf<64>traffic;//流量

	iMultiText = iMainEngine.ControlFactory().CreateDefaultMutliText();

	traffic.Zero();//月接受流量
	content.Zero();
	content.Append((*iRes->iItem)[ESystemManageRes_Traffic_MonthRev]);
	traffic.Num(iMainEngine.GetMonthTraffic_Rev()/1024.0);//由于返回的是字节，所以处理一下
	content.Append(traffic);
	content.Append(_L("KB"));//单位
	iMultiText->AppendL( content);


	traffic.Zero();//月发送流量
	content.Zero();
	content.Append((*iRes->iItem)[ESystemManageRes_Traffic_MonthSend]);
	traffic.Num(iMainEngine.GetMonthTraffic_Send()/1024.0);
	content.Append(traffic);
	content.Append(_L("KB"));//单位
	iMultiText->AppendL( content);

	traffic.Zero();//本次接受流量
	content.Zero();
	content.Append((*iRes->iItem)[ESystemManageRes_Traffic_CurrentRev]);
	traffic.Num(iMainEngine.GetCurrentTraffic_Rev()/1024.0);
	content.Append(traffic);
	content.Append(_L("KB"));//单位
	iMultiText->AppendL( content);

	traffic.Zero();//本次发送流量
	content.Zero();
	content.Append((*iRes->iItem)[ESystemManageRes_Traffic_CurrentSend]);
	traffic.Num(iMainEngine.GetCurrentTraffic_Send()/1024.0);
	content.Append(traffic);
	content.Append(_L("KB"));//单位
	iMultiText->AppendL( content);
	
	//multiText->Scrollbar()->SetShow(EFalse);
}
TBool CTraficStatisticWindow::DoPeriodTask()
{
	if (iMultiText!=NULL)
	{
		RemoveControl(iMultiText);
		InitSelectGroup();//刷新流量显示数字
		
	}
	
	return true;
}
