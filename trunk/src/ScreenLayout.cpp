/*
============================================================================
 Name        : ScreenLayout.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CScreenLayout implementation
============================================================================
*/
#include <coemain.h>
#include <W32STD.H>

#include "ScreenLayout.h"
#include "PreDefine.h"
#include "MainEngine.h"
#include "UtilityTools.h"
#include "Graphic.h"
#include "cocobmp.h"

#include <eikenv.h>
//#include "picture.h"

CScreenLayout::CScreenLayout(CMainEngine* aEngine) : iEngine(aEngine)
												   , iDefaultFont(aEngine->Graphic().GetFont())
{
}


CScreenLayout::~CScreenLayout()
{
}

CScreenLayout* CScreenLayout::NewLC(CMainEngine* aEngine)
{
	CScreenLayout* self = new (ELeave)CScreenLayout(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CScreenLayout* CScreenLayout::NewL(CMainEngine* aEngine)
{
	CScreenLayout* self=CScreenLayout::NewLC(aEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CScreenLayout::ConstructL()
{
	//iDefaultFont = iEngine->GetFont();
	ASSERT(iDefaultFont);
	InitByScreenSize();
	InitByFont();
	InitData();
}

void CScreenLayout::SetFont(const CFont* aDefaultFont)
{
	ASSERT(aDefaultFont);
	iDefaultFont = aDefaultFont;
}

const CFont* CScreenLayout::Font()const
{
	ASSERT(iDefaultFont);
	return iDefaultFont;
}

void CScreenLayout::SetClientRect(const TRect& aRect)
{
	iFullClientRect = aRect;
	iClientRect = aRect;
	iClientRect.Shrink(iOuterMargin,iOuterMargin);

	iNaviPaneRect = iClientRect;
	iNaviPaneRect.iBr.iY = iNaviPaneRect.iTl.iY + iListBoxLineHeight;
	//iWhiteBackgroundRect.Shrink(iInnerMargin,iInnerMargin);

	iWhiteBackgroundRect = iClientRect;
	iWhiteBackgroundRect.iTl.iY = iNaviPaneRect.iBr.iY + iOuterMargin;
	//iWhiteBackgroundRect.Shrink(iInnerMargin,iInnerMargin);


	iLineNum = iWhiteBackgroundRect.Height()/iListBoxLineHeight;

	iInfoRect = aRect;
	iInfoRect.iTl.SetXY(0,0);
	iInfoRect.Shrink(iOuterMargin,iOuterMargin);

	iMultiTextRect = iClientRect;
	iInfoRect.Shrink(iOuterMargin,iOuterMargin);
}

void CScreenLayout::SetTitleBarRect(const TRect& aRect)
{
	iTitleBarRect = aRect;
}

const TRect& CScreenLayout::TitleBarRect() const
{
	return iTitleBarRect;
}

TRect CScreenLayout::GetStatusPaneRect()const
{
	return iStatusPaneRect;
}

TRect CScreenLayout::GetFullClientRect()const
{
	return iFullClientRect;
}

TRect CScreenLayout::GetClientRect()const
{
	return iClientRect;
}

TRect CScreenLayout::GetControlPaneRect()const
{
	return iControlPaneRect;
}

TRect CScreenLayout::GetNaviPaneRect()const
{
	return iNaviPaneRect;
}

TRect CScreenLayout::GetListBoxRect()const
{
	return iListBoxRect;
}

TRect CScreenLayout::GetDialogRect()const
{
	return iDialogRect;
}

TRect CScreenLayout::GetDialogIconRect()const
{
	return iDialogIconRect;
}

TRect CScreenLayout::GetScrollbarRect()const
{
	return iScrollbarRect;
}

TRect CScreenLayout::LeftButtonRect()const
{
	return iLeftButtonRect;
}

TRect CScreenLayout::RightButtonRect()const
{
	return iRightButtonRect;
}

TRect CScreenLayout::MultiTextRect()const
{
	return iMultiTextRect;
}

TRect CScreenLayout::WhiteBackgroundRect()const
{
	return iWhiteBackgroundRect;
}

TRect CScreenLayout::InfoRect()const
{
	return iInfoRect;
}

TSize CScreenLayout::PopMenuItemSize()const
{
	return iPopMenuItemSize;
}

TInt CScreenLayout::TextLineHeight()const		//文字行间距
{
	//return iTextLineHeight;
// 	itemSize.iHeight = font->HeightInPixels();
// 	itemSize.iHeight += (itemSize.iHeight>>1);{
	return iListBoxLineHeight;
}

TInt CScreenLayout::LineHeight()const		//文字行间距
{
	return iListBoxLineHeight;
	//return iTextLineHeight;
}
TInt CScreenLayout::LineNum()const
{
//	WriteLogsL(_L("CScreenLayout::LineNum iLineNum = %d"),iLineNum);
	return iLineNum;
}

TInt CScreenLayout::FontHeight()const
{
	return iFontHeight;
}

TInt CScreenLayout::ScrollBarWidth()const
{
	return iScrollBarWidth;
}

TInt CScreenLayout::IconNumPerLine()const
{
	return iIconNumPerLine;
}

TInt CScreenLayout::IconLineNum()const
{
	return iIconLineNum;
}

TInt CScreenLayout::Multiple()const
{
	return iMultiple;
}

TInt CScreenLayout::Divisor()const
{
	return iDivisor;
}

TInt CScreenLayout::InnerMargin()const
{
	return iInnerMargin;
}

TInt CScreenLayout::OuterMargin() const
{
	return iOuterMargin;
}
/*
TInt CScreenLayout::AttributeOffset()const
{
	return iAttributeOffset;
}
*/

void CScreenLayout::GetAttributeOffset(TInt& aOffset1,TInt& aOffset2)const
{
	aOffset1 = iAttributeOffset1;
	aOffset2 = iAttributeOffset2;
}
void CScreenLayout::GetLoadingTextPos(TInt& aFirstTextPos,TInt& aSecondTextPost)const
{
	aFirstTextPos = iLoadingTextPos_1;
	aSecondTextPost = iLoadingTextPos_2;
}

void CScreenLayout::GetOfferRewardEditorHeight(TInt& aHeight1,TInt& aHeight2)const
{
	aHeight1 = iOfferRewardEditorHeight1;
	aHeight2 = iOfferRewardEditorHeight2;
}

void CScreenLayout::GetSaveAsPos(TPoint& aTitlePos,TPoint& aListPos)const
{
	aTitlePos = iSaveAsTitlePos;
	aListPos = iSaveAsListPos;
}

TSize CScreenLayout::ScaleByDefaultRatio(CFbsBitmap* aBitmap)const
{
	ASSERT(aBitmap);

	TSize bmpSize = aBitmap->SizeInPixels();
	#ifdef __SERIES60_3X__
		//TSize screenSize = ScreenSize();
		bmpSize.iWidth *= iMultiple;
		bmpSize.iWidth /= iDivisor;
		bmpSize.iHeight *= iMultiple;
		bmpSize.iHeight /= iDivisor;
		AknIconUtils::SetSize(aBitmap,bmpSize);
	#endif
	return bmpSize;
}

CFbsBitmap* CScreenLayout::GetIconL(TInt aId,TInt aMaxXIndex ,TInt aMaxYIndex ,TInt aXIndex,TInt aYIndex)const
{
	TFileName fileName = iEngine->GetMbmFileName();
	return GetIconL(fileName,aId,aMaxXIndex,aMaxYIndex,aXIndex,aYIndex);
}

CFbsBitmap* CScreenLayout::GetIconL(const TDesC& aFileName,TInt aId,TInt aMaxXIndex,TInt aMaxYIndex,TInt aXIndex,TInt aYIndex)const
{
	ASSERT(aMaxXIndex > 0);
	ASSERT(aMaxYIndex > 0);

	#ifdef __SERIES60_3X__
		CFbsBitmap* srcBmp = AknIconUtils::CreateIconL(aFileName,aId);
	#else
		CFbsBitmap* srcBmp =STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(aFileName,aId);
	#endif

	CleanupStack::PushL(srcBmp);
	TSize srcSize = srcBmp->SizeInPixels();
	srcSize.iWidth /= aMaxXIndex;
	srcSize.iHeight /= aMaxYIndex;
	TRect bmpRect(srcSize);
	bmpRect.Move(srcSize.iWidth*aXIndex,srcSize.iHeight*aYIndex);
	//srcRect.

	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->Create(srcSize,EColor16M);
	User::LeaveIfError(err);
	CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(bmp);
	CleanupStack::PushL(bmpDevice);
	CFbsBitGc* bmpGc;
	err = bmpDevice->CreateContext(bmpGc);
	CleanupStack::PushL(bmpGc);
	User::LeaveIfError(err);


	bmpGc->BitBlt(TPoint(0,0),srcBmp,bmpRect);

	CleanupStack::PopAndDestroy(2);		//bmpGc,bmpDevice

	#ifdef __SERIES60_3X__
		bmp = AknIconUtils::CreateIconL(bmp);
	#endif
	
	CleanupStack::Pop();	//bmp
	CleanupStack::PopAndDestroy(srcBmp);	//srcBmp

	return bmp;
}
/*
CFbsBitmap* CScreenLayout::GetIconL(const TDesC& aFileName,TInt aId,const TRect& aSrcRect)const
{
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->Create(aSrcRect.Size(),EColor16M);
	User::LeaveIfError(err);
	CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(bmp);
	CleanupStack::PushL(bmpDevice);
	CFbsBitGc* bmpGc;
	err = bmpDevice->CreateContext(bmpGc);
	CleanupStack::PushL(bmpGc);
	User::LeaveIfError(err);

	CFbsBitmap* srcBmp = AknIconUtils::CreateIconL(aFileName,aId);

	bmpGc->BitBlt(TPoint(0,0),srcBmp,aSrcRect);

	delete srcBmp;
	CleanupStack::PopAndDestroy(2)

	bmp = AknIconUtils::CreateIconL(bmp);

	ScaleByDefaultRatio(bmp);

	CleanupStack::Pop();	//bmp
	return bmp;
}
*/

TSize CScreenLayout::ScreenSize()
{
	TPixelsTwipsAndRotation pixelsTwipsAndRotation;
	CCoeEnv::Static()->ScreenDevice()->GetDefaultScreenSizeAndRotation(pixelsTwipsAndRotation);

/*
	TBuf<10> buf;
	buf.AppendNum(pixelsTwipsAndRotation.iPixelSize.iWidth);
	buf.Append(_L(":"));
	buf.AppendNum(pixelsTwipsAndRotation.iPixelSize.iHeight);
	UtilityTools::ExecuteDialog(buf);
*/

	////User::InfoPrint(buf);

	return pixelsTwipsAndRotation.iPixelSize;
}

//下面两个函数可能产生精度问题
TSize CScreenLayout::ScaleFitToScreenWidth(CFbsBitmap* aBitmap)
{
	ASSERT(aBitmap);
	TSize bmpSize = aBitmap->SizeInPixels();
	TSize screenSize = ScreenSize();

	TInt tempNum=bmpSize.iHeight*screenSize.iWidth%bmpSize.iWidth;

	bmpSize.iHeight = bmpSize.iHeight*screenSize.iWidth/bmpSize.iWidth;
	if(tempNum>5)
		bmpSize.iHeight+=1;

	bmpSize.iWidth = screenSize.iWidth;
	//AknIconUtils::SetSize(aBitmap,bmpSize,EAspectRatioNotPreserved);
	#ifdef __SERIES60_3X__
		AknIconUtils::SetSize(aBitmap,bmpSize);
	#endif
	
	return bmpSize;
}

TSize CScreenLayout::ScaleFitToScreenHeight(CFbsBitmap* aBitmap)
{
	ASSERT(aBitmap);
	TSize bmpSize = aBitmap->SizeInPixels();
	TSize screenSize = ScreenSize();

	TInt tempNum=bmpSize.iWidth*screenSize.iHeight%bmpSize.iHeight;
	bmpSize.iWidth = bmpSize.iWidth*screenSize.iHeight/bmpSize.iHeight;

	if(tempNum>5)
		bmpSize.iWidth+=1;

	bmpSize.iHeight = screenSize.iHeight;
	//AknIconUtils::SetSize(aBitmap,bmpSize,EAspectRatioNotPreserved);
	#ifdef __SERIES60_3X__
		AknIconUtils::SetSize(aBitmap,bmpSize);
	#endif
	return bmpSize;
}
/*
//////////////////////////////////////////////////////////////////////////
此类中考虑用固定的值保存图片适合的屏幕尺寸
同时保存该尺寸下使用的字体

//////////////////////////////////////////////////////////////////////////
enum   TScaleMode { EAspectRatioPreserved = 0, EAspectRatioPreservedAndUnusedSpaceRemoved = 1, EAspectRatioNotPreserved = 2 } 
//////////////////////////////////////////////////////////////////////////
*/

void CScreenLayout::InitData()
{
	TSize screenSize = ScreenSize();
//////////////////////////////////////////////////////////////////////////
	iStatusPaneRect;
	iClientRect;
	iControlPaneRect;
	//////////////////////////////////////////////////////////////////////////
	iTextLineHeight = iDefaultFont->HeightInPixels();
	iTextLineHeight += iTextLineHeight/3;
//////////////////////////////////////////////////////////////////////////
	iListBoxLineHeight = iDefaultFont->HeightInPixels();
	iListBoxLineHeight += iListBoxLineHeight/2;
//////////////////////////////////////////////////////////////////////////
	iPopMenuItemSize.iWidth = screenSize.iWidth/2;
	iPopMenuItemSize.iHeight = iTextLineHeight + 1;
//////////////////////////////////////////////////////////////////////////
	TPoint naviPanePoint;
	naviPanePoint.iX = screenSize.iWidth/40;
	naviPanePoint.iY = screenSize.iHeight/5 - 2;
	TSize naviPaneSize;
	naviPaneSize.iWidth = screenSize.iWidth - 2*naviPanePoint.iX;
	naviPaneSize.iHeight = screenSize.iHeight/10;

	iNaviPaneRect.SetRect(naviPanePoint,naviPaneSize);
//////////////////////////////////////////////////////////////////////////

	//CFbsBitmap* bmpTitleBg = NULL;//iEngine->GetBmpTitlebg()->GetBitmap();
	CFbsBitmap* bmpTitleBg = iEngine->CreateBmpById(EMbmCocobmpTitlebg);
	ASSERT(bmpTitleBg);
	TSize titleSize = bmpTitleBg->SizeInPixels();
	titleSize.iHeight = (titleSize.iHeight*screenSize.iWidth/titleSize.iWidth);
	titleSize.iWidth = screenSize.iWidth;
	iStatusPaneRect.SetRect(TPoint(0,0),titleSize);

	//CFbsBitmap* controlPaneBmp = NULL;//iEngine->GetBmpMenubg()->GetBitmap();
	CFbsBitmap* controlPaneBmp = iEngine->CreateBmpById(EMbmCocobmpMenubg);
	ASSERT(controlPaneBmp);
	TPoint controlPanePoint(0,0);
	TSize menuSize = controlPaneBmp->SizeInPixels();
	menuSize.iHeight = (menuSize.iHeight*screenSize.iWidth/menuSize.iWidth);
	menuSize.iWidth = screenSize.iWidth;
	controlPanePoint.iY = screenSize.iHeight - menuSize.iHeight;
	iControlPaneRect.SetRect(controlPanePoint,menuSize);
//////////////////////////////////////////////////////////////////////////
	//iDialogRect;

//////////////////////////////////////////////////////////////////////////

//	iWhiteBackgroundRect = iContentZoneRect;

/*
	TRect	iListBoxRect;
	TRect	iDialogRect;
	TRect	iScrollbarRect;
	TSize	iPopMenuItemSize;

	iMultiTextRect


	//TRect clientRect(TPoint(5,65),TSize(SCREEN_WIDTH,SCREEN_HEIGHT/2));	//test
	TRect clientRect(TPoint(7,62),TSize(162,119));

*/

/*
#define KNaviPaneTextPoint	TPoint(4,38)
#define KNaviPaneSize		TSize(169, 20)
#define KNaviPaneRect		TRect(KNaviPaneTextPoint,KNaviPaneSize)

#define KLeftButtonPos		TPoint(3,191)
#define KRightButtonPos		TPoint(173,191)
*/


/*
	const	CFont*	iDefaultFont;

	TRect	iStatusPaneRect;
	TRect	iContentZoneRect;
	TRect	iControlPaneRect;

	TRect	iNaviPaneRect;
	TRect	iListBoxRect;
	TRect	iDialogRect;

	TRect	iScrollbarRect;

	TSize	iPopMenuItemSize;

	TInt	iTextLineHeight;
*/

}

void CScreenLayout::InitByScreenSize()
{
	TSize screenSize = ScreenSize();

	if(screenSize.iWidth == 176&& screenSize.iHeight == 208)		//3250
	{
		iScrollBarWidth = 3;
		iIconNumPerLine = 3;
		iIconLineNum = 3;
		iInnerMargin = 2;
		iOuterMargin = 5;
		iMultiple = 1;
		iDivisor = 1;
		iLoadingTextPos_1 = 110;
		iLoadingTextPos_2 = 177;

		iSaveAsTitlePos.iX = 0;
		iSaveAsTitlePos.iY = 5;
		iSaveAsListPos.iX = 0;
		iSaveAsListPos.iY = 30;

		iAttributeOffset1 = 10;
		iAttributeOffset2 = 40;

//		iOfferRewardEditorLineHeight = 
		iOfferRewardEditorHeight1 = 74;
		iOfferRewardEditorHeight2 = 74;
	}
	else if(screenSize.iWidth == 352&& screenSize.iHeight == 416)	//N80
	{
		iScrollBarWidth = 6;
		iIconNumPerLine = 3;
		iIconLineNum = 3;
		iInnerMargin = 4;
		iOuterMargin = 10;
		iMultiple = 2;
		iDivisor = 1;
		iLoadingTextPos_1 = 230;
		iLoadingTextPos_2 = 354;

		iSaveAsTitlePos.iX = 0;
		iSaveAsTitlePos.iY = 10;
		iSaveAsListPos.iX = 0;
		iSaveAsListPos.iY = 65;

		iAttributeOffset1 = 20;
		iAttributeOffset2 = 80;

		iOfferRewardEditorHeight1 = 144;
		iOfferRewardEditorHeight2 = 144;
	}
	else if(screenSize.iWidth == 240&& screenSize.iHeight == 320)	//N73
	{
		iScrollBarWidth = 4;
		iIconNumPerLine = 3;
		iIconLineNum = 3;
		iInnerMargin = 3;
		iOuterMargin = 7;
		iMultiple = 1;
		iDivisor = 1;
		iMultiple = 4;
		iDivisor = 3;
		iLoadingTextPos_1 = 178;
		iLoadingTextPos_2 = 270;

		iSaveAsTitlePos.iX = 0;
		iSaveAsTitlePos.iY = 8;
		iSaveAsListPos.iX = 0;
		iSaveAsListPos.iY = 37;

		iAttributeOffset1 = 15;
		iAttributeOffset2 = 50;

		iOfferRewardEditorHeight1 = 135;
		iOfferRewardEditorHeight2 = 135;
	}
	else if(screenSize.iWidth == 320&& screenSize.iHeight == 240)	//E61
	{
		iScrollBarWidth = 5;
		iIconNumPerLine = 3;
		iIconLineNum = 3;
		iInnerMargin = 2;
		iOuterMargin = 5;
		iMultiple = 1;
		iDivisor = 1;
		iLoadingTextPos_1 = 125;
		iLoadingTextPos_2 = 177;

// 		iSaveAsListPos.iX = ;
// 		iSaveAsListPos.iY = ;
// 		iSaveAsListPos.iX = ;
// 		iSaveAsListPos.iY = ;
	}
	else if(screenSize.iWidth == 208&& screenSize.iHeight == 208)	//5500
	{
		iScrollBarWidth = 5;
		iIconNumPerLine = 3;
		iIconLineNum = 3;
		iInnerMargin = 2;
		iOuterMargin = 5;
		iMultiple = 1;
		iDivisor = 1;
		iLoadingTextPos_1 = 125;
		iLoadingTextPos_2 = 177;

// 		iSaveAsListPos.iX = ;
// 		iSaveAsListPos.iY = ;
// 		iSaveAsListPos.iX = ;
// 		iSaveAsListPos.iY = ;
	}
	else if(screenSize.iWidth == 800&& screenSize.iHeight == 352)	//
	{
		iScrollBarWidth = 6;

		iSaveAsListPos;
		iSaveAsListPos;
	}
	else	//其它未知机型
	{
		//ASSERT(0);
	}
}

void CScreenLayout::InitByFont()
{
//	WriteLogsL(_L("CScreenLayout::InitByFont"));
	iFontHeight = iDefaultFont->HeightInPixels();
	//////////////////////////////////////////////////////////////////////////
	iTextLineHeight = iFontHeight;
	iTextLineHeight += iTextLineHeight/3;
	//////////////////////////////////////////////////////////////////////////
	iListBoxLineHeight = iFontHeight;
	iListBoxLineHeight += iListBoxLineHeight/2;
	//////////////////////////////////////////////////////////////////////////
}

/*
void CScreenLayout::InitDialogRect()
{

	void CDialog::InitBitmaps()
	{
		iDialogBmp = iEngine->CreateBmpL(EMbmSoarskyresDialog);
		iDialogIcon = iEngine->CreateBmpL(EMbmSoarskyresDialog_icon);
		iDialogIconMask = iEngine->CreateBmpL(EMbmSoarskyresDialog_icon_mask);


		TInt multiple = iScreenLayout->Multiple();
		TInt divisor = iScreenLayout->Divisor();

		iDialogSize = iDialogBmp->SizeInPixels();
		iDialogSize.iWidth = (iDialogSize.iWidth*multiple)/divisor;
		iDialogSize.iHeight = (iDialogSize.iHeight*multiple)/divisor;

		AknIconUtils::SetSize(iDialogBmp,iDialogSize);


		iIconSize = iDialogIcon->SizeInPixels();
		iIconSize.iWidth = (iIconSize.iWidth*multiple)/divisor;
		iIconSize.iHeight = (iIconSize.iHeight*multiple)/divisor;

		AknIconUtils::SetSize(iDialogIcon,iIconSize);
		AknIconUtils::SetSize(iDialogIconMask,iIconSize);

		//上下左右居中
		TSize screenSize = CScreenLayout::ScreenSize();

		iStartPoint.iX = (screenSize.iWidth - iDialogSize.iWidth)/2;
		iStartPoint.iY = (screenSize.iHeight - iDialogSize.iHeight)/2;

		iIconPoint.iX = 0;
		iIconPoint.iY = (iDialogSize.iHeight - iIconSize.iHeight)/2;

		iTextLengthPerLine = iDialogSize.iWidth - iIconSize.iWidth/3;

		iMargin = iScreenLayout->InnerMargin();
		iTextLengthPerLine -= iMargin*2;

		// 	TSize textSize = iDialogSize;
		// 	textSize.iWidth -= iIconSize.iWidth;
	}

}
*/
