/*
============================================================================
 Name        : FileAttribute.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileAttribute declaration
============================================================================
*/

#ifndef FILEATTRIBUTE_H
#define FILEATTRIBUTE_H

#include "Control.h"
#include "PreDefine.h"

class CMainEngine;
class CFbsBitmap;

class CFileAttribute : public CControl
{
public: // Constructors and destructor
	~CFileAttribute();
	static CFileAttribute* NewL(CMainEngine& aMainEngine,const TDesC& aFileName);
	static CFileAttribute* NewLC(CMainEngine& aMainEngine,const TDesC& aFileName);

private:
	CFileAttribute(CMainEngine& aMainEngine,const TDesC& aFileName);
	void ConstructL();

public://From CControl
	virtual void	Draw(CGraphic& aGraphic)const;
	virtual TBool	KeyEventL(TInt aKeyCode);
	virtual TBool	HandleCommandL(TInt aCommand);
	virtual void	SizeChanged(const TRect& aScreenRect);	//待论证

	virtual	const TDesC&	LeftButton() const;
	virtual	const TDesC&	RightButton() const;

private:
	void	InitBitmap();

	TInt	GetFileAttributes();

private:
	CMainEngine&	iMainEngine;

	FILEATTRIBUTE	iAttribute;

	CFbsBitmap*		iBitmap;

	TFileName		iFileName;
	TSize			iBmpSize;
	TInt			iMaxLengthInPixel;
};

#endif // FILEATTRIBUTE_H
