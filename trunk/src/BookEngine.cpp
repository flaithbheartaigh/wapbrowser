
#include <f32file.h>
#include <s32file.h>
#include <charconv.h> 	
#include <eikenv.h>
#include "CoCoPreDefine.h"
#include "BookEngine.h"

CBookEngine::CBookEngine()
{
	iFileName.Zero();
	iCurrentIndex = 0;
}

CBookEngine::~CBookEngine()
{

}

//	Return ETrue if not the whole content are loaded into memory
TInt CBookEngine::LoadSectionL( const TFileName& aFileName,HBufC*& pBuf, TInt& aBytesRead )//加载Txt文件
{
	iCurrentIndex = 0; //重新开始读	
	SetPath(aFileName);

	return LoadSectionL( pBuf, 0, aBytesRead );
}


//	Added by xueyw on May 27 2007
TBookEncoding CBookEngine::GetBookEncoding( const TUint8* pUBookContent )
{
	if ( !pUBookContent )
	{
		return ENOTSUPPORTED;
	}

	if ( 0xfe == pUBookContent[0] && 0xff == pUBookContent[1] )
	{
		//	ARM is unicode big endian
		return EUNICODEBIGENDIAN;
	}

	if ( 0xff == pUBookContent[0] && 0xfe == pUBookContent[1] )
	{
		return EUNICODE;
	}

	if ( 0xef == pUBookContent[0] &&
		0xbb == pUBookContent[1] &&
		0xbf == pUBookContent[2])
	{
		return EUTF8;
	}

	return EANSI;
}

//	Modified by xueyw on May 28 2007
//	Return bytes read
//	pBuf already converted to unicode
TBool CBookEngine::LoadSectionL( HBufC*& pBuf, TInt aSeekPos, TInt& nLoadedSize )
{
	RFs rs;
	User::LeaveIfError(rs.Connect());
	CleanupClosePushL(rs);

	RFile nFile;
	CleanupClosePushL(nFile);
	nFile.Open(rs,iFileName,EFileRead);
	TInt nSize = 0;;
	nFile.Size(nSize);
	nFile.Close();

	RFileReadStream rfs;
	User::LeaveIfError(rfs.Open(rs,iFileName,EFileRead));
	CleanupClosePushL(rfs);

	rfs.PushL(); 

	pBuf = HBufC::NewMax( (nSize + 2) * 2 );
	pBuf->Des().FillZ( (nSize + 2) * 2 );

	//*****************************************************************************
	//	Added by xueyw on May 27 2007
	HBufC8* pTempBuf = HBufC8::NewMax( nSize + 2 );
	pTempBuf->Des().FillZ( nSize + 2 );

	TInt nNetContentLen = nSize;

	TUint8* pBookContent = (TUint8*)pTempBuf->Ptr();

	//	Read by byte
	//	to support ANSI/UTF-8 encoding
	for( TInt i = 0 ; i < nSize; i++ )
	{
		pBookContent[i] =  rfs.ReadUint8L();
	}

	TBookEncoding nEncoding = GetBookEncoding( pBookContent );

	//	Skip BOM
	if ( EUTF8 == nEncoding )
	{
		nNetContentLen -= 3;
		pBookContent += 3;
	}

	//	Convert to UNICODE if needed
	//	Check if there is conversion between ANSI/UTF-8 and unicode 
	if ( EUNICODE != nEncoding && EUNICODEBIGENDIAN != nEncoding )
	{

		CCnvCharacterSetConverter* converter=CCnvCharacterSetConverter::NewLC();

		TInt state = CCnvCharacterSetConverter::KStateDefault;

		if ( EUTF8 == nEncoding )
		{
			if( converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8,
				CEikonEnv::Static()->FsSession()) != CCnvCharacterSetConverter::EAvailable )
			{
				User::Leave(KErrNotSupported);
			}

		}
		else if ( EANSI == nEncoding )
		{
			if( converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierGbk,
				CEikonEnv::Static()->FsSession()) != CCnvCharacterSetConverter::EAvailable )
			{
				User::Leave(KErrNotSupported);
			}
		}

		TPtr ptr = pBuf->Des();

		if( CCnvCharacterSetConverter::EErrorIllFormedInput == converter->ConvertToUnicode(ptr, TPtrC8(pBookContent, nNetContentLen), state ) )
		{
			User::Leave(KErrArgument); 
		}

		CleanupStack::PopAndDestroy( converter );  

	}
	else
	{
		TUint16* pUnicodeBookContent = reinterpret_cast<TUint16*>( pBookContent );
		//	copy
		//	nSize == number of bytes
		*pBuf = TPtrC16( pUnicodeBookContent, nSize/2 );
	}


	delete pTempBuf;
	//*****************************************************************************

	rfs.Pop();
	rfs.Release();

	CleanupStack::PopAndDestroy(3);

	nLoadedSize = nSize;

	return EFalse;
}

