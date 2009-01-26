/*
============================================================================
 Name        : SystemInfoDialog.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 显示公告信息
============================================================================
*/

#ifndef SYSTEMINFODIALOG_H
#define SYSTEMINFODIALOG_H

// INCLUDES
#include "Control.h"
#include "CoCoPreDefine.h"


class CMainEngine;
class MInfoObserver;
class CScreenLayout;
// CLASS DECLARATION

/**
*  CSystemInfoDialog
* 
*/
class CSystemInfoDialog :public CControl
{
public: // Constructors and destructor
	~CSystemInfoDialog();
	static CSystemInfoDialog* NewL(MInfoObserver& aObserver,CMainEngine& aMainEngine);
	static CSystemInfoDialog* NewLC(MInfoObserver& aObserver,CMainEngine& aMainEngine);

private:
	CSystemInfoDialog(MInfoObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic) const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;
	
public:
	void SetShowInfo(const TDesC& aTitle, const TDesC& aText,TInt aType);

	void SaveInfoToFile(const TDesC& aInfo,const TDesC& aTitle);
private:
	void SetInitValue();

	void ParseString(const TDesC& aText);

	void DrawInfo(CGraphic& aGraphic) const;
private:
	MInfoObserver& iObserver;
	CMainEngine& iMainEngine;

	const CScreenLayout& iScreenLayout;

	TFileName iText;
	TInt iFlag;
	TInt iSFlag;

	TInt iShowType;//0-表示从启动界面调用，1-从系统设置界面调用
	
	TBuf<50> iTitle;
	TInt iPage;
	TInt iUrlIndex;
	TInt iSelIndex;
	CArrayFixFlat<TCONTENTTEXT>* iTextData;
	CArrayFixFlat<TURLTEXT>* iAIndexData;

	TRect iLRect;
	TRect iBRect;
	TSize iSSzie;
	TInt iFontHeight;
	TInt iFontWidth;
	TInt iLineHeight;
	TInt iMaxLine;
};

#endif // SYSTEMINFODIALOG_H
