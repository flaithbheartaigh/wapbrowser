/*
============================================================================
 Name        : WidgetGroup.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWidgetGroup declaration
============================================================================
*/

#ifndef WIDGETGROUP_H
#define WIDGETGROUP_H

#include "Define.h"
#include "Page.h"
#include "PageBuilder.h"
#include "Widgets.h"
#include "ScrollBar.h"
#include "WapBrowserappui.h"

/*
//////////////////////////////////////////////////////////////////////////
CWidgetGroup
//////////////////////////////////////////////////////////////////////////
*/
class CWidgetGroup : public CBase
{
public:
	CWidgetGroup();
	~CWidgetGroup();

	void AddWidget(CWidget* aWidget);
	void Draw(CGraphicsContext& aGc) const;
	void Execute();
	void Left();			//向左移动焦点
	void Right();			//向右移动焦点
	TBool HasFocus();		//是否有焦点项

	void SetPoint(const TPoint& aPoint);
	int Height();

	CWidget* FocusWidget();

	const CWidget* FocusWidget() const;

	TBool SetActive(TBool aActive);

private:
	RPointerArray<CWidget> iWidgetArray;
	TPoint iPoint;
	int iFocusIndex;
	int iTextHeight;
	//TBool iFocus;
	TBool iActive;
	CWidget* iFocusWidget;
};


#endif // WIDGETGROUP_H
