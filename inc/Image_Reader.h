/*
============================================================================
 Name        : Image_Reader.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CImage_Reader declaration
============================================================================
*/

#ifndef IMAGE_READER_H
#define IMAGE_READER_H


#include <e32base.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h> 
#include <ImageConversion.h>
#include <coecntrl.h>

class CFbsBitmap;

class MImageReadyCallBack
{
public:
	virtual void ImageReadyL(const TInt& aError) = 0;
};

class CImage_Reader : public CActive
{
public:
	CImage_Reader(MImageReadyCallBack& aNotify);
	void ConstructL(const TDesC& aFileName);
	~CImage_Reader();
public:
	CFbsBitmap* Bitmap();
protected:
	void DoCancel();
	void RunL();
private:
	void GetFileType(const TDesC& aFileName, TDes8& aFileType);
private:
	MImageReadyCallBack&	iNotify;
	CImageDecoder*			iImageDecoder;
	CFbsBitmap*				iFrame;
	TFileName				iImageName;
};

#endif // IMAGE_READER_H
