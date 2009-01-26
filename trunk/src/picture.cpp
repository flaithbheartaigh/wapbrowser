//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include <eikenv.h>
#include <akniconutils.h>

#include "picture.h"
#include "const.h"
//#include "turkeyengine.h"




#define	ALPHA_COLOR_MASK	(0x07E0F81F)	//MASK:0000 0,111 11,10 000,0 1111 1,000 00,01 1111

#define	KMaxAlpha (0x20)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBmpPicture::CBmpPicture(CMainEngine* pGameObserver)
{
	m_pBitmap = NULL;
	m_pMaskBitmap = NULL;
	m_nFileID	= -1;
	m_nPos		= -1;
	m_nMaskPos	= -1;
	m_pGameObserver = pGameObserver;
}
CBmpPicture::~CBmpPicture()
{
	Disconnect();
}

CMainEngine*	CBmpPicture::GetEngine()
{
	return m_pGameObserver;
}

TBool CBmpPicture::HasMask()
{
	if (m_pMaskBitmap)
		return	ETrue;
	else
		return	EFalse;
}
TBool CBmpPicture::IsConnect()
{
	if (m_pBitmap)
		return	ETrue;
	else
		return	EFalse;
}
TBool CBmpPicture::Create(TSize size)
{
	m_pBitmap = new (ELeave) CFbsBitmap;
	m_pBitmap->Create(size, DEFAULT_DISPLAYMODE);
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
		m_nFileID	= nFileID;
		m_nPos		= nPos;
		m_nMaskPos	= nMaskPos;
	}
	Debug_My_Assert((m_nFileID >= 0) && (m_nFileID < IMAGE_FILE_COUNT));
	TFileName	szFilename;
	szFilename.Append(*GetImageFilename(m_nFileID));
	return	Connect(szFilename, m_nPos, m_nMaskPos);
}
TBool CBmpPicture::Connect(const TDesC& szFilename, TInt nPos, TInt nMaskPos)
{
	Debug_My_Assert (nPos >= 0);
//	m_pBitmap->Load(szFilename,nPos);
	//m_pBitmap = STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(szFilename,nPos);
	m_pBitmap = AknIconUtils::CreateIconL(szFilename,nPos);
	Debug_My_Assert(m_pBitmap);
	if (nMaskPos >= 0)
	{
		//m_pMaskBitmap->Load(szFilename,nMaskPos);
		//m_pMaskBitmap = STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(szFilename,nMaskPos);
		m_pMaskBitmap = AknIconUtils::CreateIconL(szFilename,nMaskPos);
		Debug_My_Assert(m_pMaskBitmap);
	}
	return	ETrue;
}
TBool CBmpPicture::Disconnect()
{
	DELETE(m_pBitmap);
	DELETE(m_pMaskBitmap);
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

CBmpPictureArray::CBmpPictureArray(CMainEngine* pEngine) : CBmpPicture(pEngine)
{
	m_arrIconRect = NULL;
}
CBmpPictureArray::~CBmpPictureArray()
{
	DELETE(m_arrIconRect);
}

