/*
============================================================================
 Name        : SearchResultItem.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultItem declaration
============================================================================
*/

#ifndef SEARCHRESULTITEM_H
#define SEARCHRESULTITEM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
*  CSearchResultItem
* 
*/
class CSearchResultItem : public CBase
{
public: // Constructors and destructor
	~CSearchResultItem();
	static CSearchResultItem* NewL();
	static CSearchResultItem* NewLC();
private:
	CSearchResultItem();
	void ConstructL();
public:
	void SetName(const TDesC& aName);
	const TDesC& GetName();

	void SetUrl(const TDesC8& aUrl);
	const TDesC8& GetUrl();

	void SetIsLink(TBool aBool);
	TBool IsLink();

	TBool IsNull(){return iFlag;}
private:
	HBufC* iName;
	HBufC8* iUrl;
	TBool iBool;

	TBool iFlag;
};

#endif // SEARCHRESULTITEM_H
