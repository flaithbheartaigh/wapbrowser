/*
============================================================================
Name        : WapBrowserApplication.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : Main application class
============================================================================
*/
#include "WapBrowserDocument.h"
#include "WapBrowserApplication.h"

const TUid KUidWapBrowserApp = { 0xed86d796 };

CApaDocument* CWapBrowserApplication::CreateDocumentL()
{
	return (static_cast<CApaDocument*>( CWapBrowserDocument::NewL( *this ) ) );
}

TUid CWapBrowserApplication::AppDllUid() const
{
	return KUidWapBrowserApp;
}

 