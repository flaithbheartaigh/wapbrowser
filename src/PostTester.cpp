/*
============================================================================
 Name        : PostTester.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPostTester implementation
============================================================================
*/

#include "Define.h"
#include "PostTester.h"
#include "WapHttpEngine.h"

CPostTester::CPostTester()
{
	// No implementation required
}


CPostTester::~CPostTester()
{
	delete iWapHttpEngine;
}

CPostTester* CPostTester::NewLC()
{
	CPostTester* self = new (ELeave)CPostTester();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPostTester* CPostTester::NewL()
{
	CPostTester* self=CPostTester::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CPostTester::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//From MWapHttpEngineObserver
//////////////////////////////////////////////////////////////////////////
void CPostTester::HttpOk(const TDesC8& aData)
{
	UtilityTools::DeleteFile(_L("C:\\Data\\PostResponse.txt"));
	UtilityTools::WriteFileL(aData,0,_L("C:\\Data\\PostResponse.txt"));
}

void CPostTester::HttpEmpty()
{

}

void CPostTester::HttpFailed()
{

}

void CPostTester::Test()
{
	//bid=2&keyword=aaa
	//POST /cocosearch.do HTTP/1.1\r\n
	//Host: wap.cocobox.cn\r\n
	
//http://wap.cocobox.cn/cocosearch.do
	if(NULL == iWapHttpEngine)
	{
		iWapHttpEngine = CWapHttpEngine::NewL();
		iWapHttpEngine->SetObserver(this);
	}
	iWapHttpEngine->IssueHTTPPostL(_L8("http://wap.cocobox.cn/cocosearch.do"),_L8("bid=2&keyword=aaa"),_L8("http://wap.cocobox.cn/"));
}