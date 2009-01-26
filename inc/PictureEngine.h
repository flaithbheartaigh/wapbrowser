/*
============================================================================
 Name        : PictureEngine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对于特殊应用绘制特殊图片，例如透明的蒙板
============================================================================
*/

#ifndef PICTUREENGINE_H
#define PICTUREENGINE_H
#include <fbs.h>
#include <CoCoPreDefine.h>

#define	DEFAULT_DISPLAYMODE	(EColor64K)
#define IMAGE_FILE_COUNT	8

class CBmpPicture:public CBase
{
public:
	CBmpPicture();
	virtual ~CBmpPicture();

public:
	TBool HasMask();
	TBool IsConnect();
	TBool Create(TSize size);
	TBool Create(TInt nWidth, TInt nHeight);
	TBool Connect(const TDesC& szFilename, TInt nPos, TInt nMaskPos = -1);
	TBool Connect(TInt nFileID = -1, TInt nPos = -1, TInt nMaskPos = -1);
	TBool Disconnect();
	TBool AlphaBlend(TPoint pos, CBmpPicture* image, const TRect& rect, TInt nAlpha);
	TBool Shadow(TInt nAlpha, TRect* pRect = NULL);
public:
	const TDesC* GetImageFilename(TInt nFieldID);
	inline CFbsBitmap* GetBitmap()
	{
		return	iBitmap;
	}
	inline CFbsBitmap* GetMaskBitmap()
	{
		return	iMaskBitmap;
	}
	inline TBool HasCreateFromFile()
	{
		return	(iFileID >= 0);
	}
	inline TSize GetSize() const
	{
		if (iBitmap)
			return	iBitmap->SizeInPixels();
		else
			return	TSize(0,0);
	}
	inline TBool GetScanLine(TDes8& aBuf, const TPoint& aPixel, TInt aLength,TDisplayMode aDispMode)
	{
		if (iBitmap)
		{
			iBitmap->GetScanLine(aBuf, aPixel, aLength, aDispMode);
			return	ETrue;
		}
		else
			return	EFalse;
	}
	inline TBool GetScanLineMask(TDes8& aBuf, const TPoint& aPixel, TInt aLength,TDisplayMode aDispMode)
	{
		if (iMaskBitmap)
		{
			iMaskBitmap->GetScanLine(aBuf, aPixel, aLength, aDispMode);
			return	ETrue;
		}
		else
			return	EFalse;
	}
	inline TBool SetScanLine(TDes8& aBuf,TInt aY)
	{
		if (iBitmap)
		{
			iBitmap->SetScanLine(aBuf, aY);
			return	ETrue;
		}
		else
			return	EFalse;
	}
protected:
	CFbsBitmap*	iBitmap;
	CFbsBitmap*	iMaskBitmap;

	TInt	iFileID;
	TInt	iPos, iMaskPos;

};

class CBmpPictureArray : public CBmpPicture
{
public:
	CBmpPictureArray();
	~CBmpPictureArray();
	
	inline TInt AppendIcon(const TRect& rect)
	{
		if (NULL == iArrIconRect)
		{
			iArrIconRect = new (ELeave)CArrayFixFlat<TRect>(16);
		}
		iArrIconRect->AppendL(rect);
		return	iArrIconRect->Count();
	}
	inline TInt AppendIconList(const RECT* rectList)
	{
		DELETE(iArrIconRect);
		for (TInt n = 0; rectList[n].nWidth && rectList[n].nHeight; n ++)
		{
			TRect	rect = TRect(TPoint(rectList[n].nLeft, rectList[n].nTop),
								TSize(rectList[n].nWidth, rectList[n].nHeight));
			this->AppendIcon(rect);
		}
		return	iArrIconRect->Count();
	}
	inline TRect IconRect(TInt nIndex)
	{
		return	iArrIconRect->At(nIndex);
	}
	inline TSize IconSize(TInt nIndex)
	{
		return	IconRect(nIndex).Size();
	}
	inline TInt IconCount()
	{
		if (iArrIconRect)
			return	iArrIconRect->Count();
		else
			return	0L;
	}
protected:
	CArrayFixFlat<TRect>*	iArrIconRect;

};
#endif // PICTUREENGINE_H
