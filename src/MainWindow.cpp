/*
============================================================================
 Name        : MainWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CMainWindow implementation
============================================================================
*/

#include "MainWindow.h"
#include "CoCoappview.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "Dialog.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "InputDialog.h"
#include "SystemInfoDialog.h"
#include "ScreenLayout.h"
#include "SlideText.h"
#include "CoCoPreDefine.h"
#include "Scrollbar.h"
#include "BitmapFactory.h"

//add by hesanyuan
#include "FriendLink.h"
#include "ResultView.h"
#include "NewestDownload.h"
#include "SutraDownload.h"

#include "SelectBoxView.h"

#include "DownLoadEngine.h"
#include "HandleSynchronization.h"
#include "HandleAutoUpdate.h"
#include "HandleWeather.h"
#include "HandleLoginOrExit.h"
#include "HandleSynchronization.h"
#include "HandleSystemInfo.h"

//定义执行命令用的枚举常量
enum TEventCommand
{
	EMenuOpen,
	EMenuHelp,
	EMenuAutoUpdate,
	EMenuAdvice,
	EMenuWapPage,
	EMenuSysmtemInfo,
	EMenuExit,
	ENull,
	EExit
};
enum TMainViewDialogEventCommand
{
	EDialogLoginUpdate,
	EDialogLoginUpdateRight,
	EDialogUpdate,
	EDialogDownLoad,
	EDialogUpdateWaiting,
	EDialogWaiting,
	EDialogOtherWaiting
};
enum TMainViewLoginState
{
	EMainViewLoginNone,
	EMainViewLoginGateWay,
	EMainViewLoginSystemInfo,
	EMainViewLoginSyschron,
	EMainViewLoginAppUpdate
};
//////////////////////////////////////////////////////////////////////////
//CMainWindow
//////////////////////////////////////////////////////////////////////////
//构造与析构函数
//////////////////////////////////////////////////////////////////////////
CMainWindow::CMainWindow(CMainEngine& aMainEngine,const CCoCoAppView& aAppView) 
: CWindow(EMainWindow,NULL,aMainEngine)
,iBitmapFactory(aMainEngine.GetBitmapFactory())
,iAppView(aAppView)
,iSelectedIndex(0)
,iTimeCount(0)
,iPictureWhite(NULL)
,iHandleWeather(NULL)
,iHandleAutoUpdate(NULL)
,iLogin(NULL)	
,iSystemInfo(NULL)
,iSyschr(aMainEngine.GetSynchronContent())
,iRequestFirstInfo(EFalse)
{
	iMainEngine.SetMainWindow(this);
}


CMainWindow::~CMainWindow()
{
	//若可以从主界面以外的界面退出程序,则下面的调用是有用的,否则没有用

	delete iScrollbar;
	iScrollbar = NULL;

	DELETE(iHandleWeather);
	DELETE(iHandleAutoUpdate);
	DELETE(iLogin);
	DELETE(iSystemInfo);

	delete iPictureWhite;

	delete iTitleBitmap;
	iTitleBitmap = NULL;
	delete iTitleLogoBmp;
	iTitleLogoBmp = NULL;

	ResetChildWindow();
	Deactivate();
}

CMainWindow* CMainWindow::NewL(CMainEngine& aMainEngine,const CCoCoAppView& aAppView)
{
	CMainWindow* self = new (ELeave)CMainWindow(aMainEngine,aAppView);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CMainWindow::ConstructL()
{
	//ActivateL();
	iMainEngine.ControlFactory().SetParentWindow(this);
	ChangeButton();
	iSystemInfoBool=EFalse;

	iMaxLine=3;
	iSelectedLine=1;
	iTotleLine=0;
	iLineHeight=4;

	iSUpdateState=0;
	iColor=200;

	CreateChildWindow(EStartWindow);
	InitTitleBar();
	InitIconBmp();
	InitPictureWhite();
	InitScrollBar();
	//iMainEngine.SetCurWindow(this);

	if(iMainEngine.GetLoginType()==1)
	{
		iHandleWeather=CHandleWeather::NewL(iMainEngine);
		iHandleWeather->SendRequest();
		iHandleAutoUpdate=CHandleAutoUpdate::NewL(*this,iMainEngine);
	}
}

	
//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CMainWindow::DoActivateL()
{
	InitSlideText();
 	iTimer = CNotifyTimer::NewL(*this);
 	iTimer->Start(100000);
}

void CMainWindow::DoDeactivate()
{
	delete iTimer;
	iTimer = NULL;
	
	DeleteAllControl();
}

void CMainWindow::DoDraw(CGraphic& aGraphic)const
{
	DrawTitle(aGraphic);
	DrawAllIcons(aGraphic);
	DrawIcon(aGraphic);
	iScrollbar->Draw(aGraphic);
	if(iDownloadFlag>0&&iDIconNum/3<1)
		this->DrawDownLoadIcon(aGraphic);
}

TBool CMainWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = EFalse;
	keyResult = HandleIconKeyL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:	//左软键,弹出菜单
			InitPopUpMenu();
			break;

		case EKeyDevice1:	//右软键,隐藏或退出程序
			HideApplication();
			break;

		case EKeyDevice3:	//OK键,进入模块
			EnterModule();
			break;

		default:
			keyResult = EFalse;
			break;
		}
	}
	return keyResult;
}

TBool CMainWindow::DoHandleCommandL(TInt /*aCommand*/)
{
	TBool cmdResult = EFalse;
	return cmdResult;
}

void CMainWindow::DoSizeChanged()
{
}

void CMainWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Hidden]);
}
//////////////////////////////////////////////////////////////////////////
//From MTimerNotifier
//////////////////////////////////////////////////////////////////////////
TBool CMainWindow::DoPeriodTask()
{	
	if(iDIconNum>=6)
		iDIconNum=0;
	else
		iDIconNum++;

	iTimeCount++;
	if (iTimeCount>300)
	{
		//30秒保存一次流量统计
		iTimeCount=0;

		iMainEngine.SaveNetTrafficInfo();
	}

	if(iMainEngine.GetLoginType()==1)
		iDownloadFlag=iMainEngine.GetDownLoadEngine().GetDownLoadingTaskNum();

	iAppView.DrawNow();
	return ETrue;
}
//////////////////////////////////////////////////////////////////////////
//MInputObserver
//////////////////////////////////////////////////////////////////////////
void CMainWindow::InputResponseEvent(TInt /*aEvent*/,const TDesC& /*aText*/)
{
	
}
void CMainWindow::ShowInputDialog()
{
	//iMainEngine.ControlFactory().CreateInputTextDialog(*this,_L("input:"),_L("hello world!"));
}
//////////////////////////////////////////////////////////////////////////
//MInputObserver
//////////////////////////////////////////////////////////////////////////
void CMainWindow::InfoResponseEvent(TInt aEvent)
{

	if(iSystemInfoBool&&aEvent==0)
	{
		ShowWaitDialog(iMainEngine.GetDesById(ECoCoTextRes_GetSystemInfo));
		iSystemInfo->SendRequest();
	}
	else
	{
		if(iSUpdateState==1)
		{
			iSUpdateState=0;
			this->ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_CheckUpdate));
			iHandleAutoUpdate->SendRequest();
		}
		else if(iSUpdateState==2)
		{
			iSUpdateState=0;
			CreateChildWindow(EUserAdviseWindow);
		}
		else
		{
			SelectView(iSelectedIndex);
		}
	}
}
void CMainWindow::ShowInfoDialog(const TDesC& aTitle,const TDesC& aText,TInt aType)
{
	iMainEngine.ControlFactory().CreateInfoDialog(*this,aTitle,aText,0,this);
}

//////////////////////////////////////////////////////////////////////////
//MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CMainWindow::DialogEvent(TInt aCommand)
{
	TBuf<10> buf;
	buf.AppendNum(aCommand);
	//User::InfoPrint(buf);
	switch(aCommand)
	{
	case EDialogLoginUpdate:
		{
			//调用升级
			TBuf<100> updateBuf;
			updateBuf.Zero();
			updateBuf.Append (_L("4 http://59.36.96.182/download/check.jsp?name=3Gest|"));
			updateBuf.Append (iMainEngine.GetDesById(0)); 
			iMainEngine.LaunchWapBrowswerL (updateBuf);
			User::Exit(0);
		}
		break;
	case EDialogLoginUpdateRight:
		HandleSynchronState();
		break;
	case EDialogUpdate:
		break;
	case EDialogDownLoad:
		break;
	case EDialogWaiting:
		{
			if(iRequestFirstInfo)
			{
				iRequestFirstInfo=EFalse;
				this->iSyschr.CancelSendRequest();
			}
		}
		break;
	case ENull:
		break;
	case EExit:
		CreateChildWindow(EEndWindow);
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CMainWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EMenuOpen:
		SelectView(iSelectedIndex);
		break;
	case EMenuHelp:
		CreateChildWindow(EMainHelpWindow);
		break;
	case EMenuAutoUpdate:
		{
			if(iMainEngine.GetLoginType()==0)
			{
				iSUpdateState=1;
				this->InitConnection();
			}
			else
			{
				this->ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_CheckUpdate));
				iHandleAutoUpdate->SendRequest();
			}
		}
		break;
	case EMenuAdvice:
		{
			if(iMainEngine.GetLoginType()==0)
			{
				iSUpdateState=2;
				this->InitConnection();
			}
			else
			{
				CreateChildWindow(EUserAdviseWindow);
			}
		}
		break;
	case EMenuWapPage:
		iMainEngine.LaunchWapBrowswerL(KWAPHOMEPAGE);
		break;
	case EMenuSysmtemInfo:
		CreateChildWindow(ESystemMessageWindow);
		break;
	case EMenuExit:
		PrepareExit();
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
/*
void CMainWindow::SetCurWindow(CWindow* aWindow)
{
	iCurWindow = aWindow;
	if(NULL == iCurWindow)	//返回主界面
	{
		iChildWindow = iBackupWindow;
		SetChildWindow(NULL);
	}
	else
	{
		//为避免基类的模板方法屏蔽操作,将iChild置空
		iBackupWindow = iChildWindow;
		iChildWindow = NULL;
	}
}
*/

/*
void CMainWindow::RequestDraw()
{
	iIsDraw = ETrue;
}
*/

/*
void CMainWindow::DrawOffScreenBitmap(CWindowGc& gc)const
{
	CGraphic& graphic = iMainEngine.Graphic();
	const CFbsBitmap* offScreenBitmap = graphic.GetOffScreenBitmap();

	gc.BitBlt(TPoint(0,0),offScreenBitmap);
}
*/

//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
TBool CMainWindow::HandleIconKeyL(TInt aKeyCode)
{
	UtilityTools::WriteLogsL(_L("CMainWindow::HandleIconKeyL"));
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		if(iSelectedIndex/3>0)
		{
			if((iSelectedIndex/3==iFirstIndex/3)&&iFirstIndex>0)
			{
				iFirstIndex=iFirstIndex-3;
				iScrollbar->Up();
			}

			iSelectedIndex=iSelectedIndex-3;
		}
		else if(iSelectedIndex==0)
		{
			
			iSelectedIndex=(iTotleLine-1)*3+iSelectedIndex;
			if(iSelectedIndex>iMainIconCount-1)
				iSelectedIndex=iMainIconCount-1;

			iFirstIndex=(iSelectedIndex/3-2)*3;
			iScrollbar->End();
		}
		break;
	case KKeyCodeDown:
		if(iSelectedIndex/3<iTotleLine-1)
		{
			if(iSelectedIndex/3==iFirstIndex/3+2)
			{
				iFirstIndex=iFirstIndex+3;
				iScrollbar->Down();
			}

			if((iSelectedIndex+4)>iMainIconCount)
				iSelectedIndex=iMainIconCount-1;
			else
				iSelectedIndex=iSelectedIndex+3;
		}
		else if(iSelectedIndex/3==iTotleLine-1)
		{
			iSelectedIndex=iSelectedIndex-(iTotleLine-1)*3;
			iFirstIndex=0;
			iScrollbar->Home();
		}
		break;
	case KKeyCodeLeft:
		if(iSelectedIndex>0)
		{
			if((iSelectedIndex/3==iFirstIndex/3)&&iFirstIndex>0&&iSelectedIndex%3==0)
			{
				iFirstIndex=iFirstIndex-3;
				iScrollbar->Up();
			}

			iSelectedIndex--;
		}
		else if(iSelectedIndex==0)
		{
			iSelectedIndex=iMainIconCount-1;
			iFirstIndex=(iSelectedIndex/3-2)*3;
			iScrollbar->End();
		}
		break;
	case KKeyCodeRight:
		if(iSelectedIndex<iMainIconCount-1)
		{
			if((iSelectedIndex/3==iFirstIndex/3+2)&&iSelectedIndex%3==2)
			{
				iFirstIndex=iFirstIndex+3;
				iScrollbar->Down();
			}

			iSelectedIndex++;
		}
		else if(iSelectedIndex==iMainIconCount-1)
		{
			iSelectedIndex=0;
			iFirstIndex=0;
			iScrollbar->Home();
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}
	UtilityTools::WriteLogsL(_L("CMainWindow::HandleIconKeyL End"));
	return keyResult;
}

void CMainWindow::DrawAllIcons(CGraphic& aGraphic)const
{

}

void CMainWindow::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	//popUpMenu->AppendItem(_L("Enter"),EEnterModule);

	//打开-帮助-自动升级-意见反馈-WAP主页-系统消息-退出

	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuOpen),EMenuOpen);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuHelp),EMenuHelp);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuAutoUpdate),EMenuAutoUpdate);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuAdvice),EMenuAdvice);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuWapPage),EMenuWapPage);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuSysmtemInfo),EMenuSysmtemInfo);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ECoCoTextRes_MainView_MenuExit),EMenuExit);
}

void CMainWindow::HideApplication()
{
	TApaTask task( CEikonEnv::Static()->WsSession() );
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground();
}

/*
void CMainWindow::PrepareStart()
{
	CWindow* window = iMainEngine.CreateWindow(EStartWindow,this);
	ASSERT(window);
	SetChildWindow(window);
	}
*/

void CMainWindow::PrepareExit()
{
	iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_QExit),*this,EExit,ENull,this);

	//显示一个对话框,在提示确定的情况下执行以下命令
	//CreateChildWindow(EEndWindow);
}

void CMainWindow::EnterModule()
{
	SelectView(iSelectedIndex);
}

/*
void CMainWindow::ShowHelp()
{
	CWindow* window = iMainEngine.CreateWindow(EHelpWindow,this);
	ASSERT(window);
	SetChildWindow(window);	
}
*/

void CMainWindow::InitModuleArray()
{


}

void CMainWindow::InitIconBmp()
{

	iMainIconCount=iBitmapFactory.GetMainIconBmpCount();
	iIconSize=iBitmapFactory.GetMainIconBmpSize(0);

	iTotleLine=iMainIconCount/3;

	if(iMainIconCount%3>0)
	{
		iTotleLine++;
	}

	iFontHeight=iMainEngine.ScreenLayout().FontHeight();
	TRect clientRect = iMainEngine.ScreenLayout().GetFullClientRect();
	clientRect.iTl.iY += iTitleBitmap->SizeInPixels().iHeight;
	iStartPoint = clientRect.iTl;
	iFullClientSize = clientRect.Size();

	iLineW=(iFullClientSize.iWidth-3-iIconSize.iWidth*3)/6;
	iLineH=(iFullClientSize.iHeight-(iIconSize.iHeight+2+iFontHeight)*3)/4;
}
void CMainWindow::DrawIcon(CGraphic& gc) const
{
	TPoint point(0,0);
	TInt x=0;
	TInt y=0;

	TInt width=0;

	TSize size;
	TSize textSize;
	size.iHeight=iLineH/2+iLineH+iIconSize.iHeight+2+iFontHeight;
	size.iWidth=iLineW*2+iIconSize.iWidth;

	TInt index=0;

	gc.SetPenColor(KTextColor);

	for(TInt i=0;i<9;i++)
	{
		index=i+iFirstIndex;
		if((index)==10)
			break;

		width=iMainEngine.ScreenLayout().Font()->MeasureText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index));
		x=i%3;
		y=i/3;
		point=iStartPoint+TPoint((2*x)*iLineW+iIconSize.iWidth*x,(y)*iLineH+(iIconSize.iHeight+2+iFontHeight)*y);

		if(index==iSelectedIndex)
		{
			gc.AlphaBlend(point, iPictureWhite, TRect(size), iColor);
			gc.SetPenColor(KSelectedTextColor);
		}

		point.iX=point.iX+iLineW;
		point.iY=point.iY+iLineH;

		iBitmapFactory.DrawMainIconBmpByID(gc,point,index);

		point.iX=point.iX-iLineW;
		point.iY=point.iY+iIconSize.iHeight;

		textSize.iHeight=iFontHeight+iFontHeight/3;
		textSize.iWidth=size.iWidth;

		if(index==0)
		{
			if(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1).Length()>0)
			{
				gc.DrawText(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1),TRect(point,textSize),CGraphicsContext::ECenter);
			}
			else
			{
				gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index),TRect(point,textSize),CGraphicsContext::ECenter);
			}
		}
		else if(index==1)
		{
			if(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1).Length()>0)
			{
				gc.DrawText(iMainEngine.GetSynchronContent().GetValue(EContentNewDownload,-1,-1),TRect(point,textSize),CGraphicsContext::ECenter);
			}
			else
			{
				gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index),TRect(point,textSize),CGraphicsContext::ECenter);
			}
		}
		else if(index==2)
		{
			if(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1).Length()>0)
			{
				gc.DrawText(iMainEngine.GetSynchronContent().GetValue(EContentHotDownload,-1,-1),TRect(point,textSize),CGraphicsContext::ECenter);
			}
			else
			{
				gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index),TRect(point,textSize),CGraphicsContext::ECenter);
			}
		}
		else if(index==3)
		{
			if(iMainEngine.GetSynchronContent().GetValue(EContentBestDownload,-1,-1).Length()>0)
			{
				gc.DrawText(iMainEngine.GetSynchronContent().GetValue(EContentTypeDownload,-1,-1),TRect(point,textSize),CGraphicsContext::ECenter);
			}
			else
			{
				gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index),TRect(point,textSize),CGraphicsContext::ECenter);
			}
		}
		else
		{
			gc.DrawText(iMainEngine.GetDesById(ECoCoTextRes_MainView_One+index),TRect(point,textSize),CGraphicsContext::ECenter);
		}
		gc.SetPenColor(KTextColor);
	}
}

void CMainWindow::DrawTitle(CGraphic& aGraphic)const
{

	TPoint point(0,0);
	ASSERT(NULL != iTitleBitmap);
	aGraphic.BitBlt(point,iTitleBitmap);
}

void CMainWindow::InitPictureWhite()
{
	TSize size;

	size.iHeight=iLineH/2+iLineH+iIconSize.iHeight+2+iFontHeight;
	size.iWidth=iLineW*2+iIconSize.iWidth;

	if (NULL == iPictureWhite)
	{
		iPictureWhite = new (ELeave) CFbsBitmap;
		iPictureWhite->Create(size,EColor64K);
		CFbsBitmapDevice *pDc = CFbsBitmapDevice::NewL(iPictureWhite);
		CFbsBitGc*	pGc;
		User::LeaveIfError(pDc->CreateContext(pGc));
		pGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
		pGc->SetPenStyle(CFbsBitGc::ESolidPen);
		pGc->SetBrushColor(KMainIconBackgroundColor);
		//pGc->DrawRect(TRect(size));
		pGc->DrawRoundRect(TRect(size),TSize(5,5));
		DELETE(pGc);
		DELETE(pDc);
	}
}
void CMainWindow::SelectView(TInt aIndex)
{
	if(iMainEngine.GetLoginType()==0)
	{
		if(aIndex<6||aIndex==7)
		{
			this->InitConnection();
			return;
		}
	}
	if(aIndex<4)
	{
		if(iSyschr.IsUpdate(0,-1,-1))
		{
			iRequestFirstInfo=ETrue;
			this->ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
			iSyschr.SetObserver(this);
			iSyschr.SendFirstRequest();
			return;
		}
	}
	switch(aIndex)
	{
	case 0:  //精品下载//add by hesanyuan
		{
			CSutraDownload* newestlink = CSutraDownload::NewL(this,iMainEngine);
			this->SetChildWindow(newestlink);
		}
		
		break;
	case 1: //最新下载
		{
			NewestDownload* newestlink = NewestDownload::NewL(this,iMainEngine);
			this->SetChildWindow(newestlink);
		}
		break;
	case 2: //排行榜
		{
			CFriendLink* Friendlink = CFriendLink::NewL(this,iMainEngine);
			this->SetChildWindow(Friendlink);
		}
		break;
	case 3: //分类下载
		{
			CResultView* resultview = CResultView::NewL(this,iMainEngine);
			this->SetChildWindow(resultview);
		}
		break;
	case 4: //搜索
		CreateChildWindow(EMainSearchView);
		break;
	case 5: //下载管理
		CreateChildWindow(EDownloadManagerWindow);
		break;
	case 6: //已下载
		CreateChildWindow(EDownloadedWindow);
		break;
	case 7: //好友推荐
		CreateChildWindow(ERecommendWindow);
		break;
	case 8: //帮助
		CreateChildWindow(EMainHelpWindow);
		break;
	case 9: //系统管理
		CreateChildWindow(ESystemManageWindow);
		break;
	}
}
void CMainWindow::DrawDownLoadIcon(CGraphic& aGraphic) const
{
	TSize iconSize=iBitmapFactory.GetDownLoadIconBmpSize(0);
	TPoint point=iMainEngine.ScreenLayout().GetStatusPaneRect().iTl;
	point.iY=point.iY+(iLogoSize.iHeight-iconSize.iHeight)/2;
	point.iX=point.iX+iLogoSize.iWidth;

	iBitmapFactory.DrawDownLoadIconBmpByID(aGraphic,point,EDownLoadGreen);
}
void CMainWindow::InitSlideText()
{
	TPoint point=iMainEngine.ScreenLayout().GetStatusPaneRect().iTl;
	TSize size=iMainEngine.ScreenLayout().GetStatusPaneRect().Size();

	size.iWidth=size.iWidth-iLogoSize.iWidth-4-iBitmapFactory.GetDownLoadIconBmpSize(0).iWidth;
	point.iX=point.iX+iLogoSize.iWidth+4+iMainEngine.GetBitmapFactory().GetDownLoadIconBmpSize(0).iWidth;

	TBuf<100> tempBuf;
	tempBuf.Append(iMainEngine.GetDesById(ECoCoTextRes_OpenDownload));
	tempBuf.Append(_L("    "));
	tempBuf.Append(iMainEngine.GetDesById(ECoCoTextRes_OpenFileManager));
	tempBuf.Append(_L("    "));
	tempBuf.Append(iMainEngine.GetDesById(ECoCoTextRes_OpenMusicManager));

	iSlideText=iMainEngine.ControlFactory().CreateLeftCycleSlideText(TRect(point,size),tempBuf);
}
void CMainWindow::InitScrollBar()
{
	iScrollbar = new (ELeave)CScrollBar;
	iScrollbar->DefaultLayout();
	iScrollbar->SetVerticalSpace(iFullClientSize.iHeight);
	iScrollbar->SetBeginPostion(iStartPoint+TPoint(iFullClientSize.iWidth-3,0));
	iScrollbar->SetCursorColor(KScrollSpaceColor);
	iScrollbar->SetBackgroundColor(KScrollBackgroundColor);

	iScrollbar->SetMaxLinePerPage(iMaxLine);
	iScrollbar->SetTotalLineNum(iTotleLine);
	iScrollbar->Layout();
}

void CMainWindow::InitTitleBar()
{
	//iTitleBitmap = iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpTitlebg);
	iTitleBitmap = iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpLogotitlebg);
	
	iTitleLogoBmp = iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpLogo1);

	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
	TSize screenSize = iMainEngine.ScreenLayout().ScreenSize();

	iLogoSize = iTitleLogoBmp->SizeInPixels();
}
//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CMainWindow::HandleResponseEvent(TInt aEvent,TInt aType)
{
	if(iDialog)
		RemoveAndDeleteControl(iDialog);

	if(aEvent==EHttpNoError)
	{
		switch(aType)
		{
			case EHttpAutoUpdate:
			{
				if(iHandleAutoUpdate->IsUpdate())
				{
					this->AddDownLoadList(*this,EDialogDownLoad,iHandleAutoUpdate->GetUpdateUrl(),_L("*#soarsky*#"),0,0,1,1);
				}
				else
				{
					this->ShowDialog(iHandleAutoUpdate->GetInfo(),EDialogUpdate);
				}
				break;
			}
			case EHttpLogin:
				HandleLoginState();
				break;
			case EHttpFirstContentUpdate:
				{
					if(iRequestFirstInfo)
					{
						iRequestFirstInfo=EFalse;
						SelectView(iSelectedIndex);
					}
					else
					{
						HandleSynchronState();
					}
				}
				break;
			case EHttpSystemInfo:
				HandleSystemInfo();
				break;			
		}
	}
	else
	{
		if(aType==EHttpAutoUpdate)
		{
			this->ShowDialog(iHandleAutoUpdate->GetInfo(),EDialogUpdate);
		}
		else if(aType==EHttpLogin||aType==EHttpFirstContentUpdate||aType==EHttpSystemInfo)
		{
			if(iSUpdateState==1)
			{
				iSUpdateState=0;
				this->ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_CheckUpdate));
				iHandleAutoUpdate->SendRequest();
			}
			else if(iSUpdateState==2)
			{
				iSUpdateState=0;
				CreateChildWindow(EUserAdviseWindow);
			}
			else
			{
				SelectView(iSelectedIndex);
			}
		}
	}
}
void CMainWindow::ShowWaitDialog(const TDesC& aText)
{
	iDialog=iMainEngine.ControlFactory().CreateWaitDialog(aText,*this,EDialogWaiting,this);
}
void CMainWindow::ShowDialog(const TDesC& aText,TInt aType)
{
	if(aType==EDialogLoginUpdate)
	{
		iDialog=iMainEngine.ControlFactory().CreateInquireDialog(aText,*this,aType,EDialogLoginUpdateRight,this);
	}
	else
	{
		iDialog=iMainEngine.ControlFactory().CreateTipDialog(aText,*this,aType,this);
	}
}
void CMainWindow::InitConnection()
{
	ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_Conneting));
	
	iMainEngine.InitDownLoadEngine();
	iMainEngine.InitHttpManager();

	iStatueFlag=EMainViewLoginGateWay;

	iSystemInfo=CHandleSystemInfo::NewL(*this,iMainEngine);
	iLogin=CHandleLoginOrExit::NewL(*this,iMainEngine);
	iLogin->AddLoginRequest();

	iHandleWeather=CHandleWeather::NewL(iMainEngine);
	iHandleWeather->AddRequest();

	iHandleAutoUpdate=CHandleAutoUpdate::NewL(*this,iMainEngine);

	iMainEngine.SetLoginTypeL(1);
}
void CMainWindow::HandleLoginState()
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
		iStatueFlag=EMainViewLoginAppUpdate;
		
		ShowDialog(iMainEngine.GetDesById(ETurkeyTextRes_Upgrate),EDialogLoginUpdate);
	}
	else
	{
		HandleSynchronState();
	}
}
void CMainWindow::HandleSynchronState()
{
	if((iLogin->IsContentUpdate()&&iStatueFlag!=EMainViewLoginSyschron)||iSyschr.IsUpdate(0,-1,-1))
	{
		iStatueFlag=EMainViewLoginSyschron;
		ShowWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_SystemUpdating));
		iSyschr.SetObserver(this);
		iSyschr.SendFirstRequest();
	}
	else
	{
		HandleSystemInfo();
	}
}
void CMainWindow::HandleSystemInfo()
{
	if(iLogin->IsSystemInfo()&&iStatueFlag!=EMainViewLoginSystemInfo)
	{
		iStatueFlag=EMainViewLoginSystemInfo;
		//显示系统信息

		if(iSystemInfoBool)
			ShowInfoDialog(iLogin->GetTitle(),iLogin->GetSystemInfo(),0);
		else
			ShowInfoDialog(iLogin->GetTitle(),iLogin->GetSystemInfo(),1);
	}
	else 
	{
		iSystemInfoBool=iSystemInfo->IsNextSystemInfo();
		if(iSystemInfo->IsNextSystemInfo())
			ShowInfoDialog(iSystemInfo->GetTitle(),iSystemInfo->GetSystemInfo(),0);
		else
			ShowInfoDialog(iSystemInfo->GetTitle(),iSystemInfo->GetSystemInfo(),1);
	}
}