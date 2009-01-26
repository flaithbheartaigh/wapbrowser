#include "VideoPlayEngine.h"
#include <eikenv.h>
#include <VideoPlayer.h>
#include <documenthandler.h>
#include "VideoFileDetails.h"
#include <f32file.h>
#include "VideoPlayObserver.h"

CVideoPlayEngine* CVideoPlayEngine::NewL( )
{
	CVideoPlayEngine*self=new(ELeave)CVideoPlayEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CVideoPlayEngine::CVideoPlayEngine( )
{
}

void CVideoPlayEngine::ConstructL()
{
	iMediaFile=HBufC::NewL(0);
	iMyState=EPNotInitialised;
	iFileDetails=new(ELeave)CVideoFileDetails();
	iProgressUpdater=CHeartbeat::NewL(0);
}
 
CVideoPlayEngine::~CVideoPlayEngine()
{
    delete iDocHandler; iDocHandler=NULL;
	delete iMediaFile; iMediaFile=NULL;
	delete iFileDetails; iFileDetails=NULL;
    delete iProgressUpdater; iProgressUpdater=NULL;
	delete iPlayer; iPlayer=NULL;
}
 
void CVideoPlayEngine::SetNewFileL(const TDesC &aFileName)
{
	HBufC*newFile=aFileName.AllocL();
	delete iMediaFile;
	iMediaFile=newFile;
}
  
void CVideoPlayEngine::DoPlayL()
{
    if(!iProgressUpdater->IsActive())
        iProgressUpdater->Start(ETwelveOClock,this); 

	iPlayer->Play();

	switch(iMyState)
	{
		case EPStopped:
		{
			iPlayPosition=0;
			iCallback->PlaybackPositionChangedL(iPlayPosition,iFileDetails->iDurationInSeconds);
			break;
		}
		case EPPaused:
			iCallback->PlaybackPositionChangedL(iPlayPosition,iFileDetails->iDurationInSeconds);
			break;
		default:
			break;
	}

	iMyState=EPPlaying;
}
  
void CVideoPlayEngine::DoStop()
{
	if(iMyState!=EPPlaying&&iMyState!=EPPaused)
		return;

	if(iProgressUpdater->IsActive())
		iProgressUpdater->Cancel();

	iPlayPosition=0;
	iPlayer->Stop();
	iPlayer->Close();
	iMyState=EPStopped;
}
  
void CVideoPlayEngine::MvpuoOpenComplete(TInt /*aError*/)
{
    iPlayer->Prepare();
}
  
void CVideoPlayEngine::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt /*aError*/)
{
}
  
void CVideoPlayEngine::MvpuoPlayComplete(TInt aError)
{
	if(iProgressUpdater->IsActive())
		iProgressUpdater->Cancel();

	iPlayPosition=0;
	iMyState=EPStopped;
	iCallback->PlayCompletedL(aError);
	iPlayer->Close();
}
  
void CVideoPlayEngine::MvpuoPrepareComplete(TInt aError)
{    
	TInt ret=aError;
	iMyState=EPStopped;
	iFileDetails->iDurationInSeconds=0;

	//KErrMMPartialPlayback未定义,不知道其含义
	if(ret==KErrNone)// || ret==KErrMMPartialPlayback)
	{
		TSize size(0,0);
		iPlayer->VideoFrameSizeL(size);
		iFileDetails->iResolutionHeight=size.iHeight;
		iFileDetails->iResolutionWidth=size.iWidth;

		iFileDetails->iAudioTrack=iPlayer->AudioEnabledL();
		iFileDetails->iVideoTrack=iPlayer->VideoBitRateL();
		iFileDetails->iDurationInSeconds=iPlayer->DurationL().Int64()/KMPOneSecond;

		InitVolume();
	}

	if(iCallback)
		TRAPD(ignore,iCallback->InitControllerCompletedL(ret));
}
   
void CVideoPlayEngine::MvpuoEvent(const TMMFEvent& aEvent)
{
    if (aEvent.iEventType==KMMFEventCategoryVideoPlayerGeneralError && aEvent.iErrorCode==KErrHardwareNotAvailable)
        TRAPD(ignore,iCallback->PlayCompletedL(KErrAudioLost));    
}
  
void CVideoPlayEngine::InitControllerL(MVideoPlayObserver* aCallback,
                                   RWsSession& aWs,
                                   CWsScreenDevice& aScreenDevice,
                                   RWindowBase& aWindow,
                                   TRect& aScreenRect,
                                   TRect& aClipRect)   
{
	iMyState=EPInitialising;
	iCallback=aCallback;
    
	delete iPlayer; iPlayer=NULL;
	iPlayer=CVideoPlayerUtility::NewL(*this,
									  EMdaPriorityNormal,
									  EMdaPriorityPreferenceNone,
									  aWs,
									  aScreenDevice,
									  aWindow,
									  aScreenRect,
									  aClipRect);

	iPlayer->OpenFileL(iMediaFile->Des());
}

CDocumentHandler& CVideoPlayEngine::DocumentHandlerL()
{
	if(!iDocHandler)
		iDocHandler=CDocumentHandler::NewL(CEikonEnv::Static()->Process());

	return *iDocHandler;
}

void CVideoPlayEngine::Beat()
{
	if(iFileDetails->iVideoTrack)
		User::ResetInactivityTime();

	if(iMyState!=EPPlaying)
		return;

	TInt64 ret=iPlayPosition%2;
#ifdef __SERIES60_3X__
	if(ret)
#else
	if(ret.GetTInt())
#endif
	{
		iPlayPosition=iPlayPosition+1;
		if(iCallback)
			TRAPD(ignore,iCallback->PlaybackPositionChangedL(iPlayPosition,iFileDetails->iDurationInSeconds));
	}
    else
        Synchronize();
}

void CVideoPlayEngine::Synchronize()
{
	if(iMyState!=EPPlaying)
		return;
    
	TRAPD(ignore,iPlayPosition=iPlayer->PositionL().Int64()/KMPOneSecond;
				 if(iCallback)
					iCallback->PlaybackPositionChangedL(iPlayPosition,iFileDetails->iDurationInSeconds);
		 );
}

TInt CVideoPlayEngine::GetEngineState()
{
	return iMyState;
}

void CVideoPlayEngine::DoPauseL()
{
    if(iMyState==EPPlaying)
	{
        if(iProgressUpdater->IsActive())
		{
            iProgressUpdater->Cancel();
		}
		iPlayer->PauseL();
		iMyState=EPPaused;
	}
}

TInt CVideoPlayEngine::Volume()
{
	return iCurrentVolume;
}

void CVideoPlayEngine::InitVolume()
{
	iMaxVolume=iPlayer->MaxVolume();
	iCurrentVolume=iMaxVolume>>1;
	iPlayer->SetVolumeL(iCurrentVolume);
    iLastVolume = iCurrentVolume;
}

void CVideoPlayEngine::IncVolume()
{
	iCurrentVolume+=10;

	if(iCurrentVolume>iMaxVolume)
	{
		iCurrentVolume=iMaxVolume;
	}
	iPlayer->SetVolumeL(iCurrentVolume);
}

void CVideoPlayEngine::DecVolume()
{
	iCurrentVolume-=10;

	if(iCurrentVolume<0)
	{
		iCurrentVolume=0;	
	}
	iPlayer->SetVolumeL(iCurrentVolume);
}


