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

class CPage : public CBase
{
public: // Constructors and destructor
	~CPage();
// 	static CPage* NewL();
// 	static CPage* NewLC();

//private:
	CPage(CPageBuilder* aPageBuilder,const TRect& aRect);
	void ConstructL();

public:
	void Draw(CGraphicsContext& aGc) const;
	TBool KeyEvent(int aKeyCode);

	//void SetRect(const CRect& aRect);

private:
	CPageBuilder* iPageBuilder;
	int iStartYPos;


	int iTextHeight;
	TRect iRect;
};

#endif // PAGE_H
