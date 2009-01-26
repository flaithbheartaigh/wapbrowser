/*
============================================================================
 Name        : FileEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对文件相关操作的静态方法
============================================================================
*/
#include <coemain.h>
#include <BAUTILS.H>

#include "FileEngine.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "Dialog.h"
#include "InputDialog.h"
#include "FileBrowser.h"
#include "FileEngineObserver.h"
#include "PreDefine.h"

enum TEventType
{
	ECmdNull,
	ECmdDeleteFile,
	ECmdDeleteFolder,
	ECmdCancelDelete,
	ECmdCopying,
	ECmdCancelCopy,
	ECmdRenameFile,
	ECmdCreateFolder,
	ECmdSaveAs,
	ECmdDoSaveAs,
	ECmdCancelSaveAs,
};

CFileEngine::CFileEngine(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
												   , iFs(CCoeEnv::Static()->FsSession())
												   , iControlFactory(aMainEngine.ControlFactory())
												   , iFileActiveEngine(*iMainEngine.GetFileActiveEngine())
{
}


CFileEngine::~CFileEngine()
{
	delete iFileForDel;
	delete iCopiedFile;
	delete iRenameFile;
	delete iParentFolderName;
}

CFileEngine* CFileEngine::NewLC(CMainEngine& aMainEngine)
{
	CFileEngine* self = new (ELeave)CFileEngine(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileEngine* CFileEngine::NewL(CMainEngine& aMainEngine)
{
	CFileEngine* self=CFileEngine::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CFileEngine::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CFileEngine::DialogEvent(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdNull:
		break;
	case ECmdCopying:
		iFileActiveEngine.Cancel();
		break;
	case ECmdDeleteFile:
	case ECmdDeleteFolder:
		DeleteFile();
		break;

	case ECmdCancelDelete:
		delete iFileForDel;
		iFileForDel = NULL;
		break;

/*
	case ECmdDeleteFolder:
		DeleteFolderAndAllFiles();
		break;
*/

	case ECmdCancelCopy:
		delete iCopiedFile;
		iCopiedFile = NULL;
		break;

	case ECmdSaveAs:
		SaveAs();
		break;

	case ECmdCancelSaveAs:	//取消保存,重新浏览文件夹
		CancelSaveAs();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MInputObserver
//////////////////////////////////////////////////////////////////////////
void CFileEngine::InputResponseEvent(TInt aEvent,const TDesC& aText)
{
	switch(aEvent)
	{
	case ECmdRenameFile:
		RenameFile(aText);
		break;

	case ECmdCreateFolder:
		CreateFolder(aText);
		break;

	case -1:
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MFileEngineObserver
//////////////////////////////////////////////////////////////////////////
void CFileEngine::HandleComplete(TInt aStatus,TInt aType)
{
	if(KErrNone == aStatus)
	{
		switch(aType)
		{
		case 0://复制
			{
				iMainEngine.CleanupControl(iDialog);
				iDialog = NULL;
				if(iObserver)
				{
					iObserver->Update(ECopyCompleted);
				}
			}
			break;

		case 2://移动
			{
				iMainEngine.CleanupControl(iDialog);
				iDialog = NULL;
				if(iObserver)
				{
					iObserver->Update(ESaveAsCompleted);
				}
			}
			break;

		default:
			ASSERT(EFalse);
			break;
		}
	}
	else
	{
		iMainEngine.CleanupControl(iDialog);
		iDialog = NULL;
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownError),*this,ECmdNull);
	}
}
//////////////////////////////////////////////////////////////////////////
//From MFileBrowserObserver
//////////////////////////////////////////////////////////////////////////
void CFileEngine::DoFileEvent(const TDesC& aFileName,TInt aCommand)
{
	switch(aCommand)
	{
	case KCmdEventNull:
		break;

	case ECmdSaveAs:
		{
			ASSERT(NULL == iSaveAsDstPath);
			iSaveAsDstPath = aFileName.AllocL();
			iControlFactory.CreateInquireDialog(iMainEngine.GetDesById(ECoCoTestRes_IsSaveFolder),*this,ECmdSaveAs,ECmdCancelSaveAs);
		}
		break;

	case ECmdCancelSaveAs:
		{
			delete iSaveAsSrcName;
			iSaveAsSrcName = NULL;
		}
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFileEngine::SetObserver(MFileEngineEvent* aObserver)
{
	iObserver = aObserver;
}

void CFileEngine::RequestDeleteFile(const TDesC& aFileName)
{
	TBool isFolder = EFalse;
	TInt err = BaflUtils::IsFolder(iFs,aFileName,isFolder);
	if(KErrNone != err)
	{
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DelFileFailed),*this,ECmdNull);
	}
	else
	{
		if(!isFolder)
		{
			iControlFactory.CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_QDelFile),*this,ECmdDeleteFile,ECmdCancelDelete);
		}
		else
		{
			iControlFactory.CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_IsDelDir),*this,ECmdDeleteFolder,ECmdCancelDelete);
		}
		delete iFileForDel;
		iFileForDel = NULL;
		iFileForDel = aFileName.AllocL();
	}
}

void CFileEngine::CopyFile(const TDesC& aFilaName)
{
	TBool isFolder = EFalse;
	TInt err = BaflUtils::IsFolder(iFs,aFilaName, isFolder);

	if(KErrNone != err)
	{
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FilePaste_OtherError),*this,ECmdNull);
	}
	else
	{
		if(isFolder)
		{
			iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_CopyFolderNotSupported),*this,ECmdNull);
		}
		else
		{
			iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FileCopied),*this,ECmdNull);
			delete iCopiedFile;
			iCopiedFile = NULL;
			iCopiedFile = aFilaName.AllocL();
		}
	}
}

void CFileEngine::PasteFile(const TDesC& aFilaName)
{
	if(NULL == iCopiedFile)
	{
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_Copy_NotSelect),*this,ECmdNull);
	}
	else
	{
		if(!BaflUtils::FileExists(iFs,*iCopiedFile))
		{
			iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FilePaste_SourceNotExist),*this,ECmdCancelCopy);
		}
		else
		{
			TParse parse;
			parse.Set(*iCopiedFile,NULL,NULL);
			const TDesC& driveAndPath = parse.DriveAndPath();
			const TDesC& nameAndExt = parse.NameAndExt();
			
			TFileName desFileName;
			TBool isFolder = EFalse;
			TInt err = BaflUtils::IsFolder(iFs,aFilaName,isFolder);
			if(KErrNone == err && isFolder)
			{
				desFileName.Append(aFilaName);
				if(desFileName.Right(1).Compare(_L("\\"))!=0)
				{
					desFileName.Append(_L("\\"));
				}
				desFileName.Append(nameAndExt);
			}
			else
			{
				TParse parseDes;
				parseDes.Set(aFilaName,NULL,NULL);
				
				desFileName.Append(parseDes.DriveAndPath());
				if(parseDes.DriveAndPath().Right(1).Compare(_L("\\"))!=0)
				{
					desFileName.Append(_L("\\"));
				}
				desFileName.Append(nameAndExt);
			}
			iMainEngine.WriteLog16(*iCopiedFile);
			iMainEngine.WriteLog16(desFileName);

			TInt result = CheckDiskSpace(*iCopiedFile,desFileName);
			if(1 == result)
			{
				iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_SaveAs_NoSpace),*this,ECmdCancelCopy);
			}
			else if(2 == result)
			{
				iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FilePaste_OtherError),*this,ECmdCancelCopy);
			}
			else
			{
				if(BaflUtils::FileExists(iFs,desFileName))
				{
					TParse parseTemp;
					parseTemp.Set(desFileName,NULL,NULL);

					desFileName.Zero();
					desFileName.Append(parseTemp.DriveAndPath());
					desFileName.Append(iMainEngine.GetDesById(ETurkeyTextRes_Copy_SameName));
					desFileName.Append(parseTemp.NameAndExt());
				}

				iFileActiveEngine.CopyFile(*iCopiedFile,desFileName,this);

				iDialog = iControlFactory.CreateWaitDialog(iMainEngine.GetDesById(ETurkeyTextRes_NewSearch_SaveFile),*this,ECmdCopying);
				delete iCopiedFile;
				iCopiedFile = NULL;
			}
			//CleanupStack::PopAndDestroy();	//desFileName
		}
	}
}

void CFileEngine::RequestRenameFile(const TDesC& aFileName)
{
	//ASSERT(NULL == iRenameFile);
	CInputDialog* inputDialog = iControlFactory.CreateInputTextDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_ReName),KNullDesC,ECmdRenameFile);
	TParse parse;
	parse.Set(aFileName,NULL,NULL);
	inputDialog->SetEdwinText(parse.NameAndExt());
	delete iRenameFile;
	iRenameFile = NULL;
	iRenameFile = aFileName.AllocL();
//	iInputEvent = ECmdRenameFile;
}

void CFileEngine::RequestCreateFolder(const TDesC& aParentPath)
{
	//ASSERT(NULL == iParentFolderName);
	delete iParentFolderName;
	iParentFolderName = NULL;
	iParentFolderName = aParentPath.AllocL();

	TFileName szTempParentPath = aParentPath;
	if ( szTempParentPath[ szTempParentPath.Length() - 1 ] != '\\' )
	{
		szTempParentPath.Append( _L("\\") );
	}
	//获取路径深度
	TInt dirDepth = 0;
	const TUint16* pSzTempParentPath = szTempParentPath.Ptr();
	for ( TInt i = 0; i < szTempParentPath.Length(); i ++ )
	{
		if ( pSzTempParentPath[ i ] == '\\' )
		{
			dirDepth ++;
		}
	}
	if ( dirDepth >= 7 )
	{
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_CreateFolderNotSupported),*this,ECmdNull);
		delete iParentFolderName;
		iParentFolderName = NULL;
	}
	else
	{
		iControlFactory.CreateInputTextDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_InputFolderName),KNullDesC,ECmdCreateFolder);
		//iInputEvent = ECmdCreateFolder;
	}
}

void CFileEngine::RequestSaveAs(const TDesC& aFileName)
{
	ASSERT(NULL == iSaveAsSrcName);
	iSaveAsSrcName = aFileName.AllocL();
	iControlFactory.CreateFolderBrowser(*this,ECmdSaveAs,ECmdCancelSaveAs);
// 	if(iObserver)
// 	{
// 		iObserver->Update(ESaveAsCompleted);
// 	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CFileEngine::DeleteFile()
{
	TInt err = 0;
	TBool isFolder = EFalse;
	err = BaflUtils::IsFolder(iFs,*iFileForDel,isFolder);
	if(KErrNone == err && isFolder)
	{
		err=DeleteFile(*iFileForDel);
	}
	else
	{
		err = BaflUtils::DeleteFile(iFs,*iFileForDel);
	}

	delete iFileForDel;
	iFileForDel = NULL;
	if(KErrNone == err)
	{
		if(iObserver)
		{
			iObserver->Update(EDeleteCompleted);
		}
	}
	else
	{
		iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DelFileFailed),*this,ECmdNull);
	}
}
TInt CFileEngine::DeleteFile(const TDesC& aPath)
{
	
	TInt returnErr=0;
	CDir* dir;
	TInt err=iFs.GetDir(aPath,KEntryAttNormal|KEntryAttDir|KEntryAttHidden,ESortByDate,dir);

	if(err==KErrNone)
	{
		TInt tempInt = dir->Count();
		
		for(TInt i = 0;i < tempInt;i++)
		{
			TEntry iEntry = (*dir)[i];

			if(iEntry.IsDir())
			{
				TInt iLength=iEntry.iName.Length();
				if(iLength>0)
				{			
					TFileName filePath;
					filePath.Append(aPath);
					filePath.Append(iEntry.iName);
					filePath.Append(_L("\\"));
					DeleteFile(filePath);
				}
			}
			else
			{
				TFileName filePath;
				filePath.Append(aPath);
				filePath.Append(iEntry.iName);
				returnErr=BaflUtils::DeleteFile(iFs,filePath);
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

	returnErr=iFs.RmDir(aPath);
	return returnErr;
}
/*

void CFileEngine::DeleteFolderAndAllFiles()
{
	ASSERT(EFalse);
	if(iObserver)
	{
		iObserver->Update(EDeleteCompleted);
	}
}

void CFileEngine::DeleteFolderAndAllFiles(const TDesC& aPath)
{
	CDir* dir = NULL;
	TInt err = iFs.GetDir(aPath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir);
	ASSERT(dir);
	CleanupStack::PushL(dir);
	for (TInt i = 0 ; i < dir->Count() ; i++)
	{
		const TEntry& entry = (*dir)[i];
		//TFileName fileName;
		HBufC* fileName = HBufC::NewLC(KMaxFileName);
		fileName->Des().Append(aPath);
		fileName->Des().Append(entry.iName);
		if(entry.IsDir())//文件夹
		{
			fileName->Des().Append(_L("\\"));
			DeleteFolderAndAllFiles(*fileName);
		}
		else
		{
			TInt err = BaflUtils::DeleteFile(iFs,*fileName);
		}
		CleanupStack::PopAndDestroy();	//fileName
	}
	delete dir;
}*/

// void CFileEngine::DeleteFolder()
// {
// 	BaflUtils::DeleteFile(iFs,)
// }

void CFileEngine::RenameFile(const TDesC& aNewFileName)
{
	ASSERT(iRenameFile);
	if(aNewFileName.Length() > 0)
	{
		TParse parse;
		parse.Set(*iRenameFile,NULL,NULL);

		TFileName newFilePath;

		newFilePath.Append(parse.DriveAndPath());
		newFilePath.Append(aNewFileName);

		TInt err = 0;

		err=BaflUtils::RenameFile(iFs,*iRenameFile,newFilePath);

		delete iRenameFile;
		iRenameFile = NULL;
		if(err!=0)
		{
			iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DirReNameFailed),*this,ECmdNull);
		}
		else
		{
			if(iObserver)
			{
				iObserver->Update(EDeleteCompleted);
			}
		}
	}
	else
	{
		iControlFactory.CreateInputTextDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_ReName),iMainEngine.GetDesById(ETurkeyTextRes_InputNull),ECmdRenameFile);
	}
}

void CFileEngine::CreateFolder(const TDesC& aPath)
{
	if (aPath.Length() > 0)
	{
		TFileName newFolderPath;
		newFolderPath.Append( *iParentFolderName );
		newFolderPath.Append(aPath);
		newFolderPath.Append( _L("\\") );
		newFolderPath.ZeroTerminate();

		RFs& fs = CEikonEnv::Static()->FsSession();
		TInt aError = fs.MkDir( newFolderPath );
		if ( KErrNone == aError )
		{
			if(iObserver)
			{
				iObserver->Update(0);
			}
		}
		else	//创建失败
		{
			iControlFactory.CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_CreateFolderFailed),*this,ECmdNull);
		}
		delete iParentFolderName;
		iParentFolderName = NULL;
	}
	else
	{
		iControlFactory.CreateInputTextDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_InputFolderName),iMainEngine.GetDesById(ETurkeyTextRes_InputNull),ECmdCreateFolder);
	}
}

void CFileEngine::SaveAs()
{
	ASSERT(iSaveAsSrcName);
	ASSERT(iSaveAsDstPath);
	TParse parse;
	parse.Set(*iSaveAsSrcName,NULL,NULL);
	const TDesC& nameAndExt = parse.NameAndExt();
	HBufC* saveAsDstName = HBufC::NewLC(iSaveAsDstPath->Length() + nameAndExt.Length());
	saveAsDstName->Des().Append(*iSaveAsDstPath);
	saveAsDstName->Des().Append(nameAndExt);
	//iFileActiveEngine.MoveFile(*iSaveAsSrcName,*saveAsDstName,this);
	iFileActiveEngine.CopyFile(*iSaveAsSrcName,*saveAsDstName,this);
	CleanupStack::PopAndDestroy(saveAsDstName);
	delete iSaveAsSrcName;
	iSaveAsSrcName = NULL;
	delete iSaveAsDstPath;
	iSaveAsDstPath = NULL;
}

void CFileEngine::CancelSaveAs()
{
	CFileBrowser* fileBrowser = iControlFactory.CreateFolderBrowser(*this,ECmdSaveAs,ECmdCancelSaveAs);

	if(iSaveAsDstPath->Length()>3)
	{
		if(iSaveAsDstPath->Right(2).Compare(_L(":\\"))!=0)
		{
			TParse parse;
			parse.Set(iSaveAsDstPath->Left(iSaveAsDstPath->Length()-1),NULL,NULL);

			fileBrowser->SetPath(parse.DriveAndPath());
		}
	}
	delete iSaveAsDstPath;
	iSaveAsDstPath = NULL;
}

TInt CFileEngine::CheckDiskSpace(const TDesC& aSourceFile,const TDesC& aDestFile)
{
	RFile file;
	TInt rv =0;
	TInt nFileLen=0;

	rv = file.Open(iFs, aSourceFile, EFileWrite|EFileShareAny);
	if (KErrNone != rv)
	{
		return 2;
	}

	file.Size(nFileLen);

	nFileLen=nFileLen/1024;

	TInt64 nDriverSize=GetFreeSpace(aDestFile.Mid(0,1));

	if((TInt64)nFileLen>nDriverSize)
	{
		//存储空间不足
		file.Close();
		return	1;
	}
	file.Close();

	return 0;
}

TInt64 CFileEngine::GetFreeSpace(const TDesC& aDriveName)
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
