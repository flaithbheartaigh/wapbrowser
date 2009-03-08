/*
============================================================================
 Name        : Widgets.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWidgets implementation
============================================================================
*/
#include "Define.h"
#include "Widgets.h"

/*
//////////////////////////////////////////////////////////////////////////
CWidget
//////////////////////////////////////////////////////////////////////////
*/
CWidget::~CWidget()
{
	delete iLink;
}


TBool CWidget::IsLink()
{
	return iLink && iLink->Length() > 0;
}

/*
void CWidget::SetLink(const TDesC& aLink)
{
	iLink = aLink.Alloc();
}

const TDesC& CWidget::Link() const
{
	//ASSERT(iLink);		//TODO：断言不起作用，待查
	if(iLink)
	{
		return *iLink;
	}
	return KNullDesC;
}
*/
void CWidget::SetLink(const TDesC8& aLink)
{
	iLink = aLink.Alloc();
}

const TDesC8& CWidget::Link() const
{
	//ASSERT(iLink);		//TODO：断言不起作用，待查
	if(iLink)
	{
		return *iLink;
	}
	return KNullDesC8;
}

TBool CWidget::SetActive(TBool aActive)
{
	if(iLink)
	{
		iActive = aActive; 
		return ETrue;
	}
	return EFalse;
}
/*
//////////////////////////////////////////////////////////////////////////
CTextWidget
//////////////////////////////////////////////////////////////////////////
*/
//public:
CTextWidget::CTextWidget(const TDesC& aDes)
	: CWidget(_L("CTextWidget"))
	//: CWidget(EText)
{
	ASSERT(aDes.Length());
	iText = aDes.Alloc();
	ASSERT(iText);
	//iSize
	width = KDefaultFont->MeasureText(*iText);
}
CTextWidget::~CTextWidget()
{
	delete iText;
}

const TDesC& CTextWidget::Text() const
{
	RDebug::Print(_L("Text"));
	if(iText)
	{
		ASSERT(iText);
		RDebug::Print(*iText);
		return *iText;
	}
	return KNullDesC;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//void CTextWidget::Draw( /*const CWidget&element, */TBool ainRegion, CGraphicsContext &aGc, TPoint &apoint ) const

void CTextWidget::Draw(CGraphicsContext &aGc) const
{
	const TDesC& text = /*((CTextWidget&)element).*/Text();

	//gc.DrawRect(rect);

	if(/*element.*/Link().Length())		//带链接
	{
		int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);
		//aGc.SetPenColor(KRgbBlue);
		if(iActive)
		{
			int textHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
			TRect rect(point,TSize(width,textHeight));
			rect.Move(TPoint(0,-textHeight));
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGc.SetBrushColor(KRgbBlue);
			aGc.SetPenStyle(CGraphicsContext::ENullPen);
			aGc.DrawRect(rect);
			aGc.SetPenColor(KRgbWhite);
		}
		else
		{
			aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
			aGc.SetPenStyle(CGraphicsContext::ESolidPen);
			aGc.SetPenColor(KRgbBlue);
		}
		TPoint point1 = point;
		TPoint point2 = point1;
		point2.iX += width;
		aGc.DrawLine(point1,point2);
	}
	else							//不带链接
	{
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenColor(KRgbBlack);
	}
	aGc.DrawText(text,point);
}

void CTextWidget::Move(TPoint& aPoint)
{
	if(iText)
	{
		aPoint.iX += KDefaultFont->MeasureText(*iText);
	}
	//point.iX += width;
}

TSize CTextWidget::Size() const
{
	TSize size;
	size.iWidth = CCoeEnv::Static()->NormalFont()->MeasureText(Text());
	size.iHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
	return size;
}
/*
//////////////////////////////////////////////////////////////////////////
CPictureWidget
//////////////////////////////////////////////////////////////////////////
*/


//public:
	//CPictureWidget(const TDesC& aDes,const TDesC& aLink)
CPictureWidget::CPictureWidget()
	: CWidget(_L("CPictureWidget"))
{
	iTextHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
}

CPictureWidget::~CPictureWidget()
{
	delete iImageReader;
	delete iAlt;
	delete iParentLink;
	delete iPictureLink;
	delete iFileName;
}
void CPictureWidget::SetPictureLink(const TDesC& aParentLink)
{
	iParentLink = aParentLink.Alloc();
	MakePictureLink();
}

void CPictureWidget::SetPictureName(const TDesC& aName)
{
	iFileName = aName.Alloc();
	MakePictureLink();
}

void CPictureWidget::SetAlt(const TDesC& aAlt)
{
	iAlt = aAlt.Alloc();
	width = KDefaultFont->MeasureText(*iAlt);


	ASSERT(NULL == iBitmap);
	ASSERT(NULL == iImageReader);
	iImageReader = new (ELeave)CImage_Reader(*this);
	CleanupStack::PushL(iImageReader);
	iImageReader->ConstructL(_L("C:\\coco.jpg"));
	CleanupStack::Pop(iImageReader);
}

const TDesC& CPictureWidget::Alt() const
{
	RDebug::Print(_L("Alt"));
	if(iAlt)
	{
		ASSERT(iAlt);
		RDebug::Print(*iAlt);
		return *iAlt;
	}
	return KNullDesC;
}
const TDesC& CPictureWidget::Link() const
{
	ASSERT(iPictureLink);
	return *iPictureLink;
}

void CPictureWidget::MakePictureLink()
{
	if(iParentLink && iFileName && NULL == iPictureLink)
	{
		int len = iParentLink->Length() + 1 + iFileName->Length();
		HBufC* iPictureLink = HBufC::NewL(len);
		iPictureLink->Des().Append(*iParentLink);
		iPictureLink->Des().Append(_L("/"));
		iPictureLink->Des().Append(*iFileName);
	}
}

const CFbsBitmap* CPictureWidget::Bitmap() const
{
	return iBitmap;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CPictureWidget::Draw(CGraphicsContext &aGc) const
{
	const TDesC& text = /*((CPictureWidget&)element).*/Alt();

	int width = CCoeEnv::Static()->NormalFont()->MeasureText(text);

	TRect rect(point,TSize(width,iTextHeight));
	rect.Move(TPoint(0,-iTextHeight));

	if(iActive)
	{
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
		rect.Shrink(-1,-1);
		aGc.DrawRect(rect);
	}

	if(iBitmap)
	{
		aGc.DrawBitmap(TRect(point - TPoint(0,iTextHeight),iSize),iBitmap,TRect(iSize));
	}
	else
	{
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenColor(KRgbYellow);
		aGc.DrawRect(rect);

		aGc.SetPenColor(KRgbBlue);
		aGc.DrawText(text,point);
	}
}

void CPictureWidget::Move(TPoint& aPoint)
{
	aPoint.iX += width;
}

TSize CPictureWidget::Size() const
{
	/*
	TSize size;
	size.iWidth = CCoeEnv::Static()->NormalFont()->MeasureText(Alt());
	//size.iHeight = CCoeEnv::Static()->NormalFont()->HeightInPixels();
	size.iHeight = 53;
	return size;
	*/

	return iSize;
}
//////////////////////////////////////////////////////////////////////////
//From MImageReadyCallBack
//////////////////////////////////////////////////////////////////////////
void CPictureWidget::ImageReadyL(const TInt& aError)
{
	ASSERT(iImageReader);
	if(KErrNone == aError)
	{
		ASSERT(NULL == iBitmap);
		iBitmap = iImageReader->Bitmap();
		iSize = iBitmap->SizeInPixels();
	}
}

