
class MVideoPlayObserver 
{
public:

	virtual void InitControllerCompletedL(TInt aError)=0;
	virtual void PlayCompletedL(TInt aError)=0;
	virtual void PlaybackPositionChangedL(TInt64 aPlaybackPosInSeconds,TInt64 aTotalLengthInSeconds)=0;
};