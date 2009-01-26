/*
============================================================================
 Name        : HandleLocalSearch.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleLocalSearch declaration
============================================================================
*/

#ifndef HANDLELOCALSEARCH_H
#define HANDLELOCALSEARCH_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <BADESCA.H>
#include <f32file.h>
#include "FileActiveEngine.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;

/**
*  CHandleLocalSearch
* 
*/
class CHandleLocalSearch : public CBase,public MFileEngineObserver
{
public: // Constructors and destructor

	~CHandleLocalSearch();
	static CHandleLocalSearch* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType);
	static CHandleLocalSearch* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType);

private:

	CHandleLocalSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine,TInt aType);
	void ConstructL();

public:
	virtual void HandleComplete(TInt aStatus,TInt aType);
public:
	TInt GetCount();
	const TDesC& GetOneItemName(TInt aIndex);
	const TDesC& GetOneItemPath(TInt aIndex);
	void  DeleteOneItem(TInt aIndex);
	void SetKeyWord(const TDesC& aText);

	TInt CopyFile(TInt aIndex,const TDesC& aNewFilePath);
	TInt MoveFile(TInt aIndex,const TDesC& aNewFilePath);
	TInt DeleteFile(const TDesC& aFromFilePath);
private:
	void StartSearch();
	TInt SearchDir(const TDesC& aFilePath);

	TInt IsType(const TDesC& aFromFileName);

	TInt CheckFile(const TDesC& aFromFilePath,const TDesC& aFileName,const TDesC& aNewFilePath);
private:

	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;

	CDesCArray*	iDesNameArray;
	CDesCArray*	iDesPathArray;

	RFs iFs;
	TInt iType;

	HBufC* iBuf;
	HBufC* iNewPath;
	TInt iNewIndex;

	TFileName iFilePath;
	TBuf<20> iKeyWord;
};

#endif // HANDLELOCALSEARCH_H
