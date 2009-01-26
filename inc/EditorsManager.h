/*
============================================================================
 Name        : EditorsManager.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 聚合管理所有输入框
============================================================================
*/

#ifndef EDITORSMANAGER_H
#define EDITORSMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CEikEdwin;
class CEikGlobalTextEditor;
class CAknIntegerEdwin;
class CCoeControl;

class CEditorsManager : public CBase
{
public: // Constructors and destructor
	~CEditorsManager();
	static CEditorsManager* NewL(const CCoeControl* aParentControl);

private:
	CEditorsManager(const CCoeControl* aParentControl);
	void ConstructL();

public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	TInt CountComponentControls() const;

	CCoeControl* ComponentControl(TInt aIndex) const;

	CEikEdwin*	CreateEdwin(const TRect& aRect,TInt aTextLimit,TRgb aColor = KRgbBlack);
	CEikGlobalTextEditor*	CreateGlbEdwin(const TRect& aRect,TInt aTextLimit,TRgb aColor = KRgbBlack);
	CAknIntegerEdwin*	CreateIntergerEdwin(const TRect& aRect,TInt aMaxNum,TInt aMaxLength/*,TInt aMinNum = 0,TInt aInitNum = 0*/,TRgb aColor = KRgbBlack);

	void	AddEdwinToArray(CEikEdwin* aEdwin);
	void	RemoveEdwinFromArray(CEikEdwin* aEdwin);
	CEikEdwin*	CurEdwin()const;
	void	SetCurEdwin(CEikEdwin* aEdwin);
	void	ShowAllEdwins();
	void	HideAllEdwins();


	static void		SetEdwinTextColor(CEikEdwin* aEdwin,const TRgb& aColor);

	static void		SetEdwinTextFont(CEikEdwin* aEdwin,const CFont* aFont);

	static void		SetupEdwinText(CEikEdwin* aEdwin,const TRgb& aColor,const CFont* aFont);

private:
	const CCoeControl*			iParentControl;

	RPointerArray<CEikEdwin>	iEdwinArray;
	CEikEdwin*					iCurEdwin;
	CEikEdwin*					iTmpEdwin;
};

#endif // EDITORSMANAGER_H
