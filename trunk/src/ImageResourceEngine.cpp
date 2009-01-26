/*
============================================================================
 Name        : ImageResourceEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 将所有图片读入缓存中
============================================================================
*/
#include <coemain.h>
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#else
	#include <eikenv.h>
#endif
#include <BAUTILS.H>

#include "ImageResourceEngine.h"
#include "MainEngine.h"
#include "PictureEngine.h"
#include "ScreenLayout.h"
#include "cocobmp.h"

#ifdef __SERIES60_3X__
	#ifdef __WINS__
		_LIT(KMbmFileName,		"Z:\\resource\\apps\\coco.mbm");
	#else
		_LIT(KMbmFileName,		"\\resource\\apps\\coco.mbm");
	#endif
#else
	#ifdef __WINS__
		_LIT(KMbmFileName,		"Z:\\system\\apps\\coco\\coco.mbm");
	#else
		_LIT(KMbmFileName,		"\\system\\apps\\coco\\coco.mbm");
	#endif
#endif


CImageResourceEngine::CImageResourceEngine(CMainEngine& aMainControlEngine):iMainEngine(aMainControlEngine)
{
	// No implementation required
}


CImageResourceEngine::~CImageResourceEngine()
{
	
}

CImageResourceEngine* CImageResourceEngine::NewLC(CMainEngine& aMainControlEngine)
{
	CImageResourceEngine* self = new (ELeave)CImageResourceEngine(aMainControlEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CImageResourceEngine* CImageResourceEngine::NewL(CMainEngine& aMainControlEngine)
{
	CImageResourceEngine* self=CImageResourceEngine::NewLC(aMainControlEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CImageResourceEngine::ConstructL()
{
	
}


CFbsBitmap* CImageResourceEngine::GetBitmapByID(TInt aBitmapID) const
{
	CFbsBitmap* bitmap = CreateBmpById(aBitmapID);
	ASSERT(bitmap);
	return bitmap;
}

TFileName CImageResourceEngine::GetMbmFileName() const
{
	TFileName fileName;
#ifndef __WINS__
	fileName.Append(iMainEngine.GetAppDrive());
#endif
	fileName.Append(KMbmFileName);
	//User::InfoPrint(fileName);
	RFs& fs = CCoeEnv::Static()->FsSession();
	ASSERT(BaflUtils::FileExists(fs,fileName));
	return fileName;
}

CFbsBitmap* CImageResourceEngine::CreateBmpById(TInt aId) const
{
	#ifdef __SERIES60_3X__
		CFbsBitmap* bitmap=AknIconUtils::CreateIconL(GetMbmFileName(),aId);
	#else
		CFbsBitmap* bitmap=STATIC_CAST( CEikonEnv* ,CCoeEnv::Static())->CreateBitmapL(GetMbmFileName(),aId);
	#endif

	return bitmap;
}
