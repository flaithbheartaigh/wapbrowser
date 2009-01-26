/*
============================================================================
 Name        : FileActiveEngine.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFileActiveEngine declaration
============================================================================
*/

#ifndef FILEACTIVEENGINE_H
#define FILEACTIVEENGINE_H

#include <e32base.h>
#include <f32file.h>
#include <e32std.h>
#include "CoCoPreDefine.h"

class MFileEngineObserver
	{
	public:
		
		virtual void HandleComplete(TInt aStatus,TInt aType)=0;
	};

class CFileActiveEngine : public CActive,public MFileManObserver
    {
	public: // methods

        CFileActiveEngine();

        void ConstructL();

        CFileActiveEngine::~CFileActiveEngine();

        void CopyFile( const TDesC& aSouceFile,
                         const TDesC& aDestFile,
                         MFileEngineObserver* aObserver );

		void DelFile(const TDesC& aSouceFile,
                         MFileEngineObserver* aObserver );

		//卞涛增加剪切功能
		void MoveFile( const TDesC& aSouceFile,
                         const TDesC& aDestFile,
                         MFileEngineObserver* aObserver );
		
		TControl NotifyFileManStarted();
		TControl NotifyFileManOperation();
		TControl NotifyFileManEnded();

    private: // from CActive

		virtual void RunL();
		virtual void DoCancel();
		virtual TInt RunError(TInt aError);

	private:
		RFs rfs;

		CFileMan *fileMan;

		MFileEngineObserver* iObserver;

		TInt iCancel;

		TFileName iNewFileName;

		TInt iHandleType;
    };
#endif // FILEACTIVEENGINE_H
