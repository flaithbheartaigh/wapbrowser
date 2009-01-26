/*
============================================================================
 Name        : UtilityTools.cpp
 Author      : Sungrass
 Version     :
 Copyright   : Your copyright notice
 Description : CUtilityTools implementation
============================================================================
*/
#include <f32file.h>
#include <coemain.h>
#include <apgcli.h>
#include <s32file.h>
#include <aknnotewrappers.h>            // CAknInformationNote
#include <eikedwin.h>
#include <utf.h>
#include <W32STD.H>
#include <bautils.h>

#include "UtilityTools.h"
#include "PreDefine.h"
// #include "soarskyAppUi.h"
// #include "soarskyContainer.h"

#undef _DEBUG


HBufC8* UtilityTools::GetFileContentL(RFs& fs,const TDesC& aFileName)
{
	RFile file;
	TInt err = file.Open(fs,aFileName,EFileRead);
	User::LeaveIfError(err);
	CleanupClosePushL(file);
	TInt size;
	err = file.Size(size);
	User::LeaveIfError(err);
	HBufC8* fileData = HBufC8::NewLC(size);
	TPtr8 ptr = fileData->Des();
	err = file.Read(ptr);
	User::LeaveIfError(err);
	file.Close();
	CleanupStack::Pop(2);
	return fileData;
}

//////////////////////////////////////////////////////////////////////////
//从文件中读取数据
//////////////////////////////////////////////////////////////////////////
void UtilityTools::ReadFileL(TDes8& aDes,TInt aLength,TInt aOffset,  const TDesC& aFileName)
{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs,aFileName,EFileRead));
	file.Seek(ESeekStart, aOffset);
	file.Read(aDes,aLength);
	CleanupStack::PopAndDestroy(2);
}
//////////////////////////////////////////////////////////////////////////
//向文件中写入数据
//////////////////////////////////////////////////////////////////////////
void UtilityTools::CreateFile(const TDesC& aFileName,TUint aFileMode)
{
	RFs& fs = CCoeEnv::Static()->FsSession();
	RFile file;
	file.Create(fs,aFileName,aFileMode);
	file.Close();
}

void UtilityTools::WriteFileL(const TDesC8& aDes, TInt aOffset, const TDesC& aFileName)
{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RFile file;
	User::LeaveIfError(file.Open(fs,aFileName,EFileWrite));
	CleanupClosePushL(file);
	file.Seek(ESeekStart, aOffset);
	file.Write(aDes);
	CleanupStack::PopAndDestroy(2);
}

void UtilityTools::PlayVideoL(const TDesC& aFileName)
{
	TInt error = KErrNone;
	RApaLsSession session;
	error = session.Connect();
	User::LeaveIfError(error);
	CleanupClosePushL(session);
	TThreadId threadId;
	error = session.StartDocument(aFileName, threadId);
	User::LeaveIfError(error);
/*	session.Close();

	RThread thread;
	error = thread.Open(threadId);
	User::LeaveIfError(error);
	TRequestStatus status;
	thread.Logon(status);	//同步执行
	//User::WaitForRequest(status);
	*/
	CleanupStack::PopAndDestroy();	//session
}

/*
GetDocHandler().OpenFileEmbeddedL(txtBuf, data) ;

Wayne(16506876) 01:25:42
CDocumentHandler& CMcAppUi::GetDocHandler() {
	if(!iDocHandler) {
		iDocHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );
	}
	return *iDocHandler ; 
} 

_LIT(filename,"c:\\System\\Apps\\VideoRing\\xc_helper.txt");
TDataType aDT;
CDocumentHandler* iDocHandler = CDocumentHandler::NewLC( CEikonEnv::Static()->Process() );
iDocHandler->OpenFileL( filename, aDT ); 
CleanupStack::PopAndDestroy(); // handler 
*/

TInt UtilityTools::DeleteFile(const TDesC& aFileName)
{
	RFs& fs = CCoeEnv::Static()->FsSession();
	return fs.Delete(aFileName);
}

TInt UtilityTools::GetFileSize(const TDesC& aFileName,TInt& aSize)
{
	aSize = 0;
	RFs& fs = CCoeEnv::Static()->FsSession();
	RFile file;
	TInt err = file.Open(fs,aFileName,EFileRead);
	if (err != KErrNone)
	{
		return err;
	}
	err = file.Size(aSize);
	file.Close();
	return err;
}

HBufC* UtilityTools::PrependLC( const TDesC& aString1, TDesC& aString2 )
{
	HBufC* result = HBufC::NewMaxLC( aString1.Length() + aString2.Length() );
	TPtr resultDes = result->Des();
	resultDes = aString1;
	resultDes.Append( aString2 );
	return result;
}

void UtilityTools::AddHeaderL(RHTTPSession& aSession, RHTTPHeaders& aHeaders, TInt aHeaderField, const TDesC8& aHeaderValue)
{
	RStringPool stringPool = aSession.StringPool();
	RStringF valStr = stringPool.OpenFStringL(aHeaderValue);
	THTTPHdrVal headerVal(valStr);
	aHeaders.SetFieldL(stringPool.StringF(aHeaderField, RHTTPSession::GetTable()), headerVal);
	valStr.Close();
}

void UtilityTools::ExecuteDialog(const TDesC& aDes)
{
	CAknInformationNote* note = new (ELeave) CAknInformationNote;
	note->ExecuteLD(aDes);
}

void UtilityTools::ExecuteDialog(TInt aResourceID)
{
	HBufC* buf = CCoeEnv::Static()->AllocReadResourceLC(aResourceID);
	ExecuteDialog(*buf);
	CleanupStack::PopAndDestroy(buf);
}

void UtilityTools::WriteTestFileL(const TDesC& aFileName,const TDesC8& aFileContent)
{
//#ifdef __WINS__		//仅在windows下做测试
	//_LIT(KResultContent,	"c:\\system\\Apps\\Coco\\UploadContent.txt");

/*
	UtilityTools::DeleteFile(aFileName);
	UtilityTools::CreateFile(aFileName,EFileWrite | EFileRead);
	UtilityTools::WriteFileL(aFileContent,0,aFileName);*/

//#ifdef _DEBUG
#ifndef _DEBUG
	RFs fs;
	TInt err = fs.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(fs);
	err = fs.Delete(aFileName);
	if(err != KErrNotFound)
	{
		User::LeaveIfError(err);
	}
	RFileWriteStream file;
	err = file.Create(fs,aFileName,EFileWrite);
	User::LeaveIfError(err);
	CleanupClosePushL(file);
	//////////////////////////////////////////////////////////////////////////
	{
		HBufC8* buf8 = HBufC8::NewLC(aFileContent.Length());
		TPtr8 ptr(buf8->Des());
		ptr.Copy(aFileContent);
		file.WriteL(*buf8);
		CleanupStack::PopAndDestroy(3);
	}
	//////////////////////////////////////////////////////////////////////////
	//file.WriteL(*iResponseBuffer);
	file.Close();

	fs.Close();

#endif
}

void UtilityTools::WriteTestFileL(const TDesC& aFileName,const TDesC& aFileContent)
{
//#ifdef _DEBUG
//#ifndef _DEBUG
	//WriteLogsL(aFileName.Length());
	//InfoPrint(aFileName.Length());
	RFs fs;
	TInt err = fs.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(fs);
	err = fs.Delete(aFileName);
	RFileWriteStream file;
	err = file.Create(fs,aFileName,EFileWrite);
	User::LeaveIfError(err);
	CleanupClosePushL(file);

	HBufC8* buf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aFileContent);
	CleanupStack::PushL(buf);
	file.WriteL(*buf);
	CleanupStack::PopAndDestroy(3);

	file.Close();

	fs.Close();
//#endif
}

void UtilityTools::InfoPrint(TInt aNum)
{
	TBuf<100> buf;
	buf.AppendNum(aNum);
	//User::InfoPrint(buf);
}

HBufC8* UtilityTools::ConvertToHBufC8L(const TDesC& aDes)
{
	HBufC8* buf = HBufC8::NewL(aDes.Length()+2);
	buf->Des().Copy(aDes);
	return buf;
}

HBufC* UtilityTools::CreateHBufCFromCharL(const char* aStr)
{
	//中文是否可以正常解析
	TPtrC8 ptr((const TUint8*)aStr);
	HBufC* buf = HBufC::NewL(ptr.Length());
	buf->Des().Copy(ptr);
	return buf;
}

void UtilityTools::SetEdwinTextColor(CEikEdwin* aEdwin,const TRgb& aColor)
{
	CCharFormatLayer* FormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor=aColor;
	charFormatMask.SetAttrib(EAttColor);
	FormatLayer->SetL(charFormat, charFormatMask);
	aEdwin->SetCharFormatLayer(FormatLayer);  // Edwin takes the ownership
}

void UtilityTools::SetEdwinTextFont(CEikEdwin* aEdwin,const CFont* aFont)
{
	CCharFormatLayer* FormatLayer=CEikonEnv::NewDefaultCharFormatLayerL(); 
	TCharFormat charFormat; 
	TCharFormatMask charFormatMask; 
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight); 
	charFormat.iFontSpec = aFont->FontSpecInTwips();
	FormatLayer->SetL(charFormat, charFormatMask); 
	aEdwin->SetCharFormatLayer(FormatLayer);
}

void UtilityTools::SetupEdwinText(CEikEdwin* aEdwin,const TRgb& aColor,const CFont* aFont)
{
	CCharFormatLayer* FormatLayer = CEikonEnv::NewDefaultCharFormatLayerL();
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	FormatLayer->Sense(charFormat, charFormatMask);
	charFormat.iFontPresentation.iTextColor=aColor;
	charFormatMask.SetAttrib(EAttColor);
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight); 
	charFormat.iFontSpec = aFont->FontSpecInTwips();
	FormatLayer->SetL(charFormat, charFormatMask);
	aEdwin->SetCharFormatLayer(FormatLayer);  // Edwin takes the ownership

}
/*
CsoarskyAppUi* UtilityTools::AppUi()
{
	CCoeAppUi* appUi = CCoeEnv::Static()->AppUi();
	CsoarskyAppUi* turkeyAppUi = (CsoarskyAppUi*)appUi;

	return turkeyAppUi;
}

CsoarskyContainer* UtilityTools::Container()
{
	return AppUi()->Container();
}
*/

CFbsBitmap* UtilityTools::CreateBitmapL(const TDesC& aFileName,TInt aId)
{
	CFbsBitmap* bitmap = new (ELeave)CFbsBitmap;
	bitmap->Load(aFileName,aId);
	return bitmap;
}

CFbsBitmap* UtilityTools::CreateBitmapL(TInt aId)
{
	_LIT(MbmPath, "Z:\\System\\Apps\\soarsky\\soarskyres.MBM");
	CFbsBitmap* bitmap = new (ELeave)CFbsBitmap;
	bitmap->Load(MbmPath,aId);
	return bitmap;
}

/*
#include <Etel3rdParty.h>
void UtilityTools::GetImei(TDes& aSerialNumber)
{	
	const TInt KPhoneSerialNumberSize = 50;
	CTelephony* telephony = CTelephony::NewLC();
	TRequestStatus status;
	TBuf8<KPhoneSerialNumberSize> serialNumber;
	telephony->GetPhoneId(status,serialNumber);
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy();
	aSerialNumber.Copy(serialNumber);
}
*/

#include <aknnotewrappers.h>
void UtilityTools::ShowDialog(const TDesC& aDes)
{
	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	informationNote->ExecuteLD(aDes);

}

// const CFont* UtilityTools::CurrentFont()
// {
// 	const CFont* font = AknLayoutUtils::FontFromId(KCurrentFontID);
// }

void UtilityTools::CompleteWithFileDriver(TDes& aFilePath)
{
	TFileName tmpFileName;
	TFileName fileName;
	::CompleteWithAppPath(fileName);
	TParse parse;
	parse.Set(fileName,NULL,NULL);
#ifdef __WINS__
	tmpFileName.Append(_L("C:"));
#else
	tmpFileName.Append(parse.Drive());
#endif
	tmpFileName.Append(aFilePath);
	aFilePath.Zero();
	aFilePath.Append(tmpFileName);
}

/*
HBufC* UtilityTools::CompleteWithAppPathL(const TDesC& aFilePath)
{
	//KResourcePath
	TFileName fileName;
	::CompleteWithAppPath(fileName);
	TParse parse;
	parse.Set(fileName,NULL,NULL);
// 	aFilePath.Append(parse.Drive());
// 	aFilePath.Append(KResourcePath);
}
*/


TSize UtilityTools::ScreenSize()
{
	TPixelsTwipsAndRotation pixelsTwipsAndRotation;
	CCoeEnv::Static()->ScreenDevice()->GetDefaultScreenSizeAndRotation(pixelsTwipsAndRotation);
	return pixelsTwipsAndRotation.iPixelSize;
}


void UtilityTools::WriteLogsL(TRefByValue<const TDesC8> aFmt, ...)
{
	//#ifndef _DEBUG
#ifdef __WRITE_LOG__
	_LIT(KSettingPathname,"c:\\");
	_LIT(KIniFileName,"log_static.txt");

	TFileName fn;
	fn = KSettingPathname;
	fn.Append(KIniFileName);

	TBuf8<128> buf;
	VA_LIST	list;
	VA_START(list,aFmt);
	buf.Zero();
	buf.AppendFormatList(aFmt,list);
	VA_END(list);

	RFs rfs;
	rfs.Connect();

	RFile oFile;
	if (KErrNone != oFile.Open(rfs, fn, EFileWrite|EFileShareAny))
	{
#if 0
		if (KErrNone != oFile.Create(rfs, fn, EFileWrite|EFileShareAny))
		{
			rfs.Close();
			return;
		}
#else
		rfs.Close();
		return;
#endif
	}
	TInt	nSeekPos = 0;
	oFile.Seek(ESeekEnd, nSeekPos);

	TBuf<64>	szTime16;
	TBuf8<64>	szTime8;
	TTime	oTime;
	oTime.HomeTime();
	oTime.FormatL(szTime16, _L("%D%M%Y%2/%3 %H:%T:%S "));
	for (TInt n = 0; n < szTime16.Length(); n ++)
	{
		szTime8.Append((TUint8*)(szTime16.Ptr() + n), 1);
	}
	oFile.Write(szTime8);
	oFile.Write(buf);
	TBuf8<4>	szCrlf;
	szCrlf.Copy(_L8("\x0a\x0d"));
	oFile.Write(szCrlf);

	oFile.Close();
	rfs.Close();
#endif
};

void UtilityTools::WriteLogsL(TRefByValue<const TDesC16> aFmt, ...)
{
	//#ifndef _DEBUG
#ifdef __WRITE_LOG__
	_LIT(KSettingPathname,"c:\\");
	_LIT(KIniFileName,"log_static.txt");

	TFileName fn;
	fn = KSettingPathname;
	fn.Append(KIniFileName);

	TBuf<400> buf16;
	VA_LIST	list;
	VA_START(list,aFmt);
	buf16.Zero();
	buf16.AppendFormatList(aFmt,list);
	VA_END(list);

	TBuf8<400> buf;
	for (TInt i = 0; i < buf16.Length(); i ++)
	{
		buf.Append((TUint8*)(buf16.Ptr() + i), 1);
	}

	RFs rfs;
	rfs.Connect();

	RFile oFile;
	if (KErrNone != oFile.Open(rfs, fn, EFileWrite|EFileShareAny))
	{
#if 0
		if (KErrNone != oFile.Create(rfs, fn, EFileWrite|EFileShareAny))
		{
			rfs.Close();
			return;
		}
#else
		rfs.Close();
		return;
#endif
	}
	TInt	nSeekPos = 0;
	oFile.Seek(ESeekEnd, nSeekPos);

	TBuf<64>	szTime16;
	TBuf8<64>	szTime8;
	TTime	oTime;
	oTime.HomeTime();
	oTime.FormatL(szTime16, _L("%D%M%Y%2/%3 %H:%T:%S "));
	for (TInt n = 0; n < szTime16.Length(); n ++)
	{
		szTime8.Append((TUint8*)(szTime16.Ptr() + n), 1);
	}
	oFile.Write(szTime8);
	oFile.Write(buf);
	TBuf8<4>	szCrlf;
	szCrlf.Copy(_L8("\x0a\x0d"));
	oFile.Write(szCrlf);

	oFile.Close();
	rfs.Close();
#endif
};

TInt UtilityTools::CheckIsNumber(const TDesC& aInfo)
{
	TLex temp(aInfo);
	TInt b;

	if (( temp.Val( b ) == KErrNone ) && (temp.Remainder().Length() == 0))
		return 1;
	else
		return 0;
}

TInt UtilityTools::FileIsExist(const TDes& aFileName)
{
	//判断文件是否存在,1:存在，0-不存在
	RFs rfs;
	RFile tFile;
	TInt ret =0;

	rfs.Connect();

	if (BaflUtils::FileExists(rfs, aFileName))
	{
		rfs.Close();
		return 1;
	} 
	else
	{
		rfs.Close();
		return 0;
	}
}

void UtilityTools::GetFileName(const TDesC& aUrl, int& aFileNum, int& aFileExNum)
{
	int i =  aUrl.Length()-1;	
	const TUint16* p = aUrl.Ptr();
	TBuf<16> buf;
	buf.Append(_L("/"));	
	TBuf<16> buf2;
	buf2.Append(_L("."));
	const TUint16* pL = buf.Ptr();	
	const TUint16* pL1 = buf2.Ptr();	
	int index = 0;
	aFileExNum = 0;
	for (; i>=0; i--)
	{
		if (p[i] == pL1[0] && (!aFileExNum))
			aFileExNum = index;
		if (p[i] == pL[0])		
			break;
		index++;
	}
	aFileNum = index;	
}

//--------------------------------------------------------------------------
//ParseParentPath,该方法的实现比较曲折,不直观
//--------------------------------------------------------------------------
TFileName UtilityTools::ParseParentPath(const TDesC& aPath)
{
	//ASSERT(BaflUtils::PathExists(iFs,aPath));

	HBufC* tempPath = HBufC::NewLC(aPath.Length() - 1);
	tempPath->Des().Append(aPath.Left(aPath.Length() - 1));					//去掉最后一个反斜杠
	TParse parse;
	parse.Set(*tempPath,NULL,NULL);
	const TDesC& curFolder = parse.NameAndExt();
	TFileName newPath;
	TInt length = tempPath->Length() - curFolder.Length();
	newPath.Append(tempPath->Left(length));	//去掉当前文件夹名称
	CleanupStack::PopAndDestroy();

	return newPath;
}