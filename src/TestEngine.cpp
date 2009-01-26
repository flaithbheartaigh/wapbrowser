/*
============================================================================
 Name        : TestEngine.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CTestEngine implementation
============================================================================
*/

#include "TestEngine.h"
#include "MainEngine.h"
#include "CacheEngine.h"
#include "UtilityTools.h"

CTestEngine::CTestEngine(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
{
}


CTestEngine::~CTestEngine()
{
	delete iCacheEngine;
}

CTestEngine* CTestEngine::NewLC(CMainEngine& aMainEngine)
{
	CTestEngine* self = new (ELeave)CTestEngine(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestEngine* CTestEngine::NewL(CMainEngine& aMainEngine)
{
	CTestEngine* self=CTestEngine::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CTestEngine::ConstructL()
{
	//TestCacheEngine();
}
//////////////////////////////////////////////////////////////////////////
//From MCacheObserver
//////////////////////////////////////////////////////////////////////////
void CTestEngine::CacheUpdate(const TDesC& aFileName)
{
	UtilityTools::ShowDialog(aFileName);
}

void CTestEngine::CacheFailed()
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CTestEngine::TestCacheEngine()
{
	iCacheEngine = iMainEngine.CreateCacheEngineL(*this,1,5,0);
	iCacheEngine->RequestPageFileName(2);
}