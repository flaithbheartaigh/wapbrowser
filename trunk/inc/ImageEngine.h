/*
============================================================================
 Name        : ImageEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CImageEngine declaration
============================================================================
*/

#ifndef IMAGEENGINE_H
#define IMAGEENGINE_H

#include <e32std.h>
#include <e32base.h>
#include "Define.h"

class CWapEngine;
class CWapHttpEngine;
class CPictureWidget;

class CPicUrlPair
{
public:
	CPicUrlPair() 
		: iUrl(NULL) 
		, iWidget(NULL)
	{
	}
	~CPicUrlPair()
	{
		delete iUrl;
	}
public:
	HBufC8* iUrl;
	CPictureWidget* iWidget;
};

class CImageEngine 
	: public CBase
	, public MWapHttpEngineObserver
{
public: // Constructors and destructor
	~CImageEngine();
	static CImageEngine* NewL(CWapEngine& aWapEngine);
	static CImageEngine* NewLC(CWapEngine& aWapEngine);

private:
	CImageEngine(CWapEngine& aWapEngine);
	void ConstructL();

public://MWapHttpEngineObserver
	virtual void HttpOk(const TDesC8& aData);
	virtual void HttpEmpty();
	virtual void HttpFailed();

public:
	void AddPicUrl(const TDesC8& aPicUrl,CPictureWidget* aPictureWidget);
	void Remove(CPictureWidget* aPictureWidget);
	void CancelAllSession();
	void StartPicSession();

private:
	void RequestNextPic();
	void RequestPic(const TDesC8& aPicUrl);
	void UpdatePic(const TDesC8& aData);
	void SavePic(const TDesC8& aData,const TDesC& aFileName);
	void UpdateWindow();

private:
	CWapEngine&			iWapEngine;
	CWapHttpEngine*		iWapHttpEngine;
	//HBufC8* iReferer;	//TODO:移入CWapEngine
	//RPointerArray<HBufC8> iPictureUrls;
	RPointerArray<CPicUrlPair> iPicUrlPairs;	//注意内存泄漏
	CPicUrlPair*		iCurPicUrlPair;
	TBool				iIsRequesting;
};

#endif // IMAGEENGINE_H
