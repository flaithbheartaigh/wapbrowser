/*
============================================================================
 Name        : FileExecuter.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileExecuter declaration
 描述		 : 该类封装执行文件的操作
============================================================================
*/

#ifndef FILEEXECUTER_H
#define FILEEXECUTER_H

#include <e32std.h>
#include <e32base.h>
#ifdef __SERIES60_3X__
	#include <aknserverapp.h> 
#else
	#include <apparc.h>
#endif
#include "DialogObserver.h"
#include "SelectAppObserver.h"

class CMainEngine;
class CDocumentHandler;
class MSelectAppObserver;
class CWindow;

#ifdef __SERIES60_3X__
class CFileExecuter : public CBase,public MAknServerAppExitObserver,public MDialogObserver,MSelectAppObserver
#else
class CFileExecuter : public CBase,public MApaEmbeddedDocObserver,public MDialogObserver, MSelectAppObserver
#endif
{
public: // Constructors and destructor
	~CFileExecuter();
	static CFileExecuter* NewL(CMainEngine& aMainEngine);
	static CFileExecuter* NewLC(CMainEngine& aMainEngine);

	#ifdef __SERIES60_3X__
	//MAknServerAppExitObserver
	void HandleServerAppExit(TInt aReason);
	#else
	//from MApaEmbeddedDocObserver
	void NotifyExit(TExitMode aMode);    
	#endif

	//MDialogObserver
	void DialogEvent(TInt aCommand);

	//MSelectAppObserver
	void DoSelectEvent(const TDesC& aAppName,TInt aCommand) ;

	void SetParentWindow(CWindow* aWindow);

	void OpenFileByDefault(TFileName aFileName);
	void OpenFileByOpenWith(TFileName aFileName);
private:
	CFileExecuter(CMainEngine& aMainEngine);
	void ConstructL();

	void OpenFileBySystemDefaultL(TFileName aFileName);

	void OpenFileByApp(TFileName aAppName,TFileName aFileName);

	void OpenFileByInternal( TFileName aFileName);

	void OpenAppFile( TFileName aFileName);
private:
	TBool GetAssociatedApp( TFileName aFileExtension, TFileName& aAppPath );
	TBool SetAssociatedApp( TFileName aFileExtension, TFileName aAppPath );

	TInt ParseExPathNum(const TDesC& aFileName);

	TInt GetBookMark(TFileName aBook);

	TBool RefreshDocumentFileL(const TUid& aUid, const TDesC& aFileName );
private:
	CMainEngine&	iMainEngine;

	CDocumentHandler* iDocHandler;

	TFileName	iFileName;

	CWindow*	iParentWindow;
};

#endif // FILEEXECUTER_H
