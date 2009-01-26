/*
============================================================================
 Name        : FileEngine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 对文件相关操作的静态方法
============================================================================
*/

#ifndef FILEENGINE_H
#define FILEENGINE_H

#include <e32std.h>
#include <e32base.h>
#include "FileActiveEngine.h"
#include "DialogObserver.h"
#include "FileBrowserObserver.h"
#include "MControlObserver.h"

class CMainEngine;
class CControl;
class CControlFactory;
class MFileEngineEvent;

class CFileEngine : public CBase , public MDialogObserver , public MInputObserver , public MFileEngineObserver , public MFileBrowserObserver
{
public: // Constructors and destructor
	~CFileEngine();
	static CFileEngine* NewL(CMainEngine& aMainEngine);
	static CFileEngine* NewLC(CMainEngine& aMainEngine);

private:
	CFileEngine(CMainEngine& aMainEngine);
	void ConstructL();

public://From MDialogObserver
	virtual void DialogEvent(TInt aCommand);

public://From MInputObserver
	virtual void InputResponseEvent(TInt aEvent,const TDesC& aText);

public://From MFileEngineObserver
	virtual void HandleComplete(TInt aStatus,TInt aType);

public://From MFileBrowserObserver
	virtual void	DoFileEvent(const TDesC& aFileName,TInt aCommand);

public:
	void	SetObserver(MFileEngineEvent* aObserver);

	void	RequestDeleteFile(const TDesC& aFileName);

	void	CopyFile(const TDesC& aFilaName);

	void	PasteFile(const TDesC& aFilaName);

	void	RequestRenameFile(const TDesC& aFileName);

	void	RequestCreateFolder(const TDesC& aParentPath);

	void	RequestSaveAs(const TDesC& aFileName);

private:
	void	DeleteFile();
	TInt	DeleteFile(const TDesC& aPath);

// 	void	DeleteFolderAndAllFiles();
// 
// 	void	DeleteFolderAndAllFiles(const TDesC& aPath);

	void	DeleteFolder();

	void	RenameFile(const TDesC& aNewFileName);

	void	CreateFolder(const TDesC& aPath);

	void	SaveAs();

	void	CancelSaveAs();

	TInt	CheckDiskSpace(const TDesC& aSourceFile,const TDesC& aDestFile);

	TInt64	GetFreeSpace(const TDesC& aDriveName);

private:
	CMainEngine&	iMainEngine;
	CFileActiveEngine&		iFileActiveEngine;
	const CControlFactory&	iControlFactory;

	MFileEngineEvent*	iObserver;

	RFs&			iFs;

	HBufC*			iFileForDel;
	HBufC*			iCopiedFile;
	HBufC*			iRenameFile;
	HBufC*			iParentFolderName;
	HBufC*			iSaveAsSrcName;
	HBufC*			iSaveAsDstPath;

	CControl*		iDialog;
	//TInt			iInputEvent;
};

#endif // FILEENGINE_H
