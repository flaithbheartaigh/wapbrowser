/*
============================================================================
Name        : WapBrowserDocument.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : Application document class (model)
============================================================================
*/
#include "WapBrowserAppUi.h"
#include "WapBrowserDocument.h"
CWapBrowserDocument* CWapBrowserDocument::NewL( CEikApplication& aApp )
{
	CWapBrowserDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
}

CWapBrowserDocument* CWapBrowserDocument::NewLC( CEikApplication&aApp )
{
	CWapBrowserDocument* self =	new ( ELeave ) CWapBrowserDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
}

void CWapBrowserDocument::ConstructL()
{
}

CWapBrowserDocument::CWapBrowserDocument( CEikApplication& aApp )
	: CAknDocument( aApp )
{
}

CWapBrowserDocument::~CWapBrowserDocument()
{
}

CEikAppUi* CWapBrowserDocument::CreateAppUiL()
{
	return ( static_cast <CEikAppUi*> ( new ( ELeave )CWapBrowserAppUi) );
}

