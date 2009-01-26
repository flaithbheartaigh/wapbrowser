/*
============================================================================
Name        : MyImage.cpp
Author      : 
Version     :
Copyright   : Your copyright notice
Description : CPicEngine implementation
============================================================================
*/

#include <FBS.H>
#include <GDI.H>
#include <bitstd.h>
#include <bitdev.h>

#include "PicEngine.h"
#include "CoCoPreDefine.h"
#include "eikenv.h"
//#include <soarsky.rsg>
#include "aknnotewrappers.h"

#include "StaticCommonTools.h"

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

CPicEngine::CPicEngine()
{
	// No implementation required
	iBitmap = NULL;
	iUtil = NULL;
	iCurrentFrame = 0;
	iSaveBitmap = NULL;
	iIsFinished = ETrue;
}

CPicEngine::~CPicEngine()
{
	iBitmap =  NULL;
	if(NULL != iSaveBitmap)
		delete iSaveBitmap;
	iSaveBitmap = NULL;

	if(NULL != iUtil )
		delete iUtil;
	iUtil = NULL;
	if(iGifImage.Count() > 0)
		iGifImage.ResetAndDestroy();
}


TInt CPicEngine::LoadL( const TFileName& aFileName,TDisplayMode aDisplayMode)
{
	::WriteLogsTemp(_L("LoadL-1")); 
	iCurrentFrame  = 0;
	iDisplayMode   = aDisplayMode;
	iFileName      = aFileName;
	iStatus = KErrNone;

	::WriteLogsTemp(_L("LoadL-2")); 
//#ifndef __SERIES60_2X__
//	CMdaImageFileToBitmapUtility* util = CMdaImageFileToBitmapUtility::NewL( *this ) ;
//#else
	CPicConvEngine*  util = CPicConvEngine::NewL(this);
//#endif

	CleanupStack::PushL( util );
	::WriteLogsTemp(_L("LoadL-3")); 
	util->OpenL( aFileName );
//#ifndef __SERIES60_2X__
//	CActiveScheduler::Start();
//#endif
	User::LeaveIfError( iStatus );
	::WriteLogsTemp(_L("LoadL-4")); 
	iTotal = util->FrameCount();
	TFrameInfo frameInfo;
	util->FrameInfo(iCurrentFrame, frameInfo );
	::WriteLogsTemp(_L("LoadL-5")); 
	if(NULL != iBitmap)
	{
		delete iBitmap;
		iBitmap = NULL;
	}
	iBitmap = new(ELeave)CFbsBitmap();
	User::LeaveIfError( iBitmap->Create( frameInfo.iOverallSizeInPixels, aDisplayMode ) );

	util->ConvertL( *iBitmap );	
	::WriteLogsTemp(_L("LoadL-6")); 
	CActiveScheduler::Start();
	::WriteLogsTemp(_L("LoadL-7")); 
	User::LeaveIfError( iStatus );
	::WriteLogsTemp(_L("LoadL-8")); 
	CleanupStack::PopAndDestroy( util );
	::WriteLogsTemp(_L("LoadL-9")); 
	for(TInt i=0;i<iGifImage.Count();i++)
	{
		iGifImage.Remove(i);
	}
	::WriteLogsTemp(_L("LoadL-11")); 
	//if(iGifImage.Count() > 0)
	iGifImage.Reset();
	::WriteLogsTemp(_L("LoadL-12")); 
	iGifImage.Append(iBitmap);
	::WriteLogsTemp(_L("LoadL-13")); 
	return iTotal;
}


TInt CPicEngine::Update()
{
//#ifndef __SERIES60_3X__
	iCurrentFrame++;
	if(iGifImage.Count() >= iTotal )
	{
		if(iCurrentFrame >= iTotal)
			iCurrentFrame = 0;	
		return ETrue;
	}

	if(NULL == iUtil)
		iUtil = CPicConvEngine::NewL(this);

	if(!iIsFinished)
	{
		iCurrentFrame--;
		return ETrue;	
	}


	iUtil->OpenL(iFileName);
	User::LeaveIfError(iStatus);

	//TInt nTotalCount = iUtil->FrameCount();

	/*if(iCurrentFrame >= nTotalCount)
	{
	iCurrentFrame = 0;	
	iGifImage.ResetAndDestroy();
	}*/

	TFrameInfo frameInfo;
	iUtil->FrameInfo(iCurrentFrame, frameInfo );

	iBitmap = new(ELeave)CFbsBitmap();
	User::LeaveIfError( iBitmap->Create( frameInfo.iOverallSizeInPixels, iDisplayMode ) );

	iUtil->ConvertL( *iBitmap );
	iIsFinished = EFalse;	
	CActiveScheduler::Start();
	User::LeaveIfError( iStatus );

	if(NULL != iBitmap)
		iGifImage.Append(iBitmap);
//#endif    
	return 0;
	//ASSERT(EFalse);	//没有返回值,请修改
}

TInt CPicEngine::Deactivate()
{

	if(NULL != iUtil )
		delete iUtil;
	iUtil = NULL;

	return ETrue;	
}

TInt CPicEngine::ParseFileExt(const TFileName& aFileName)
{
	TParse parse;
	parse.Set(aFileName, NULL,NULL);
	TPtrC ptrC = parse.Ext();

	TPtr ptr((TUint16*)ptrC.Ptr(),ptrC.Length(),ptrC.Length());
	ptr.LowerCase();

	if(ptr.Compare(KJPG()) == 0)
	{
		iFileType = EJpg;
	}
	else if(ptr.Compare(KGIF()) == 0)
	{
		iFileType = EGif;
	}
	else if(ptr.Compare(KBMP()) == 0)
	{
		iFileType = EBmp;
	}
	else if(ptr.Compare(KMBM()) == 0)
	{
		iFileType = EMbm;
	}
	else
	{
		iFileType = ENotSupport;
	}
	return iFileType;
}

void CPicEngine::SaveL(const TFileName& aFileName, CFbsBitmap* aBitmap)
{
	iStatus = KErrNone;

	ParseFileExt(aFileName);
//#ifndef __SERIES60_2X__        
//	CMdaImageBitmapToFileUtility* util = CMdaImageBitmapToFileUtility::NewL( *this ) ;
//	CleanupStack::PushL( util );
//
//	TMdaClipFormat* pPicFormat = NULL;
//	switch(iFileType)
//	{
//	case EJpg:
//		{
//			TMdaJfifClipFormat* pJpgFormat = new (ELeave) TMdaJfifClipFormat();
//			pJpgFormat->iSettings.iQualityFactor = KJpgQuality;
//			pJpgFormat->iSettings.iSampleScheme = TMdaJpgSettings::TColorSampling(TMdaJpgSettings::EColor420);
//			pPicFormat = pJpgFormat;	
//		}
//		break;
//	case EGif:
//		{
//			pPicFormat = new(ELeave)TMdaGif89aClipFormat();  	
//		}
//		break;
//	case EBmp:
//		{
//			pPicFormat = new(ELeave)TMdaBmpClipFormat();  	    	
//		}
//		break;
//	}
//	CleanupStack::PushL(pPicFormat);
//
//	TMdaPackage* codec = NULL;	
//	util->CreateL(aFileName,pPicFormat,codec,NULL );
//	CActiveScheduler::Start();
//	User::LeaveIfError( iStatus );
//
//	if(NULL != aBitmap)
//	{
//		util->ConvertL(*aBitmap, 0);
//		CActiveScheduler::Start();
//	}
//	else if(NULL != iBitmap)
//	{
//		util->ConvertL(*iBitmap, 0);
//		CActiveScheduler::Start();	
//	}
//	User::LeaveIfError( iStatus );
//
//	CleanupStack::PopAndDestroy( pPicFormat);
//
//#else
	CPicConvEngine*  util = CPicConvEngine::NewL(this);
	CleanupStack::PushL( util );


	TImageDataBlock* pPicData = NULL;
	switch(iFileType)
	{
	case EJpg:
		{
			util->CreateL(aFileName,KJpgMime());
			User::LeaveIfError( iStatus );

			TJpegImageData* jpgData = new (ELeave) TJpegImageData();
			CleanupStack::PushL(jpgData);	// jpgData will be deleted by the Multi Media Framework
			jpgData->iQualityFactor = KJpgQuality;
			jpgData->iSampleScheme = TJpegImageData::EColor420;

			pPicData = jpgData;
		}
		break;
	case EGif:
		{
			util->CreateL(aFileName,KGifMime());
			User::LeaveIfError( iStatus );

			TGifComment* gifData    = new (ELeave)TGifComment();
			CleanupStack::PushL(gifData);
			gifData->iComment = NULL;

			pPicData = gifData;
		}
		break;
	case EBmp:
		{
			util->CreateL(aFileName,KBmpMime());
			User::LeaveIfError( iStatus );

			TBmpImageData* bmpData    = new (ELeave)TBmpImageData();
			CleanupStack::PushL(bmpData);
			bmpData->iBitsPerPixel = 24;

			pPicData = bmpData;
		}
		break;
	default:
		{
			CleanupStack::PopAndDestroy();   	
		} 
		break;

	}
	CFrameImageData* pImageData = CFrameImageData::NewL();
	User::LeaveIfError(pImageData->AppendImageData(pPicData));	// Passes ownership if successful.

	CleanupStack::Pop(pPicData); // jpgData now owned by iJpgImageData.

	if(NULL != aBitmap)
	{
		util->ConvertL(*aBitmap, pImageData);
		CActiveScheduler::Start();
	}
	else if(NULL != iBitmap)
	{
		util->ConvertL(*iBitmap, pImageData);
		CActiveScheduler::Start();	
	}
	User::LeaveIfError( iStatus );
	delete pImageData;
	//pJpgImageData
//#endif	
	CleanupStack::PopAndDestroy( util );
}

TInt CPicEngine::SaveMbmL(const TFileName& aFileName, CFbsBitmap* aBitmap)
{
	if(NULL != aBitmap)
	{
		aBitmap->Save(aFileName);
		return ETrue;
	}
	else if(NULL != iBitmap)
	{
		iBitmap->Save(aFileName);
		return ETrue;
	}
	return EFalse;
}

TInt CPicEngine::SetToWallPaper()
{
	//卞涛修改设置壁纸20070623
	if(iFileName.Length()==0)
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
		if(iGifImage.Count() <= 0)
			return EFalse;

		CFbsBitmap*   pTempBitmap = iGifImage[0];
		TSize aSize = pTempBitmap->SizeInPixels();
		TBool nRes = ETrue;

		CFbsBitmap* pBitmap = NULL;
		CFbsBitmapDevice* pBitmapDevices = NULL;
		CFbsBitGc* pGc  = NULL;

		if(aSize.iWidth > 176 || aSize.iHeight > 144)
		{
			pBitmap  = new(ELeave)CFbsBitmap();
			CleanupStack::PushL(pBitmap); 
			pBitmap->Create(TSize(176,144),iDisplayMode);

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

			/*HBufC* message = CEikonEnv::Static()->AllocReadResourceLC(R_MESSAGE_TEXT);
			// Show information note dialog
			CAknInformationNote* note = new (ELeave) CAknInformationNote;
			note->ExecuteLD(message->Des());
			CleanupStack::PopAndDestroy(message);*/
			return ETrue;
		}
		return EFalse;
	}
	#endif
}

void CPicEngine::MiuoConvertComplete( TInt aError )
{
	iIsFinished = ETrue;
	iStatus = aError;   
	CActiveScheduler::Stop();

	//通知打开完成
	if(NULL!=iState)
		iState->OpenCompleteEvent ();
}


void CPicEngine::MiuoCreateComplete( TInt aError )
{
	// manual create...
	iStatus = aError;
//#ifndef __SERIES60_2X__   
//	CActiveScheduler::Stop();
//#endif
}


void CPicEngine::MiuoOpenComplete( TInt aError )
{
	iStatus = aError;
//#ifndef __SERIES60_2X__
//	CActiveScheduler::Stop();
//#endif
}

void CPicEngine::DrawImage(CGraphicsContext& aGc,TRect& /*aDesRect*/,const TRect& /*aSrcRect*/,TInt aDrawType) const
{
	TInt nMaxWidth;
	TInt nMaxHeight;
	TInt nBeginY;
	TInt nBeginX;

	TInt nTotal = iGifImage.Count();
	if(iCurrentFrame >= nTotal)
		return;

	CFbsBitmap* pBitmap =  iGifImage[iCurrentFrame];
	if(NULL != pBitmap)
	{ 

		//if(aDrawType==0)
		//{
		////适配显示
		//   nMaxWidth=176;
		//nMaxHeight=144;
		//nBeginY=38;
		//}
		//else
		//{
		////全屏显示
		//   nMaxWidth=176;
		//nMaxHeight=208;
		//nBeginY=0;
		//}
		nMaxWidth=iShowRect.iBr.iX - iShowRect.iTl.iX;
		nMaxHeight=iShowRect.iBr.iY - iShowRect.iTl.iY;
		nBeginY=iShowRect.iTl.iY;
		nBeginX=iShowRect.iTl.iX;
		/*TBuf<100> nTmp;
		nTmp.Zero ();
		nTmp.Append (_L("iTl.iX="));
		nTmp.AppendNum(iShowRect.iTl.iX); 
		nTmp.Append (_L(",iTl.iY="));
		nTmp.AppendNum(iShowRect.iTl.iY); 

		nTmp.Append (_L("iBr.iX="));
		nTmp.AppendNum(iShowRect.iBr.iX); 
		nTmp.Append (_L(",iBr.iY="));
		nTmp.AppendNum(iShowRect.iBr.iY); 
		::WriteLogsTemp(nTmp); */

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
