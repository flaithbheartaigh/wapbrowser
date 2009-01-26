/*
============================================================================
Name        : WapBrowser.cpp
Author      : 浮生若茶
Version     :
Copyright   : Your copyright notice
Description : Application entry point
============================================================================
*/
// INCLUDE FILES
#include <eikstart.h>
#include "WapBrowserApplication.h"

LOCAL_C CApaApplication* NewApplication()
{
	return new CWapBrowserApplication;
}

GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}

