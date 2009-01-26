/*
============================================================================
 Name        : CoCo.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application entry point
============================================================================
*/

// INCLUDE FILES
#ifdef __SERIES60_3X__
	#include <eikstart.h>
#endif
#include "CoCoApplication.h"
#include "PreDefine.h"
#include "UtilityTools.h"


#ifdef __SERIES60_3X__
	LOCAL_C CApaApplication* NewApplication()
	{
		return new CCoCoApplication;
	}
	GLDEF_C TInt E32Main()
		{
		return EikStart::RunApplication( NewApplication );
		}
#else
	EXPORT_C CApaApplication* NewApplication()
	{
		return new CCoCoApplication;
	}
	GLDEF_C TInt E32Dll( TDllReason )
	{
		return KErrNone;
	}
#endif
