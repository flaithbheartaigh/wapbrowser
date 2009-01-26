/*
============================================================================
 Name        : FileActiveEngine.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFileActiveEngine implementation
============================================================================
*/

#include "FileActiveEngine.h"

// -----------------------------------------------------------------------------
CFileActiveEngine::CFileActiveEngine()
: CActive( CActive::EPriorityStandard )
,fileMan(NULL)
    {
    }

// -----------------------------------------------------------------------------
void CFileActiveEngine::ConstructL()
    {
    
    CActiveScheduler::Add(this); 
    }

// -----------------------------------------------------------------------------
CFileActiveEngine::~CFileActiveEngine()
    {
	Cancel();
	
	Deque();

	delete fileMan;

	rfs.Close();    
    }

// -----------------------------------------------------------------------------
void CFileActiveEngine::CopyFile( const TDesC& aSouceFile,
                         const TDesC& aDestFile,
                         MFileEngineObserver* aObserver )
    {

		if(IsActive())
			Cancel();
		if(fileMan)
		{
			delete fileMan;
			fileMan=NULL;
			rfs.Close();    
		}

		User::LeaveIfError( rfs.Connect());
	
		fileMan = CFileMan::NewL(rfs);
		fileMan->SetObserver(this);

	iStatus=0;
	iHandleType=0;
	iNewFileName.Copy (aDestFile);

    iObserver = aObserver;
	iCancel=0;

	fileMan->Copy(aSouceFile, aDestFile, CFileMan::EOverWrite,iStatus);
	SetActive();
    }

void CFileActiveEngine::DelFile( const TDesC& aSouceFile,
                         MFileEngineObserver* aObserver )
    {
		if(IsActive())
			Cancel();

		if(fileMan)
		{
			delete fileMan;
			fileMan=NULL;
			rfs.Close();    
		}

		User::LeaveIfError( rfs.Connect());
	
		fileMan = CFileMan::NewL(rfs);
		fileMan->SetObserver(this);

	iStatus=0;
	iHandleType=1;

    iObserver = aObserver;
	iCancel=0;

	fileMan->Delete(aSouceFile, 0,iStatus);
	SetActive();
    }
void CFileActiveEngine::MoveFile( const TDesC& aSouceFile,
                         const TDesC& aDestFile,
                         MFileEngineObserver* aObserver )
    {

		if(IsActive())
			Cancel();

		if(fileMan)
		{
			delete fileMan;
			fileMan=NULL;
			rfs.Close();    
		}

		User::LeaveIfError( rfs.Connect());
	
		fileMan = CFileMan::NewL(rfs);
		fileMan->SetObserver(this);

	iStatus=0;
	iHandleType=2;
	iNewFileName.Copy (aDestFile);

    iObserver = aObserver;
	iCancel=0;

	fileMan->Move(aSouceFile, aDestFile, CFileMan::EOverWrite,iStatus);
	SetActive();
    }
// -----------------------------------------------------------------------------
void CFileActiveEngine::RunL()
    {
    iObserver->HandleComplete (iStatus.Int(),iHandleType);           
	}


// -----------------------------------------------------------------------------
void CFileActiveEngine::DoCancel()
    {
	iCancel=1;
    }

TInt CFileActiveEngine::RunError(TInt aError)
{
	return aError;
}

MFileManObserver::TControl CFileActiveEngine::NotifyFileManStarted()
{
	if(iCancel)
	{
		iObserver->HandleComplete (99,iHandleType);
		return EAbort;
	}
	else
	{
		return EContinue;
	}
}

MFileManObserver::TControl CFileActiveEngine::NotifyFileManOperation()
{
	if(iCancel)
	{
		iObserver->HandleComplete (99,iHandleType);
		return EAbort;
	}
	else
	{
		return EContinue;
	}

}

MFileManObserver::TControl CFileActiveEngine::NotifyFileManEnded()
{
	if(iCancel)
	{
		iObserver->HandleComplete (99,iHandleType);
		return EAbort;
	}
	else
	{
		return EContinue;
	}
}
