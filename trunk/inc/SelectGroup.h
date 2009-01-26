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
	void AddSelectItem(const TDesC& aText,const TDesC& aName,TInt aType,TBool aBool=EFalse); //��ӵ�ѡ���߶�ѡ
	void AddTextItem(const TDesC& aText); //�����ʾ����

	TInt GetCount(const TDesC& aName); //��ȡĳһ�����
	
	TInt GetSelectRedio(const TDesC& aName); //��ȡ��ѡ�е�ѡ
	TBool GetSelectCheckBox(const TDesC& aName,TInt aIndex); //��Ҫѭ����ȡ��ѡ�ж�ѡ

	void SelectItemReset();

	void InitLayout(TRect aRect);
	void LayoutScrollBar();
public :
	//�������ó�ʼ��ѡ���ĳ���飨aGroupName���е�ĳ�aIndex��
	TInt SetSelected(const TDesC& aGroupName,TInt aIndex);//��Х������
private:
	void ChangeUpSelectedIndex();
	void ChangeDownSelectedIndex();
	void CheckSelect();
private:

	CMainEngine& iMainEngine;	//��ʱʹ��
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
