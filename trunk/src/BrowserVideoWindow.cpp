
#include <avkon.rsg>
#include <StringLoader.h> 
#include "soarsky.rsg"

#include "BrowserVideoWindow.h"
#include "MusicPlayManageEngine.h"
#include "VideoPlayEngine.h"
#include "Dialog.h"
#include "TitleBar.h"
#include "MainEngine.h"
#include "Graphic.h"
#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "ControlFactory.h"
#include "CoCoPreDefine.h"

#include "CoCoappui.h"
#include "CoCoappview.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum TDialogEventType
{
	ECmdNull,
	ECmdRePlay
};

CBrowserVideoWindow::CBrowserVideoWindow(CWindow* aParent,CMainEngine& aMainEngine)
:CWindow(EOpenFileWindow_Video,aParent,aMainEngine),
iMainEngine(aMainEngine),
iRightStop(true),
iVideo(NULL)
{
}

CBrowserVideoWindow::~CBrowserVideoWindow()
{
	delete iVideo; iVideo=NULL;
	delete iMinSecFormatString; iMinSecFormatString=NULL;
	delete iHourMinSecFormatString; iHourMinSecFormatString=NULL;
}

CBrowserVideoWindow* CBrowserVideoWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserVideoWindow* self = new (ELeave)CBrowserVideoWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CBrowserVideoWindow* CBrowserVideoWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserVideoWindow* self=CBrowserVideoWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CBrowserVideoWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	iShowControlText=EFalse;

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;

	iTitleBar=iMainEngine.ControlFactory().CreateDefaultTitleBarL(_L(""));
}

void CBrowserVideoWindow::DoActivateL()
{
/*
	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
	TRegionFix<2> region;
	region.AddRect(screenLayout.TitleBarRect());
	region.AddRect(screenLayout.GetControlPaneRect());
	iMainEngine.SetClippingRegion(region);
*/
	//iMainEngine.SetClippingRect(TRect(0,0,0,0));
//	iMainEngine.SetClippingRect(iMainEngine.ScreenLayout().ScreenSize());

	//CTitleBar* titleBar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle);
}

void CBrowserVideoWindow::DoDeactivate()
{
//	iMainEngine.CancelClipping();
	//DeleteAllControl();
}

void CBrowserVideoWindow::DoDraw(CGraphic& aGraphic) const
{
	if(iVideo && (iVideo->iMyState==EPPlaying||iVideo->iMyState==EPPaused))
	{
		const CFont* pFont=aGraphic.GetFont();
		aGraphic.SetPenColor(TRgb(KRgbBlack));

		TInt nWidth = pFont->TextWidthInPixels(iFileName);

		if(nWidth>iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2)
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2,0), iFileName);
		else
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX-nWidth,0), iFileName);

		nWidth = pFont->TextWidthInPixels(iTime);

		if(nWidth>iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2)
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2,iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iY-iMainEngine.ScreenLayout().LineHeight() ), iTime);
		else
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX-nWidth,iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iY-iMainEngine.ScreenLayout().LineHeight()), iTime);
	}

	if(iVideo && (iVideo->iMyState==EPPaused || iVideo->iMyState==EPStopped))
	{
		TBuf<100> nInfo;
		nInfo.Zero ();

		if (iVideo->iMyState==EPPaused)
		{
			nInfo.Append (iMainEngine.GetDesById(ECoCoTextRes_PlayVideo_Paused));
		} 
		else
		{
			nInfo.Append (iMainEngine.GetDesById(ECoCoTextRes_PlayVideo_Stopped));
		}

		const CFont* pFont=aGraphic.GetFont();
		TInt nWidth= pFont->TextWidthInPixels(nInfo);

		TPoint nCenter;
		nCenter.iX=iMainEngine.ScreenLayout().ScreenSize().iWidth/2-nWidth/2;
		nCenter.iY=(iMainEngine.ScreenLayout().GetClientRect().iBr.iY -iMainEngine.ScreenLayout().GetClientRect().iTl.iY)/2+iMainEngine.ScreenLayout().GetClientRect().iTl.iY;      
		
		aGraphic.SetPenColor(TRgb(255,255,255));
		aGraphic.SetPenStyle(CFbsBitGc::ESolidPen);
		aGraphic.DrawText(nCenter,nInfo);
	}
}

TBool CBrowserVideoWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;

	switch(aKeyCode)
	{
	case KKeyCodeUp:
	case KKeyCodeRight:
		if(iVideo)
		{
			if (iVideo->iMyState==EPPlaying)
			{
				iVideo->IncVolume();

				keyResult = EFalse;
			}
		}
		break;

	case KKeyCodeDown:
	case KKeyCodeLeft:	
		if(iVideo)
		{
			if (iVideo->iMyState==EPPlaying)
			{
				iVideo->DecVolume();

				keyResult = EFalse;
			}
		}
		break;
	
	case KOKKey:	
		if(iVideo==NULL)
		{
			Play();
		}
		else
		{
			switch(iVideo->iMyState)
			{
			case EPNotInitialised:
			case EPStopped:
			case EPPaused:
				{
					Play();
					break;
				}
			case EPPlaying:
				{
					Pause();
					break;
				}
			}
		}
		break;

	case KLeftSoftKey:
		if(iVideo==NULL || iIsPlayError==true)
		{
			Play();
		}
		else
		{
			switch(iVideo->iMyState)
			{
			case EPNotInitialised:
			case EPStopped:
			case EPPaused:
				{
					Play();
					break;
				}
			case EPPlaying:
				{
					Pause();
					break;
				}
			}
		}
		break;
	case KRightSoftKey:
		if(iRightStop==false || iIsPlayError==true)
		{
			iRightStop=true;

			//((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->MyDraw();

			if(iVideo)
			{
				if(iIsPlayError==false)
					Stop();

				/*delete iVideo; 
				iVideo=NULL;		*/
			}

			//20070410-张晖增加恢复音乐播放
			iMainEngine.MusicPlayManageEngine().RestoreAudioPlay (); 

			GoBackToParentWindow();
		}
		else
		{
			SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
			SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Play));

			iRightStop=false;
	
			//Pause();
			if(iVideo)
			{
				if(iIsPlayError==false)
					Stop();

				/*delete iVideo; 
				iVideo=NULL;		*/
			}
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}

	return keyResult;
}

TBool CBrowserVideoWindow::DoHandleCommandL(TInt aCommand)
{
	if (aCommand==KKeyForegroundEvent)
	{
		//程序状态更改为前台
		if(iVideo)
		{
			if(iVideo->iMyState==EPPaused && iSystemPaused==true)
			{
				Play();
			}
		}
	} 
	else if(aCommand==KKeyBackgroundEvent || aCommand==KKeyPhoneIncomingEvent)
	{
		//程序状态更改为后台
		if(iVideo)
		{
			if(iVideo->iMyState==EPPlaying)
			{
				Pause();
				iSystemPaused=true;
			}
		}
	}

	return EFalse;
}

void CBrowserVideoWindow::DoSizeChanged()
{

}

void CBrowserVideoWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	ResetLeftButton();
	ResetRightButton();
	SetLeftButtonL(textRes[ETurkeyTextRes_Pause]);
	SetRightButtonL(textRes[ETurkeyTextRes_Stop]);
}

void CBrowserVideoWindow::SetFileName(const TDesC& aFileName)
{
	if (aFileName.Length()) 
	{
		TParse parse;
		parse.Set(aFileName,NULL,NULL);
		iFileName.Copy(parse.NameAndExt());

		if (iFileName.Length()>=10)
		{
			TBuf<20> nFile;
			nFile.Copy(iFileName.Left(10));
			nFile.Append(_L("..."));

			iTitleBar->SetTextL(nFile);
		}	
		else
		{
			iTitleBar->SetTextL(iFileName);
		}
		//iTitleBar->SetTextL(iFileName);
		
		iFullFileName.Delete(0,iFullFileName.Length());
		iFullFileName.Append(aFileName);

		//20070410-张晖增加中止音乐播放
		iMainEngine.MusicPlayManageEngine().PauseAudioPlay();; 

		InitVideo(iFullFileName);
	}
}

/*--------------------------- video --------------------------------*/
void CBrowserVideoWindow::InitVideo(const TDesC& aFileName)
{
	delete iVideo; iVideo=NULL;
	iVideo=CVideoPlayEngine::NewL();
	delete iMinSecFormatString;
	delete iHourMinSecFormatString;

	iMinSecFormatString=CEikonEnv::Static()->AllocReadResourceL(R_QTN_TIME_DURAT_MIN_SEC); 
	iHourMinSecFormatString=CEikonEnv::Static()->AllocReadResourceL(R_QTN_TIME_DURAT_LONG);
	iVideo->SetNewFileL(aFileName);
	RWsSession& wsSession=CCoeEnv::Static()->WsSession();
	CWsScreenDevice& device=*(CCoeEnv::Static()->ScreenDevice());
	RWindowBase& win=((CCoCoAppUi*)CEikonEnv::Static()->EikAppUi())->Container()->GetWindow();
	
	TRect rect =iMainEngine.ScreenLayout().WhiteBackgroundRect();//iMainEngine.ScreenLayout().GetClientRect() ;
	TRect rect1 = iMainEngine.ScreenLayout().WhiteBackgroundRect();//iMainEngine.ScreenLayout().GetClientRect();
	iVideo->InitControllerL(this,wsSession,device,win,rect,rect1);
}

void CBrowserVideoWindow::Play()
{
	iIsPlayError=false;
	if(iVideo==NULL || iVideo->iMyState==EPStopped)
	{
		InitVideo(iFullFileName);
		return;
	}

	iVideo->DoPlayL();
	iSystemPaused=false;

	ResetLeftButton();
	ResetRightButton();
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Pause));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Stop));

	iRightStop=true;
}

void CBrowserVideoWindow::Pause()
{
	if(!iVideo)
		return;

	iVideo->DoPauseL();

	ResetLeftButton();
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Play));
}

void CBrowserVideoWindow::Stop()
{
	if(!iVideo)
		return;

	iVideo->DoStop();

	//((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->StartPeriodic();
}

void CBrowserVideoWindow::InitControllerCompletedL(TInt aError)
{
	if(aError!=KErrNone)
	{
		iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_Video_PlayError),*this,ECmdRePlay,ECmdNull);

		ResetLeftButton();
		ResetRightButton();
		SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Play));
		SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));

		iIsPlayError=true;
	}
	else 
	{
		ResetLeftButton();
		ResetRightButton();
		SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Pause));
		SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Stop));
		
		iRightStop=true;
		//((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->MyDraw();
		iVideo->DoPlayL();
		//((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->StopPeriodic();
	}

	iMainEngine.RequestDraw();
}

void CBrowserVideoWindow::PlayCompletedL(TInt aError)
{
	if(aError!=KErrNone)
	{
		iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_Video_PlayError),*this,ECmdRePlay,ECmdNull);

		ResetLeftButton();
		ResetRightButton();
		SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Play));
		SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));

		iIsPlayError=true;
	}
	else
	{
		ResetLeftButton();
		ResetRightButton();
		SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Play));
		SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));

		iRightStop=false;

		//((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->MyDraw();
	}

	iMainEngine.RequestDraw();
}

void CBrowserVideoWindow::PlaybackPositionChangedL(TInt64 aPlaybackPosInSeconds,TInt64 aTotalLengthInSeconds)
{
	TTime posTime=TTime(aPlaybackPosInSeconds*KMPOneSecond);
	TTime durTime=TTime(aTotalLengthInSeconds*KMPOneSecond);

	TBuf<16> pos;
	TBuf<16> dur;

	if(aTotalLengthInSeconds>0 && aTotalLengthInSeconds<KOneHourInSeconds)
	{
		posTime.FormatL(pos,*iMinSecFormatString);
		durTime.FormatL(dur,*iMinSecFormatString);     
	}
	else
	{
		posTime.FormatL(pos,*iHourMinSecFormatString);
		aTotalLengthInSeconds=0;
	}

	if(aTotalLengthInSeconds>0)
	{
		/*CDesCArrayFlat* strings=new CDesCArrayFlat(2);
		CleanupStack::PushL(strings);
		strings->AppendL(pos);
		strings->AppendL(dur);
		HBufC* stringholder=StringLoader::LoadLC(R_VIDEO_NAVI_TIME,*strings,CEikonEnv::Static());
		
		iTime.Copy(*stringholder);
		CleanupStack::PopAndDestroy(2);*/
	}    
	else
	{
		iTime.Copy(pos);
	}
	/*((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->Invalidate(iMainEngine.ScreenLayout().GetStatusPaneRect() );
	((CsoarskyAppUi*)CEikonEnv::Static()->EikAppUi())->iAppView->Invalidate(iMainEngine.ScreenLayout().GetControlPaneRect() );*/
}

void CBrowserVideoWindow::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdNull:
		break;

	case ECmdRePlay:
		Play();
		break;

	default:
		iMainEngine.WriteLog16(_L("DialogEvent-2"));
		ASSERT(EFalse);
		break;
	}
	iMainEngine.WriteLog16(_L("DialogEvent-3"));
}