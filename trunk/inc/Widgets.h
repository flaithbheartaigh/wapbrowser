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
CCommand
//////////////////////////////////////////////////////////////////////////
*/
class CCommand
{
public:
	virtual ~CCommand(){};

public:
	virtual void Execute() = 0;
};
/*
//////////////////////////////////////////////////////////////////////////
CWidget
//////////////////////////////////////////////////////////////////////////
*/
class CWidget : public CBase //元素基类
{
public:
	//CWidget(TType aType);
	CWidget(const TDesC& aName)
	{
		//iActive = ETrue;
		iWidgetName.Zero();
		ASSERT(aName.Length() <= iWidgetName.MaxLength());
		iWidgetName.Append(aName);
	};
	virtual ~CWidget();
	TBool IsLink();
// 	void SetLink(const TDesC& aLink);
// 	const TDesC& Link() const;
	void SetLink(const TDesC8& aLink);
	void SetBody(const TDesC8& aBody);
	const TDesC8& Link() const;
	const TDesC8& Body() const;
//	TType Type() const;
	TBool SetActive(TBool aActive);

	TBool Equal(const CWidget& aWidget) const
	{
		return aWidget.iWidgetName.Compare(iWidgetName) == 0;
	};
	TBool Equal(const TDesC& aWidgetName) const
	{
		return iWidgetName.Compare(aWidgetName) == 0;
	}

	void SetCommand(CCommand* aCommand)
	{
		iCommand = aCommand;
	}

	CCommand* Command() const
	{
		return iCommand;
	}

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
	HBufC8* iBody;
	TSize iSize;
	TBool iActive;
	TBuf<20> iWidgetName;
	CCommand* iCommand;
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
#include "Image_Reader.h"

class CPictureWidget 
	: public CWidget
	, public MImageReadyCallBack
{
public:
	CPictureWidget();
	~CPictureWidget();
	void SetPictureLink(const TDesC& aParentLink);
	void SetPictureName(const TDesC& aName);
	void SetImage(const TDesC& aName);
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

public://From MImageReadyCallBack
	virtual void ImageReadyL(const TInt& aError);

private:
	HBufC* iPictureLink;
	HBufC* iParentLink;
	HBufC* iFileName;
	HBufC* iImageName;	
	HBufC* iAlt;
	CFbsBitmap* iBitmap;
	CImage_Reader* iImageReader;
	TSize iSize;
	//int width;
};

#endif // WIDGETS_H
