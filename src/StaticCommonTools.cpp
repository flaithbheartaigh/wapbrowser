/*
============================================================================
 Name        : StaticCommonTools.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 一些常用的方法，这些方法是静态的
============================================================================
*/
#include <coemain.h>
#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include <utf.h>

#include "PreDefine.h"
#include "StaticCommonTools.h"
#include "BitmapFactory.h"
#include "CoCoPreDefine.h"

TInt64 GetFreeSpace(const TDesC& aDriveName)
{
	TVolumeInfo info;
	RFs	iFs;

	User::LeaveIfError(iFs.Connect());	

	info.iFree = 0;
	if (aDriveName.CompareF(_L("A")) == 0)
	{
		iFs.Volume(info, EDriveA);
	}
	else if(aDriveName.CompareF(_L("B")) == 0)
	{
		iFs.Volume(info, EDriveB);
	}
	else if(aDriveName.CompareF(_L("C")) == 0)
	{
		iFs.Volume(info, EDriveC);
	}
	else if(aDriveName.CompareF(_L("D")) == 0)
	{
		iFs.Volume(info, EDriveD);
	}
	else if(aDriveName.CompareF(_L("E")) == 0)
	{
		iFs.Volume(info, EDriveE);
	}
	else if(aDriveName.CompareF(_L("F")) == 0)
	{
		iFs.Volume(info, EDriveF);
	}
	else if(aDriveName.CompareF(_L("G")) == 0)
	{
		iFs.Volume(info, EDriveG);
	}
	else if(aDriveName.CompareF(_L("H")) == 0)
	{
		iFs.Volume(info, EDriveH);
	}
	else if(aDriveName.CompareF(_L("I")) == 0)
	{
		iFs.Volume(info, EDriveI);
	}
	else if(aDriveName.CompareF(_L("J")) == 0)
	{
		iFs.Volume(info, EDriveJ);
	}
	else if(aDriveName.CompareF(_L("K")) == 0)
	{
		iFs.Volume(info, EDriveK);
	}
	else if(aDriveName.CompareF(_L("L")) == 0)
	{
		iFs.Volume(info, EDriveL);
	}
	else if(aDriveName.CompareF(_L("M")) == 0)
	{
		iFs.Volume(info, EDriveM);
	}
	else if(aDriveName.CompareF(_L("N")) == 0)
	{
		iFs.Volume(info, EDriveN);
	}
	else if(aDriveName.CompareF(_L("O")) == 0)
	{
		iFs.Volume(info, EDriveO);
	}
	else if (aDriveName.CompareF(_L("P")) == 0)
	{
		iFs.Volume(info, EDriveP);
	}
	else if(aDriveName.CompareF(_L("Q")) == 0)
	{
		iFs.Volume(info, EDriveQ);
	}
	else if(aDriveName.CompareF(_L("R")) == 0)
	{
		iFs.Volume(info, EDriveR);
	}
	else if(aDriveName.CompareF(_L("S")) == 0)
	{
		iFs.Volume(info, EDriveS);
	}
	else if(aDriveName.CompareF(_L("T")) == 0)
	{
		iFs.Volume(info, EDriveT);
	}
	else if(aDriveName.CompareF(_L("U")) == 0)
	{
		iFs.Volume(info, EDriveU);
	}
	else if(aDriveName.CompareF(_L("V")) == 0)
	{
		iFs.Volume(info, EDriveV);
	}
	else if(aDriveName.CompareF(_L("W")) == 0)
	{
		iFs.Volume(info, EDriveW);
	}
	else if(aDriveName.CompareF(_L("X")) == 0)
	{
		iFs.Volume(info, EDriveX);
	}
	else if(aDriveName.CompareF(_L("Y")) == 0)
	{
		iFs.Volume(info, EDriveY);
	}
	else if(aDriveName.CompareF(_L("Z")) == 0)
	{
		iFs.Volume(info, EDriveZ);
	}
	else
	{
		return 0;
	}

	iFs.Close();

	return (TInt64)info.iFree/1024;
}
TInt FileIsExistL(const TDesC& aFileName)
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
TInt FileIsExist(const TDes& aFileName)
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
	/*TBool nAnswer;
	rfs.IsFileOpen(aFileName,nAnswer);

	if(nAnswer)
	{
	return 1;
	}
	ret = tFile.Open(rfs, aFileName, EFileWrite|EFileShareAny);
	if (KErrNone == ret)
	{
	tFile.Close();
	rfs.Close();
	return 1;
	}
	else
	{
	tFile.Close();
	rfs.Close();
	return 0;
	}*/
}

TInt DelFile(const TDesC& aFileName)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt nRet=fs.Delete(aFileName);
	fs.Close();
	return nRet;
}
TInt WriteFile(const TDesC& aFileName,const TDesC& aData)
{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RFile file;
	User::LeaveIfError(file.Replace(fs,aFileName,EFileWrite));
	CleanupClosePushL(file);
	HBufC8* buf = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aData);
	CleanupStack::PushL(buf);
	file.Write(*buf);
	CleanupStack::PopAndDestroy(3);
	return 0;
}
TInt WriteFile8(const TDesC& aFileName,const TDesC8& aData)
{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RFile file;
	User::LeaveIfError(file.Replace(fs,aFileName,EFileWrite));
	CleanupClosePushL(file);
	file.Write(aData);
	CleanupStack::PopAndDestroy(2);
	return 0;
}
TInt MakeDir(const TDesC& aFileName)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	if(!BaflUtils::PathExists(fs,aFileName))
	{
		TInt err = fs.MkDir(aFileName);
		User::LeaveIfError(err);
	}
	fs.Close();
	return 0;
}
TInt DeleteDir(const TDesC& aFileName)
{
	RFs& fs = CCoeEnv::Static()->FsSession();
	return BaflUtils::DeleteFile(fs,aFileName);
/*
	RFs fs;
	User::LeaveIfError(fs.Connect());
	if(BaflUtils::PathExists(fs,aFileName))
	{
		TInt err = fs.RmDir(aFileName);
		User::LeaveIfError(err);
	}
	fs.Close();
*/

	return 0;
}
void GetFileName(const TDesC& aUrl, int& aFileNum, int& aFileExNum)
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
TInt ConvertTDesCToTInt(const TDesC& aText)
{
	TInt returnValue=0;
	TLex temp(aText);
	TInt err=temp.Val(returnValue);
	return returnValue;
}
TInt ConvertTDesC8ToTInt(const TDesC8& aText)
{
	TInt returnValue=0;
	TLex8 temp(aText);
	TInt err=temp.Val(returnValue);
	return returnValue;
}
TInt CheckIsTInt(const TDesC& aText)
{
	TInt num=0;;
	TLex temp(aText);
	TInt err=temp.Val(num);
	return err;
}
TBool ParseBuf(const TDesC& aText,const TDesC& aDes,TInt aIndex,TDes& aValue)
{
	TBool tBool=EFalse;
	HBufC* newData = HBufC::NewLC(aText.Length());
	newData->Des().Copy(aText);

	TInt post=0;
	TInt index=0;
	post=newData->Find(aDes);

	while(post>-1)
	{
		
		if(index==aIndex&&post>-1)
		{
			tBool=ETrue;
			
			aValue.Append(newData->Left(post));
			break;
		}
		index++;
		newData->Des().Delete(0,post+1);
		post=newData->Find(aDes);
	}

	CleanupStack::PopAndDestroy(newData);
	return tBool;
}
TInt CheckFileType(const TDesC& aType)
{
	TInt type=EFileTypeOther;

	TBuf<50> fileType;
	fileType.Zero();
	fileType.Copy(aType);
	fileType.LowerCase();

	if(fileType.Compare(KMP3)==0)//音乐
		type=EFileTypeMusic;
	else if(fileType.Compare(KWMA)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KMIDI)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KMID)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KWAV)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KAAC)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KOGG)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KMMF)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KIMY)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KAMR)==0)
		type=EFileTypeMusic;
	else if(fileType.Compare(KBMP)==0)//酷图
		type=EFileTypeImage;
	else if(fileType.Compare(KJPG)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KGIF)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KPNG)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KWBMP)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KTIFF)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KEXIF)==0)
		type=EFileTypeImage;
	else if(fileType.Compare(KSIS)==0)//游戏
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KSISX)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KJAR)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KJAD)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(K3GP)==0)//视频
		type=EFileTypeVideo;
	else if(fileType.Compare(K3GPP)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KRM)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KAVI)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KMP4)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KMOV)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KASF)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KRMVB)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KWMV)==0)
		type=EFileTypeVideo;
	else if(fileType.Compare(KSIS)==0)//工具
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KSISX)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KJAR)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KJAD)==0)
		type=EFileTypeAppOrGame;
	else if(fileType.Compare(KTXT)==0)//电子书
		type=EFileTypeEBook;
	else if(fileType.Compare(KUMD)==0)
		type=EFileTypeEBook;
	else if(fileType.Compare(KPDB)==0)
		type=EFileTypeEBook;
	else if(fileType.Compare(KPDF)==0)
		type=EFileTypeEBook;
	else if(fileType.Compare(KTHM)==0)//主题
		type=EFileTypeTheme;
	else if(fileType.Compare(KMTF)==0)
		type=EFileTypeTheme;
	else if(fileType.Compare(KNTH)==0)
		type=EFileTypeTheme;
	else if(fileType.Compare(KSDT)==0)
		type=EFileTypeTheme;
	else if(fileType.Compare(KHME)==0)
		type=EFileTypeTheme;

	return type;
}

TInt ParseString(const TDesC& aText,const CFont* aFont,TInt aWidth)
{
	TInt textPos = 0;
	TInt lineNum=0;
 	while(textPos < aText.Length())
	{	
		TInt charNum = aFont->TextCount(aText.Mid(textPos),aWidth);
 		textPos += charNum;
		lineNum++;
	}

	return lineNum;
}

void WriteLogsTemp(TRefByValue<const TDesC16> aFmt, ...)
{
#ifdef __WRITE_LOG__
	_LIT(KSettingPathname,"c:\\");
	_LIT(KIniFileName,"log_temp.txt");

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