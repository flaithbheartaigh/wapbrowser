/*
============================================================================
 Name        : BitmapFactory.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CBitmapFactory implementation
============================================================================
*/

#include "BitmapFactory.h"
#include <coemain.h>

#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif

#include "MainEngine.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "PreDefine.h"
#include "cocobmp.h"

CBitmapFactory::CBitmapFactory(CMainEngine& aMainEngine)
:iMainEngine(aMainEngine)
{
	// No implementation required
}


CBitmapFactory::~CBitmapFactory()
{

	this->ClearFileTypeIcon();
	this->ClearGradeBmp();
	this->ClearInputIconBmp();
	this->ClearLogoIcon();
	this->ClearMainIconBmp();
	this->ClearOperationIcon();
	this->ClearWeatherIconBmp();
	ClearMusicPlayIcon();
	ClearMusicTimeIcon();
	ClearMusicSelectIcon();
	ClearMusicBugleIcon();
	this->ClearArrowIcon();
	this->ClearDownLoadIcon();
	this->ClearDialogIcon();
}

CBitmapFactory* CBitmapFactory::NewLC(CMainEngine& aMainEngine)
{
	CBitmapFactory* self = new (ELeave)CBitmapFactory(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CBitmapFactory* CBitmapFactory::NewL(CMainEngine& aMainEngine)
{
	CBitmapFactory* self=CBitmapFactory::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CBitmapFactory::ConstructL()
{
	InitGradeBmp();
	InitMainIcon();
	InitWeatherIcon();
	InitInputIcon();
	InitOperationIcon();
	InitLogoIcon();
	InitFileTypeIcon();
	InitMusicPlayIcon();
	InitMusicBugleIcon();
	InitMusicTimeIcon();
	InitMusicSelectIcon(); 
	InitDialogIcon();
	InitDownLoadIcon();
	this->InitArrowIcon();
}
CFbsBitmap* CBitmapFactory::GetBitmap(TInt aID)
{
	CFbsBitmap* bitmap=iMainEngine.CreateBmpById(aID);
#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = bitmap->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	AknIconUtils::SetSize(bitmap,size);
#endif
	return bitmap;
}
CFbsBitmap* CBitmapFactory::GetBitmapByWidth(TInt aID)
{
	CFbsBitmap* bitmap=iMainEngine.CreateBmpById(aID);

	iMainEngine.ScreenLayout().ScaleFitToScreenWidth(bitmap);

	return bitmap;
}
////////////////////////////////////
//评分
///////////////////////////////////
void CBitmapFactory::DrawGradeBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iGradeIconArray[aIndex],iGradeIconMaskArray[aIndex]);
}

void CBitmapFactory::InitGradeBmp()
{
	for(TInt i=0;i<3;i++)
	{
		iGradeIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpStar,3,1,i,0));
		iGradeIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpStar_mask,3,1,i,0));
	}


	#ifdef __SERIES60_3X__
		TInt multiple = iMainEngine.ScreenLayout().Multiple();
		TInt divisor = iMainEngine.ScreenLayout().Divisor();

		TSize size;
		size = iGradeIconArray[0]->SizeInPixels();
		size.iWidth = (size.iWidth*multiple)/divisor;
		size.iHeight = (size.iHeight*multiple)/divisor;

		for(TInt i=0;i<3;i++)
		{
			AknIconUtils::SetSize(iGradeIconArray[i],size);
			AknIconUtils::SetSize(iGradeIconMaskArray[i],size);
		}
	#endif
}

TSize CBitmapFactory::GetGradeBmpSize(TInt aIndex) const
{
	return iGradeIconArray[aIndex]->SizeInPixels();
}

TInt CBitmapFactory::GetGradeBmpCount()
{
	return iGradeIconArray.Count();
}
void CBitmapFactory::ClearGradeBmp()
{
	iGradeIconArray.Close();
	iGradeIconMaskArray.Close();
}
////////////////////////////////////
//主界面九宫格
////////////////////////////////////
void CBitmapFactory::DrawMainIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iMainIconArray[aIndex],iMainIconMaskArray[aIndex]);
}

TSize CBitmapFactory::GetMainIconBmpSize(TInt aIndex) const
{
	return iMainIconArray[aIndex]->SizeInPixels();
}
void CBitmapFactory::InitMainIcon()
{
	for(TInt i=0;i<10;i++)
	{
		iMainIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMainicon,1,10,0,i));
		iMainIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMainicon_mask,1,10,0,i));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iMainIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<10;i++)
	{
		AknIconUtils::SetSize(iMainIconArray[i],size);
		AknIconUtils::SetSize(iMainIconMaskArray[i],size);
	}
#endif
}

TInt CBitmapFactory::GetMainIconBmpCount()
{
	return iMainIconArray.Count();
}
void CBitmapFactory::ClearMainIconBmp()
{
	iMainIconArray.Close();
	iMainIconMaskArray.Close();
}
////////////////////////////////////
//天气
////////////////////////////////////
void CBitmapFactory::DrawWeatherIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iWeatherIconArray[aIndex],iWeatherIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetWeatherIconBmpSize(TInt aIndex) const
{
	return iWeatherIconArray[aIndex]->SizeInPixels();
}
void CBitmapFactory::InitWeatherIcon()
{
	for(TInt i=0;i<7;i++)
	{
		iWeatherIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpWeather,1,7,0,i));
		iWeatherIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpWeather_mask,1,7,0,i));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iWeatherIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<7;i++)
	{
		AknIconUtils::SetSize(iWeatherIconArray[i],size);
		AknIconUtils::SetSize(iWeatherIconMaskArray[i],size);
	}
#endif
}
TInt CBitmapFactory::GetWeatherIconBmpCount()
{
	return iWeatherIconArray.Count();
}
void CBitmapFactory::ClearWeatherIconBmp()
{
	iWeatherIconArray.Close();
	iWeatherIconMaskArray.Close();
}
////////////////////////////////////
//输入法
////////////////////////////////////
void CBitmapFactory::DrawInputIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iInputIconArray[aIndex],iInputIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetInputIconBmpSize(TInt aIndex) const
{
	return iInputIconArray[aIndex]->SizeInPixels();
}
void CBitmapFactory::InitInputIcon()
{
	for(TInt i=0;i<5;i++)
	{
		iInputIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpInputtype,1,5,0,i));
		iInputIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpInputtype_mask,1,5,0,i));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iInputIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<5;i++)
	{
		AknIconUtils::SetSize(iInputIconArray[i],size);
		AknIconUtils::SetSize(iInputIconMaskArray[i],size);
	}
#endif
}


TInt CBitmapFactory::GetInputIconBmpCount()
{
	return iInputIconArray.Count();
}
void CBitmapFactory::ClearInputIconBmp()
{
	iInputIconArray.Close();
	iInputIconMaskArray.Close();
}
const CFbsBitmap* CBitmapFactory::GetInputIconBmp(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iInputIconArray.Count());
		return iInputIconArray[aIndex];
}

const CFbsBitmap* CBitmapFactory::GetInputIconBmpMask(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iInputIconMaskArray.Count());
	return iInputIconMaskArray[aIndex];
}
////////////////////////////////////
//操作选项
////////////////////////////////////
void CBitmapFactory::DrawOperationIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iOperationIconArray[aIndex],iOperationIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetOperationIconBmpSize(TInt aIndex) const
{
	return iOperationIconArray[aIndex]->SizeInPixels();
}
void CBitmapFactory::InitOperationIcon()
{
	for(TInt i=0;i<8;i++)
	{
		iOperationIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpOperation,8,1,i,0));
		iOperationIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpOperation_mask,8,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iOperationIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<8;i++)
	{
		AknIconUtils::SetSize(iOperationIconArray[i],size);
		AknIconUtils::SetSize(iOperationIconMaskArray[i],size);
	}
#endif
}


TInt CBitmapFactory::GetOperationIconBmpCount()
{
	return iOperationIconArray.Count();
}
void CBitmapFactory::ClearOperationIcon()
{
	iOperationIconArray.Close();
	iOperationIconMaskArray.Close();
}

////////////////////////////////////
//文件类别图标
////////////////////////////////////
void CBitmapFactory::InitFileTypeIcon()
{
	for(TInt i=0;i<9;i++)
	{
		iFileTypeIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpIcon,9,1,i,0));
		iFileTypeIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpIcon_mask,9,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iFileTypeIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<9;i++)
	{
		AknIconUtils::SetSize(iFileTypeIconArray[i],size);
		AknIconUtils::SetSize(iFileTypeIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawFileTypeIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iFileTypeIconArray[aIndex],iFileTypeIconMaskArray[aIndex]);
}

TSize CBitmapFactory::GetFileTypeIconBmpSize(TInt aIndex) const
{
	return iFileTypeIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetFileTypeIconBmpCount()
{
	return iFileTypeIconArray.Count();
}
void CBitmapFactory::ClearFileTypeIcon()
{
	iFileTypeIconArray.Close();
	iFileTypeIconMaskArray.Close();
}

const CFbsBitmap* CBitmapFactory::GetFileTypeIcon(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iFileTypeIconArray.Count());
		return iFileTypeIconArray[aIndex];
}

const CFbsBitmap* CBitmapFactory::GetFileTypeIconMask(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iFileTypeIconMaskArray.Count());
	return iFileTypeIconMaskArray[aIndex];
}
////////////////////////////////////
//登陆界面4个图片
////////////////////////////////////
void CBitmapFactory::InitLogoIcon()
{
	for(TInt i=0;i<4;i++)
	{
		iLogoIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpLogo,4,1,i,0));
		iLogoIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpLogo_mask,4,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iLogoIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<4;i++)
	{
		AknIconUtils::SetSize(iLogoIconArray[i],size);
		AknIconUtils::SetSize(iLogoIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawLogoIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iLogoIconArray[aIndex],iLogoIconMaskArray[aIndex]);
}

TSize CBitmapFactory::GetLogoIconBmpSize(TInt aIndex) const
{
	return iLogoIconArray[aIndex]->SizeInPixels();
}

TInt CBitmapFactory::GetLogoIconBmpCount()
{
	return iLogoIconArray.Count();
}

void CBitmapFactory::ClearLogoIcon()
{
	iLogoIconArray.Close();
	iLogoIconMaskArray.Close();
}
////////////////////////////////////
//等待图标
////////////////////////////////////
void CBitmapFactory::InitDialogIcon()
{
	for(TInt i=0;i<6;i++)
	{
		iDialogIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpDialog_icon,6,1,i,0));
		iDialogIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpDialog_icon_mask,6,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iDialogIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<6;i++)
	{
		AknIconUtils::SetSize(iDialogIconArray[i],size);
		AknIconUtils::SetSize(iDialogIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawDialogIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iDialogIconArray[aIndex],iDialogIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetDialogIconBmpSize(TInt aIndex) const
{
	return iDialogIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetDialogIconBmpCount()
{
	return iDialogIconArray.Count();
}
void CBitmapFactory::ClearDialogIcon()
{
	iDialogIconArray.Close();
	iDialogIconMaskArray.Close();
}
//音乐播放
void CBitmapFactory::InitMusicPlayIcon()
{
	for(TInt i=0;i<4;i++)
	{
		iMusicPlayIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicplay,4,1,i,0));
		iMusicPlayIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicplay_mask,4,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iMusicPlayIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<4;i++)
	{
		AknIconUtils::SetSize(iMusicPlayIconArray[i],size);
		AknIconUtils::SetSize(iMusicPlayIconMaskArray[i],size);
	}
#endif
}

void CBitmapFactory::DrawMusicPlayIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iMusicPlayIconArray[aIndex],iMusicPlayIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetMusicPlayIconBmpSize(TInt aIndex) const
{
	return iMusicPlayIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetMusicPlayIconBmpCount()
{
	return iMusicPlayIconArray.Count();
}
void CBitmapFactory::ClearMusicPlayIcon()
{
	iMusicPlayIconArray.Close();
	iMusicPlayIconMaskArray.Close();
}

//音乐进度
void CBitmapFactory::InitMusicTimeIcon()
{
	for(TInt i=0;i<2;i++)
	{
		iMusicTimeIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusictime,1,2,0,i));
		iMusicTimeIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusictime_mask,1,2,0,i));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iMusicTimeIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<2;i++)
	{
		AknIconUtils::SetSize(iMusicTimeIconArray[i],size);
		AknIconUtils::SetSize(iMusicTimeIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawMusicTimeIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iMusicTimeIconArray[aIndex],iMusicTimeIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetMusicTimeIconBmpSize(TInt aIndex) const
{
	return iMusicTimeIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetMusicTimeIconBmpCount()
{
	return iMusicTimeIconArray.Count();
}
void CBitmapFactory::ClearMusicTimeIcon()
{
	iMusicTimeIconArray.Close();
	iMusicTimeIconMaskArray.Close();
}
//音乐选择
void CBitmapFactory::InitMusicSelectIcon()
{
	for(TInt i=0;i<4;i++)
	{
		iMusicSelectIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicselect,4,1,i,0));
		iMusicSelectIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicselect_mask,4,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iMusicSelectIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<4;i++)
	{
		AknIconUtils::SetSize(iMusicSelectIconArray[i],size);
		AknIconUtils::SetSize(iMusicSelectIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawMusicSelectIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iMusicSelectIconArray[aIndex],iMusicSelectIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetMusicSelectIconBmpSize(TInt aIndex) const
{
	return iMusicSelectIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetMusicSelectIconBmpCount()
{
	return iMusicSelectIconArray.Count();
}
void CBitmapFactory::ClearMusicSelectIcon()
{
	iMusicSelectIconArray.Close();
	iMusicSelectIconMaskArray.Close();
}
//音乐喇叭
void CBitmapFactory::InitMusicBugleIcon()
{
	for(TInt i=0;i<2;i++)
	{
		iMusicBugleIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicbugle,2,1,i,0));
		iMusicBugleIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpMusicbugle_mask,2,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iMusicBugleIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<2;i++)
	{
		AknIconUtils::SetSize(iMusicBugleIconArray[i],size);
		AknIconUtils::SetSize(iMusicBugleIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawMusicBugleIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iMusicBugleIconArray[aIndex],iMusicBugleIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetMusicBugleIconBmpSize(TInt aIndex) const
{
	return iMusicBugleIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetMusicBugleIconBmpCount()
{
	return iMusicBugleIconArray.Count();
}
void CBitmapFactory::ClearMusicBugleIcon()
{
	iMusicBugleIconArray.Close();
	iMusicBugleIconMaskArray.Close();
}
//下载图标
void CBitmapFactory::InitDownLoadIcon()
{
	for(TInt i=0;i<7;i++)
	{
		iDownLoadIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpDownloadicon,7,1,i,0));
		iDownLoadIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpDownloadicon_mask,7,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iDownLoadIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<7;i++)
	{
		AknIconUtils::SetSize(iDownLoadIconArray[i],size);
		AknIconUtils::SetSize(iDownLoadIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawDownLoadIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iDownLoadIconArray[aIndex],iDownLoadIconMaskArray[aIndex]);
}

TSize CBitmapFactory::GetDownLoadIconBmpSize(TInt aIndex) const
{
	return iDownLoadIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetDownLoadIconBmpCount()
{
	return iDownLoadIconArray.Count();
}
void CBitmapFactory::ClearDownLoadIcon()
{
	iDownLoadIconArray.Close();
	iDownLoadIconMaskArray.Close();
}
const CFbsBitmap* CBitmapFactory::GetDownLoadIcon(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iDownLoadIconArray.Count());
		return iDownLoadIconArray[aIndex];
}

const CFbsBitmap* CBitmapFactory::GetDownLoadIconMask(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iDownLoadIconMaskArray.Count());
	return iDownLoadIconMaskArray[aIndex];
}

const CFbsBitmap*	CBitmapFactory::GetMusicTimeIcon(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iMusicTimeIconArray.Count());
	return iMusicTimeIconArray[aIndex];
}

const CFbsBitmap*	CBitmapFactory::GetMusicTimeIconMask(TInt aIndex) const
{
	ASSERT(aIndex >= 0 && aIndex < iMusicTimeIconMaskArray.Count());
	return iMusicTimeIconMaskArray[aIndex];
}
//左右箭头
void CBitmapFactory::InitArrowIcon()
{
	for(TInt i=0;i<2;i++)
	{
		iArrowIconArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpArrow,2,1,i,0));
		iArrowIconMaskArray.Append(iMainEngine.ScreenLayout().GetIconL(KMBMPATH,EMbmCocobmpArrow_mask,2,1,i,0));
	}

#ifdef __SERIES60_3X__
	TInt multiple = iMainEngine.ScreenLayout().Multiple();
	TInt divisor = iMainEngine.ScreenLayout().Divisor();

	TSize size;
	size = iArrowIconArray[0]->SizeInPixels();
	size.iWidth = (size.iWidth*multiple)/divisor;
	size.iHeight = (size.iHeight*multiple)/divisor;

	for(TInt i=0;i<2;i++)
	{
		AknIconUtils::SetSize(iArrowIconArray[i],size);
		AknIconUtils::SetSize(iArrowIconMaskArray[i],size);
	}
#endif
}
void CBitmapFactory::DrawArrowIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const
{
	if(aIndex>-1)
		gc.BitBltMasked(aPoint,iArrowIconArray[aIndex],iArrowIconMaskArray[aIndex]);
}
TSize CBitmapFactory::GetArrowIconBmpSize(TInt aIndex) const
{
	return iArrowIconArray[aIndex]->SizeInPixels();
}
TInt CBitmapFactory::GetArrowIconIconBmpCount()
{
	return iArrowIconArray.Count();
}
void CBitmapFactory::ClearArrowIcon()
{
	iArrowIconArray.Close();
	iArrowIconMaskArray.Close();
}
