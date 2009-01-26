/*
============================================================================
 Name        : UtilityTools.h
 Author      : Sungrass
 Version     :
 Copyright   : Your copyright notice
 Description : CUtilityTools declaration

 放置公用的静态函数
============================================================================
*/

#ifndef UTILITYTOOLS_H
#define UTILITYTOOLS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <http\rhttpheaders.h>
#include <http\rhttpsession.h>

class CEikEdwin;
// class CsoarskyAppUi;
// class CsoarskyContainer;

class UtilityTools
{
public:
	static HBufC8*	GetFileContentL(RFs& fs,const TDesC& aFileName);

	static void		ReadFileL(TDes8& aDes,TInt aLength,TInt aOffset,  const TDesC& aFileName);

	static void		CreateFile(const TDesC& aFileName,TUint aFileMode);

	static void		WriteFileL(const TDesC8& aDes, TInt aOffset,  const TDesC& aFileName);

	static void		PlayVideoL(const TDesC& aFileName);

	static TInt		DeleteFile(const TDesC& aFileName);

	static TInt		GetFileSize(const TDesC& aFileName,TInt& aSize);

	static HBufC*	PrependLC( const TDesC& aString1, TDesC& aString2 );

	static void		AddHeaderL(RHTTPSession& aSession, RHTTPHeaders& aHeaders, TInt aHeaderField, const TDesC8& aHeaderValue);

	static void		ExecuteDialog(const TDesC& aDes);

	static void		ExecuteDialog(TInt aResourceID);

	static void		WriteTestFileL(const TDesC& aFileName,const TDesC8& aFileContent);

	static void		WriteTestFileL(const TDesC& aFileName,const TDesC& aFileContent);

	static void		InfoPrint(TInt aNum);

	static HBufC8*	ConvertToHBufC8L(const TDesC& aDes);

	static HBufC*	CreateHBufCFromCharL(const char* aStr);

	static void		SetEdwinTextColor(CEikEdwin* aEdwin,const TRgb& aColor);

	static void		SetEdwinTextFont(CEikEdwin* aEdwin,const CFont* aFont);

	static void		SetupEdwinText(CEikEdwin* aEdwin,const TRgb& aColor,const CFont* aFont);

	static TInt		CheckIsNumber(const TDesC& aInfo);

	static TInt		FileIsExist(const TDes& aFileName);

	static void GetFileName(const TDesC& aUrl, int& aFileNum, int& aFileExNum);

/*
	//以下方法依据具体的工程做修改
	static CsoarskyAppUi* AppUi();		

	static CsoarskyContainer* Container();
*/

	static CFbsBitmap*	CreateBitmapL(const TDesC& aFileName,TInt aId);

	static CFbsBitmap*	CreateBitmapL(TInt aId);

	static void		GetImei(TDes& aSerialNumber);

	static void		ShowDialog(const TDesC& aDes);

	static const CFont*	CurrentFont();

	static void		CompleteWithFileDriver(TDes& aFilePath);

	static HBufC*	CompleteWithAppPathL(const TDesC& aFilePath);

	static TSize	ScreenSize();

	static void WriteLogsL(TRefByValue<const TDesC8> aFmt, ...);

	static void WriteLogsL(TRefByValue<const TDesC16> aFmt, ...);


	static TFileName	ParseParentPath(const TDesC& aPath);
};

#endif // UTILITYTOOLS_H
