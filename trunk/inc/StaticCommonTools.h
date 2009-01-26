/*
============================================================================
 Name        : StaticCommonTools.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 一些常用的方法，这些方法是静态的
============================================================================
*/

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <GDI.H>

extern TInt64 GetFreeSpace(const TDesC& aDriveName);
extern TInt FileIsExist(const TDes& aFileName);
extern TInt FileIsExistL(const TDesC& aFileName);
extern  TInt DelFile(const TDesC& aFileName);

extern  TInt MakeDir(const TDesC& aFileName);
extern  TInt DeleteDir(const TDesC& aFileName);

extern  TInt WriteFile(const TDesC& aFileName,const TDesC& aData);
extern  TInt WriteFile8(const TDesC& aFileName,const TDesC8& aData);

extern  void GetFileName(const TDesC& aUrl, int& aFileNum, int& aFileExNum);
extern TInt ConvertTDesCToTInt(const TDesC& aText);
extern TInt ConvertTDesC8ToTInt(const TDesC8& aText);
extern TInt CheckIsTInt(const TDesC& aText);
extern TBool ParseBuf(const TDesC& aText,const TDesC& aDes,TInt aIndex,TDes& aValue);
extern TInt CheckFileType(const TDesC& aType);
extern TInt ParseString(const TDesC& aText,const CFont* aFont,TInt aWidth);

void WriteLogsTemp(TRefByValue<const TDesC16> aFmt, ...);