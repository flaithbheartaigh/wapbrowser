/*
============================================================================
 Name        : PictureEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对于特殊应用绘制特殊图片，例如透明的蒙板
============================================================================
*/

#include "PictureEngine.h"

#include <eikenv.h>

#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif

#define	ALPHA_COLOR_MASK	(0x07E0F81F)	//MASK:0000 0,111 11,10 000,0 1111 1,000 00,01 1111

#define	KMaxAlpha (0x20)


CBmpPicture::CBmpPicture()
{
	iBitmap = NULL;
	iMaskBitmap = NULL;
	iFileID	= -1;
	iPos		= -1;
	iMaskPos	= -1;

}
CBmpPicture::~CBmpPicture()
{
	Disconnect();
}

TBool CBmpPicture::HasMask()
{
	if (iMaskBitmap)
		return	ETrue;
	else
		return	EFalse;
}
TBool CBmpPicture::IsConnect()
{
	if (iBitmap)
		return	ETrue;
	else
		return	EFalse;
}
TBool CBmpPicture::Create(TSize size)
{
	iBitmap = new (ELeave) CFbsBitmap;
	iBitmap->Create(size, DEFAULT_DISPLAYMODE);
	return	ETrue;
}
TBool CBmpPicture::Create(TInt nWidth, TInt nHeight)
{
	return	Create(TSize(nWidth, nHeight));
}
TBool CBmpPicture::Connect(TInt nFileID, TInt nPos, TInt nMaskPos)
{
	if (IsConnect())
		return ETrue;
	if (nFileID >= 0)
	{
		iFileID	= nFileID;
		iPos		= nPos;
		iMaskPos	= nMaskPos;
	}
	//Debug_My_Assert((iFileID >= 0) && (iFileID < IMAGE_FILE_COUNT));
	TFileName	szFilename;
	szFilename.Append(*GetImageFilename(iFileID));
	return	Connect(szFilename, iPos, iMaskPos);
}
TBool CBmpPicture::Connect(const TDesC& szFilename, TInt nPos, TInt nMaskPos)
{

	#ifdef __SERIES60_3X__
		iBitmap = AknIconUtils::CreateIconL(szFilename,nPos);
	#else
		//iBitmap->Load(szFilename,nPos);
		iBitmap = STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(szFilename,nPos);
	#endif
	if (nMaskPos >= 0)
	{
		//iMaskBitmap->Load(szFilename,nMaskPos);
		//iMaskBitmap = STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(szFilename,nMaskPos);
		#ifdef __SERIES60_3X__
			iMaskBitmap = AknIconUtils::CreateIconL(szFilename,nMaskPos);
		#else
			//iMaskBitmap->Load(szFilename,nMaskPos);
			iMaskBitmap = STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(szFilename,nMaskPos);
		#endif
		//Debug_My_Assert(iMaskBitmap);
	}
	return	ETrue;
}
TBool CBmpPicture::Disconnect()
{
	DELETE(iBitmap);
	DELETE(iMaskBitmap);
	return	ETrue;
}

TBool CBmpPicture::AlphaBlend(TPoint pos, CBmpPicture* image, const TRect& rect, TInt nAlpha)
{
	union UColor
	{
		TUint16	wColor[2];
		TUint32	dwColor;
	};

	TUint16* uSrcBuf = new TUint16[SCREEN_WIDTH*2];
	TUint8*  uSrcBufMask = new TUint8[SCREEN_WIDTH*2];
	TPtr8	pSrcBuf((TUint8*)(uSrcBuf), sizeof(TUint16)*SCREEN_WIDTH*2);
	TPtr8	pSrcBufMask(uSrcBufMask, sizeof(TUint8)*SCREEN_WIDTH*2);


	TUint16* uDesBuf = new TUint16[SCREEN_WIDTH*2];
	TUint8*  uDesBufMask = new TUint8[SCREEN_WIDTH*2];
	TPtr8	pDesBuf((TUint8*)(uDesBuf), sizeof(TUint16)*SCREEN_WIDTH*2);
	TPtr8	pDesBufMask(uDesBufMask, sizeof(TUint8)*SCREEN_WIDTH*2);

	TBool	bHasSrcMask = image->HasMask();
	TBool	bHasDesMask = HasMask();

	UColor	uSrcColor, uDesColor;

	nAlpha = (nAlpha >> 3) & 0x1F;

	for (TInt nLine = 0; nLine < rect.Size().iHeight; nLine ++)
	{
		image->GetScanLine(pSrcBuf, rect.iTl + TPoint(0,nLine), rect.Width(), DEFAULT_DISPLAYMODE);
		if (bHasSrcMask)
			image->GetScanLineMask(pSrcBufMask, rect.iTl + TPoint(0,nLine), rect.Width(), EColor256);

		TUint16*	pSrcColor = uSrcBuf;
		TUint8*		pSrcMask = uSrcBufMask;

		GetScanLine(pDesBuf, TPoint(0, pos.iY + nLine), GetSize().iWidth, DEFAULT_DISPLAYMODE);
		if (bHasDesMask)
			GetScanLineMask(pDesBufMask, TPoint(0, pos.iY + nLine), GetSize().iWidth, EColor256);
		TUint16*	pDesColor = &uDesBuf[pos.iX];
		TUint8*		pDesMask = &uDesBufMask[pos.iX];

		for (TInt nCol = 0; nCol < rect.Width(); nCol ++, pSrcColor ++, pDesColor ++, pSrcMask ++, pDesMask++)
		{
			if (bHasSrcMask && !(*pSrcMask))
				continue;
			if (bHasDesMask && !(*pDesMask))
				continue;

			//5:6:5
			uSrcColor.wColor[0] = *pSrcColor;
			uSrcColor.wColor[1] = *pSrcColor;
			uSrcColor.dwColor &= ALPHA_COLOR_MASK;
			uDesColor.wColor[0] = *pDesColor;
			uDesColor.wColor[1] = *pDesColor;
			uDesColor.dwColor &= ALPHA_COLOR_MASK;

			uDesColor.dwColor = uDesColor.dwColor * (KMaxAlpha - nAlpha) + uSrcColor.dwColor * nAlpha;
			uDesColor.dwColor = (uDesColor.dwColor >> 5) & ALPHA_COLOR_MASK;
			*pDesColor = (TUint16)(uDesColor.wColor[0] | uDesColor.wColor[1]);
		}
		SetScanLine(pDesBuf, pos.iY + nLine);
	}

 	delete [] uSrcBuf;
 	delete [] uSrcBufMask;
 	delete [] uDesBuf;
 	delete [] uDesBufMask;

	return	ETrue;
}
//*/

TBool CBmpPicture::Shadow(TInt nAlpha, TRect* pRect)
{
	union UColor
	{
		TUint16	wColor[2];
		TUint32	dwColor;
	};
	TUint16	uBuf[SCREEN_WIDTH];
	TUint8	uBufMask[SCREEN_WIDTH];
	TPtr8	pBuf((TUint8*)(&uBuf[0]), sizeof(uBuf));
	TPtr8	pBufMask(uBufMask, sizeof(uBufMask));
	TBool	bHasMask = HasMask();
	UColor	uColor;
	TRect	rect;

	nAlpha = (nAlpha >> 3) & 0x1F;
	rect = pRect ? (*pRect) : TRect(TPoint(0,0), GetSize());

	for (TInt nLine = 0; nLine < rect.Size().iHeight; nLine ++)
	{
		GetScanLine(pBuf, TPoint(0, rect.iTl.iY + nLine), GetSize().iWidth, DEFAULT_DISPLAYMODE);
		if (bHasMask)
			GetScanLineMask(pBufMask, TPoint(0, rect.iTl.iY + nLine), GetSize().iWidth, EColor256);
		TUint16*	pColor = &uBuf[rect.iTl.iX];
		TUint8*		pMask = &uBufMask[rect.iTl.iX];

		for (TInt nCol = 0; nCol < rect.Width(); nCol ++, pColor ++, pMask++)
		{
			if (bHasMask && !(*pMask))
				continue;
			uColor.wColor[0] = *pColor;
			uColor.wColor[1] = *pColor;
			uColor.dwColor &= ALPHA_COLOR_MASK;

			uColor.dwColor = uColor.dwColor * nAlpha;
			uColor.dwColor = (uColor.dwColor >> 5) & ALPHA_COLOR_MASK;
			*pColor = (TUint16)(uColor.wColor[0] | uColor.wColor[1]);
		}
		SetScanLine(pBuf, rect.iTl.iY + nLine);
	}
	return	ETrue;
}

const TDesC* CBmpPicture::GetImageFilename(TInt /*nFieldID*/)
{
	return NULL;
}

CBmpPictureArray::CBmpPictureArray() : CBmpPicture()
{
	iArrIconRect = NULL;
}
CBmpPictureArray::~CBmpPictureArray()
{
	DELETE(iArrIconRect);
}

