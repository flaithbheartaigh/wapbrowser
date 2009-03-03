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
#include "Page.h"

CPageBuilder::CPageBuilder(const TRect& aRect)
	: iRect(aRect)
{
	// No implementation required
}


CPageBuilder::~CPageBuilder()
{
	delete iPage;
	delete iRootLink;
	iElementArray.ResetAndDestroy();
}

CPageBuilder* CPageBuilder::NewLC(const TRect& aRect)
{
	CPageBuilder* self = new (ELeave)CPageBuilder(aRect);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPageBuilder* CPageBuilder::NewL(const TRect& aRect)
{
	CPageBuilder* self=CPageBuilder::NewLC(aRect);
	CleanupStack::Pop(); // self;
	return self;
}

void CPageBuilder::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//From MPageBuilder
//////////////////////////////////////////////////////////////////////////
void CPageBuilder::AddBr()
{
	//CBrWidget* w = new CBrWidget;
	//w->SetSize(TSize(0,KDefaultFont->HeightInPixels()));
	//AddWidget(new CBrWidget);

	Page()->Br();
}

void CPageBuilder::AddPicture(const char* aName,const char* aAlt,const char* aLink)
{
	//return;
	ASSERT(aName);
	ASSERT(aAlt);
	CPictureWidget* w = new CPictureWidget;//(TPtrC8((const TUint*)alt),TPtrC8((const TUint*)alt));

	//TPtrC8 ptr((const TUint8*)aName);
	HBufC* name = HBufC::NewLC(strlen(aName) + 1);
	name->Des().Copy(TPtrC8((const TUint8*)aName));
	w->SetPictureName(*name);
	CleanupStack::PopAndDestroy();

// 	HBufC* alt = HBufC::NewLC(strlen(aAlt) + 1);
// 	alt->Des().Copy(TPtrC8((const TUint8*)aAlt));
	TPtrC8 ptr((const TUint8*)aAlt);
	HBufC* alt = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
	CleanupStack::PushL(alt);
	w->SetAlt(*alt);
	CleanupStack::PopAndDestroy();

	if(aLink)
	{
/*
		HBufC* link = HBufC::NewLC(strlen(aLink) + 1);
		link->Des().Copy(TPtrC8((const TUint8*)aLink));
		w->SetLink(*link);
		CleanupStack::PopAndDestroy();
*/

		TPtrC8 ptr((const TUint8*)aLink);
		w->SetLink(ptr);
	}

	//iElementArray.Append(element);

	AddWidget(w);
}

void CPageBuilder::AddText(const char* aText,const char* aLink)
{
	//return;
	ASSERT(aText);
	//HBufC* text = HBufC::NewLC(strlen(aText) + 1);
	//text->Des().Copy(TPtrC8((const TUint8*)aText));
	TPtrC8 ptr((const TUint8*)aText);
	HBufC* text = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptr);
	CleanupStack::PushL(text);
	CTextWidget* w = new CTextWidget(*text);
	//w->SetSize(TSize(KDefaultFont->MeasureText(*text),KDefaultFont->HeightInPixels()));
	CleanupStack::PopAndDestroy();	//text

	if(aLink)
	{
/*
		HBufC* link = HBufC::NewLC(strlen(aLink) + 1);
		link->Des().Copy(TPtrC8((const TUint8*)aLink));
		w->SetLink(*link);
		CleanupStack::PopAndDestroy();
*/
		TPtrC8 ptr((const TUint8*)aLink);
		w->SetLink(ptr);

	}

	AddWidget(w);
	//iElementArray.Append(element);
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
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
CPage* CPageBuilder::FetchPage()
{
	if(iPage)
	{
		CPage* page = iPage;
		page->Layout();
		iPage = NULL;
		return page;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CPageBuilder::AddWidget(CWidget* aWidget)
{
	Page()->AddWidget(aWidget);
	//iElementArray.Append(element);
}

CPage* CPageBuilder::Page()
{
	if(NULL == iPage)
	{
		iPage = new CPage;
		iPage->SetRect(iRect);
	}
	return iPage;
}