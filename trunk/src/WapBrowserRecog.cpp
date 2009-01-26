/*
============================================================================
 Name        : CWapBrowserRecog from WapBrowserRecog.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : The recognizer implementation.
============================================================================
*/
#include <f32file.h>
#ifdef __SERIES60_3X__
#include <ImplementationProxy.h>
#endif /* __SERIES60_3X__ */
#include "WapBrowserRecog.h"

static const TInt KWapBrowserRecogDataTypeCount = 1;
_LIT8(KWapBrowserRecogMimeType, "");

CWapBrowserRecog::CWapBrowserRecog() : CApaDataRecognizerType(KWapBrowserRecogDllUid, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = KWapBrowserRecogDataTypeCount;
	}

TUint CWapBrowserRecog::PreferredBufSize()
	{
	return 0;
	}

TDataType CWapBrowserRecog::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KWapBrowserRecogMimeType);
	}

void CWapBrowserRecog::DoRecognizeL(const TDesC& aName, const TDesC8& /*aBuffer*/)
	{
	TParse parse;
	User::LeaveIfError(parse.Set(aName, NULL, NULL));
	_LIT(KDot, ".");

	if(parse.ExtPresent() && !parse.Ext().CompareF(KDot))
		{
		iConfidence = ECertain;
		iDataType = TDataType(KWapBrowserRecogMimeType);
		}
	}

#ifdef __SERIES60_3X__
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KWapBrowserRecogImplementationUid, CWapBrowserRecog::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CApaDataRecognizerType* CWapBrowserRecog::CreateRecognizerL()
	{
	return new(ELeave) CWapBrowserRecog();
	}
#else
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}

EXPORT_C CApaDataRecognizerType* CreateRecognizer()
	{
	return new CWapBrowserRecog();
	}
#endif /* __SERIES60_3X__ */
