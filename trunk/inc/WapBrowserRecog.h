/*
============================================================================
 Name        : CWapBrowserRecog from WapBrowserRecog.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Declares the recognizer implementation.
============================================================================
*/
#ifndef __WAPBROWSERRECOG_H__
#define __WAPBROWSERRECOG_H__

#include <apmrec.h>

const TUid KWapBrowserRecogDllUid = { 0x0b718bea };
#ifdef __SERIES60_3X__
const TInt KWapBrowserRecogImplementationUid = 0x0b718bea;
#endif /* __SERIES60_3X__ */

class CWapBrowserRecog : public CApaDataRecognizerType
	{

	public:

#ifdef __SERIES60_3X__
		static CApaDataRecognizerType* CreateRecognizerL();
#endif /* __SERIES60_3X__ */

		CWapBrowserRecog();
		TUint PreferredBufSize();
		TDataType SupportedDataTypeL(TInt aIndex) const;

	private:

		void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

	};
#endif /* __WAPBROWSERRECOG_H__ */
