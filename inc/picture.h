
#ifndef __PICTURE_H__
#define __PICTURE_H__

#include "const.h"
//#include <e32std.h>

#include <fbs.h>



#define	DEFAULT_DISPLAYMODE	(EColor64K)
#define IMAGE_FILE_COUNT	8


class CMainEngine;
class CBmpPicture:public CBase
{
public:
	CBmpPicture(CMainEngine* pGameObserver);
	virtual ~CBmpPicture();

public:
	TBool HasMask();
	TBool IsConnect();
	TBool Create(TSize size);
	TBool Create(TInt nWidth, TInt nHeight);
	TBool Connect(const TDesC& szFilename, TInt nPos, TInt nMaskPos = -1);
	TBool Connect(TInt nFileID = -1, TInt nPos = -1, TInt nMaskPos = -1);
	TBool Disconnect();
	CMainEngine*	GetEngine();
	TBool AlphaBlend(TPoint pos, CBmpPicture* image, const TRect& rect, TInt nAlpha);
	TBool Shadow(TInt nAlpha, TRect* pRect = NULL);
public:
	const TDesC* GetImageFilename(TInt nFieldID);
	inline CFbsBitmap* GetBitmap()
	{
		return	m_pBitmap;
	}
	inline CFbsBitmap* GetMaskBitmap()
	{
		return	m_pMaskBitmap;
	}
	inline TBool HasCreateFromFile()
	{
		return	(m_nFileID >= 0);
	}
	inline TSize GetSize() const
	{
		if (m_pBitmap)
			return	m_pBitmap->SizeInPixels();
		else
			return	TSize(0,0);
	}
	inline TBool GetScanLine(TDes8& aBuf, const TPoint& aPixel, TInt aLength,TDisplayMode aDispMode)
	{
		if (m_pBitmap)
		{
			m_pBitmap->GetScanLine(aBuf, aPixel, aLength, aDispMode);
			return	ETrue;
		}
		else
			return	EFalse;
	}
	inline TBool GetScanLineMask(TDes8& aBuf, const TPoint& aPixel, TInt aLength,TDisplayMode aDispMode)
	{
		if (m_pMaskBitmap)
		{
			m_pMaskBitmap->GetScanLine(aBuf, aPixel, aLength, aDispMode);
			return	ETrue;
		}
		else
			return	EFalse;
	}
	inline TBool SetScanLine(TDes8& aBuf,TInt aY)
	{
		if (m_pBitmap)
		{
			m_pBitmap->SetScanLine(aBuf, aY);
			return	ETrue;
		}
		else
			return	EFalse;
	}
protected:
	CFbsBitmap*	m_pBitmap;
	CFbsBitmap*	m_pMaskBitmap;

	TInt	m_nFileID;
	TInt	m_nPos, m_nMaskPos;
	CMainEngine*		m_pGameObserver;

// 	TUint16	uSrcBuf[SCREEN_WIDTH*2];
// 	TUint8	uSrcBufMask[SCREEN_WIDTH*2];
// 
// 	TUint16	uDesBuf[SCREEN_WIDTH*2];
// 	TUint8	uDesBufMask[SCREEN_WIDTH*2];

};

class CBmpPictureArray : public CBmpPicture
{
public:
	CBmpPictureArray(CMainEngine* pEngine);
	~CBmpPictureArray();
	
	inline TInt AppendIcon(const TRect& rect)
	{
		if (NULL == m_arrIconRect)
		{
			m_arrIconRect = new (ELeave)CArrayFixFlat<TRect>(16);
		}
		m_arrIconRect->AppendL(rect);
		return	m_arrIconRect->Count();
	}
	inline TInt AppendIconList(const RECT* rectList)
	{
		DELETE(m_arrIconRect);
		for (TInt n = 0; rectList[n].nWidth && rectList[n].nHeight; n ++)
		{
			TRect	rect = TRect(TPoint(rectList[n].nLeft, rectList[n].nTop),
								TSize(rectList[n].nWidth, rectList[n].nHeight));
			this->AppendIcon(rect);
		}
		return	m_arrIconRect->Count();
	}
	inline TRect IconRect(TInt nIndex)
	{
//		MY_ASSERT(m_arrIconRect);
//		MY_ASSERT((nIndex>=0) && (nIndex < m_arrIconRect->Count()));
		return	m_arrIconRect->At(nIndex);
	}
	inline TSize IconSize(TInt nIndex)
	{
		return	IconRect(nIndex).Size();
	}
	inline TInt IconCount()
	{
		if (m_arrIconRect)
			return	m_arrIconRect->Count();
		else
			return	0L;
	}
protected:
	CArrayFixFlat<TRect>*	m_arrIconRect;

};

#endif // !defined(AFX_PIXMAP_H__C1726089_5981_42D1_9BAE_88F77C2D2CDF__INCLUDED_)
