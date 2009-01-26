/*
============================================================================
 Name        : SearchResultTempItem.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultTempItem declaration
============================================================================
*/

#ifndef SEARCHRESULTTEMPITEM_H
#define SEARCHRESULTTEMPITEM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


class CSearchResultItem;
/**
*  CSearchResultTempItem
* 
*/
class CSearchResultTempItem : public CBase
{
public: // Constructors and destructor
	~CSearchResultTempItem();
	static CSearchResultTempItem* NewL();
	static CSearchResultTempItem* NewLC();
private:
	CSearchResultTempItem();
	void ConstructL();
public:
	void AddItem(const TDesC& aName,const TDesC8 aUrl,TBool aBool);
	void CreatNewItem();

	CSearchResultItem& GetItem(TInt aIndex);
	TInt GetItemCount();

	void SetPrePageUrl(const TDesC8& aUrl);
	void SetNextPageUrl(const TDesC8& aUrl);

	const TDesC8& GetHomePageUrl();
	const TDesC8& GetPrePageUrl();
	const TDesC8& GetNextPageUrl();
	const TDesC8& GetEndPageUrl();
	const TDesC8& GetGoToPageUrl(TInt aPage,TDes8& aUrl);

	const TDesC8& GetCurUrl(){return *iCurUrl;}
	void  SetCurUrl(const TDesC8& aUrl);

	void SetKeyWord(const TDesC& aText);
	const TDesC& GetKeyWord(){ return *iKeyWord; }

	void SetCurPage(TInt aPage);
	TInt GetCurPage(){ return iCurPage; }
	void SetAllPage(TInt aPage);
	TInt GetAllPage(){ return iAllPage; }

	TBool IsTurnPage();
private:
	void ParseUrl(const TDesC8& aUrl);
private:
	RPointerArray<CSearchResultItem> iPointArray;
	HBufC8* iHomePageUrl;
	HBufC8* iPrePageUrl;
	HBufC8* iNextPageUrl;
	HBufC8* iEndPageUrl;

	HBufC8* iUrl;

	HBufC8* iCurUrl;

	HBufC* iKeyWord;

	TInt iCurPage;
	TInt iAllPage;
};

#endif // SEARCHRESULTTEMPITEM_H
