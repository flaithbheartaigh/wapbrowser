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
//public:
/*
(
enum CWidget::TType		//TODO:改为多态
{
	EText,
	EPicture,
	EBr
};)
*/

//public:
CWidget::CWidget(TType aType)
	: iType(aType)
{
}
CWidget::~CWidget()
{
	delete iLink;
}

void CWidget::SetLink(const TDesC& aLink)
{
	iLink = aLink.Alloc();
}

TBool CWidget::IsLink()
{
	return iLink && iLink->Length() > 0;
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

CWidget::TType CWidget::Type() const
{
	return iType;
}

/*
//////////////////////////////////////////////////////////////////////////
CBrWidget
//////////////////////////////////////////////////////////////////////////
*/
//public:
CBrWidget::CBrWidget()
	: CWidget(EBr)
{
}

/*
//////////////////////////////////////////////////////////////////////////
CTextWidget
//////////////////////////////////////////////////////////////////////////
*/
//public:
CTextWidget::CTextWidget(const TDesC& aDes)
	: CWidget(EText)
{
	ASSERT(aDes.Length());
	iText = aDes.Alloc();
	ASSERT(iText);
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
/*
//////////////////////////////////////////////////////////////////////////
CPictureWidget
//////////////////////////////////////////////////////////////////////////
*/


//public:
	//CPictureWidget(const TDesC& aDes,const TDesC& aLink)
CPictureWidget::CPictureWidget()
	: CWidget(EPicture)
{
}

CPictureWidget::~CPictureWidget()
{
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

const TSize& CPictureWidget::Size() const
{
	return iSize;
}
