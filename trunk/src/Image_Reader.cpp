/*
============================================================================
 Name        : Image_Reader.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CImage_Reader implementation
============================================================================
*/
#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>
#include <APMREC.H>
#include <APGCLI.H>


#include "Image_Reader.h"


CImage_Reader::CImage_Reader(MImageReadyCallBack& aNotify)
:CActive(0),iNotify(aNotify)
{
}

CImage_Reader::~CImage_Reader()
{
	Cancel();
	delete iFrame;
}


void CImage_Reader::ConstructL(const TDesC& aFileName)
{
	CActiveScheduler::Add(this);

	iImageName.Copy(aFileName);

	TBuf8<255> ImageType;
	GetFileType(aFileName, ImageType);

	if(ImageType.Length() && iImageName.Length())
	{		
		iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(),aFileName,ImageType);

		delete iFrame;
		iFrame = NULL;
		iFrame = new(ELeave)CFbsBitmap();
		iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels,iImageDecoder->FrameInfo(0).iFrameDisplayMode);

		iImageDecoder->Convert(&iStatus,*iFrame,0);
		SetActive();
	}
	else
	{
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNotSupported);
		SetActive();
	}
}

void CImage_Reader::GetFileType(const TDesC& aFileName, TDes8& aFileType)
{
	TEntry FileEntry;

	if(CCoeEnv::Static()->FsSession().Entry(aFileName,FileEntry) == KErrNone)
	{
		TBuf8<255> FileBuffer;

		if(!FileEntry.IsDir())
		{
			TInt FileSize = FileEntry.iSize;

			if(FileSize > 255)
			{
				FileSize = 255;
			}

			if(CCoeEnv::Static()->FsSession().ReadFileSection(aFileName,0,FileBuffer,FileSize) == KErrNone)
			{
				RApaLsSession RSession;
				if(RSession.Connect() == KErrNone)
				{	
					TDataRecognitionResult FileDataType;

					RSession.RecognizeData(aFileName,FileBuffer,*&FileDataType);

					//if(FileDataType.iConfidence > aResult.iConfidence)
					{
						aFileType.Copy(FileDataType.iDataType.Des8());
					}

					RSession.Close();
				}
			}
		}
	}
}

void CImage_Reader::DoCancel()
{
	iImageDecoder->Cancel();
}

CFbsBitmap* CImage_Reader::Bitmap()
{
	return iFrame;
}


void CImage_Reader::RunL()
{
	iNotify.ImageReadyL(iStatus.Int());
}