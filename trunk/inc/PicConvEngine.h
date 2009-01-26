/*
============================================================================
Name        : MyImageConv.h
Author      : 
Version     :
Copyright   : Your copyright notice
Description : CPicConvEngine declaration
============================================================================
*/

#ifndef MYIMAGECONV_H
#define MYIMAGECONV_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <f32file.h>	// RFs
#include <FBS.H>

//#ifndef __SERIES60_2X__
//#include <mdaimageconverter.h>
//#else
#define __WITHIN_MCI_LIBRARY
#include <ICL\ImageData.h>
#include <ICL\ImageCodecData.h>
//#endif

class CImageDecoder;
class CImageEncoder;
class CFrameImageData;
class MImageConvObserver
{
public :
	virtual void MiuoConvertComplete( TInt aError ) = 0;
	virtual void MiuoCreateComplete( TInt aError )  = 0;
	virtual void MiuoOpenComplete( TInt aError )    = 0;
};
class CPicConvEngine : public CActive
{
public:
	static CPicConvEngine* NewL(MImageConvObserver* pObserver);
	static CPicConvEngine* NewLC(MImageConvObserver* pObserver);

	// Cancel and destroy
	~CPicConvEngine();

public: // New functions
	// Function for making the initial request
	void OpenL(const TFileName& aFileName);
	void ConvertL(CFbsBitmap& aBitmap);
	void ConvertL(CFbsBitmap& aBitmap,CFrameImageData* pImageData);
	void FrameInfo( TInt aFrameIndex, TFrameInfo& aframeInfo);

	void CPicConvEngine::CreateL(const TFileName& aFileName,const TDesC8& aMIMEType);
	TInt FrameCount();
private: // From CActive


	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// C++ constructor
	CPicConvEngine(MImageConvObserver* pObserver);

	// Second-phase constructor
	void ConstructL();

private:
	TInt              iState;
	CImageDecoder*    iDecoder;
	CImageEncoder*    iEncoder;
	RFs               iFs;
	TFileName         iFileName;
	MImageConvObserver* iObserver;
	TInt              iFrameIndex;
	TBool             iFlag;
};

#endif
