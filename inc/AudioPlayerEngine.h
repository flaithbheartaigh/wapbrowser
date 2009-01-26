

#ifndef AUDIOPLAYERENGINE_H
#define AUDIOPLAYERENGINE_H

#include <e32std.h>
#include <e32base.h>
#include <MdaAudioSamplePlayer.h>

typedef RPointerArray<CMMFMetaDataEntry> RMetaDataArray;


class MAudioPlayerUpdateObserver
{
public:
	virtual void AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent)=0;

	virtual void AudioPlayComplete()=0;
	virtual void AudioInitComplete(TInt aError)=0;
};


class CAudioPlayerEngine : public CBase,MMdaAudioPlayerCallback
{
public:

	static CAudioPlayerEngine* NewL();
	static CAudioPlayerEngine* NewLC();
	~CAudioPlayerEngine();

    void OpenFileL(const TDesC& aFileName,RMetaDataArray* aMetaData );
	void PlayL();	
	void StopL();
	void PauseL();
	void Close();

	TTimeIntervalMicroSeconds& Duration();
	TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);
	TInt GetCurrentPercent();
	TInt MaxVolume();
	void SetVolume(TInt aVolume);
	TInt Volume();
	void InitVolume();
	void IncVolume();
	void DecVolume();
	void SetUpdateObserver(MAudioPlayerUpdateObserver* aUpdateObserver);

	void StartShowVolumeTime();

	void RealDoPeriodic();

	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);

	enum TState
	{
		EIdle,
	    ENotReady,	
	    EReadyToPlay,	
		EPause,
		EStoped,
	    EPlaying,
		EGetMetaData
	};

private:

	CAudioPlayerEngine();
	void ConstructL();

	static TInt DoPeriodic(TAny* a);
	void AppendTime(TDes& aStr,const TTimeIntervalMicroSeconds& aDuration);

public:

	TState iState;

private:

	CMdaAudioPlayerUtility* iAudioPlayerUtility;
	RMetaDataArray* iMetaData;
	TTimeIntervalMicroSeconds iDuration;
	TInt64 iTotalLen;
	TInt iMaxVolume;
	TInt iCurrentVolume;

	MAudioPlayerUpdateObserver* iUpdateObserver;
	CPeriodic* iPeriodic;

	TInt iIsInitVolume;
};

class MAudioPlayerEngineObserver
{
	virtual void HandleEngineState(CAudioPlayerEngine::TState aState, TInt aError) = 0;
};



#endif // AUDIOPLAYERENGINE_H
