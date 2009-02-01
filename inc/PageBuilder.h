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
#include "Widgets.h"
// CLASS DECLARATION

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

	const CWidget& Element(int aIndex) const
	{
		ASSERT(aIndex >= 0 && aIndex < ElementAmount());
		CWidget* element = iElementArray[aIndex];
		return *element;
	}

private:
	RPointerArray<CWidget> iElementArray;
	HBufC* iRootLink;
};

#endif // PAGEBUILDER_H
