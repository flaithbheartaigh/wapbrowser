/*
============================================================================
Name        : PhoneNumEngine.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : CPhoneNumEngine implementation
============================================================================
*/
#include "PhoneNumEngine.h"
#include "WapHttpEngine.h"
#include "WapBrowserappui.h"
#include "UtilityTools.h"

_LIT(KTestXmlFile,		"C:\\Data\\m.xml");

CPhoneNumEngine::CPhoneNumEngine()
{
}

CPhoneNumEngine::~CPhoneNumEngine()
{
	delete iWapHttpEngine;
	delete iPhoneNum;
}

CPhoneNumEngine* CPhoneNumEngine::NewLC()
{
	CPhoneNumEngine* self = new (ELeave)CPhoneNumEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPhoneNumEngine* CPhoneNumEngine::NewL()
{
	CPhoneNumEngine* self=CPhoneNumEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CPhoneNumEngine::ConstructL()
{
}
//////////////////////////////////////////////////////////////////////////
//MWapHttpEngineObserver
//////////////////////////////////////////////////////////////////////////
void CPhoneNumEngine::HttpOk(const TDesC8& aData)
{
	UtilityTools::WriteLogsL(_L("CPhoneNumEngine::HttpOk"));
	Parse(aData);
	if(iAppUi)
	{
		iAppUi->RequestPageL();
	}
	UtilityTools::WriteLogsL(_L("CPhoneNumEngine::HttpOk End"));
}

void CPhoneNumEngine::HttpEmpty()
{
	//	iAppView.StopShowWaiting();
}

void CPhoneNumEngine::HttpFailed()
{
	//	iAppView.StopShowWaiting();
}
//////////////////////////////////////////////////////////////////////////
//public:
//////////////////////////////////////////////////////////////////////////
void CPhoneNumEngine::RequestL(const TDesC8& aUrl)
{
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	iWapHttpEngine->IssueHTTPGetL(aUrl);
}

void CPhoneNumEngine::TestRequest()
{
}

void CPhoneNumEngine::TestParse()
{
	HBufC8* fileData = UtilityTools::ReadFileLC(KTestXmlFile);
	Parse(*fileData);
	HBufC* buf = HBufC::NewLC(PhoneNum().Length());
	buf->Des().Copy(PhoneNum());
	CAknInformationNote* iInfoNote = new (ELeave) CAknInformationNote;
	iInfoNote->ExecuteLD(*buf);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(fileData);

	/*
	CPhoneNumParser parser;
	//parser.Parse(_L8("http://211.139.164.132/wap/?mobileID=15710788274fasfdasfadsfdasfdsafewqfasdfewqfadgasdfsafdasdfaewra"));
	//parser.Parse(_L8("mobileID=15710788274fasfdasfadsfdasfdsafewqfasdfewqfadgasdfsafdasdfaewra"));
	parser.Parse(_L8("http://211.139.164.132/wap/?mobileID=15710788274"));
	//parser.Parse(_L8("mobileID=15710788274"));
	UtilityTools::WriteLogsL(parser.PhoneNum());
	*/
}

void CPhoneNumEngine::SetAppUi(CWapBrowserAppUi* aAppUi)
{
	iAppUi = aAppUi;
}

const TDesC8& CPhoneNumEngine::PhoneNum()  const
{
	if(iPhoneNum)
	{
		return *iPhoneNum;
	}
	return KNullDesC8;
}
//////////////////////////////////////////////////////////////////////////
//private:
//////////////////////////////////////////////////////////////////////////
void CPhoneNumEngine::Parse(const TDesC8& aData)
{
	ASSERT(NULL == iPhoneNum);
	TPtrC8 findStr(_L8("mobileID="));
	int pos = aData.Find(findStr);
	pos += findStr.Length();
	TPtrC8 phoneNum = aData.Mid(pos,11);
	iPhoneNum = HBufC8::NewL(phoneNum.Length());
	iPhoneNum->Des().Append(phoneNum);
}
