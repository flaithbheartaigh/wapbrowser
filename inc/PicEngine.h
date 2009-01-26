/*
============================================================================
Name        : MyImage.h
Author      : 
Version     :
Copyright   : Your copyright notice
Description : CPicEngine declaration
============================================================================
*/

#ifndef MYIMAGE_H
#define MYIMAGE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

//#ifndef __SERIES60_2X__
//#include <MdaImageConverter.h>
//#else
#include "PicConvEngine.h"
//#endif

typedef enum
{
	EJpg,
	EGif,
	EBmp,
	EMbm,
	ENotSupport
}TFileConvType;

class CFbsBitmap;
class MPicServer;

class MPicServer 
{
public:

	virtual void OpenCompleteEvent() = 0;
};

class CPicEngine : public CBase, public MImageConvObserver
{
public: // Constructors and destructor
	/**
	* Constructor for performing 1st stage construction
	*/
	CPicEngine();

	/**
	* Destructor.
	*/
	~CPicEngine();

	TInt ParseFileExt(const TFileName& aFileName);
	TInt LoadL( const TFileName& aFileName, TDisplayMode aDisplayMode);
	void SaveL(const TFileName& aFileName, CFbsBitmap* aBitmap);
	TInt SaveMbmL(const TFileName& aFileName, CFbsBitmap* aBitmap);
	void DrawImage(CGraphicsContext& aGc, TRect& aDesPoint,const TRect& aSrcRect,TInt aDrawType) const;
	void MiuoConvertComplete( TInt aError );
	void MiuoCreateComplete( TInt aError );
	void MiuoOpenComplete( TInt aError );
	TInt Update();
	TInt Deactivate();
	TInt SetToWallPaper();

	inline void SetState(MPicServer* aState)
	{
		iState = aState;
	}

	inline void SetShowRect(TRect aShow)
	{
		iShowRect=aShow;
	}
private:


private:
	CFbsBitmap*     iBitmap;
	CFbsBitmap*     iSaveBitmap;
	TInt		    iStatus;	// contains loading and conversion error status
	TInt            iCurrentFrame;
	CPicConvEngine*   iUtil;
	TDisplayMode    iDisplayMode;
	TInt			iFileType;    
	TFileName       iFileName;
	RPointerArray<CFbsBitmap>    iGifImage;
	TBool           iIsFinished;
	TInt            iTotal;

	MPicServer*		iState;

	TRect iShowRect;
};

#endif // MYIMAGE_H
