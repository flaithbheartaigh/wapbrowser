/*
============================================================================
 Name        : ConfigEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CConfigEngine declaration
============================================================================
*/

#ifndef CONFIGENGINE_H
#define CONFIGENGINE_H

#include <e32std.h>
#include <e32base.h>

class CWapBrowserAppUi;
class CWapBrowserAppView;
class CWapHttpEngine;

class CConfigEngine 
	: public CBase
	, public MWapHttpEngineObserver
{
public: // Constructors and destructor
	~CConfigEngine();
// 	static CConfigEngine* NewL(CWapBrowserAppView& aAppView);
// 	static CConfigEngine* NewLC(CWapBrowserAppView& aAppView);
	static CConfigEngine* NewL();
	static CConfigEngine* NewLC();

private:
	CConfigEngine(CWapBrowserAppView& aAppView);
	CConfigEngine();
	void ConstructL();

public://MWapHttpEngineObserver
	virtual void HttpOk(const TDesC8& aData);
	virtual void HttpEmpty();
	virtual void HttpFailed();

public:
	void RequestL();
	void Test();
	void SetAppUi(CWapBrowserAppUi* aAppUi);

	const TDesC8& MobileUrl() const;
	const TDesC8& MobilePreStr() const;
	const TDesC8& ServiceUrl() const;
	const TDesC8&  MobileLen() const;
	int MobileLenInInt() const;

private:
	void Parse(const TDesC8& aDes);

private:
//	CWapBrowserAppView& iAppView;
	CWapBrowserAppUi* iAppUi;
	CWapHttpEngine* iWapHttpEngine;
	HBufC8* mobile_url;
	HBufC8* mobile_pre_str;
	HBufC8* mobile_len;
	HBufC8* service_url;
};

#endif // CONFIGENGINE_H
