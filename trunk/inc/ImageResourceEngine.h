/*
============================================================================
 Name        : ImageResourceEngine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 将所有图片读入缓存中
============================================================================
*/

#ifndef IMAGERESOURCEENGINE_H
#define IMAGERESOURCEENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <CoCoPreDefine.h>
// CLASS DECLARATION
class CMainEngine;
class CBmpPicture;
class CBmpPictureArray;
class CFbsBitmap;
/**
*  CImageResourceEngine
* 
*/
class CImageResourceEngine : public CBase
{
public: // Constructors and destructor

	~CImageResourceEngine();
	static CImageResourceEngine* NewL(CMainEngine& aMainControlEngine);
	static CImageResourceEngine* NewLC(CMainEngine& aMainControlEngine);

public:
	
	CFbsBitmap*	GetBitmapByID(TInt aBitmapID) const;

	TFileName GetMbmFileName() const;

	CFbsBitmap* CreateBmpById(TInt aId) const;

private:

	CImageResourceEngine(CMainEngine& aMainControlEngine);
	void ConstructL();
private:

	TFileName iResPath;

	CMainEngine& iMainEngine;

};

#endif // IMAGERESOURCEENGINE_H
