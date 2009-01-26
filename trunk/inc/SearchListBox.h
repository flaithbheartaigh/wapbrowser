/*
============================================================================
 Name        : SearchListBox.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchListBox declaration
============================================================================
*/

#ifndef SEARCHLISTBOX_H
#define SEARCHLISTBOX_H

// INCLUDES
#include <eikedwin.h>

#include "Control.h"

#include "ListBoxItem.h"//add by hesanyuan
class CMainEngine;
class CScreenLayout;
class MSearchListObserver;
class CListBox;
// CLASS DECLARATION

/**
*  CSearchListBox
* 
*/
class CSearchListBox :  public CControl
{
public: // Constructors and destructor

	~CSearchListBox();
	static CSearchListBox* NewL(MSearchListObserver& aObserver,CMainEngine& aMainEngine);
	static CSearchListBox* NewLC(MSearchListObserver& aObserver,CMainEngine& aMainEngine);

private:

	CSearchListBox(MSearchListObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void	InitListBox();
	void AppendListItem(const TDesC& aText);
	void AppendListItem(const TDesC& aText,const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void AppendListItem(const TDesC& aOneText,const TDesC& aTwoText,const TDesC& aThreeText,const CFbsBitmap* aIcon,const CFbsBitmap* aIconMask);
	void DeleteListItem(TInt aIndex);
	TInt GetSelectedIndex();
	TInt GetListCount();

	void    ListBoxDraw();
	void   SetEdwinVisible(TBool aBool);
	TBool  GetEdwinVisible();

	void GetText(TDes& aDes);

	void PageUp();
	void PageDown();
private:
	void	InitEditor();
	void    DrawEditor(CGraphic& gc) const;
private:

	CListBox*	iListBox;

	MSearchListObserver& iObserver;

	CMainEngine& iMainEngine;
	const CScreenLayout& iScreenLayout;

	CEikEdwin* iEdwin;

	TPoint iPoint;
	TBool iFlag;
};

#endif // SEARCHLISTBOX_H
