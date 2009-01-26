

#include "AudioPlayerEngine.h"
#include <eikenv.h>

CAudioPlayerEngine::CAudioPlayerEngine()
	:iIsInitVolume(0)
{
}


CAudioPlayerEngine::~CAudioPlayerEngine()
{
	delete iAudioPlayerUtility;
	delete iPeriodic;
}

CAudioPlayerEngine* CAudioPlayerEngine::NewLC()
{
	CAudioPlayerEngine* self = new (ELeave)CAudioPlayerEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CAudioPlayerEngine* CAudioPlayerEngine::NewL()
{
	CAudioPlayerEngine* self=CAudioPlayerEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CAudioPlayerEngine::ConstructL()
{
	iAudioPlayerUtility = CMdaAudioPlayerUtility::NewL( *this );
	iState = ENotReady;
}


void CAudioPlayerEngine::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
{	
	iUpdateObserver->AudioInitComplete(aError);
	if( aError )
	{
		/*TBuf<10> buf;
		buf.AppendNum( aError );
		CEikonEnv::Static()->AlertWin( buf );*/
		iUpdateObserver->AudioPlayComplete ();
		return;
	}
	switch( iState )
	{
	case EReadyToPlay:
		PlayL();

	case EGetMetaData:
		iDuration = aDuration ;

		TInt numEntries = 0;
		iAudioPlayerUtility -> GetNumberOfMetaDataEntries(numEntries);
		for (TInt i=0; i<numEntries; i++)
			iMetaData->Append(iAudioPlayerUtility->GetMetaDataEntryL(i));
		iState = EReadyToPlay;
		if(iIsInitVolume==0)
		{
			InitVolume();
		}
		else
		{
			SetVolume(iCurrentVolume);
		}
		PlayL();
		return;
	}
}

void CAudioPlayerEngine::MapcPlayComplete(TInt aError)
{
	/*if( aError )
	{*/
		iUpdateObserver->AudioPlayComplete ();
	//}
}

void CAudioPlayerEngine::OpenFileL(const TDesC& aFileName,RMetaDataArray* aMetaData)
{
	iMetaData = aMetaData;
	iAudioPlayerUtility->OpenFileL( aFileName );
	iState = EGetMetaData;
}
void CAudioPlayerEngine::PlayL()
{
	if( iState == EReadyToPlay || iState==EPause || iState==EStoped)
	{
		iAudioPlayerUtility->Play();

		if(iState==EStoped)
		{
			//重新计时
			StartShowVolumeTime();
		}
		iState = EPlaying;
	}
}

void CAudioPlayerEngine::StopL()
{
	iAudioPlayerUtility->Stop();
	iState = EStoped;
	iPeriodic->Cancel();
}

void CAudioPlayerEngine::PauseL()
{
	TInt aError = iAudioPlayerUtility->Pause();
	if( aError != KErrNone )
	{
	}
	iState = EPause;
}

void CAudioPlayerEngine::Close()
{
	iAudioPlayerUtility->Close();
	iState = ENotReady;
}

TTimeIntervalMicroSeconds& CAudioPlayerEngine::Duration()
{
	iDuration=iAudioPlayerUtility->Duration();
	iTotalLen=iDuration.Int64();
	return iDuration;
}

TInt CAudioPlayerEngine::GetPosition(TTimeIntervalMicroSeconds& aPosition)
{
	return iAudioPlayerUtility->GetPosition(aPosition);
}

TInt CAudioPlayerEngine::GetCurrentPercent()
{
	TTimeIntervalMicroSeconds tCurrent;
	iAudioPlayerUtility->GetPosition(tCurrent);
	TInt64 current=tCurrent.Int64();
#ifdef __SERIES60_3X__
	return current*100/iTotalLen;
#else
	return (current*100/iTotalLen).GetTInt();
#endif
}

TInt CAudioPlayerEngine::MaxVolume()
{
	if(iIsInitVolume==0)
	{
		return 0;
	}
	else
	{
		return iAudioPlayerUtility->MaxVolume();
	}
}

void CAudioPlayerEngine::SetVolume(TInt aVolume)
{
	iAudioPlayerUtility->SetVolume(aVolume);
	iCurrentVolume=aVolume;
}

void CAudioPlayerEngine::InitVolume()
{
	iMaxVolume=iAudioPlayerUtility->MaxVolume();
	iCurrentVolume=iMaxVolume>>1;
	SetVolume(iCurrentVolume);
	iIsInitVolume=1;
}

TInt CAudioPlayerEngine::Volume()
{
	return iCurrentVolume;
}

void CAudioPlayerEngine::IncVolume()
{
	if(iCurrentVolume<iMaxVolume)
	{
		iCurrentVolume+=iMaxVolume/10;
		SetVolume(iCurrentVolume);
	}
}

void CAudioPlayerEngine::DecVolume()
{
	if(iCurrentVolume>0)
	{
		iCurrentVolume-=iMaxVolume/10;
		SetVolume(iCurrentVolume);
	}
}

void CAudioPlayerEngine::SetUpdateObserver(MAudioPlayerUpdateObserver* aUpdateObserver)
{
	iUpdateObserver=aUpdateObserver;
}

void CAudioPlayerEngine::StartShowVolumeTime()
{
	iPeriodic=CPeriodic::NewL(EPriorityNormal);
	iPeriodic->Start(0,1000000,TCallBack(DoPeriodic,this));
}

TInt CAudioPlayerEngine::DoPeriodic(TAny* a)
{
	((CAudioPlayerEngine*)a)->RealDoPeriodic();
	return KErrNone;
}

void CAudioPlayerEngine::RealDoPeriodic()
{
	if(iState==EIdle || iState==ENotReady)
		return; 

	/*TBuf<100> timeStr(_L("Time:  "));

	TTimeIntervalMicroSeconds position;
	GetPosition(position);
	AppendTime(timeStr,position);

	timeStr.Append(_L(" / "));
	AppendTime(timeStr,Duration());*/

	TBuf<20> currentTime(NULL);
	TTimeIntervalMicroSeconds position;
	GetPosition(position);
	AppendTime(currentTime,position);
	
	TBuf<20> totalTime(NULL);
	AppendTime(totalTime,Duration());

	TBuf<30> volumeStr(NULL);//_L("Volume:  "));
	volumeStr.AppendNum(iCurrentVolume);
	volumeStr.Append(_L(" / "));
	volumeStr.AppendNum(iMaxVolume);

	TInt percent = 0;
#ifdef __SERIES60_3X__
	TInt n = Duration().Int64();
	if (n)
	{
		percent=(position.Int64()*100.0)/Duration().Int64();
	}
#else
	TInt n = Duration().Int64().Low();
	if (n)
	{
		percent=(position.Int64().Low()*100.0)/Duration().Int64().Low();
	}
#endif

		//percent=position.Int64().Low()*100/Duration().Int64().Low();


	iUpdateObserver->AudioUpdate(volumeStr,currentTime,totalTime,percent);
}

void CAudioPlayerEngine::AppendTime(TDes& aStr,const TTimeIntervalMicroSeconds& aDuration) // hour:minute:second
{
	TTime time(aDuration.Int64());
	TDateTime dt=time.DateTime();

	/*if(dt.Hour()>0)
	{
		if(dt.Hour()<10)
			aStr.Append(_L("0"));
		aStr.AppendNum(dt.Hour());
		aStr.Append(_L(":"));
	}*/

	//if(dt.Minute()>0)
	{
		if(dt.Minute()<10)
			aStr.Append(_L("0"));
		aStr.AppendNum(dt.Minute());
 		aStr.Append(_L(":"));
	}

	if(dt.Second()<10)
		aStr.Append(_L("0"));
	aStr.AppendNum(dt.Second());
}























