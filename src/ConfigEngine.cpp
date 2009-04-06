/*
============================================================================
Name        : ConfigEngine.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CConfigEngine implementation
============================================================================
*/
#include "Define.h"
#include "ConfigEngine.h"
#include "WapHttpEngine.h"
#include "WapBrowserappui.h"
#include "WapBrowserappview.h"

_LIT8(KConfigUrl,		"http://59.36.98.140/g.txt");
_LIT(KTestConfigFile,	"C:\\Data\\g.txt");

//CConfigEngine::CConfigEngine(CWapBrowserAppView& aAppView)
CConfigEngine::CConfigEngine()
//	: iAppView(aAppView)
{
}

CConfigEngine::~CConfigEngine()
{
	delete iWapHttpEngine;
	delete mobile_url;
	delete mobile_pre_str;
	delete mobile_len;
	delete service_url;
}

/*
CConfigEngine* CConfigEngine::NewLC(CWapBrowserAppView& aAppView)
{
	CConfigEngine* self = new (ELeave)CConfigEngine(aAppView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CConfigEngine* CConfigEngine::NewL(CWapBrowserAppView& aAppView)
{
	CConfigEngine* self=CConfigEngine::NewLC(aAppView);
	CleanupStack::Pop(); // self;
	return self;
}*/


CConfigEngine* CConfigEngine::NewLC()
{
	CConfigEngine* self = new (ELeave)CConfigEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CConfigEngine* CConfigEngine::NewL()
{
	CConfigEngine* self=CConfigEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}
void CConfigEngine::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//MWapHttpEngineObserver
//////////////////////////////////////////////////////////////////////////
void CConfigEngine::HttpOk(const TDesC8& aData)
{
	Parse(aData);
	if(iAppUi)
	{
		iAppUi->GetPhoneNumL();
	}
//	iAppView.StopShowWaiting();
}

void CConfigEngine::HttpEmpty()
{
//	iAppView.StopShowWaiting();
}

void CConfigEngine::HttpFailed()
{
//	iAppView.StopShowWaiting();
}
//////////////////////////////////////////////////////////////////////////
//public:
//////////////////////////////////////////////////////////////////////////
void CConfigEngine::RequestL()
{
/*
	//http://218.16.120.168/miniwap/g.txt
	HTTPEngine().IssueHTTPGetL(_L8("http://59.36.98.140/g.txt"));
	//HTTPEngine().IssueHTTPGetL(_L8("http://59.36.98.140/g.txt"));
	iRequestType = ERequestConfig;
	iAppView->ShowWaiting();
*/
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	iWapHttpEngine->IssueHTTPGetL(KConfigUrl);
//	iAppView.ShowWaiting();
}

void CConfigEngine::Test()
{
	HBufC8* fileData = UtilityTools::ReadFileLC(KTestConfigFile);
	Parse(*fileData);
	CleanupStack::PopAndDestroy(fileData);
	if(iAppUi)
	{
		//另一种通知
	}
}

void CConfigEngine::SetAppUi(CWapBrowserAppUi* aAppUi)
{
	iAppUi = aAppUi;
}

const TDesC8& CConfigEngine::MobileUrl() const
{
	if(mobile_url)
	{
		return *mobile_url;
	}
	else
	{
		return KNullDesC8;
	}
}

const TDesC8& CConfigEngine::MobilePreStr() const
{
	if(mobile_pre_str)
	{
		return *mobile_pre_str;
	}
	else
	{
		return KNullDesC8;
	}
}

const TDesC8& CConfigEngine::ServiceUrl() const
{
	if(service_url)
	{
		return *service_url;
	}
	else
	{
		return KNullDesC8;
	}
}

const TDesC8& CConfigEngine::MobileLen() const
{
	if(mobile_len)
	{
		return *mobile_len;
	}
	else
	{
		return KNullDesC8;
	}
}

int CConfigEngine::MobileLenInInt() const
{
	int len;
	if(mobile_len)
	{
		TLex8 lex(*mobile_len);
		lex.Val((TUint&)len);
	}
	return len;
}
//////////////////////////////////////////////////////////////////////////
//private:
//////////////////////////////////////////////////////////////////////////
void CConfigEngine::Parse(const TDesC8& aDes)
{
	int firstPos = 0;
	int lastPos = 0;

	TPtrC8 ptr;
	ptr.Set(aDes);

	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));	
	TPtrC8 mobile_url = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
	TRACE(mobile_url);
	ASSERT(this->mobile_url);
	delete this->mobile_url;
	this->mobile_url = mobile_url.AllocL();


	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));		
	TPtrC8 mobile_pre_str = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
	TRACE(mobile_pre_str);
	ASSERT(this->mobile_pre_str);
	delete this->mobile_pre_str;
	this->mobile_pre_str = mobile_pre_str.AllocL();

	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	lastPos = ptr.Find(_L8("\n"));		
	TPtrC8 mobile_len = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
	TRACE(mobile_len);
	delete this->mobile_len;
	this->mobile_len = mobile_len.AllocL();

	ptr.Set(ptr.Mid(lastPos + 1));
	firstPos = ptr.Find(_L8("="));
	//lastPos = ptr.Find(_L8("0X0A"));	
	TPtrC8 service_url = ptr.Mid(firstPos + 1,ptr.Length() - firstPos - 3);
	TRACE(service_url);
	ASSERT(this->service_url);
	delete this->service_url;
	this->service_url = service_url.AllocL();
}