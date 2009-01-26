/*
============================================================================
 Name        : HandleLocalSearch.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleLocalSearch implementation
============================================================================
*/

#include "HandleLocalSearch.h"
#include "CoCoPreDefine.h"
#include "MainEngine.h"
#include "MControlObserver.h"
#include "FileActiveEngine.h"
#include <f32file.h>
#include <s32file.h>
#include "StaticCommonTools.h"
#include "utf.h"

CHandleLocalSearch::CHandleLocalSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iDesNameArray(NULL)
,iDesPathArray(NULL)
,iType(aType)
,iBuf(NULL)
,iNewPath(NULL)
,iNewIndex(-1)
{
	// No implementation required
}


CHandleLocalSearch::~CHandleLocalSearch()
{
	if(iDesNameArray)
	{
		delete iDesNameArray;
		iDesNameArray=NULL;
	}

	if(iDesPathArray)
	{
		delete iDesPathArray;
		iDesPathArray=NULL;
	}

	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}
	if(iNewPath)
	{
		delete iNewPath;
		iNewPath=NULL;
	}
}

CHandleLocalSearch* CHandleLocalSearch::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType)
{
	CHandleLocalSearch* self = new (ELeave)CHandleLocalSearch(aObserver,aMainEngine,aType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleLocalSearch* CHandleLocalSearch::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType)
{
	CHandleLocalSearch* self=CHandleLocalSearch::NewLC(aObserver,aMainEngine,aType);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleLocalSearch::ConstructL()
{
	iDesNameArray = new (ELeave) CDesCArrayFlat(10);
	iDesPathArray = new (ELeave) CDesCArrayFlat(10);
}

TInt CHandleLocalSearch::GetCount()
{
	return iDesNameArray->Count();
}
const TDesC& CHandleLocalSearch::GetOneItemName(TInt aIndex)
{
	ASSERT(aIndex >= 0 && aIndex < iDesNameArray->Count());
	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}

	const TPtrC& ptr =(*iDesNameArray)[aIndex];

	iBuf=HBufC::NewL(ptr.Length());

	iBuf->Des().Append(ptr);

	return *iBuf;
}
const TDesC& CHandleLocalSearch::GetOneItemPath(TInt aIndex)
{
	ASSERT(aIndex >= 0 && aIndex < iDesPathArray->Count());

	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}

	const TPtrC& ptr =(*iDesPathArray)[aIndex];

	iBuf=HBufC::NewL(ptr.Length());

	iBuf->Des().Append(ptr);

	return *iBuf;
}
void  CHandleLocalSearch::DeleteOneItem(TInt aIndex)
{
	ASSERT(aIndex >= 0 && aIndex < iDesPathArray->Count());
	ASSERT(aIndex >= 0 && aIndex < iDesNameArray->Count());

	iDesPathArray->Delete(aIndex);
	iDesNameArray->Delete(aIndex);
}
void CHandleLocalSearch::StartSearch()  //寻找不同驱动盘符,并开始搜索
{

	this->iDesNameArray->Reset();
	this->iDesPathArray->Reset();

	User::LeaveIfError(iFs.Connect());
	
	iFilePath.Zero();
	iFilePath.Append(_L("C:\\"));
	SearchDir(iFilePath);

	if (RFs::IsValidDrive(EDriveE)) 
	{
		iFilePath.Zero();
		iFilePath.Append(_L("E:\\"));
		SearchDir(iFilePath);
	}

	iFs.Close();

	iObserver.HandleResponseEvent(0,ELocalSearch);
}

TInt CHandleLocalSearch::SearchDir(const TDesC& aFilePath)  //搜索调用递归算法搜索文件内容
{
	
	TInt returnErr=0;

	CDir* dir;
	TInt err=iFs.GetDir(aFilePath,KEntryAttNormal|KEntryAttDir|KEntryAttHidden,ESortByDate,dir);

	if(err==KErrNone)
	{
		TInt tempInt = dir->Count();
		
		for(TInt i = 0;i < tempInt;i++)
		{
			TEntry iEntry = (*dir)[i];

			//if(iEntry.IsSystem()||iEntry.IsHidden())
				//break;

			if(iEntry.IsDir())
			{
				TInt iLength=iEntry.iName.Length();
				if(iLength>0)
				{			
					iFilePath.Append(iEntry.iName);
					iFilePath.Append(_L("\\"));
					SearchDir(iFilePath);
					iFilePath.Delete(iFilePath.Length()-1-iLength,1+iLength);
				}
			}
			else
			{
				if(iEntry.IsSystem()||iEntry.IsHidden())
					continue;

				TInt iLength=iEntry.iName.Length();
				if(iLength>0)
				{	
					if(IsType(iEntry.iName)==1)
					{
						if(iEntry.iName.Find(iKeyWord)>-1)
						{
							iDesNameArray->AppendL(iEntry.iName);
							iDesPathArray->AppendL(iFilePath);
						}
					}
				}
			}
		}
	}
	else
	{
		returnErr=err;
	}

	if(dir)
	{
		delete dir;
		dir=NULL;
	}

   return returnErr;
}

TInt CHandleLocalSearch::IsType(const TDesC& aFromFileName)
{

	TInt returnErr=0;
	TInt t=0;
	for(TInt i=0;i<aFromFileName.Length();i++)
	{
		if(aFromFileName.Mid(i,1).Compare(_L("."))==0)
		{
			t=i;
		}
	}

	if(t>0)
	{
		t++;
		switch(iType) 
		{
		case 1:  //音乐
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMP3)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KWMA)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMIDI)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMID)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KWAV)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KAAC)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KOGG)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMMF)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KIMY)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KAMR)==0)
					returnErr=1;

			}
			break;
		case 0:  //酷图
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KBMP)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KJPG)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KGIF)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KPNG)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KWBMP)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KTIFF)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KEXIF)==0)
					returnErr=1;
			}
			break;
		case 2: //游戏
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KSIS)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KSISX)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KJAR)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KJAD)==0)
					returnErr=1;
			}
			break;
		case 3: //视频
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(K3GP)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(K3GPP)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KRM)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KAVI)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMP4)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMOV)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KASF)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KRMVB)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KWMV)==0)
					returnErr=1;
			}
			break;
		case 4:  //工具
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KSIS)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KSISX)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KJAR)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KJAD)==0)
					returnErr=1;
			}
			break;
		case 5:  //电子书
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KTXT)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KUMD)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KPDB)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KPDF)==0)
					returnErr=1;
			}
			break;
		case 6:  //主题
			{
				if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KTHM)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KMTF)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KNTH)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KSDT)==0)
					returnErr=1;
				else if(aFromFileName.Right(aFromFileName.Length()-t).Compare(KHME)==0)
					returnErr=1;
			}
			break;
		}
	}
	return returnErr;
}
TInt CHandleLocalSearch::CheckFile(const TDesC& aFromFilePath,const TDesC& aFileName,const TDesC& aNewFilePath)  //将文件复制或剪切
{
	TInt returnErr=0;

	RFs oFs;
	User::LeaveIfError(oFs.Connect());

	TFileName newFileNamePath;
	newFileNamePath.Zero();
	newFileNamePath.Append(aNewFilePath);

	iMainEngine.WriteLog16(aFromFilePath);
	iMainEngine.WriteLog16(aFileName);
	iMainEngine.WriteLog16(newFileNamePath);

	if(aFromFilePath.Find(newFileNamePath)>-1)
	{
		//路径重复
		oFs.Close();
		returnErr=4;
	}
	else
	{
		newFileNamePath.Append(aFileName);
		
		//判断是否存储空间足够
		TInt rv =0;

		RFile reFile;
		rv = reFile.Open(oFs, aFromFilePath, EFileRead | EFileShareAny);
		if (KErrNone != rv) 
		{
			//文件正在被使用
			oFs.Close();
			returnErr=	2;
		}
		else
		{
			TInt aFileLen=0;

			reFile.Size(aFileLen);

			aFileLen=aFileLen/1024;

			TInt64 nDriverSize=::GetFreeSpace (newFileNamePath.Mid(0,1));

			if((TInt64)aFileLen>nDriverSize)
			{
				//存储空间不足
				returnErr=3;
			}
			reFile.Close();
			oFs.Close();
		}
	}
	return returnErr;
}
TInt CHandleLocalSearch::CopyFile(TInt aIndex,const TDesC& aNewFilePath)  //将文件复制或剪切
{
	ASSERT(aIndex >= 0 && aIndex < iDesNameArray->Count());

	TFileName fileName;
	fileName.Zero();
	fileName.Append((*iDesNameArray)[aIndex]);

	TFileName filePath;
	filePath.Zero();
	filePath.Append((*iDesPathArray)[aIndex]);

	TFileName newFileNamePath;
	newFileNamePath.Zero();
	newFileNamePath.Append(filePath);
	newFileNamePath.Append(fileName);

	iMainEngine.WriteLog16(newFileNamePath);
	iMainEngine.WriteLog16(fileName);
	iMainEngine.WriteLog16(aNewFilePath);

	TInt rErr=CheckFile(newFileNamePath,fileName,aNewFilePath);
	if(rErr==KErrNone)
	{
		
		iMainEngine.GetFileActiveEngine()->Cancel();

		iMainEngine.GetFileActiveEngine()->CopyFile(newFileNamePath,aNewFilePath,this);
	}
	return rErr;
}

TInt CHandleLocalSearch::MoveFile(TInt aIndex,const TDesC& aNewFilePath)  //将文件复制或剪切
{
	if(iNewPath)
	{
		delete iNewPath;
		iNewPath=NULL;
	}
	iNewPath=aNewFilePath.Alloc();
	iNewIndex=aIndex;

	ASSERT(aIndex >= 0 && aIndex < iDesNameArray->Count());

	TFileName fileName;
	fileName.Zero();
	fileName.Append((*iDesNameArray)[aIndex]);

	TFileName filePath;
	filePath.Zero();
	filePath.Append((*iDesPathArray)[aIndex]);


	TFileName newFileNamePath;
	newFileNamePath.Zero();
	newFileNamePath.Append(filePath);
	newFileNamePath.Append(fileName);


	iMainEngine.WriteLog16(newFileNamePath);
	iMainEngine.WriteLog16(fileName);
	iMainEngine.WriteLog16(aNewFilePath);

	TInt rErr=CheckFile(newFileNamePath,fileName,aNewFilePath);

	if(rErr==KErrNone)
	{
		iMainEngine.GetFileActiveEngine()->Cancel();
		iMainEngine.GetFileActiveEngine()->MoveFile(newFileNamePath,aNewFilePath,this);
	}
	return rErr;
}
TInt CHandleLocalSearch::DeleteFile(const TDesC& aFromFilePath)  //剪切,在复制之后,删除源文件
{
	TInt rErr=0;

	RFs dFs;
	User::LeaveIfError(dFs.Connect());
	RFile dFile;
	if (KErrNone != dFile.Open(dFs, aFromFilePath, EFileRead|EFileShareAny))
	{
		//文件不存在
		dFs.Close();
		rErr= 1;
	}
	dFile.Close();
	dFs.Delete(aFromFilePath);
	dFs.Close();
	return rErr;
}

void CHandleLocalSearch::SetKeyWord(const TDesC& aText)
{
	iKeyWord.Zero();
	iKeyWord.Append(aText);
	this->StartSearch();
}

void CHandleLocalSearch::HandleComplete(TInt aStatus,TInt aType)
{
	if(aStatus==0)
	{
		if(aType==2)
		{
			if(iNewIndex>-1)
			{
				iDesPathArray->Delete(iNewIndex);
				iDesPathArray->InsertL(iNewIndex,*iNewPath);
				iNewIndex=-1;
				if(iNewPath)
				{
					delete iNewPath;
					iNewPath=NULL;
				}
			}
		}
	}
}