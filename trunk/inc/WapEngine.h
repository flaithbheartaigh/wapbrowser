/*
============================================================================
 Name        : WapEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWapEngine declaration
============================================================================
*/

#ifndef WAPENGINE_H
#define WAPENGINE_H

#include "Define.h"

class CImageEngine;
//class CWmlParser;
class CWapHttpEngine;
class CWapBrowserAppUi;
class CWapBrowserAppView;

class CWapEngine 
	: public CBase
	, public MWapHttpEngineObserver
{
public: // Constructors and destructor
	~CWapEngine();
	static CWapEngine* NewL();
	static CWapEngine* NewLC();

private:
	CWapEngine();
	void ConstructL();

public://MWapHttpEngineObserver
	virtual void HttpOk(const TDesC8& aData);
	virtual void HttpEmpty();
	virtual void HttpFailed();

public:
	void RequestPageL(const TDesC8& aUrl);
	void RequestPageL(const TDesC8& aUrl,const TDesC8& aBody);
	void SetReferer(const TDesC8& aReferer);
	const TDesC8& Referer() const;
	void Reset();	//TODO:有可能合并上两个
	void UpdateWindow();
	void SetAppUi(CWapBrowserAppUi* aAppUi);
	void SetAppView(CWapBrowserAppView* aAppView);

	void ParseData(HBufC8* aBuf);
	void ParseFile(const TDesC8& aFileName);
	void TestParse();

	CImageEngine& ImageEngine();

private:
	//CWmlParser& WmlParser();

private:

private:
	CWapBrowserAppView*	iAppView;
	CWapBrowserAppUi*	iAppUi;
	CWapHttpEngine*		iWapHttpEngine;
	HBufC8*				iReferer;
	CImageEngine*		iImageEngine;
//	CWmlParser*			iWmlParser;
	TBool				iIsRequesting;
};

#endif // WAPENGINE_H
