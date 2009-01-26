/*
============================================================================
 Name        : ContentItem.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CContentItem declaration
============================================================================
*/

#ifndef CONTENTITEM_H
#define CONTENTITEM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#ifdef __SERIES60_3X__
	#include <e32cmn.h>
#endif

class CContentItem : public CBase
{
public: // Constructors and destructor


	~CContentItem();
	static CContentItem* NewL();
	static CContentItem* NewLC();

private:

	CContentItem();
	void ConstructL();
public:

	CContentItem& GetChildItem(TInt aIndex);
	TInt GetChildCount();//子目录个数

	void AppendChildItem(const TDesC& aName,TInt aID,TInt aTime);

	void SetName(const TDesC& aName);
	const TDesC& GetName();

	void SetID(TInt aID);
	TInt GetID();

	void SetTimeFlag(TInt aTime);
	TInt GetTimeFlag();

	void SetPreTimeFlag(TInt aTime);
	TBool IsUpdate();

	void ClearChildItem();
private:
	TInt CheckChildItem(TInt aID);
	void EditChildItem(const TDesC& aName,TInt aIndex,TInt aTime);
private:
	RPointerArray<CContentItem> iChildArray;
	HBufC* iName;
	TInt iID;
	TInt iTime;
	TInt iPreTime;
};

#endif // CONTENTITEM_H
