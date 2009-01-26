
#include "BrowserMusicWindow.h"
#include "TypeDefine.h"
#include "PopUpMenu.h"

#include "PreDefine.h"
#include "CoCoPreDefine.h"
#include "MainEngine.h"
#include "Graphic.h"
#include "ListBox.h"
#include "ScreenLayout.h"
#include "ControlFactory.h"
#include "UtilityTools.h"
//#include "MusicPlayManageEngine.h"
#include "BitmapFactory.h"
#include "cocobmp.h"
#include "ListBoxItem.h"

enum TEventPopCommand
{
	ERandomPlay,
	ESortPlay,
	EUpMusic,
	ENextMusic,
	EPauseMusic,
	EPlayMusic,
	EStopMusic,
	EHaveVol,
	ENoVol,
	EDownVol,
	EUpVol,
	EClose
};

CBrowserMusicWindow::CBrowserMusicWindow(CWindow* aParent,CMainEngine& aMainEngine)
:CWindow(EOpenFileWindow_Music,aParent,aMainEngine),
iMainEngine(aMainEngine),
iPercent(0),
iCurrentPlayIndex(0),
iListBox(NULL)
{
}

CBrowserMusicWindow::~CBrowserMusicWindow()
{
	iMainEngine.MusicPlayManageEngine().SetAudioPlayNoticeWindow(NULL);

	delete iMusicMainImage;
	delete iMusicVoiceImage;
}

CBrowserMusicWindow* CBrowserMusicWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserMusicWindow* self = new (ELeave)CBrowserMusicWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CBrowserMusicWindow* CBrowserMusicWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserMusicWindow* self=CBrowserMusicWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CBrowserMusicWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	iFileNameArray=iMainEngine.MusicPlayManageEngine().GetFileName();

	InitImage();

	iMainEngine.MusicPlayManageEngine().SetAudioPlayNoticeWindow(this);

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;

	iCurrentTime.Zero();
	iCurrentTime.Append(_L("00:00"));

	iTotalTime.Zero();
	iTotalTime.Append(_L("00:00"));

	InitListBox();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CBrowserMusicWindow::DoActivateL()
{
	//CTitleBar* titleBar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle);
}

void CBrowserMusicWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CBrowserMusicWindow::DoDraw(CGraphic& aGraphic) const
{
	InitMusicInterface(aGraphic);
}

TBool CBrowserMusicWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;

	//如果没有歌曲则不处理按键操作
	//除了右软键（返回）
	if (iFileNameArray->Count()==0 && aKeyCode!=KRightSoftKey)
	{
		return keyResult;
	}

	switch(aKeyCode)
	{
		case KKeyCodeUp:
		case KKeyCodeDown:

			break;
		case '3':
		case KKeyCodeRight:
			DoPopUpMenuCommand(EUpVol);
			break;
		case '1':
		case KKeyCodeLeft:
			DoPopUpMenuCommand(EDownVol);
			break;
		case KOKKey:	
			{
				CNormalListBoxItem& item = (CNormalListBoxItem&)iListBox->CurItem();

				iMainEngine.MusicPlayManageEngine().PlaySelectMusic(iListBox->CurItemIndex(),item.GetText());
				break;
			}
		case KLeftSoftKey:
			InitPopUpMenu();
			break;
		case KRightSoftKey:
			GoBackToParentWindow();

			return keyResult;
			break;
		case '0':
			if(iMainEngine.MusicPlayManageEngine().GetIsPlay()==1)
			{
				DoPopUpMenuCommand(EPauseMusic);
			}
			else
			{
				DoPopUpMenuCommand(EPlayMusic);
			}
			break;
		case '4':
			DoPopUpMenuCommand(EUpMusic);
			break;
		case '5':
			if(iMainEngine.MusicPlayManageEngine().GetPlayType()==0)
			{
				DoPopUpMenuCommand(ERandomPlay);
			}
			else
			{
				DoPopUpMenuCommand(ESortPlay);
			}
			break;
		case '6':
			DoPopUpMenuCommand(ENextMusic);
			break;
		case '7':
			if(iMainEngine.MusicPlayManageEngine().GetIsSlice())
			{
				DoPopUpMenuCommand(EHaveVol);
			}
			else
			{
				DoPopUpMenuCommand(ENoVol);
			}
			break;
		case '8':
			DoPopUpMenuCommand(EStopMusic);
			break;
		case '9':
			DoPopUpMenuCommand(EClose);

			return keyResult;
			break;
		default:
			keyResult = EFalse;
			break;
	}
	
	//特殊处理
	if (iMainEngine.MusicPlayManageEngine().GetIsPlay())
	{
		iListBox->GetItem(iCurrentPlayIndex).SetTextColor(KRgbRed);
	}

	return keyResult;
}

TBool CBrowserMusicWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CBrowserMusicWindow::DoSizeChanged()
{

}

void CBrowserMusicWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//========================================================================================================
void CBrowserMusicWindow::InitImage()
{
	iMusicVoiceImage=iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpMusicvolume);

	iMusicMainImage=iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpMusicbackground);
}

void CBrowserMusicWindow::InitPopUpMenu()
{

	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	if(iMainEngine.MusicPlayManageEngine().GetPlayType()==0)
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_RandomPlay),ERandomPlay);
	else
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_SortPlay),ESortPlay);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_UpMusic),EUpMusic);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_NextMusic),ENextMusic);

	if (iMainEngine.MusicPlayManageEngine().GetIsPlay()==1)
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_Pause),EPauseMusic);
	else
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_Play),EPlayMusic);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_Stop),EStopMusic);

	if(iMainEngine.MusicPlayManageEngine().GetIsSlice())
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_HaveVol),EHaveVol);
	else
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_NoVol),ENoVol);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_DownVol),EDownVol);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_UpVol),EUpVol);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_MusicMenu_Close),EClose);
}

void CBrowserMusicWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case ERandomPlay:
		{
			//随机播放
			iMainEngine.MusicPlayManageEngine().SetPlayType(1);
		}
		break;
	case ESortPlay:
		{
			//顺序播放
			iMainEngine.MusicPlayManageEngine().SetPlayType(0);
		}
		break;
	case EUpMusic:
		{
			//上一首
			iMainEngine.MusicPlayManageEngine().PlayUpMusic();
		}
		break;
	case ENextMusic:
		{
			//下一首
			iMainEngine.MusicPlayManageEngine().PlayNextMusic();
		}
		break;
	case EPauseMusic:
		{
			//中止
			iMainEngine.MusicPlayManageEngine().AudioPause();
		}
		break;
	case EPlayMusic:
		{
			//播放
			iMainEngine.MusicPlayManageEngine().PlayMusic();
		}
		break;
	case EStopMusic:
		{
			//停止
			iMainEngine.MusicPlayManageEngine().AudioStop();
		}
		break;
	case EHaveVol:
		{
			//恢复音量
			iMainEngine.MusicPlayManageEngine().SetSliceVolume(false);
		}
		break;
	case ENoVol:
		{
			//静音
			iMainEngine.MusicPlayManageEngine().SetSliceVolume(true);
		}
		break;
	case EDownVol:
		{
			//放小音量
			iMainEngine.MusicPlayManageEngine().DecVolume();
		}
		break;
	case EUpVol:
		{
			//放大音量
			iMainEngine.MusicPlayManageEngine().IncVolume();
		}
		break;
	case EClose:
		{
			//关闭
			iMainEngine.MusicPlayManageEngine().AudioStop();

			GoBackToParentWindow();
		}
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}

void CBrowserMusicWindow::InitListBox()
{
	if (iListBox)
	{
		RemoveAndDeleteControl(iListBox);
	}

	TRect nArea;
	nArea.iTl=TPoint(0,0);
	nArea.iBr=TPoint(iMainEngine.ScreenSize().iWidth ,iMainEngine.ScreenLayout().GetControlPaneRect().iTl.iY-iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpMusicbackground)->SizeInPixels().iHeight-4);

	iListBox = iMainEngine.ControlFactory().CreateListBox(nArea);
	iListBox->SetTextColor(KRgbWhite,KRgbBlack);
	iListBox->SetUseHotKey(false);

	//iMainEngine.CurWindow()->RemoveControl(iListBox);
	for (TInt i = 0 ; i < iFileNameArray->Count() ; i++)
	{
		const CFbsBitmap* icon = NULL;
		const CFbsBitmap* iconMask = NULL;

		icon = iMainEngine.GetBitmapFactory().GetFileTypeIcon(6);
		iconMask = iMainEngine.GetBitmapFactory().GetFileTypeIconMask(6);

		iListBox->AppendL(CNormalListBoxItem::NewL((*iFileNameArray)[i],icon,iconMask));
	}

	iListBox->Layout();

	if (iMainEngine.MusicPlayManageEngine().GetIsPlay())
	{
		iCurrentPlayIndex=iMainEngine.MusicPlayManageEngine().GetPlayIndex();

		iListBox->GetItem(iCurrentPlayIndex).SetTextColor(KRgbRed);
	}

	iMainEngine.RequestDraw();
}

void CBrowserMusicWindow::InitMusicInterface(CGraphic& aGraphic) const
{
	//播放界面
	//播放界面主框
	TPoint nX;
	
	nX.iX=(iMainEngine.ScreenSize().iWidth-iMusicMainImage->SizeInPixels().iWidth)/2;

	nX.iY=iMainEngine.ScreenLayout().GetControlPaneRect().iTl.iY-iMusicMainImage->SizeInPixels().iHeight-2;

	aGraphic.BitBlt(nX,iMusicMainImage);

	//播放进度条
	nX.iY=nX.iY+iMainEngine.ScreenLayout().FontHeight()+iMainEngine.ScreenLayout().FontHeight()/3;
	
	nX.iX=(iMusicMainImage->SizeInPixels().iWidth-iMainEngine.GetBitmapFactory().GetMusicTimeIconBmpSize().iWidth)/2;

	iMainEngine.GetBitmapFactory().DrawMusicTimeIconBmpByID(aGraphic,nX,1);

	TRect nPlayPercent;

	const CFbsBitmap* nMusicPlayPercent = NULL;
	const CFbsBitmap* nMusicPlayPercentMask = NULL;

	nMusicPlayPercent=iMainEngine.GetBitmapFactory().GetMusicTimeIcon(0);
	nMusicPlayPercentMask=iMainEngine.GetBitmapFactory().GetMusicTimeIconMask(0);

	nPlayPercent.iTl=TPoint(0,0);
	nPlayPercent.iBr.iY=nMusicPlayPercent->SizeInPixels().iHeight;
	nPlayPercent.iBr.iX=nMusicPlayPercent->SizeInPixels().iWidth*iPercent/100;
	
	aGraphic.BitBltMasked(nX,nMusicPlayPercent,nMusicPlayPercentMask,nPlayPercent);
	/*aGraphic.BitBlt(nX,nMusicPlayPercent,nPlayPercent);*/

	//显示播放时间
	TPoint nTime;

	nTime=nX;
	nTime.iX=nTime.iX+iMainEngine.ScreenLayout().FontHeight()/2;
	nTime.iY=nTime.iY-iMainEngine.ScreenLayout().FontHeight()*5/4;

	aGraphic.SetPenColor(TRgb(255,255,255));
	aGraphic.SetPenStyle(CFbsBitGc::ESolidPen);
	aGraphic.DrawText(nTime,iCurrentTime);

	nTime.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicTimeIconBmpSize(0).iWidth-iMainEngine.ScreenLayout().Font()->TextWidthInPixels(iTotalTime)-iMainEngine.ScreenLayout().FontHeight()/2;
	aGraphic.DrawText(nTime,iTotalTime);

	//播放、上一首、下一首按钮
	TPoint nNum;

	nX.iY=nX.iY+iMainEngine.ScreenLayout().FontHeight();

	iMainEngine.GetBitmapFactory().DrawMusicSelectIconBmpByID(aGraphic,nX,0);
	
	nNum=nX;
	nNum.iX=nNum.iX+iMainEngine.GetBitmapFactory().GetMusicSelectIconBmpSize(0).iWidth;
	nNum.iY=nNum.iY+iMainEngine.GetBitmapFactory().GetMusicSelectIconBmpSize(0).iHeight/2-2;
	aGraphic.DrawText(nNum,_L("4"));

	nX.iY=nX.iY-iMainEngine.ScreenLayout().FontHeight()/4;
	nX.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicSelectIconBmpSize(0).iWidth+iMainEngine.ScreenLayout().FontHeight();

	if (iMainEngine.MusicPlayManageEngine().GetIsPlay()==1)
	{
		iMainEngine.GetBitmapFactory().DrawMusicPlayIconBmpByID(aGraphic,nX,2);
	} 
	else
	{
		iMainEngine.GetBitmapFactory().DrawMusicPlayIconBmpByID(aGraphic,nX,0);
	}
	
	nNum.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicPlayIconBmpSize(0).iWidth;

	aGraphic.DrawText(nNum,_L("0"));

	nX.iY=nX.iY+iMainEngine.ScreenLayout().FontHeight()/4;
	nX.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicPlayIconBmpSize(0).iWidth+iMainEngine.ScreenLayout().FontHeight();

	iMainEngine.GetBitmapFactory().DrawMusicSelectIconBmpByID(aGraphic,nX,2);

	nNum.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicSelectIconBmpSize(2).iWidth;
	aGraphic.DrawText(nNum,_L("6"));

	//静音提示
	nX.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicSelectIconBmpSize(2).iWidth+iMainEngine.ScreenLayout().FontHeight()*2;
	nX.iY=nX.iY+iMainEngine.ScreenLayout().FontHeight()/4;

	if(iMainEngine.MusicPlayManageEngine().GetIsSlice())
	{
		iMainEngine.GetBitmapFactory().DrawMusicBugleIconBmpByID(aGraphic,nX,1);
	}
	else
	{
		iMainEngine.GetBitmapFactory().DrawMusicBugleIconBmpByID(aGraphic,nX,0);
	}

	//音量大小
	nX.iX=nX.iX+iMainEngine.GetBitmapFactory().GetMusicBugleIconBmpSize(0).iWidth+iMainEngine.ScreenLayout().FontHeight()/2;
	nX.iY=nX.iY-iMainEngine.ScreenLayout().FontHeight()/4;
	
	TRect nVoiceRect;

	nVoiceRect.iTl=TPoint(0,0);
	nVoiceRect.iBr.iY=iMusicVoiceImage->SizeInPixels().iHeight;

	if (iMainEngine.MusicPlayManageEngine().GetMaxVoice()==0)
	{
		nVoiceRect.iBr.iX=iMusicVoiceImage->SizeInPixels().iWidth/2;
	}
	else
	{
		nVoiceRect.iBr.iX=iMusicVoiceImage->SizeInPixels().iWidth*iMainEngine.MusicPlayManageEngine().GetCurVoice()/iMainEngine.MusicPlayManageEngine().GetMaxVoice();
	}

	aGraphic.BitBlt(nX,iMusicVoiceImage,nVoiceRect);
}

void CBrowserMusicWindow::AudioUpdate(const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent) 
{
	iCurrentTime.Delete(0, iCurrentTime.Length());
	iCurrentTime.Append(aCurrentTime);

	iTotalTime.Delete(0, iTotalTime.Length());
	iTotalTime.Append(aTotalTime);

	iPercent = aPercent;

	iMainEngine.RequestDraw();
}

void CBrowserMusicWindow::AudioSelected(const TInt aSelectIndex)
{
	iListBox->GetItem(iCurrentPlayIndex).SetTextColor(KRgbWhite);

	iCurrentPlayIndex=aSelectIndex;

	iListBox->GetItem(iCurrentPlayIndex).SetTextColor(KRgbRed);

	iMainEngine.RequestDraw();
}

void CBrowserMusicWindow::AudioFileUpdate()
{
	iFileNameArray=iMainEngine.MusicPlayManageEngine().GetFileName();

	iCurrentTime.Zero();
	iCurrentTime.Append(_L("00:00"));

	iTotalTime.Zero();
	iTotalTime.Append(_L("00:00"));

	InitListBox();	
}