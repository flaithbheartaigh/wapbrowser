/*
============================================================================
 Name        : SearchResultItem.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultItem implementation
============================================================================
*/

#include "SearchResultItem.h"
#include "CoCoPreDefine.h"

CSearchResultItem::CSearchResultItem()
:iName(NULL)
,iUrl(NULL)
,iBool(EFalse)
,iFlag(ETrue)
{
	// No implementation required
}


CSearchResultItem::~CSearchResultItem()
{
	DELETE(iName);
	DELETE(iUrl);
}

CSearchResultItem* CSearchResultItem::NewLC()
{
	CSearchResultItem* self = new (ELeave)CSearchResultItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSearchResultItem* CSearchResultItem::NewL()
{
	CSearchResultItem* self=CSearchResultItem::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchResultItem::ConstructL()
{
	iUrl = HBufC8::NewL(0);
	iName = HBufC::NewL(0);

}
void CSearchResultItem::SetName(const TDesC& aName)
{
	iFlag=EFalse;
	if (iName)
	{
		iName = iName->ReAllocL(iName->Length() + aName.Length());
		iName->Des().Append(aName);
	}
	else
	{
		iName = HBufC::NewL(aName.Length());
		iName->Des().Copy(aName);
	}
}
const TDesC& CSearchResultItem::GetName()
{
	return *iName;
}
void CSearchResultItem::SetUrl(const TDesC8& aUrl)
{
	iFlag=EFalse;
	DELETE(iUrl);
	iUrl = HBufC8::NewL(aUrl.Length());
	iUrl->Des().Copy(aUrl);
}
const TDesC8& CSearchResultItem::GetUrl()
{
	return *iUrl;
}
void CSearchResultItem::SetIsLink(TBool aBool)
{
	iFlag=EFalse;
	iBool=aBool;
}
TBool CSearchResultItem::IsLink()
{
	if(iUrl==NULL)
		iBool=EFalse;

	return iBool;
}