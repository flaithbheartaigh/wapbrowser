/*
============================================================================
Name        : PicConvEngine.cpp
Author      : 
Version     :
Copyright   : Your copyright notice
Description : CPicConvEngine implementation
============================================================================
*/


#include "PicConvEngine.h"
#include "ImageConversion.h"

CPicConvEngine* CPicConvEngine::NewL(MImageConvObserver* pObserver)
{
	CPicConvEngine* pSelf = CPicConvEngine::NewLC(pObserver);
	CleanupStack::Pop();
	return pSelf;
}

CPicConvEngine* CPicConvEngine::NewLC(MImageConvObserver* pObserver)
{
	CPicConvEngine* pSelf = new(ELeave)CPicConvEngine(pObserver);
	CleanupStack::PushL(pSelf);
	pSelf->ConstructL();
	return pSelf;
}

CPicConvEngine::CPicConvEngine(MImageConvObserver* pObserver) : CActive(EPriorityStandard)	// Standard priority
{
	iObserver = pObserver;
	iDecoder    = NULL;
	iEncoder    = NULL;
	iFlag     = EFalse;
	iFrameIndex  = 0;
}

void CPicConvEngine::ConstructL()
{
	User::LeaveIfError(iFs.Connect());
	CActiveScheduler::Add(this);				// Add to scheduler
}

CPicConvEngine::~CPicConvEngine()
{
	if(NULL !=iDecoder)
		iDecoder->Cancel(); // Cancel any request, if outstanding
	delete iDecoder;

	if(NULL != iEncoder)
		iEncoder->Cancel();
	delete iEncoder;
	if(iFlag)
		User::WaitForRequest(iStatus);
	iFs.Close();
	// Delete instance variables if any
}

void CPicConvEngine::OpenL(const TFileName& aFileName)
{
	//判断此文件是否存在
	//aFile
	iFileName = aFileName;

	delete iDecoder;
	iDecoder = NULL;
	iDecoder = CImageDecoder::FileNewL(iFs,iFileName);

	if(NULL != iObserver)
		iObserver->MiuoOpenComplete( 0 );//没有错误
}


TInt CPicConvEngine::FrameCount()
{
	if(NULL == iDecoder)
		return -1;

	return iDecoder->FrameCount();
}
void CPicConvEngine::CreateL(const TFileName& aFileName,const TDesC8& aMIMEType)
{
	delete iEncoder;
	iEncoder = NULL;
	iEncoder = CImageEncoder::FileNewL(iFs, aFileName, aMIMEType, CImageEncoder::EOptionNone);
	if(NULL != iObserver)
		iObserver->MiuoCreateComplete( 0 );//没有错误
}

void CPicConvEngine::ConvertL(CFbsBitmap& aBitmap)
{
	iDecoder->Convert(&iStatus,aBitmap,iFrameIndex);
	SetActive();
	iFlag = ETrue; 
	//User::WaitForRequest(iStatus);
}

void CPicConvEngine::ConvertL(CFbsBitmap& aBitmap,CFrameImageData* pImageData)
{
	// Begin conversion process
	iEncoder->Convert(&iStatus, aBitmap, pImageData);
	SetActive();
	iFlag = ETrue; 
	//User::WaitForRequest(iStatus);
}

void CPicConvEngine::FrameInfo(TInt aFrameIndex, TFrameInfo& aframeInfo)
{
	iFrameIndex = aFrameIndex;
	if(NULL != iDecoder)
		aframeInfo = iDecoder->FrameInfo(aFrameIndex);
}

void CPicConvEngine::DoCancel()
{
	if(NULL != iDecoder)
		iDecoder->Cancel();
	if(NULL != iEncoder)
		iEncoder->Cancel();
}

void CPicConvEngine::RunL()
{
	iFlag = EFalse;
	if(NULL != iObserver)
		iObserver->MiuoConvertComplete( 0 );//转换成功
}
