/*
============================================================================
 Name        : PhoneNumEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPhoneNumEngine declaration
============================================================================
*/

#ifndef PHONENUMENGINE_H
#define PHONENUMENGINE_H

#include <e32std.h>
#include <e32base.h>

class CWapBrowserAppUi;
class CWapBrowserAppView;
class CWapHttpEngine;

class CPhoneNumEngine 
	: public CBase
	, public MWapHttpEngineObserver
{
public:
	~CPhoneNumEngine();
	static CPhoneNumEngine* NewL();
	static CPhoneNumEngine* NewLC();

private:
	CPhoneNumEngine();
	void ConstructL();

public://MWapHttpEngineObserver
	virtual void HttpOk(const TDesC8& aData);
	virtual void HttpEmpty();
	virtual void HttpFailed();

public:
	void RequestL(const TDesC8& aUrl);
	void TestRequest();
	void TestParse();
	void SetAppUi(CWapBrowserAppUi* aAppUi);
	const TDesC8& PhoneNum()  const;

private:
	void Parse(const TDesC8& aData);

private:
	CWapBrowserAppUi* iAppUi;
	CWapHttpEngine* iWapHttpEngine;
	HBufC8* iPhoneNum;
};

#endif // PHONENUMENGINE_H
