/*
============================================================================
 Name        : SelectBox.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectBox declaration
============================================================================
*/

#ifndef SELECTBOX_H
#define SELECTBOX_H

// INCLUDES
#include "Control.h"
#include "NotifyTimer.h"

class CMainEngine;
class CScreenLayout;
class CBitmapFactory;
// CLASS DECLARATION

enum TSelectType
{
	ESelectText,
	ESelectRedio,
	ESelectCheckBox
};
/**
*  CSelectBox
* 
*/
class CSelectBox : public CControl
{
public: // Constructors and destructor

	~CSelectBox();
	static CSelectBox* NewL(CMainEngine& aMainEngine,TInt aType);
	static CSelectBox* NewLC(CMainEngine& aMainEngine,TInt aType);

private:

	CSelectBox(CMainEngine& aMainEngine,TInt aType);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void DrawItem(CGraphic& gc,TRect aRect) const;
	inline TBool IsSelected()
	{
		return iBool;
	}
	inline TInt GetType()
	{
		return iType;
	}
	void	SetSelected(TBool aBool);
	void	SetText(const TDesC& aText);
	void	SetGroupName(const TDesC& aName);

	const TDesC& GetGroupName();
private:
	void InitBitmap();
	void DrawSelectIcon(CGraphic& gc,TPoint aPoint,TInt aID) const;
private:
	CMainEngine&			iMainEngine;	//¡Ÿ ± π”√
	const CScreenLayout&	iScreenLayout;
	CBitmapFactory& iBitmapFactory;

	HBufC* iText;
	HBufC* iName;

	TRect iRect;
	TSize iSelectIconSize;

	TInt iType;

	TBool iBool;
};

#endif // SELECTBOX_H
