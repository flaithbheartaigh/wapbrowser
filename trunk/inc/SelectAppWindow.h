/*
============================================================================
Name        : SelectAppWindow.h
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CSelectAppWindow declaration
描述		 : 浏览文件或文件夹
说明		 : 初始化时将目录设置到根目录,可以从盘符浏览文件
可调用SetPath设置当前目标,如果设置的目录不存在,则报错
============================================================================
*/

#ifndef SelectAppWindow_H
#define SelectAppWindow_H

#include "Control.h"

class CMainEngine;
class CListBox;
class MSelectAppObserver;
class CFbsBitmap;

_LIT(KCoCoAppPath, "\\sys\\bin\\CoCo.exe");
_LIT(KDefaultAppPath,	"Default");

class CSelectAppWindow : public CControl
{
public: // Constructors and destructor
	~CSelectAppWindow();
	static CSelectAppWindow* NewL(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver);
	static CSelectAppWindow* NewLC(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver);

private:
	CSelectAppWindow(CMainEngine& aMainEngine,MSelectAppObserver& aSelectAppObserver);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;
	
private:
	void	DoConfirm();

	void	DoCancel();

	void	InitListBox();

	void	InitBitmaps();

	void	InitAppInfo();
public:
	MSelectAppObserver&	iObserver;
	CMainEngine&	iMainEngine;
	CListBox*		iListBox;

	HBufC*			iTitleText;
	
	TFileName		iCurrentPath;

	CFbsBitmap*		iSaveAsBmp;

	CArrayFixFlat<TFileName>*				iPathArray;
	CArrayFixFlat<TFileName>*				iCaptionArray;
	//CArrayFixFlat<CApaMaskedBitmap>*		iIconList; 

	TInt			iRecommendedAppCount;

	TSize			iBmpSize;
	TPoint			iStartPoint;
	TPoint			iTextPoint;
	TInt			iLineHeight;

	TRect			iListBoxRect;
};

#endif // SelectAppWindow_H
