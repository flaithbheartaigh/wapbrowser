/*
============================================================================
 Name        : DirBrowser.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDirBrowser declaration
============================================================================
*/

#ifndef DIRBROWSER_H
#define DIRBROWSER_H

#include "Control.h"
#include "PreDefine.h"

class CMainEngine;

class CDirBrowser : public CControl
{
public: // Constructors and destructor
	~CDirBrowser();
	static CDirBrowser* NewL(CMainEngine& aMainEngine,const TDesC& aTitleText,TInt aSelType);
	static CDirBrowser* NewLC(CMainEngine& aMainEngine,const TDesC& aTitleText,TInt aSelType);

private:
	CDirBrowser(CMainEngine& aMainEngine,TInt aSelType);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);

public:
	void SetPath(const TDesC& aFilePath);
	TInt SaveFile(TInt IsOverwrite);
	void ReadSubDir();
	void ReadParentsDir();
/*
	void SetTitleString(TPOPUPTEXT nTitleText)
	{
		if(nTitleText.Length() <= 0)
			return;

		if(m_nTitleText.Length() > 0)
			m_nTitleText.Delete(0,m_nTitleText.Length());

		m_nTitleText.Copy(nTitleText);
	}
*/

	void NewDialogEvent(TInt nKey);
	//From MDialogEvent
	virtual void DialogConfirm();
	virtual void DialogCancel();

	void ShowDialog(const TDesC& aDes,TInt aType);

	void HandleComplete(TInt aStatus,TInt aType);

private:
	void	InitBitmaps();

private:
	CMainEngine&	iMainEngine;

	TInt m_nSelIndex;
	TInt m_nFirstIndex;
	TFileName m_pCurPath;
	CArrayFixFlat<TFileName>*	m_pPathArray;


	TFileName m_pFilePath;

	//TPOPUPTEXT   m_nTitleText;
	HBufC*		iTitleText;
	TInt m_nSelType;
	RFs rfs;
	CDialog*		iDialog;
	TInt	m_nFlag;

	CFileEngine* iFileEngine;
	TFileName iNewFileName;
	TInt iCancel;

	CFbsBitmap*		iSaveAsBmp;
	TSize			iBmpSize;
	TPoint			iStartPoint;
	TInt			iLineHeight;

};

#endif // DIRBROWSER_H
