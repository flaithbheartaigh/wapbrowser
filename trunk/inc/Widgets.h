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
	//virtual void Draw() {};

public:
	virtual void Draw(CGraphicsContext& /*aGc*/) const{};
	virtual void Move(TPoint& aPoint) = 0;

	void SetTextHeight(int aTextHeight) const
	{
		iTextHeight = aTextHeight;
	}
	mutable int iTextHeight;

	void SetInRegion(TBool aInRegion)
	{
		inRegion = aInRegion;
	}
	TBool inRegion;
	void SetPoint(const TPoint& aPoint)
	{
		point = aPoint;
	}

	TPoint point;

/*
	void SetSize(const TSize& aSize)
	{
		iSize = aSize;
	}
	TSize Size()
	{
		return iSize;
	}
*/

public:
	HBufC* iLink;
	TType iType;
	TSize iSize;
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

public:
	virtual void Move(TPoint& aPoint);
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

public:
	void Draw(/* const CWidget&element, */TBool ainRegion, CGraphicsContext &aGc, TPoint &apoint ) const;

	virtual void Move(TPoint& aPoint);
/*
	void SetInRegion(TBool aInRegion)
	{
		inRegion = aInRegion;
	}
	TBool inRegion;
	void SetPoint(const TPoint& aPoint)
	{
		point = aPoint;
	}

	void MovePoint(TPoint& aPoint)
	{

	}
	TSize Size()
	{

	}
	TPoint point;
*/

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

public:
	void Draw(/* const CWidget&element, */TBool inRegion, TPoint &point, CGraphicsContext &aGc ) const;
	virtual void Move(TPoint& aPoint);

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
