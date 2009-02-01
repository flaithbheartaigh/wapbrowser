/*
============================================================================
 Name        : Widgets.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWidgets declaration
============================================================================
*/

#ifndef WIDGETS_H
#define WIDGETS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


/*
//////////////////////////////////////////////////////////////////////////
CWidget
//////////////////////////////////////////////////////////////////////////
*/
class CWidget : public CBase //元素基类
{
public:
	enum TType		//TODO:改为多态
	{
		EText,
		EPicture,
		EBr
	};
public:
	CWidget(TType aType);
	~CWidget();
	void SetLink(const TDesC& aLink);
	TBool IsLink();
	const TDesC& Link() const;
	TType Type() const;

public:
	//virtual void Draw(CGraphicsContext& aGc) const = 0;

public:
	HBufC* iLink;
	TType iType;
};

/*
//////////////////////////////////////////////////////////////////////////
CBrWidget
//////////////////////////////////////////////////////////////////////////
*/

class CBrWidget : public CWidget
{
public:
	CBrWidget();
};
/*
//////////////////////////////////////////////////////////////////////////
CTextWidget
//////////////////////////////////////////////////////////////////////////
*/
class CTextWidget : public CWidget
{
public:
	CTextWidget(const TDesC& aDes);
	~CTextWidget();
	const TDesC& Text() const;

private:
	HBufC* iText;
};

/*
//////////////////////////////////////////////////////////////////////////
CPictureWidget
//////////////////////////////////////////////////////////////////////////
*/
class CPictureWidget : public CWidget
{
public:
	//CPictureWidget(const TDesC& aDes,const TDesC& aLink)
	CPictureWidget();
	~CPictureWidget();
	void SetPictureLink(const TDesC& aParentLink);
	void SetPictureName(const TDesC& aName);
	void SetAlt(const TDesC& aAlt);
	const TDesC& Alt() const;
	const TDesC& Link() const;
	void MakePictureLink();
	const CFbsBitmap* Bitmap() const;
	const TSize& Size() const;

private:
	HBufC* iPictureLink;
	HBufC* iParentLink;
	HBufC* iFileName;
	HBufC* iAlt;
	CFbsBitmap* iBitmap;
	TSize iSize;
};

#endif // WIDGETS_H
