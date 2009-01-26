/*
============================================================================
 Name        : FileExecuter.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileExecuter implementation
============================================================================
*/
#include "Window.h"
#include "FileExecuter.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "StaticCommonTools.h"
#include "CoCoappui.h"
#include "TypeDefine.h"
#include "BrowserBookWindow.h"
#include "BrowserPicWindow.h"
#include "BrowserVideoWindow.h"
#include "CoCoPreDefine.h"
#include "DBEngine.h"
#include "SelectAppWindow.h"
#include "BrowserMusicWindow.h"

#include <DocumentHandler.h>
#include <eikenv.h>
#include <eikapp.h>
#include <eikdll.h>

#include <AknIconArray.h> 
#include <AknsSkinInstance.h> 
#include <AknUtils.h>
#include <avkon.hrh>
#include <AknsUtils.h> 
#include <apgcli.h>

#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#else
	#include <instapp.h>
#endif

CFileExecuter::CFileExecuter(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
{
}


CFileExecuter::~CFileExecuter()
{
	delete iDocHandler;
}

CFileExecuter* CFileExecuter::NewLC(CMainEngine& aMainEngine)
{
	CFileExecuter* self = new (ELeave)CFileExecuter(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileExecuter* CFileExecuter::NewL(CMainEngine& aMainEngine)
{
	CFileExecuter* self=CFileExecuter::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CFileExecuter::ConstructL()
{
	iDocHandler=CDocumentHandler::NewL(CEikonEnv::Static()->Process());
}

void CFileExecuter::OpenFileByDefault(TFileName aFileName)
{
	if (::FileIsExist(aFileName)==1)
	{
		iFileName.Zero ();
		iFileName.Append(aFileName);

		TInt i = aFileName.LocateReverseF('.');
		if(i<0)
		{
			iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FileTypeNotSupport),*this,0);
			return;
		}
		TFileName nFileExt = aFileName.Right( aFileName.Length() - i - 1 );
		nFileExt.ZeroTerminate();
		
		iMainEngine.WriteLog16(nFileExt);

		TFileName nAssociatedApp;

		TBool nIsFindApp = GetAssociatedApp( nFileExt, nAssociatedApp );
		if ( nIsFindApp == true)
		{	
			if (nAssociatedApp.Find(KDefaultAppPath)!=KErrNotFound)
			{
				OpenFileBySystemDefaultL(aFileName);
			}
			else if ( nAssociatedApp.FindF( KCoCoAppPath )!=KErrNotFound)
			{
				//使用内部模块打开
				OpenFileByInternal(aFileName);
			}
			else 
			{
				OpenFileByApp(nAssociatedApp,aFileName);
			}
		}
		else
		{
			//如果没有默认的打开方式
			TInt nType=ParseExPathNum(aFileName);

			#ifdef __SERIES60_3X__
			if(nType==0 || nType==1 || nType==2 || nType==3)
			#else
			if(nType==0 || nType==1 || nType==2 || nType==3 || nType==4)
			#endif
			{
				//使用内部模块打开
				OpenFileByInternal(aFileName);
			}
			else
			{
				//采用系统默认方式打开
				OpenFileBySystemDefaultL(aFileName);
			}
		}
	}
	else
	{
		//提示用户文件不存在
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FileNotExist),*this,0);
	}
}

void CFileExecuter::OpenFileByOpenWith(TFileName aFileName)
{
	if (::FileIsExist(aFileName)==1)
	{
		iFileName.Zero ();
		iFileName.Append(aFileName);

		iMainEngine.ControlFactory().CreateSelectAppWindow(*this);
	}
	else
	{
		//提示用户文件不存在
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FileNotExist),*this,0);
	}
}

void CFileExecuter::OpenFileBySystemDefaultL(TFileName aFileName)
{
	#ifndef __SERIES60_3X__
	{
		//如果是Jar、Sis或Sisx文件则调用内容打开
		TInt nType=ParseExPathNum(aFileName);

		if (nType==2)
		{
			OpenAppFile(aFileName);

			return;
		}
	}
	#endif
	
	TDataType empty=TDataType();

	#ifdef __SERIES60_3X__
	{
		iDocHandler->SetExitObserver(this);

		TRAPD(error,iDocHandler->OpenFileEmbeddedL(aFileName,empty));
	}
	#else
	{
		iDocHandler->OpenFileL(aFileName,empty );

		//If the standalone handler is already running then update the
		//document file
		TUid handlerUid;
		TInt err = KErrNone;

		#ifdef __SERIES60_3X__
		err = iDocHandler->HandlerAppUid(handlerUid);
		#else
		RApaLsSession apaLs;
		User::LeaveIfError( apaLs.Connect() );
		err = apaLs.AppForDocument(aFileName, handlerUid, empty);  
		apaLs.Close();          
		#endif

		if( !err )
		{
			RefreshDocumentFileL(handlerUid, aFileName);
		}
		else if( err == KNotInitialized )
		{
			//Handler not initialized
		}
		else
		{
			//Some other error
		}

	}
	#endif
}

TBool CFileExecuter::GetAssociatedApp( TFileName aFileExtension, TFileName& aAppPath )
{
	TBool aFoundType;

	CCoCoAppUi* nAppUi = (CCoCoAppUi*)(CEikonEnv::Static()->AppUi());
	CDictionaryStore* nlinkStore = nAppUi->Application()->OpenIniFileLC( CEikonEnv::Static()->FsSession() );

	aFileExtension.LowerCase();
	const TUint16* aFileExtensionPtr = aFileExtension.Ptr();

	TUint nDefaultUid = 0;
	TUint nMemberValue;
	for ( TInt i = 0; i < aFileExtension.Length(); i ++ )
	{
		nMemberValue = aFileExtensionPtr[i];
		if ( nMemberValue >= 97 && nMemberValue <= 122 )
		{
			nMemberValue -= 97;
		}
		else if ( nMemberValue >= 48 && nMemberValue <= 57 )
		{
			nMemberValue -= 48;
		}

		nDefaultUid = nDefaultUid* 10 + nMemberValue;
	}


	if ( nlinkStore->IsPresentL( TUid::Uid(nDefaultUid) ) ) 
	{
		RDictionaryReadStream inLinkStoreStream;
		inLinkStoreStream.OpenLC( *nlinkStore, TUid::Uid(nDefaultUid) );
		inLinkStoreStream >> aAppPath;
		CleanupStack::PopAndDestroy();

		aFoundType = true;
	}
	else
	{
		aFoundType = false;
	}		

	CleanupStack::PopAndDestroy();

	return aFoundType;
}


TBool CFileExecuter::SetAssociatedApp( TFileName aFileExtension, TFileName aAppPath )
{
	CCoCoAppUi* nAppUi = (CCoCoAppUi*)(CEikonEnv::Static()->AppUi());
	CDictionaryStore* nlinkStore = nAppUi->Application()->OpenIniFileLC( CEikonEnv::Static()->FsSession() );

	aFileExtension.LowerCase();
	const TUint16* aFileExtensionPtr = aFileExtension.Ptr();

	TUint nDefaultUid = 0;
	TUint nMemberValue;
	for ( TInt i = 0; i < aFileExtension.Length(); i ++ )
	{
		nMemberValue = aFileExtensionPtr[i];
		if ( nMemberValue >= 97 && nMemberValue <= 122 )
		{
			nMemberValue -= 97;
		}
		else if ( nMemberValue >= 48 && nMemberValue <= 57 )
		{
			nMemberValue -= 48;
		}

		nDefaultUid = nDefaultUid* 10 + nMemberValue;
	}

	RDictionaryWriteStream outLinkStoreStream;
	outLinkStoreStream.AssignLC( *nlinkStore, TUid::Uid(nDefaultUid) );
	outLinkStoreStream << aAppPath;
	outLinkStoreStream.CommitL();
	CleanupStack::PopAndDestroy();
	nlinkStore->CommitL();
	CleanupStack::PopAndDestroy( nlinkStore );

	return ETrue;
}

void CFileExecuter::OpenFileByApp(TFileName aAppName,TFileName aFileName)
{
	#ifdef __SERIES60_3X__
	{
		CApaCommandLine * cmd = CApaCommandLine::NewL();  
		cmd->SetExecutableNameL(aAppName);
		cmd->SetDocumentNameL(aFileName);
		cmd->SetCommandL(EApaCommandOpen);
		RApaLsSession apaSession;
		apaSession.Connect();
		TThreadId id;
		apaSession.StartApp(*cmd,id);
		apaSession.Close();

		delete cmd;
	}
	#else
	{
		CApaCommandLine * cmd = CApaCommandLine::NewL();        
		cmd->SetLibraryNameL( aAppName );        
		cmd->SetDocumentNameL( aFileName );
		cmd->SetCommandL( EApaCommandOpen );

		EikDll::StartAppL( *cmd );

		delete cmd;
	}
	#endif
}

void CFileExecuter::OpenFileByInternal( TFileName aFileName ) 
{
	ASSERT(iParentWindow);

	TInt n = ParseExPathNum(aFileName);

	switch(n) {
	case 0:
		{
			//图片
			iMainEngine.WriteLog16(_L("Picture Open-1"));
			CWindow* nWindow= iParentWindow->MakeChildWindow(EOpenFileWindow_Pic);
			iMainEngine.WriteLog16(_L("Picture Open-2"));
			((CBrowserPicWindow *)nWindow)->SetFileName(aFileName,1);
			iMainEngine.WriteLog16(_L("Picture Open-3"));
		}
		break;
	case 1:
		{	
			//音乐
			iMainEngine.MusicPlayManageEngine().SetFileName(aFileName,1);

			CWindow* nWindow= iParentWindow->MakeChildWindow(EOpenFileWindow_Music);
		}
		break;
	case 2:
		{
			//应用程序
			OpenAppFile(aFileName);
		}
		break;
	case 3:
		{
			//视频
			CWindow* nWindow= iParentWindow->MakeChildWindow(EOpenFileWindow_Video);

			((CBrowserVideoWindow *)nWindow)->SetFileName(aFileName);

		}
		break;
	case 4:
		{
			//电子书
			CWindow* nWindow= iParentWindow->MakeChildWindow(EOpenFileWindow_Book);

			((CBrowserBookWindow *)nWindow)->SetFileName(aFileName,GetBookMark(aFileName));

		}
		break;
	default:
		{
			//其他类型提示不能打开
			iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_FileTypeNotSupport),*this,0);
		}
		break;
	}
}

void CFileExecuter::OpenAppFile( TFileName aFileName)
{
	#ifdef __SERIES60_3X__
	{
		RApaLsSession session;
		session.Connect();
		TThreadId nThreadId;
		TInt err = session.StartDocument(aFileName, nThreadId);
		session.Close();
	}
	#else
	{
		CApaCommandLine * cmd=CApaCommandLine::NewL(); 
		RApaLsSession nSession;
		TThreadId id;

		nSession.Connect();

		cmd->SetLibraryNameL(_L("z:\\System\\apps\\appinst\\appinst.app"));
		cmd->SetDocumentNameL(aFileName);
		cmd->SetTailEndL(KInstallCommandLineTail);
		cmd->SetCommandL(EApaCommandRun);			
		nSession.StartApp(*cmd,id);	

		nSession.Close();
		delete cmd;
	}
	#endif
}

TInt CFileExecuter::ParseExPathNum(const TDesC& aFileName)
{
	TBuf<16> nExName;
	TParse parse;

	parse.Set(aFileName,NULL,NULL);
	nExName.Copy(parse.Ext());

	nExName.Delete(0,1);

	nExName.LowerCase();

	if (NULL == nExName.Compare(KBMP) 
		||	NULL == nExName.Compare(KJPG)
		||	NULL == nExName.Compare(KGIF)
		||	NULL == nExName.Compare(KPNG)
		)
		return 0;
	else if (	NULL == nExName.Compare(KMP3)
		||	NULL == nExName.Compare(KMIDI)
		||	NULL == nExName.Compare(KMID)
		||	NULL == nExName.Compare(KWAV)
		)
		return 1;
	#ifdef __SERIES60_3X__
	else if (	NULL == nExName.Compare(KSISX)
	#else
	else if (	NULL == nExName.Compare(KSIS)
	#endif
		||	NULL == nExName.Compare(KJAR)			
		)
		return 2;
	else if (	NULL == nExName.Compare(K3GP)
		||	NULL == nExName.Compare(K3GPP)
		)
		return 3;
	else if (	NULL == nExName.Compare(KTXT)			
		) 
		return 4;
	return 5;
}

void CFileExecuter::DialogEvent(TInt aCommand)
{

}

TInt CFileExecuter::GetBookMark(TFileName aBook)
{
	TParse parseFileName;
	parseFileName.Set(aBook,NULL,NULL);
	TPtrC ptr = parseFileName.Name();

	TInt nRet= iMainEngine.DBEngine().GetBookMarksByKeyL(_L("BookName"),ptr);

	TBuf<10> nTmp;
	nTmp.Zero ();
	nTmp.AppendNum (nRet);

	if(nRet==-1)
	{
		return 0;
	}
	else
	{
		return nRet;
	}
}

void CFileExecuter::DoSelectEvent(const TDesC& aAppName,TInt aCommand) 
{
	if (aCommand==1)
	{
		if (aAppName.Find(KDefaultAppPath)==KErrNotFound)
		{
			//调用用户选中的程序打开
			if ( KErrNotFound == aAppName.FindF( KCoCoAppPath ) )
			{
				OpenFileByApp(aAppName,iFileName);
			}
			else
			{
				//使用内部模块打开
				OpenFileByInternal(iFileName);
			}
		} 
		else
		{
			//调用系统默认程序打开
			OpenFileBySystemDefaultL(iFileName);
		}

		//保存用户的选则
		TBuf<16> nExName;
		TParse parse;

		parse.Set(iFileName,NULL,NULL);
		nExName.Copy(parse.Ext());

		nExName.Delete(0,1);
		nExName.LowerCase();

		SetAssociatedApp(nExName,aAppName);
	} 
}

void CFileExecuter::SetParentWindow(CWindow* aWindow)
{
	iParentWindow=aWindow;
}

#ifdef __SERIES60_3X__
void CFileExecuter::HandleServerAppExit(TInt aReason)
{
	//MAknServerAppExitObserver::HandleServerAppExit(aReason);
}
#else
void CFileExecuter::NotifyExit(TExitMode aMode)    
{
	//Handle the closing of the handler application
	//The mode can be set by the handler application by calling
	//iDoorObserver->NotifyExit(TExitMode);

	switch (aMode) 
	{

		/** Changes to the embedded document must be saved. */
	case EKeepChanges:
		{
			//TODO
		}
		break;

		/** Reverts back to the saved version of the embedded document, i.e. reloads the 
		whole document. */
	case ERevertToSaved:
		{
			//TODO
		}
		break;
		/** No changes have been made to the embedded document. */
	case ENoChanges:
		{
			//TODO
		}
		break;
		/** The embedded document is empty. */
	case EEmpty:
		{
			//TODO
		}
		break;
	default:
		//TODO
		break;
	}
}
#endif

TBool CFileExecuter::RefreshDocumentFileL(const TUid& aUid, const TDesC& aFileName )    
{
	TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	TApaTask task = (taskList.FindApp(aUid));

	if (task.Exists())
	{
		//calls AppUi::OpenFileL, requires SwEvent capability      
		User::LeaveIfError(task.SwitchOpenFile(aFileName));
		//task.BringToForeground();
		return ETrue;
	}
	return EFalse;    
}    