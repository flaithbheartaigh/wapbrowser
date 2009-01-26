/*
============================================================================
 Name        : LinkListBox.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CLinkListBox declaration
============================================================================
*/

#ifndef LINKLISTBOX_H
#define LINKLISTBOX_H

// INCLUDES
#include "Control.h"

class CMainEngine;
class CScreenLayout;
// CLASS DECLARATION
class CSearchResultItem;
class MLinkListObserver;
/**
*  CLinkListBox
* 
*/
class CLinkListBox : public CControl
{
public: // Constructors and destructor

	~CLinkListBox();
	static CLinkListBox* NewL(MLinkListObserver& aObserver,CMainEngine& aMainEngine);
	static CLinkListBox* NewLC(MLinkListObserver& aObserver,CMainEngine& aMainEngine);

private:

	CLinkListBox(MLinkListObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void SetRect(TRect aRect);
	void AppendItem(const TDesC& aName,const TDesC8& aUrl,TBool aBool);

	void ResetList();
	void LayoutList();

	TBool IsFirst();

	const TDesC8& GetUrl();
	const TDesC& GetName();
private:
	void InitSize();
	void HandleDownKey();
	void HandleUpKey();
	
private:
	RPointerArray<CSearchResultItem> iPointArray;

	MLinkListObserver& iObserver;
	CMainEngine& iMainEngine;
	HBufC8* iUrl;
	HBufC* iName;
	TInt iFirstIndex;
	TInt iSelectIndex;
	TInt iFirstSelectIndex;

	TInt iMaxLine; 

	TSize iTextSize;
	TRect iRect;

	TBool iBool;

};

#endif // LINKLISTBOX_H
