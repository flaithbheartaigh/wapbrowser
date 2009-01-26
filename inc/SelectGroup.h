/*
============================================================================
 Name        : SelectGroup.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectGroup declaration
============================================================================
*/

#ifndef SELECTGROUP_H
#define SELECTGROUP_H

// INCLUDES
//#include <e32cmn.h>
// CLASS DECLARATION
#include "Control.h"

class CSelectBox;
class CMainEngine;
class CScreenLayout;
class CScrollBar;
/**
*  CSelectGroup
* 
*/
class CSelectGroup :  public CControl
{
public: // Constructors and destructor

	~CSelectGroup();
	static CSelectGroup* NewL(CMainEngine& aMainEngine);
	static CSelectGroup* NewLC(CMainEngine& aMainEngine);

private:

	CSelectGroup(CMainEngine& aMainEngine);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

public:
	void AddSelectItem(const TDesC& aText,const TDesC& aName,TInt aType,TBool aBool=EFalse); //添加单选或者多选
	void AddTextItem(const TDesC& aText); //添加显示文字

	TInt GetCount(const TDesC& aName); //获取某一组个数
	
	TInt GetSelectRedio(const TDesC& aName); //获取被选中单选
	TBool GetSelectCheckBox(const TDesC& aName,TInt aIndex); //需要循环获取被选中多选

	void SelectItemReset();

	void InitLayout(TRect aRect);
	void LayoutScrollBar();
public :
	//用于设置初始的选中项，某个组（aGroupName）中的某项（aIndex）
	TInt SetSelected(const TDesC& aGroupName,TInt aIndex);//王啸华增加
private:
	void ChangeUpSelectedIndex();
	void ChangeDownSelectedIndex();
	void CheckSelect();
private:

	CMainEngine& iMainEngine;	//临时使用
	const CScreenLayout& iScreenLayout;

	TInt iLineWidth;
	TInt iLineHeight;

	TInt iIndex;
	TInt iFirstIndex;
	TInt iSelectedIndex;

	TInt iMaxLinePerPage;
	TSize iItemSize;
	TPoint iStartPoint;

	TRect iRect;

	RPointerArray<CSelectBox> iSelectArray;

	CScrollBar* iScrollbar;
};

#endif // SELECTGROUP_H
