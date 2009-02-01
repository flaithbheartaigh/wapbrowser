/*
============================================================================
 Name        : Page.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPage declaration
============================================================================
*/
#ifndef PAGE_H
#define PAGE_H

#include <e32std.h>
#include <e32base.h>

class CPageBuilder;
class CWidget;

class CPage : public CBase
{
public: // Constructors and destructor
	CPage();
	~CPage();
	void ConstructL();

public:
	void Draw(CGraphicsContext& aGc) const;

	TBool KeyEvent(int aKeyCode);

	void SetRect(const TRect& aRect);


	void AddWidget(CWidget* aWidget);

private:
	//以下两个函数考虑修改为迭代器
/*
	int ElementAmount() const
	{
		return iWidgetArray.Count();
	}
*/

/*
	const CWidget& Element(int aIndex) const
	{
		ASSERT(aIndex >= 0 && aIndex < ElementAmount());
		CWidget* element = iWidgetArray[aIndex];
		return *element;
	}
*/

	void DrawText( const CWidget& element, TBool inRegion, CGraphicsContext &aGc, TPoint &point ) const; 
	void DrawPicture( const CWidget& element, TBool inRegion, TPoint &point, CGraphicsContext &aGc ) const;

private:
	RPointerArray<CWidget> iWidgetArray;

	CPageBuilder* iPageBuilder;
	int iStartYPos;


	int iTextHeight;
	TRect iRect;
};

#endif // PAGE_H
