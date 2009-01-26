/*
============================================================================
 Name        : PageBuilder.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPageBuilder declaration
============================================================================
*/

#ifndef PAGEBUILDER_H
#define PAGEBUILDER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "Define.h"
// CLASS DECLARATION


//////////////////////////////////////////////////////////////////////////
//CElement
//////////////////////////////////////////////////////////////////////////
class CElement : public CBase //元素基类
{
public:
	enum TType		//后期改为多态
	{
		EText,
		EPicture,
		EBr
	};
public:
	CElement(TType aType)
		: iType(aType)
	{
	}
	~CElement()
	{
		delete iLink;
	}

	void SetLink(const TDesC& aLink)
	{
		iLink = aLink.Alloc();
	}

	TBool IsLink()
	{
		return iLink && iLink->Length() > 0;
	}

	const TDesC& Link() const
	{
		//ASSERT(iLink);		//TODO：断言不起作用，待查
		if(iLink)
		{
			return *iLink;
		}
		return KNullDesC;
	}

	TType Type() const
	{
		return iType;
	}

public:
	HBufC* iLink;
	TType iType;
};

//////////////////////////////////////////////////////////////////////////
//CBrElement
//////////////////////////////////////////////////////////////////////////

class CBrElement : public CElement
{
public:
	CBrElement()
		: CElement(EBr)
	{
	}
};
//////////////////////////////////////////////////////////////////////////
//CTextElement
//////////////////////////////////////////////////////////////////////////
class CTextElement : public CElement
{
public:
	CTextElement(const TDesC& aDes)
		: CElement(EText)
	{
		ASSERT(aDes.Length());
		iText = aDes.Alloc();
		ASSERT(iText);
	}
	~CTextElement()
	{
		delete iText;
	}

	const TDesC& Text() const
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

private:
	HBufC* iText;
};

//////////////////////////////////////////////////////////////////////////
//CPictureElement
//////////////////////////////////////////////////////////////////////////
class CPictureElement : public CElement
{
public:
	//CPictureElement(const TDesC& aDes,const TDesC& aLink)
	CPictureElement()
		: CElement(EPicture)
	{
	}

	~CPictureElement()
	{
		delete iAlt;
		delete iParentLink;
		delete iPictureLink;
		delete iFileName;
	}
	void SetPictureLink(const TDesC& aParentLink)
	{
		iParentLink = aParentLink.Alloc();
		MakePictureLink();
	}

	void SetPictureName(const TDesC& aName)
	{
		iFileName = aName.Alloc();
		MakePictureLink();
	}

	void SetAlt(const TDesC& aAlt)
	{
		iAlt = aAlt.Alloc();
	}

	const TDesC& Alt() const
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
	const TDesC& Link() const
	{
		ASSERT(iPictureLink);
		return *iPictureLink;
	}

	void MakePictureLink()
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

	const CFbsBitmap* Bitmap() const
	{
		return iBitmap;
	}

	const TSize& Size() const
	{
		return iSize;
	}

private:
	HBufC* iPictureLink;
	HBufC* iParentLink;
	HBufC* iFileName;
	HBufC* iAlt;
	CFbsBitmap* iBitmap;
	TSize iSize;
};

//////////////////////////////////////////////////////////////////////////
//CPageBuilder
//////////////////////////////////////////////////////////////////////////
class CPageBuilder 
	: public CBase
	, public MPageBuilder 
{
public: // Constructors and destructor
	~CPageBuilder();
	static CPageBuilder* NewL();
	static CPageBuilder* NewLC();

private:
	CPageBuilder();
	void ConstructL();

public://From MPageBuilder
	virtual void AddBr();
	virtual void AddPicture(const char* aName,const char* alt,const char* aLink);
	virtual void AddText(const char* aText,const char* aLink);
	virtual void SetRootLink(const char* aLink);

public:
	void AddPicture(const TDesC& aName,const TDesC& aLink = KNullDesC);
	void AddText(const TDesC& aText,const TDesC& aLink = KNullDesC);

/*
	void AddBr();

	void AddPicture(const char* aName,const char* alt,const char* aLink = NULL);
	void AddText(const char* aText,const char* aLink = NULL);
*/

	/*
	void SetPictureLink(const TDesC& aParentLink)
	{
	iParentLink = aParentLink.Alloc();
	MakePictureLink();
	}

	void SetPictureName(const TDesC& aName)
	*/

	//以下两个函数考虑修改为迭代器
	int ElementAmount() const
	{
		return iElementArray.Count();
	}

	const CElement& Element(int aIndex) const
	{
		ASSERT(aIndex >= 0 && aIndex < ElementAmount());
		CElement* element = iElementArray[aIndex];
		return *element;
	}

private:
	RPointerArray<CElement> iElementArray;
	HBufC* iRootLink;
};

#endif // PAGEBUILDER_H
