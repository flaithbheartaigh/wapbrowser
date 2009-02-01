/*
============================================================================
 Name        : PageBuilder.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPageBuilder implementation
============================================================================
*/
#include <string.h>
#include <fbs.h>
#include <utf.h>

#include "PageBuilder.h"

CPageBuilder::CPageBuilder()
{
	// No implementation required
}


CPageBuilder::~CPageBuilder()
{
	iElementArray.ResetAndDestroy();
}

CPageBuilder* CPageBuilder::NewLC()
{
	CPageBuilder* self = new (ELeave)CPageBuilder();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPageBuilder* CPageBuilder::NewL()
{
	CPageBuilder* self=CPageBuilder::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CPageBuilder::ConstructL()
{

}

/*
void CPageBuilder::AddPicture(const TDesC& aParentLink,const TDesC& aName,const TDesC& aLink)
{

}

void CPageBuilder::AddText(const TDesC& aText,const TDesC& aLink)
{

}
*/
//////////////////////////////////////////////////////////////////////////
//From MPageBuilder
//////////////////////////////////////////////////////////////////////////
void CPageBuilder::AddBr()
{
	iElementArray.Append(new CBrWidget);
}

void CPageBuilder::AddPicture(const char* aName,const char* aAlt,const char* aLink)
{
	ASSERT(aName);
	ASSERT(aAlt);
	CPictureWidget* element = new CPictureWidget;//(TPtrC8((const TUint*)alt),TPtrC8((const TUint*)alt));

	//TPtrC8 ptr((const TUint8*)aName);
	HBufC* name = HBufC::NewLC(strlen(aName) + 1);
	name->Des().Copy(TPtrC8((const TUint8*)aName));
	element->SetPictureName(*name);
	CleanupStack::PopAndDestroy();

// 	HBufC* alt = HBufC::NewLC(strlen(aAlt) + 1);
// 	alt->Des().Copy(TPtrC8((const TUint8*)aAlt));
	TPtrC8 ptr((const TUint8*)aAlt);
	HBufC* alt = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
	CleanupStack::PushL(alt);
	element->SetAlt(*alt);
	CleanupStack::PopAndDestroy();

	if(aLink)
	{
		HBufC* link = HBufC::NewLC(strlen(aLink) + 1);
		link->Des().Copy(TPtrC8((const TUint8*)aLink));
		element->SetLink(*link);
		CleanupStack::PopAndDestroy();
	}

	iElementArray.Append(element);

}

void CPageBuilder::AddText(const char* aText,const char* aLink)
{
	ASSERT(aText);
	//HBufC* text = HBufC::NewLC(strlen(aText) + 1);
	//text->Des().Copy(TPtrC8((const TUint8*)aText));
	TPtrC8 ptr((const TUint8*)aText);
	HBufC* text = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
	CleanupStack::PushL(text);
	CTextWidget* element = new CTextWidget(*text);
	CleanupStack::PopAndDestroy();	//text

	if(aLink)
	{
		HBufC* link = HBufC::NewLC(strlen(aLink) + 1);
		link->Des().Copy(TPtrC8((const TUint8*)aLink));
		element->SetLink(*link);
		CleanupStack::PopAndDestroy();
	}

	iElementArray.Append(element);
}

void CPageBuilder::SetRootLink(const char* aLink)
{
	ASSERT(aLink);
	ASSERT(NULL == iRootLink);
	//TPtrC8 ptr((const TUint8*)aLink);

 	iRootLink = HBufC::NewLC(strlen(aLink) + 1);
 	iRootLink->Des().Copy(TPtrC8((const TUint8*)aLink));
	CleanupStack::PopAndDestroy();
}