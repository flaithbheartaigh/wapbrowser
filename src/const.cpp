#include "const.h"

#include <f32file.h>
#include <s32file.h>
#include <flogger.h>
const RECT Const_Icon_Rect[]=
{
//文件类型 0 图片 1 音乐　2程序/游戏/主题　3视频　4电子书　5其他 (主题、程序、游戏属于一类)
{97,0,15,15},//图片
{65,1,15,15},
{17,0,15,16},
{114,0,14,16},
{129,0,17,14},
{0,0,16,16},//其他
{81,0,15,15},//主题
{33,0,15,16},
{49,0,15,15},
{0,0,0,0}
};
const RECT Const_DownLoad_Rect[]=
{
 {0,0,85,12},{0,12,85,12},{0,0,0,0}
};

const RECT Const_PlayerInterface_Rect[]=
{
  {0,0,165,53},	//主播放界面
  
  {0,53,150,6},	//进度条
  
  {13,60,14,14},//播放
  {0,60,14,14},//暂停
  
  {152,54,3,6},//音量进度条
  
  {0,0,0,0}
};

const RECT Const_MainIcon_Rect[]=
{
{42,10,30,25},
{42,44,30,25},
{42,78,30,25},
{42,112,30,25},
{42,146,30,25},
{42,179,30,25},
{42,213,30,25},
{42,247,30,25},
{42,281,30,25},
{0,0,41,34},
{0,34,41,34},
{0,68,41,34},
{0,102,41,34},
{0,136,41,34},
{0,170,41,34},
{0,204,41,34},
{0,238,41,34},
{0,272,41,34},
{0,0,0,0}
};

const RECT Const_Loadinglogo_Rect[]=
{
	{0,0,62,50},
	{0,50,62,32},
	{0,0,0,0}
};

const RECT Const_MenuArrow_Rect[]=
{
	{1,0,2,14}
	,{6,0,7,14}
	,{0,0,0,0}
};

const RECT Const_Dialog_Rect[] =
{
	{0,0,23,22},
	{23,0,23,22},
	{46,0,23,22},
	{0,23,134,68},
	{0,0,0,0}
};

const RECT Const_InputType_Rect[]=
{
	{0,0,24,12},  //123
	{0,13,24,12},  //Abc
	{0,26,24,12},  //abc
	{0,39,24,12},  //ABC
	{0,52,24,12},  //中文
	{0,0,0,0}
};

const RECT Const_Weather_Rect[] =
{
	{0,0,14,14},  //雨
	{0,15,14,14},  //风
	{0,30,14,14},  //雪
	{0,45,14,14},  //多云
	{0,60,14,14},  //晴
	{0,75,14,14},  //雾
	{0,90,14,14},  //阴天
	{0,0,0,0}
};

const RECT Const_Operation_Rect[] =
{
	{0,3,13,7},  //上
	{14,3,13,7},  //下
	{31,0,7,13},  //左
	{45,0,7,13},  //右
	{56,1,11,11},  //圈，未选
	{70,1,11,11},  //圈，选中
	{84,0,12,12},  //方，未选
	{98,0,12,12},  //方，选中
	{0,0,0,0}
};

const RECT Const_Star_Rect[] =
{
	{0,0,20,19},  //1
	{22,0,20,19},  //0.5
	{44,0,20,19},  //0
	{0,0,0,0}
};

const RECT Const_BoyGirl_Rect[] =
{
	{0,0,40,40},  //1
	{41,0,40,40},  //0.5
	{0,0,0,0}
};

void MyAssertFunc(TInt32 nCondition, const char* strFile, int nLine)
{
	if (!nCondition)
	{
		WriteLogsL(_L8("test"));
		WriteLogsL(_L8("%s [%d]"), strFile, nLine);
		ASSERT(0);
	}
};

void WriteLogsL(TRefByValue<const TDesC8> aFmt, ...)
{
//#ifndef _DEBUG
#ifdef __WRITE_LOG__
	_LIT(KSettingPathname,"c:\\");
	_LIT(KIniFileName,"log.txt");

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

void WriteLogsL(TRefByValue<const TDesC16> aFmt, ...)
{
//#ifndef _DEBUG
#ifdef __WRITE_LOG__
	_LIT(KSettingPathname,"c:\\");
	_LIT(KIniFileName,"log.txt");

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


void WriteLogs_1L(TRefByValue<const TDesC8> aFmt, ...)
{
#ifdef __WINS__
	{
	_LIT(KIniFilePath,"testjpg");
	TBuf8<128> buf;
	VA_LIST	list;
	VA_START(list,aFmt);
	buf.Zero();
	buf.AppendFormatList(aFmt,list);
	VA_END(list);

	RFileLogger fileLogger;
	fileLogger.Connect();
	fileLogger.CreateLog(KIniFilePath, _L("testjpg.log"), EFileLoggingModeAppend);
	fileLogger.Write(buf);
	fileLogger.CloseLog();
	fileLogger.Close();
	}
#endif	
};

TUint16* GetOneText(const TDesC& aFileName,TInt aIndex,TUint16& aTextLen)
{
	TUint16* pFile = GetFileText(aFileName);
	TUint16* pText = GetText(pFile,aIndex,aTextLen);
	TUint16* p = new (ELeave) TUint16[aTextLen];
	int i=0;
	for (;i<aTextLen; i++) {
		p[i] = pText[i];
	}
	delete pFile;
	return p;
}
TUint16* GetText(TUint16* aFileText,TInt aIndex,TUint16& aTextLen)
{
	if (!aFileText && aIndex > aFileText[0])
		return NULL;
	TUint16* aText = NULL;
	int index =1;
	int textIndex = 1;//词组第一个就是1，不是0
	for (int i=0; i<aIndex +1 ; i++)
	{
		if (aIndex == textIndex)
		{
			aTextLen = aFileText[index++];
			aText = &aFileText[index];
			return aText;
		}
		else
		{
			index += aFileText[index];//词组个数
			index ++;//保存词组长度
			textIndex ++;
		}
	}
	return NULL;
}

TUint16* GetFileText(const TDesC& aFileName)
{
	TUint16* aFileText = NULL;
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	RFileReadStream rfrs;
	rfrs.Open(rfs,aFileName,EFileRead); 
	TUint16 textCount;
	textCount = rfrs.ReadUint16L(); //第一个short放置有多少组汉字
	TUint16 tCount;
	tCount = rfrs.ReadUint16L(); //第一个short放置有多少个汉字
	aFileText = new (ELeave) TUint16[textCount+1 + tCount];
	TUint16 index;
	index = 0;
	aFileText[index++] = textCount;
	for (int i=0; i<aFileText[0]; i++)
	{
		textCount = rfrs.ReadUint16L(); //放置这组汉字有多少个字
		aFileText[index++] = textCount ;
		for (int z=0; z<textCount; z++)
		{		
			aFileText[index++] = rfrs.ReadUint16L(); 
		}
	}	
	rfrs.Close();
	rfs.Close();
	return aFileText;
}
void  ReadResource(TUint16* pText,TDes& buf,TInt n)
{
	if(pText ==  NULL)
		return ;
	TUint16 len = 0;
	TUint16* p =GetText(pText,n,len);
	if (p == NULL ) {
		return;
	}
	buf.Append(p,len);
}


void ReadImageFileNameList(TDesC& strPath, CArrayFixFlat<TFileName>* pArray)
{

	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Append(strPath);
	//szPath.Append(KJPGPATH);
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	
	RDir oDir;
	if (KErrNone != oDir.Open(oRFs, szPath, KEntryAttNormal)) {
		oRFs.Close();
		return;
	}
	TEntryArray oArray;
	oDir.Read(oArray);
	int i=0;
	for(i=0; i<oArray.Count(); i++)
	{
		const	TEntry & oEntry = oArray[i];
		TFileName oName = oEntry.iName;	
		TPtrC16 oPtr =  oName.Right(4);		
		TFileName sz = oPtr;
		sz.LowerCase();
		TInt n = sz.Compare(_L(".jpg"));
		if(!n)
		{
			pArray->AppendL(oName);	
		}
			
	}
	oDir.Close();
	oRFs.Close();
}

void CreateImageDir(TDesC& strPath)
{
	if (NULL == strPath.Length()) 
		return;
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	
	CDir*	pDir;
	oRFs.GetDir(strPath, KEntryAttNormal|KEntryAttDir, ESortByName, pDir);
	for (TInt n = 0; pDir && (n < pDir->Count()); n ++)
	{
		TEntry	oFile = (*pDir)[n];
		if (oFile.IsDir())
		{
			TFileName oName = oFile.iName;
			oName.LowerCase();
			TInt n = oName.Compare(_L("jpg"));
			if (!n) 
			{
				DELETE(pDir);
				oRFs.Close();
				return;		
			}
		}		
	}
	TFileName sz;
	sz.Append(strPath);
	sz.Append(_L("jpg\\"));
	oRFs.MkDirAll(sz);
	DELETE(pDir);
	oRFs.Close();
}

TInt DelFile(const TDesC& aFileName)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt nRet=fs.Delete(aFileName);
	fs.Close();
	return nRet;
}

TInt ReNameDir(const TDesC& aOldDirName,const TDesC& aNewDirName)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt nRet=fs.Rename(aOldDirName,aNewDirName);
	fs.Close();
	return nRet;
}

TInt DelDir(const TDesC& aDirName)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CFileMan* fileMan = CFileMan::NewL(fs);
	
	TInt nRet=fileMan->RmDir(aDirName);

	delete fileMan;
	fs.Close();    

	return nRet;
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

void GetFileName1(const TDesC& aUrl, int& aFileNum, int& aFileExNum)
{
	if (aUrl.Length() <=3 ) 
		return;
	int i =  aUrl.Length()-1;	
	const TUint16* p = aUrl.Ptr();
	TBuf<16> buf;
	buf.Append(_L("\\"));	
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

 void ReadProList(CArrayFixFlat<TFileName>* pArray, const TDesC& aFile)
 {
	 if (NULL == pArray)
		 return;
	RFs rfs;
	User::LeaveIfError(rfs.Connect());
	RFile oFile;
	TInt nSize = 0;
	if (KErrNone != oFile.Open(rfs, aFile, EFileRead )) {
		rfs.Close();
		return;
	} //| EFileShareAny
	oFile.Size(nSize);
	RFileReadStream rfrs(oFile);
	nSize /= 2;
	int i=0;
	TFileName szFileName;
	for (; i<nSize; i++) 
	{
		TUint16 oText = rfrs.ReadUint16L();
		if (oText == 0x000a || nSize-1 == i)
		{
			if (oText != 0x000a)
				szFileName.Append(&oText, 1);		
			if (szFileName.Length())
				pArray->AppendL(szFileName);
			szFileName.Delete(0, szFileName.Length());
			continue;
		}
		if (oText == 0x000d)
			continue;
		szFileName.Append(&oText, 1);		
	}
	rfrs.Close();
	oFile.Close();
	rfs.Close();
 }

 void ReadProNameList(CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TFileName>* pNameArray)
 {
	if (NULL == pArray || NULL == pNameArray)
		return;
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());		
	for (int j=0; pArray->Count() && j<pArray->Count(); j++) 
	{
		RDir oDir;
		TFileName path;
		path.Append(pArray->At(j));		
		path.Append(_L("\\"));
		if (KErrNone != oDir.Open(oRFs, path, KEntryAttNormal)) {
			oRFs.Close();
			return ;
		}
		TEntryArray oArray;
		oDir.Read(oArray);
		int i=0;		
		for(i=0; i<oArray.Count(); i++)
		{
			const	TEntry & oEntry = oArray[i];
			if (oEntry.IsDir()) {
			}
			else
			{				
				TFileName oName = oEntry.iName;	
				TPtrC16 oPtr =  oName.Right(4);		
				TFileName sz = oPtr;
				sz.LowerCase();
				TInt n = sz.Compare(_L(".app"));
				if(!n)
				{
					pNameArray->AppendL(oName);	
					break;
				}
			}
				
		}
		oDir.Close();
	}
	oRFs.Close();
 }

  void DeleteFiles(const TDesC& strPath, const TDesC& strFilter)
{
	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Copy(strPath);
	if (szPath.Right(1) != _L("\\"))
		szPath.Append(_L("\\"));

	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());
	CDir*	pDir;
	oRFs.GetDir(szPath, KEntryAttNormal|KEntryAttDir, ESortByName, pDir);
	for (TInt n = 0; pDir && (n < pDir->Count()); n ++)
	{
		TEntry	oFile = (*pDir)[n];
		if (oFile.IsDir())
		{
			//这里应该是用递归调用删除子目录，但是现在还没有这个需求，所以没有做处理
			//在递归调用的时候，可能使用的oRFs会出现问题
			//建议在以后的处理中，将Folder的记录保存下来，留在oRFs.Close()后一起删除掉
		}
		else
		{
			TFileName	szPathname;
			if ((strFilter.Length() == 0L) || (oFile.iName.Find(strFilter) >= 0))
			{
				szPathname.Format(_L("%S%S"), &szPath, &oFile.iName);
				oRFs.Delete(szPathname);
			}
		}
	}
	DELETE(pDir);
	oRFs.Close();
}

 void ReadDirNameList(const TDesC& path, CArrayFixFlat<TFileName>* pNameArray)
 {
	if (NULL == pNameArray)
		return;
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());		
	RDir oDir;	
	if (KErrNone != oDir.Open(oRFs, path, KEntryAttNormal)) {
		oRFs.Close();
		return ;
	}
	TEntryArray oArray;
	oDir.Read(oArray);
	int i=0;		
	for(i=0; i<oArray.Count(); i++)
	{
		const	TEntry & oEntry = oArray[i];
		if (oEntry.IsDir()) {
		}
		else
		{				
			TFileName oName = oEntry.iName;				
			pNameArray->AppendL(oName);					
		}
			
	}
	oDir.Close();
	oRFs.Close();
 }

 void ReadFileNameList(const TDesC& strPath, CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TUint8>* pDirArray)
{
	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Append(strPath);	
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	
 	
	CDir* dir = NULL;
	User::LeaveIfError(oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));

	int i;
	i=(*dir).Count()-1;
	for(;i>=0; i--)
	{
		const	TEntry & oEntry = (*dir)[i];
		TFileName oName = oEntry.iName;	

		if(oName.Right(2)!=_L("3G"))
		{
			pArray->AppendL(oName);	
			if (oEntry.IsDir()) 
				pDirArray->AppendL(1);
			else
				pDirArray->AppendL(0);
		}
	}

	dir=NULL;
	oRFs.Close();
}

void ReadFileNameList(const TDesC& strPath, CArrayFixFlat<TFileName>* pArray)
{
	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Append(strPath);	
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	
 	
	CDir* dir = NULL;
	User::LeaveIfError(oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));

	int i;
	i=(*dir).Count()-1;
	for(;i>=0; i--)
	{
		const	TEntry & oEntry = (*dir)[i];
		TFileName oName = oEntry.iName;	

		if(oName.Right(2)!=_L("3G"))
		{
				
			if (oEntry.IsDir()) 
			{}
			else
				pArray->AppendL(oName);
		}
	}

	dir=NULL;
	oRFs.Close();
}
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

TInt CheckIsNumber(const TDesC& aInfo)
{
	TLex temp(aInfo);
	TInt b;

	if (( temp.Val( b ) == KErrNone ) && (temp.Remainder().Length() == 0))
		return 1;
	else
		return 0;
}
TInt FileIsExist(const TDes& aFileName)
{
	//判断文件是否存在,1:存在，0-不存在
	RFs rfs;
	RFile tFile;
	TInt ret =0;

	rfs.Connect();
	
	TBool nAnswer;
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
	}
}
