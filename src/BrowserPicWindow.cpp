
#include <bitstd.h>

#include "PicEngine.h"
#include "BrowserPicWindow.h"
#include "TypeDefine.h"
#include "PopUpMenu.h"
#include "Dialog.h"
#include "PreDefine.h"
#include "CoCoPreDefine.h"
#include "MainEngine.h"
#include "Graphic.h"
#include "TitleBar.h"
#include "ScreenLayout.h"
#include "ControlFactory.h"
#include "UtilityTools.h"
#include "ImageManipAdapter.h" 

#ifdef __SERIES60_3X__
#include "aknswallpaperutils.h"
#else
#include "SharedDataI.h"
#endif

const TInt KJpgQuality = 65;

_LIT(KMBM,".mbm");

_LIT8(KJpgMime, "image/jpeg");
_LIT8(KBmpMime, "image/bmp");
_LIT8(KGifMime, "image/gif");
_LIT8(KMbmMime,"image/mbm");

_LIT(kIdleImgKey, "IdleImg");
_LIT(kIdleIndexKey, "IdleIndex");
_LIT(kDimIndexKey, "DimIndex");
_LIT(kIdlePathKey, "IdlePath");
_LIT(kIdlePathValueKey, "c:\\system\\data\\backgroundimage.mbm");

enum TEventPopCommand
{
	EPopUp,
	EPopNext,
	EPopSetWallPaper,
	EPopScreenShow
};

CBrowserPicWindow::CBrowserPicWindow(CWindow* aParent,CMainEngine& aMainEngine)
:CWindow(EOpenFileWindow_Pic,aParent,aMainEngine),
iMainEngine(aMainEngine),
iFullScreenShow(EFalse),
iSelIndex(0),
iOpenComplete(ETrue),
iIsAnimation (EFalse),
iTitleBar(NULL),
iShowBmp(NULL),
iAdapter(NULL),
iDisplayModel(CCoeEnv::Static()->ScreenDevice()->DisplayMode())
{
	
}

CBrowserPicWindow::~CBrowserPicWindow()
{
	
	DELETE(iAdapter);
	CLEAR(iShowBmp);

	delete iFileNameArray;

	iFileNameArray=NULL;
}

CBrowserPicWindow* CBrowserPicWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserPicWindow* self = new (ELeave)CBrowserPicWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CBrowserPicWindow* CBrowserPicWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CBrowserPicWindow* self=CBrowserPicWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CBrowserPicWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);
	
	iTitleBar=iMainEngine.ControlFactory().CreateDefaultTitleBarL(_L(""));

	iShowControlText=EFalse;

	//不允许弹出0*#界面
	iCanPopUpWindow=EFalse;

	SetShowRect(iMainEngine.ScreenLayout().WhiteBackgroundRect());
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CBrowserPicWindow::DoActivateL()
{
	//CTitleBar* titleBar = iMainEngine.ControlFactory().CreateDefaultTitleBarL(*iTitle);
}

void CBrowserPicWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CBrowserPicWindow::DoDraw(CGraphic& aGraphic) const
{
	if(iFullScreenShow)
	{
		DrawBackground(aGraphic);

		aGraphic.DrawRect( iMainEngine.ScreenLayout().ScreenSize());

		if(NULL != iShowBmp)   
		{
			TRect rect(0,0,0,0);
			TRect rect1(0,0,0,0);
			
			CFbsBitGc& pGc = aGraphic.GetBmpGc();

			DrawImage(pGc,rect,rect1,1); 
		}
	}
	else
	{
		//iPicture->SetShowRect(iMainEngine.ScreenLayout().GetClientRect()); 
		//SetShowRect(iMainEngine.ScreenLayout().WhiteBackgroundRect()); 

		/*TInt nWidth = iMainEngine.ScreenLayout().Font()->MeasureText(ptr);

		aGraphic.SetPenColor(KBlackRgb);
		aGraphic.SetPenStyle(CFbsBitGc::ESolidPen);

		if(nWidth>iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2)
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX/2,15), ptr);
		else
			aGraphic.DrawText(TPoint(iMainEngine.ScreenLayout().GetStatusPaneRect().iBr.iX-nWidth,15), ptr);*/

		if(NULL != iShowBmp)   
		{
			TRect rect(0,0,0,0);
			TRect rect1(0,0,0,0);

			CFbsBitGc& pGc = aGraphic.GetBmpGc();
			DrawImage(pGc,rect,rect1,0); 
		}
	}

	//左右箭头提示翻页
	TRect nScreen;
	TPoint nLeft;
	TPoint nRight;

	if(iFullScreenShow)
		nScreen=iMainEngine.ScreenLayout().ScreenSize();
	else
		nScreen=iMainEngine.ScreenLayout().GetClientRect();

	nLeft.iX=0;
	nLeft.iY=nScreen.iTl.iY +(nScreen.iBr.iY-nScreen.iTl.iY)/2  ;

	TInt nWidth = iMainEngine.ScreenLayout().Font()->MeasureText(iMainEngine.GetDesById(ETurkeyTextRes_RightArrow));
	nRight.iX=nScreen.iBr.iX-nWidth+nScreen.iTl.iX  ;
	nRight.iY=nScreen.iTl.iY +(nScreen.iBr.iY-nScreen.iTl.iY)/2;

	if(iSelIndex==0 && iFileNameArray->Count()>1)
		aGraphic.DrawText_Bold(nRight, iMainEngine.GetDesById(ETurkeyTextRes_RightArrow));
	else if(iSelIndex==iFileNameArray->Count()-1 && iFileNameArray->Count()>1)
		aGraphic.DrawText_Bold(nLeft , iMainEngine.GetDesById(ETurkeyTextRes_LeftArrow));
	else if(iFileNameArray->Count()>1)
	{
		aGraphic.DrawText_Bold(nLeft, iMainEngine.GetDesById(ETurkeyTextRes_LeftArrow));
		aGraphic.DrawText_Bold(nRight , iMainEngine.GetDesById(ETurkeyTextRes_RightArrow));
	}
}

TBool CBrowserPicWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;

	if(iOpenComplete)
	{
		switch(aKeyCode)
		{
		case KKeyCodeUp:
		case KKeyCodeDown:
			
			break;
		case KKeyCodeLeft:
			DoPopUpMenuCommand(EPopUp);
			break;
		case KKeyCodeRight:
			DoPopUpMenuCommand(EPopNext);
			break;
		case KOKKey:	
			DoPopUpMenuCommand(EPopScreenShow);
			break;
		case KLeftSoftKey:
			InitPopUpMenu();
			break;
		case KRightSoftKey:
			if (iFullScreenShow)
			{
				iFullScreenShow=false;
				
				SetShowRect(iMainEngine.ScreenLayout().WhiteBackgroundRect());

				iIsNormalWindow=true;

				ShowTitleBar();

				iMainEngine.RequestDraw();
			} 
			else
			{
				GoBackToParentWindow();
			}
			break;
		case '4':
			DoPopUpMenuCommand(EPopUp);
			break;
		case '6':
			DoPopUpMenuCommand(EPopNext);
			break;
		case '0':
			DoPopUpMenuCommand(EPopScreenShow);
			break;
		default:
			keyResult = EFalse;
			break;
		}
	}
	return keyResult;
}

TBool CBrowserPicWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CBrowserPicWindow::DoSizeChanged()
{

}

void CBrowserPicWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//========================================================================================================
void CBrowserPicWindow::InitImage(const TDesC& aDes)
{
	iMainEngine.WriteLog16(_L("InitImage-1"));
	iMainEngine.WriteLog16(aDes);
	//DELETE(iPicture);
	DELETE(iAdapter);
	CLEAR(iShowBmp);

	iMainEngine.WriteLog16(_L("InitImage-2"));


	iAdapter=NULL;
	iAdapter = CImageManipAdapter::NewL(*this,iDisplayModel);

	iOpenComplete=EFalse;
	//::WriteLogsL(_L("InitImage-1")); 
	iMainEngine.WriteLog16(_L("InitImage-3"));
	TRAPD(error,
		iAdapter->OpenImageByPath(aDes);;	 
	)
	iMainEngine.WriteLog16(_L("InitImage-4"));
		//::WriteLogsL(_L("InitImage-2")); 
	if(error!=KErrNone)
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_File_OpenImageError),*this,0);

		iOpenComplete=ETrue;
	}
	
	iMainEngine.WriteLog16(_L("InitImage-5"));
	//重画界面
	ShowTitleBar();

	iMainEngine.RequestDraw();
	iMainEngine.WriteLog16(_L("InitImage-6"));
	//::WriteLogsL(_L("InitImage-4")); 
}

void CBrowserPicWindow::DialogEvent(TInt aCommand)
{

}

void CBrowserPicWindow::SetFileName(const TDesC& aDes,TInt aType)
{
	if(aType==0)
	{
		//内部添加
		if (aDes.Length()) {
			iFileName.Delete(0,iFileName.Length());
			iFileName.Append (iFilePath);
			iFileName.Append(aDes);
			//判断文件是否存在
			if(UtilityTools::FileIsExist(iFileName)==1)
			{
				InitImage(iFileName);
			}
			else
			{
				//获取文件信息
				if (iFileNameArray)
					delete iFileNameArray;

				iFileNameArray = new (ELeave) CArrayFixFlat<TFileName>(10);

				ReadImageFileNameList(iFilePath, iFileNameArray);

				if(iFileNameArray->Count()>0)
				{
					if(iSelIndex>=iFileNameArray->Count())
					{
						iSelIndex = iFileNameArray->Count()-1;

						iFileName.Delete(0,iFileName.Length());
						iFileName.Append (iFilePath);
						iFileName.Append(iFileNameArray->At(iSelIndex));
					}
					else
					{
						SetFileName(iFileNameArray->At(iSelIndex),0);
					}
				}
			}
		}
	}
	else
	{
		//外部添加
		if (aDes.Length()) {

			iMainEngine.WriteLog16(_L("Picture Open-3"));

			TBuf<50> nFileName;
			TParse parse;

			parse.Set(aDes,NULL,NULL);
			nFileName.Copy(parse.NameAndExt());
			
			iMainEngine.WriteLog16(_L("Picture Open-4"));

			TInt i=aDes.Find(nFileName);

			iFilePath.Zero ();
			iFilePath.Append (aDes.Mid(0,i));

			//获取文件信息
			if (iFileNameArray)
				delete iFileNameArray;

			iFileNameArray = new (ELeave) CArrayFixFlat<TFileName>(10);

			ReadImageFileNameList(iFilePath, iFileNameArray);
			
			iMainEngine.WriteLog16(_L("Picture Open-5"));

			iFileName.Delete(0,iFileName.Length());
			iFileName.Append (aDes);

			//获取文件在列表中的位置
			for(TInt i=0;i<iFileNameArray->Count();i++)
			{
				if(nFileName==iFileNameArray->At(i))
				{
					iSelIndex = i;
					break;
				}
			}
			
			iMainEngine.WriteLog16(_L("Picture Open-6"));
			InitImage(iFileName);		
			iMainEngine.WriteLog16(_L("Picture Open-7"));
		}
	}
}   

void CBrowserPicWindow::InitPopUpMenu()
{

	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);

	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PicMenu_UpPic),EPopUp);
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PicMenu_NextPic),EPopNext);

	#ifdef __SERIES60_3X__

	#else
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PicMenu_SetWallPaper),EPopSetWallPaper);
	#endif
	
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PicMenu_ScreenShow),EPopScreenShow);
}

void CBrowserPicWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EPopUp:
		{
			//上一张
			if(--iSelIndex < 0)
			{
				iSelIndex =0;
				return;
			}

			SetFileName(iFileNameArray->At(iSelIndex),0);
		}
		break;
	case EPopNext:
		{
			//下一张
			if (++iSelIndex >= iFileNameArray->Count()) 
			{
				iSelIndex = iFileNameArray->Count()-1;
				return;
			}
			SetFileName(iFileNameArray->At(iSelIndex),0);
		}
		break;
	case EPopSetWallPaper:
		{
			if (NULL != iShowBmp)
			{
				if(SetToWallPaper())
				{
					iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_SetWaFi),*this,0);
				}
				else
				{
					iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_SetWaLo),*this,0);
				}
			}	
		}
		break;
	case EPopScreenShow:
		{
			//全屏显示
			if(!iFullScreenShow)
			{
				iFullScreenShow=ETrue;

				SetShowRect(iMainEngine.ScreenLayout().ScreenSize());

				iIsNormalWindow=false;

				ShowTitleBar();

				iMainEngine.RequestDraw();
			}
		}
		break;
	default:
		ASSERT(EFalse);
		break;
	}
}

void CBrowserPicWindow::RedrawView(TInt aType)
{
	iOpenComplete=ETrue;
	CLEAR(iShowBmp);
	if(aType==0)
	{
		iShowBmp=&(iAdapter->Image());
	}
	else
	{
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_File_OpenImageError),*this,0);
	}
	iMainEngine.RequestDraw();
}

void CBrowserPicWindow::ReadImageFileNameList(const TDesC& aFilePath, CArrayFixFlat<TFileName>* aArray)
{
	//获取指定目录下的所有图片文件名称
	if (NULL == aFilePath.Length())
		return;

	TFileName	szPath;
	szPath.Append(aFilePath);	
	RFs	oRFs;
	User::LeaveIfError(oRFs.Connect());	

	CDir* dir = NULL;
	User::LeaveIfError(oRFs.GetDir(aFilePath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));

	int i;
	i=(*dir).Count()-1;
	for(;i>=0; i--)
	{
		const	TEntry & oEntry = (*dir)[i];
		TFileName oName = oEntry.iName;	

		if(IsImageFile(oName)==1 && !oEntry.IsDir())
		{
			aArray->AppendL(oName);
		}
	}

	dir=NULL;
	oRFs.Close();
}

TInt CBrowserPicWindow::IsImageFile(const TDesC& aFile)
{
	TInt nFileNum=0, nFileExNum=0;
	UtilityTools::GetFileName(aFile, nFileNum, nFileExNum);
	TFileName szName;
	szName = aFile.Right(nFileNum);

	TBuf<16> szExName;
	szExName = aFile.Right(nFileExNum);
	szExName.LowerCase();
	if (NULL == szExName.Compare(KBMP) 
		||	NULL == szExName.Compare(KJPG)
		||	NULL == szExName.Compare(KGIF)
		||	NULL == szExName.Compare(KPNG)
		)
		return 1;
	else 	
		return 0;
}

void CBrowserPicWindow::ShowTitleBar()
{
	if(iFullScreenShow)
	{
		//全屏不显示TitleBar
		RemoveControl(iTitleBar);
	}
	else
	{
		AddControl(iTitleBar);

		TParse parseFileName;
		parseFileName.Set(iFileName,NULL,NULL);
		TPtrC ptr = parseFileName.Name();

		iTitleBar->SetTextL(ptr);
	}
}

void CBrowserPicWindow::DrawImage(CGraphicsContext& aGc,TRect& /*aDesRect*/,const TRect& /*aSrcRect*/,TInt aDrawType) const
{
	TInt nMaxWidth;
	TInt nMaxHeight;
	TInt nBeginY;
	TInt nBeginX;

	CFbsBitmap* pBitmap =  iShowBmp;
	if(NULL != pBitmap)
	{ 
		nMaxWidth=iShowRect.iBr.iX - iShowRect.iTl.iX;
		nMaxHeight=iShowRect.iBr.iY - iShowRect.iTl.iY;
		nBeginY=iShowRect.iTl.iY;
		nBeginX=iShowRect.iTl.iX;

		TSize aSize = pBitmap->SizeInPixels();

		//计算出目标图片大小
		TRect nDesRect(TPoint(nBeginX,nBeginY),TSize(nMaxWidth,nMaxHeight));
		TInt nHalfHeight = aSize.iHeight >> 1;
		TInt nHalfWidth = aSize.iWidth >> 1;
		TBool nIsScale = EFalse;
		if(aSize.iWidth >= nMaxWidth && aSize.iHeight >= nMaxHeight)
		{
			TReal nDouble = 0;
			if(aSize.iWidth > aSize.iHeight)
			{
				nDouble       = aSize.iWidth * 1.0 / nMaxWidth;	
				aSize.iHeight = TInt(aSize.iHeight * 1.0 / nDouble);
				aSize.iWidth  = nMaxWidth;

				nHalfWidth = aSize.iWidth >> 1;
				nHalfHeight = aSize.iHeight >> 1;
			}
			else
			{
				nDouble = aSize.iHeight * 1.0 / nMaxHeight ;	
				aSize.iWidth  =  (TInt)(aSize.iWidth * 1.0 / nDouble);
				aSize.iHeight = nMaxHeight;

				nHalfWidth = aSize.iWidth >> 1;
				nHalfHeight = aSize.iHeight >> 1;
			}
			nIsScale = ETrue;
		}
		if(aSize.iWidth >= nMaxWidth )
		{        
			nDesRect.iTl.iY = nBeginY + (nMaxHeight >> 1) - nHalfHeight; //左上点Y轴值
			nDesRect.iBr.iY = nBeginY + (nMaxHeight >> 1) + nHalfHeight; //右下点Y轴值
		}
		else if(aSize.iHeight >= nMaxHeight)
		{
			nDesRect.iTl.iX = nBeginX+(nMaxWidth>>1) - nHalfWidth; //左上点Y轴值
			nDesRect.iBr.iX = nBeginX+(nMaxWidth>>1) + nHalfWidth;//右下点Y轴值
		}
		else if(aSize.iWidth < nMaxWidth && aSize.iHeight < nMaxHeight)
		{
			nDesRect.iTl.iY = nBeginY + (nMaxHeight >> 1) - nHalfHeight; //左上点Y轴值
			nDesRect.iTl.iX = nBeginX+(nMaxWidth>>1) - nHalfWidth; //左上点Y轴值
			nDesRect.iBr.iY = nBeginY + (nMaxHeight >> 1) + nHalfHeight; //右下点Y轴值
			nDesRect.iBr.iX = nBeginX+(nMaxWidth>>1) + nHalfWidth;//右下点Y轴值	
		}
		if(nIsScale)
		{
			aSize = pBitmap->SizeInPixels();
			aGc.DrawBitmap(nDesRect,pBitmap,TRect(TPoint(0,0),aSize));	
		}
		else
		{	
			aGc.DrawBitmap(nDesRect,pBitmap,TRect(TPoint(0,0),aSize));
		}
	}
} 

TInt CBrowserPicWindow::SetToWallPaper()
{
	if(iShowBmp==NULL)
		return EFalse;

#ifdef __SERIES60_3X__
	{
		TInt er=AknsWallpaperUtils::SetIdleWallpaper(iFileName,NULL);
		TBuf<20> ber;
		ber.AppendNum(er);

		return ETrue;
	}
#else
	{
		CFbsBitmap*   pTempBitmap = iShowBmp;
		TSize aSize = pTempBitmap->SizeInPixels();
		TBool nRes = ETrue;

		CFbsBitmap* pBitmap = NULL;
		CFbsBitmapDevice* pBitmapDevices = NULL;
		CFbsBitGc* pGc  = NULL;

		if(aSize.iWidth > 176 || aSize.iHeight > 144)
		{
			pBitmap  = new(ELeave)CFbsBitmap();
			CleanupStack::PushL(pBitmap); 
			pBitmap->Create(TSize(176,144),iDisplayModel);

			pBitmapDevices  = CFbsBitmapDevice::NewL(pBitmap);
			CleanupStack::PushL(pBitmapDevices); 

			pGc = CFbsBitGc::NewL();
			pBitmapDevices->CreateContext(pGc);
			CleanupStack::PushL(pGc); 

			pGc->DrawBitmap(TRect(TPoint(0,0),TSize(176,144)),pTempBitmap,TRect(TPoint(0,0),aSize));

			nRes = SaveMbmL(kIdlePathValueKey(),pBitmap);

			CleanupStack::PopAndDestroy(pGc);
			CleanupStack::PopAndDestroy(pBitmapDevices);
			CleanupStack::PopAndDestroy(pBitmap);
		}
		else
		{
			nRes = SaveMbmL(kIdlePathValueKey(),pTempBitmap); 	
		}

		if(nRes)
		{
			const TUid KIdleUid = {0x10005a32};
			CSharedDataI * shData = CSharedDataI::NewL(KIdleUid, EFalse);
			CleanupStack::PushL(shData);
			TInt a = 1;
			TInt b = 0;
			TInt err = shData->Set(kIdleImgKey(), a);
			err = shData->Set(kIdleIndexKey(), b);
			a = 1;
			err = shData->Set(kDimIndexKey(), a);
			err = shData->Set(kIdlePathKey(), kIdlePathValueKey());

			CleanupStack::PopAndDestroy(shData);	

			return ETrue;
		}
		return EFalse;
	}
#endif
}

TInt CBrowserPicWindow::SaveMbmL(const TFileName& aFileName, CFbsBitmap* aBitmap)
{
	if(NULL != aBitmap)
	{
		aBitmap->Save(aFileName);
		return ETrue;
	}
	
	return EFalse;
}
