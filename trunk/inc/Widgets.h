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
/*
public:
	enum TType		//TODO:改为多态
	{
		EText,
		EPicture,
		EBr
	};
*/

public:
	//CWidget(TType aType);
	CWidget(){};
	virtual ~CWidget();
	TBool IsLink();
// 	void SetLink(const TDesC& aLink);
// 	const TDesC& Link() const;
	void SetLink(const TDesC8& aLink);
	const TDesC8& Link() const;
//	TType Type() const;

public:
	//virtual void Draw() {};

public:
	virtual void Draw(CGraphicsContext& /*aGc*/) const = 0;
	virtual void Move(TPoint& aPoint) = 0;
	virtual TSize Size() const = 0;


	void SetTextHeight(int aTextHeight) const
	{
		iTextHeight = aTextHeight;
	}
	mutable int iTextHeight;

	void SetPoint(const TPoint& aPoint)
	{
		point = aPoint;
	}

	TPoint point;

public:
	//HBufC* iLink;
	HBufC8* iLink;
	TSize iSize;
};

/*
//////////////////////////////////////////////////////////////////////////
CBrWidget
//////////////////////////////////////////////////////////////////////////
*/

/*
class CBrWidget : public CWidget
{
public:
	CBrWidget();

public:
	virtual void Move(TPoint& aPoint);
};
*/

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

public:
	virtual void Draw(CGraphicsContext &aGc) const;
	virtual void Move(TPoint& aPoint);
	virtual TSize Size() const;

private:
	HBufC* iText;
	int width;
};

/*
//////////////////////////////////////////////////////////////////////////
CPictureWidget
//////////////////////////////////////////////////////////////////////////
*/
class CPictureWidget : public CWidget
{
public:
	CPictureWidget();
	~CPictureWidget();
	void SetPictureLink(const TDesC& aParentLink);
	void SetPictureName(const TDesC& aName);
	void SetAlt(const TDesC& aAlt);
	const TDesC& Alt() const;
	const TDesC& Link() const;
	void MakePictureLink();
	const CFbsBitmap* Bitmap() const;
	//const TSize& Size() const;

public:
	virtual void Draw(CGraphicsContext &aGc) const;
	virtual void Move(TPoint& aPoint);
	virtual TSize Size() const;

private:
	HBufC* iPictureLink;
	HBufC* iParentLink;
	HBufC* iFileName;
	HBufC* iAlt;
	CFbsBitmap* iBitmap;
	TSize iSize;
	int width;
};

#endif // WIDGETS_H
