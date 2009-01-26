/*
============================================================================
 Name        : ContentItem.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CContentItem implementation
============================================================================
*/

#include "ContentItem.h"
#include "CoCoPreDefine.h"

CContentItem::CContentItem()
{
	// No implementation required
}


CContentItem::~CContentItem()
{
	iChildArray.Close();
	DELETE(iName);
}

CContentItem* CContentItem::NewLC()
{
	CContentItem* self = new (ELeave)CContentItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CContentItem* CContentItem::NewL()
{
	CContentItem* self=CContentItem::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CContentItem::ConstructL()
{
	
}
CContentItem& CContentItem::GetChildItem(TInt aIndex)
{
	return *iChildArray[aIndex];
}
TInt CContentItem::GetChildCount()//子目录个数
{

	return iChildArray.Count();

}

void CContentItem::AppendChildItem(const TDesC& aName,TInt aID,TInt aTime)
{
	TInt index=CheckChildItem(aID);
	if(index>-1)
	{
		EditChildItem(aName,index,aTime);
	}
	else
	{
		CContentItem* contentItem=CContentItem::NewL();
		contentItem->SetName(aName);
		contentItem->SetID(aID);
		contentItem->SetTimeFlag(aTime);
		iChildArray.Append(contentItem);
	}
}


void CContentItem::SetName(const TDesC& aName)
{
	DELETE(iName);
	iName=aName.Alloc();
}
const TDesC& CContentItem::GetName()
{
	return *iName;
}
void CContentItem::SetID(TInt aID)
{
	iID=aID;
}
TInt CContentItem::GetID()
{
	return iID;
}
void CContentItem::SetTimeFlag(TInt aTime)
{
	iPreTime=iTime;
	iTime=aTime;
}
TInt CContentItem::GetTimeFlag()
{
	return iTime;
}

void CContentItem::SetPreTimeFlag(TInt aTime)
{
	iTime=aTime;
	iPreTime=aTime;
}
TBool CContentItem::IsUpdate()
{
	TBool tBool=EFalse;
	if(iPreTime<iTime)
		tBool=ETrue;

	return tBool;
}
TInt CContentItem::CheckChildItem(TInt aID)
{
	TInt returnValue=-1;

	for(TInt i=0;i<iChildArray.Count();i++)
	{
		if(iChildArray[i]->GetID()==aID)
		{
			returnValue=i;
			break;
		}
	}
	return returnValue;
}
void CContentItem::EditChildItem(const TDesC& aName,TInt aIndex,TInt aTime)
{
	iChildArray[aIndex]->SetName(aName);
	iChildArray[aIndex]->SetTimeFlag(aTime);
}
void CContentItem::ClearChildItem()
{
	iChildArray.Close();
}