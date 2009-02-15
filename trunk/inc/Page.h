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
class CWidgetGroup;
class CScrollBar;

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
	void Br();

	void Layout();

private:
	void AddGroup();

	CWidgetGroup* CurWidgetGroup() const;

	void Execute();
	void Up();
	void Down();
	void Left();
	void Right();

private:
	RPointerArray<CWidgetGroup> iWidgetGroupArray;

	//RPointerArray<CWidget> iWidgetArray;

	CWidgetGroup* iWidgetGroup;

	int iTextHeight;
	TRect iRect;

	int iFocusIndex;		//焦点项的索引
	int iStartIndex;		//绘制的开始索引,闭区间
	int iEndIndex;			//绘制的结束索引,开区间

	TBool iLayout;			//是否已经调用了Layout();

	int iTotalHeight;

	CScrollBar* iScrollBar;
};

#endif // PAGE_H
