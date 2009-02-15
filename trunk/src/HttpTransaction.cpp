/*
============================================================================
 Name        : HttpTransaction.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CHttpTransaction implementation
============================================================================
*/

#include "HttpTransaction.h"
#include "HTTPEngine.h"

CHttpTransaction::CHttpTransaction()
{
	// No implementation required
}

CHttpTransaction::~CHttpTransaction()
{
	delete iHTTPEngine;
}

CHttpTransaction* CHttpTransaction::NewLC()
{
	CHttpTransaction* self = new (ELeave)CHttpTransaction();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHttpTransaction* CHttpTransaction::NewL()
{
	CHttpTransaction* self=CHttpTransaction::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CHttpTransaction::ConstructL()
{

}

CHTTPEngine& CHttpTransaction::HTTPEngine()
{
/*
	if(NULL == iHTTPEngine)
	{
		iHTTPEngine = CHTTPEngine::NewL(*this);
	}*/

	return *iHTTPEngine;
}
