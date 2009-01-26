#ifndef _MYBOOK_H
#define _MYBOOK_H

#include <e32base.h>
#include "CoCoPreDefine.h"

class CBookEngine : public CBase
{
public:
	CBookEngine();
	~CBookEngine();

	//¼ÓÔØTxtÎÄ¼þ
	TInt LoadSectionL( const TFileName& aFileName,HBufC*& pBuf, TInt& aBytesRead );

	void SetPath(const TFileName& aFileName)
	{
		if(iFileName.Length() > 0)
			iFileName.Delete(0,iFileName.Length());		       	    
		iFileName.Copy(aFileName);	 	
	};

	TBool CBookEngine::LoadSectionL( HBufC*& pBuf, TInt aSeekPos, TInt& nLoadedSize );

private:
	TBookEncoding GetBookEncoding( const TUint8* pUBookContent );

private: 
	TFileName  iFileName;
	TInt       iCurrentIndex;
};
#endif
