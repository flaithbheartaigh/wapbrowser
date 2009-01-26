
#include "AudioPlayerEngine.h"

class CMainEngine;

class MAudioPlayNoticeObserver
{
public:
	virtual void AudioUpdate(const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent) = 0;

	virtual void AudioSelected(const TInt aSelectIndex) = 0;

	virtual void AudioFileUpdate() = 0;
};

class CMusicPlayManageEngine :  public CBase,public MAudioPlayerUpdateObserver
{
public:
	~CMusicPlayManageEngine();
	static CMusicPlayManageEngine* NewL(CMainEngine& aMainEngine);
	static CMusicPlayManageEngine* NewLC(CMainEngine& aMainEngine);

private:
	CMusicPlayManageEngine(CMainEngine& aMainEngine);
	void ConstructL();

public:
	void AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent);

	void AudioPlayComplete();
	void AudioInitComplete(TInt aError);

public:
	void AudioPause();
	void AudioStop();

	void SetFileName(const TDesC& aDes,TInt aType);

	void PauseAudioPlay();
	void RestoreAudioPlay();

	void SetPhoneStatus(TInt aStatus);

	void PlayMusic();
	void PlaySelectMusic(TInt aSelectIndex,const TDesC& aMusicFile);
	void PlayNextMusic();
	void PlayUpMusic();

	void DecVolume();
	void IncVolume();

	void SetSliceVolume(TBool aSlice);

	inline TInt GetPlayType()
	{
		return iPlayType;
	}

	inline TInt SetPlayType(TInt aType)
	{
		iPlayType=aType;
		//ASSERT(EFalse);	//这里代码没有写返回值,请相关人员修改
		return 1;
	}

	inline TInt GetPlayIndex()
	{
		return iPlayIndex;
	}

	inline TBool GetIsSlice()
	{
		return iIsSliceVolume;
	}
	
	inline CArrayFixFlat<TFileName>* GetFileName()
	{
		return iFileNameArray;
	}

	void SetAudioPlayNoticeWindow(MAudioPlayNoticeObserver* aAudioPlayNoticWindow);

	TInt GetMaxVoice();

	TInt GetCurVoice();
	
	TInt GetIsPlay();
	
private:
	void InitAudio(const TDesC& aFileName);

	void ReadMusicFileNameList(const TDesC& aFilePath, CArrayFixFlat<TFileName>* aArray);

	TInt IsMusicFile(const TDesC& aFile);
	
	void AudioPlay();

	void GetNextMusic();
	void GetUpMusic();
private:
	CMainEngine& iMainEngine;

	CAudioPlayerEngine* iAudio;
	RMetaDataArray* iMetaData;
	
	MAudioPlayNoticeObserver* iAudioPlayNoticeWindow;

	TInt iTimer;
	TInt iCurrentPercent; // 100%

	TFileName	iFileName;

	TInt iPlay;

	TBuf<256> iVol;

	TInt iBakVol;

	CArrayFixFlat<TFileName>*	iFileNameArray;
	TFileName	iFilePath;

	TInt iPlayType;	//播放类型：0-顺序播放，1-随机播放

	TBool iFirstPlay;

	TBool iStopbyVideo;
	TInt iPlayIndex;	//播放的歌曲编号
	TInt iCanPlay;

	TFileName iPlayMusicName;

	TBool iSystemPaused;

	TBool iIsSliceVolume;
};
