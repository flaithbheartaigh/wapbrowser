
#include "e32base.h"
#include <VideoPlayer.h>

class CVideoPlayerUtility;
class CDocumentHandler;
class CVideoFileDetails;
class DRMCommon;
class CHeartbeat;
class MVideoPlayObserver;

const TInt KMPOneSecond( 1000000 );
const TInt KMPOneMinute( 60000000 );
const TInt KOneHourInSeconds( 3600 );

enum TMPControlCommands
{
    EMPControlCmdPlay,
    EMPControlCmdPause,
    EMPControlCmdStop,
    EMPControlCmdStartSeekForward,
    EMPControlCmdStartSeekBackward,
    EMPControlCmdStopSeeking,
    EMPControlCmdVolumeUpByOne,
    EMPControlCmdVolumeDownByOne,
    EMPControlCmdVolumeMute,
    EMPControlCmdVolumeUnMute,
    EMPControlCmdOpenNextPlaylistItem,
    EMPControlCmdClose,
    EMPControlCmdReOpen
};

enum TPlayerErrCode
{
	KErrReOpen = 101,
	KErrAutoDisconnect,
	KErrAudioLost
};

enum TPlayState
{
    EPNotInitialised,
    EPInitialising,
    EPStopped,
    EPPlaying,     
    EPPaused
};

class CVideoPlayEngine : public CBase,MVideoPlayerUtilityObserver,MBeating
{

public:

	static CVideoPlayEngine* NewL();
	virtual ~CVideoPlayEngine();

	void InitControllerL(MVideoPlayObserver* aCallback,
						 RWsSession& aWs,
						 CWsScreenDevice& aScreenDevice,
						 RWindowBase& aWindow,
						 TRect& aScreenRect,
						 TRect& aClipRect);
	void SetNewFileL(const TDesC& aFileName);

	CDocumentHandler& DocumentHandlerL();
	TInt GetEngineState();
	void DoStop();
	void DoPlayL();
	void DoPauseL();
	
	TInt Volume();
	void InitVolume();
	void IncVolume();
	void DecVolume();

	TPlayState iMyState;

private:

	CVideoPlayEngine();
	void ConstructL();

	void Beat();
	void Synchronize();

	void MvpuoOpenComplete(TInt aError);
	void MvpuoPrepareComplete(TInt aError);
	void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	void MvpuoPlayComplete(TInt aError);
	void MvpuoEvent(const TMMFEvent& aEvent);

	CVideoPlayerUtility* iPlayer;
	HBufC* iMediaFile;
	CVideoFileDetails* iFileDetails;
	MVideoPlayObserver* iCallback;
	CDocumentHandler* iDocHandler;
	CHeartbeat* iProgressUpdater;

	TInt64 iPlayPosition;
	TInt ignore;

	TInt iMaxVolume;
	TInt iCurrentVolume;
    TInt iLastVolume;

};









