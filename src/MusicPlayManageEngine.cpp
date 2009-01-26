
#include "MusicPlayManageEngine.h"
#include "MainEngine.h"
#include <bitstd.h>
#include <e32math.h>
#include "AudioPlayerEngine.h"
#include "UtilityTools.h"

_LIT(KTimeSeparator,"/");

CMusicPlayManageEngine::CMusicPlayManageEngine(CMainEngine& aMainEngine)
:iMainEngine(aMainEngine),
iPlay(0),
iAudio(NULL),
iAudioPlayNoticeWindow(NULL),
iMetaData(NULL),
iFileNameArray(NULL),
iPlayType(0),
iCanPlay(1),
iBakVol(0),
iFirstPlay(ETrue),
iIsSliceVolume(false),
iSystemPaused(false)
{	
}

CMusicPlayManageEngine::~CMusicPlayManageEngine()
{
	delete iAudio;
	delete iMetaData;
	delete iFileNameArray;
}

CMusicPlayManageEngine* CMusicPlayManageEngine::NewLC(CMainEngine& aMainEngine)
{
	CMusicPlayManageEngine* self = new (ELeave)CMusicPlayManageEngine(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CMusicPlayManageEngine* CMusicPlayManageEngine::NewL(CMainEngine& aMainEngine)
{
	CMusicPlayManageEngine* self=CMusicPlayManageEngine::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CMusicPlayManageEngine::ConstructL()
{
	iAudio=CAudioPlayerEngine::NewL();
	iAudio->SetUpdateObserver(this);

	iMetaData=new(ELeave) RMetaDataArray();
}

void CMusicPlayManageEngine::SetPhoneStatus(TInt aStatus)
{
	if(aStatus==KKeyPhoneIncomingEvent)
	{
		if (iPlay && iFileNameArray->Count()>0)
		{
			AudioPause();
			iPlay = 0;

			iSystemPaused=true;
		}
	}
	else if(aStatus==KKeyPhoneIdleEvent)
	{
		if (iPlay==0 && iFileNameArray->Count()>0 && iSystemPaused==true)
		{
			AudioPlay();
			iPlay = 1;
		}
	}
}

/*--------------------------- audio --------------------------------*/
void CMusicPlayManageEngine::InitAudio(const TDesC& aFileName)
{
	if(iCanPlay==0)
		return;

	TFileName path;
	path.Append(aFileName);
	
	if(iPlayMusicName.Compare (path)==0)
	{
		iAudio->PlayL();

		iPlay = 1;
	}
	else
	{	
		if((iPlay==1 && iFirstPlay==EFalse) || iAudio->iState== 3)
		{
			AudioStop();

			iPlay=0;
		}
		iAudio->OpenFileL(path,iMetaData);
		
		iCanPlay=0;
		
		iAudio->StartShowVolumeTime();
		
		iPlay = 1;

		iFirstPlay=EFalse;

		iPlayMusicName.Zero ();
		iPlayMusicName.Append (path);

		if(iAudioPlayNoticeWindow)
		{
			iAudioPlayNoticeWindow->AudioSelected(iPlayIndex);
		}
	}
}

void CMusicPlayManageEngine::AudioPlay()
{
	iSystemPaused=false;
	iAudio->PlayL();
}

void CMusicPlayManageEngine::AudioPause()
{
	if (iAudio!=NULL && iPlay==1)
	{
		iAudio->PauseL();

		iPlay=0;
	}
}

void CMusicPlayManageEngine::AudioStop()
{
	if ((iAudio!=NULL && iPlay==1) || (iAudio!=NULL && iAudio->iState==3))
	{
		iAudio->StopL();

		iPlayMusicName.Zero ();

		iPlay=0;
	}
}

void CMusicPlayManageEngine::SetFileName(const TDesC& aDes,TInt aType)
{
	if(aType==0)
	{
		//内部添加
		if (aDes.Length()) {
			iFileName.Delete(0,iFileName.Length());
			iFileName.Append (iFilePath);
			iFileName.Append(aDes);
			
			//判断文件是否存在
			if(UtilityTools::FileIsExist(iFileName)==1)
			{
				InitAudio(iFileName);
			}
			else
			{
				//获取文件信息
				if (iFileNameArray)
					delete iFileNameArray;
				
				iFileNameArray = new (ELeave) CArrayFixFlat<TFileName>(10);
				
				ReadMusicFileNameList(iFilePath, iFileNameArray);
				
				if(iFileNameArray->Count()>0)
				{
					if(iPlayIndex>=iFileNameArray->Count())
					{
						//如果原来是最后一首则跳到第一首
						iPlayIndex=0;
					}
					
					TBuf<100> nName;
					nName.Copy (iFileNameArray->At(iPlayIndex));

					SetFileName(nName,0);
				}
				else
				{
					iPlayIndex=0;
				}

				//通知显示界面刷新
				if (iAudioPlayNoticeWindow)
				{
					iAudioPlayNoticeWindow->AudioFileUpdate();
				}
			}
		}
	}
	else if(aType==1)
	{
		//外部添加
		if (aDes.Length()) 
		{
			TBuf<255> nFileName;
			TParse parse;

			parse.Set(aDes,NULL,NULL);
			nFileName.Copy(parse.NameAndExt());

			TInt i=aDes.Find(nFileName);

			iFilePath.Zero ();
			iFilePath.Append (aDes.Mid(0,i));

			//获取文件信息
			if (iFileNameArray)
				delete iFileNameArray;

			iFileNameArray = new (ELeave) CArrayFixFlat<TFileName>(10);

			ReadMusicFileNameList(iFilePath, iFileNameArray);

			iFileName.Delete(0,iFileName.Length());
			iFileName.Append(aDes);

			//获取文件在列表中的位置
			for(TInt i=0;i<iFileNameArray->Count();i++)
			{
				if(nFileName==iFileNameArray->At(i))
				{
					iPlayIndex=i;		
					break;
				}
			}

			InitAudio(iFileName);			
		}
	}
	else if(aType==2)
	{
		//初始化列表
		if (aDes.Length()) 
		{
			iFilePath.Zero ();
			iFilePath.Append (aDes);

			//获取文件信息
			if (iFileNameArray)
				delete iFileNameArray;

			iFileNameArray = new (ELeave) CArrayFixFlat<TFileName>(10);

			ReadMusicFileNameList(iFilePath, iFileNameArray);

			iPlayIndex=0;
		}
	}
}

void CMusicPlayManageEngine::AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent)
{
	iVol.Delete(0, iVol.Length());
	iVol.Append(aVol);

	if(iAudioPlayNoticeWindow)
	{
		iAudioPlayNoticeWindow->AudioUpdate(aCurrentTime,aTotalTime,aPercent);
	}
}

void CMusicPlayManageEngine::AudioPlayComplete()
{
	TInt i=0;
	iPlay=0;

	//音乐播放完成
	if(iPlayType==0)
	{
		//顺序播放
		GetNextMusic();

		TBuf<100> nName;
		nName.Copy (iFileNameArray->At(iPlayIndex));

		SetFileName(nName,0);
	}
	else
	{
		if(iFileNameArray->Count()>2)
		{
			//随机播放
			TTime time;
			time.HomeTime();

			TInt64 smallRandSeed=time.Int64();

			do
			{
				i = Math::Rand(smallRandSeed)%(iFileNameArray->Count());
			}while(i==iPlayIndex);

			iPlayIndex=i;

			TBuf<100> nName;
			nName.Copy (iFileNameArray->At(iPlayIndex));

			SetFileName(nName,0);
		}
		else
		{
			GetNextMusic();

			TBuf<100> nName;
			nName.Copy (iFileNameArray->At(iPlayIndex));

			SetFileName(nName,0);
		}
	}	
}

void CMusicPlayManageEngine::GetNextMusic()
{
	iPlayIndex++ ;

	if(iPlayIndex >= (iFileNameArray->Count()))
	{
		iPlayIndex= 0;
	}
}

void CMusicPlayManageEngine::GetUpMusic()
{
	iPlayIndex--;

	if(iPlayIndex< 0)
	{
		iPlayIndex=iFileNameArray->Count()-1;	
	}
}

void CMusicPlayManageEngine::PauseAudioPlay()
{
	if (iPlay==1 && !iFirstPlay)
	{
		AudioPause();
		iPlay = 0;

		iStopbyVideo=ETrue;
	}			
}

void CMusicPlayManageEngine::RestoreAudioPlay()
{
	if (iPlay==0 && !iFirstPlay && iStopbyVideo)
	{
		AudioPlay();
		iPlay = 1;

		iStopbyVideo=EFalse;
	}
}

void CMusicPlayManageEngine::ReadMusicFileNameList(const TDesC& aFilePath, CArrayFixFlat<TFileName>* aArray)
{
	//获取指定目录下的所有音乐文件名称
	if (NULL == aFilePath.Length())
		return;

	TFileName	szPath;
	szPath.Append(aFilePath);	
	RFs	oRFs;
	User::LeaveIfError(oRFs.Connect());	

	CDir* dir = NULL;
	User::LeaveIfError(oRFs.GetDir(aFilePath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));

	int i;
	i=(*dir).Count()-1;
	for(;i>=0; i--)
	{
		const	TEntry & oEntry = (*dir)[i];
		TFileName oName = oEntry.iName;	

		if(IsMusicFile(oName)==1 && !oEntry.IsDir())
		{
			aArray->AppendL(oName);
		}
	}

	dir=NULL;
	oRFs.Close();
}

TInt CMusicPlayManageEngine::IsMusicFile(const TDesC& aFile)
{
	TInt nFileNum=0, nFileExNum=0;
	UtilityTools::GetFileName(aFile, nFileNum, nFileExNum);

	TBuf<16> szExName;
	szExName = aFile.Right(nFileExNum);
	szExName.LowerCase();
	if (NULL == szExName.Compare(KMP3)
		||	NULL == szExName.Compare(KMIDI)
		||	NULL == szExName.Compare(KMID)
		||	NULL == szExName.Compare(KWAV)
		)
		return 1;
	else 	
		return 0;
}

void CMusicPlayManageEngine::AudioInitComplete(TInt aError)
{
	iCanPlay=1;
}

void CMusicPlayManageEngine::PlayUpMusic()
{
	if (iFileNameArray->Count()>1 && iCanPlay==1)
	{
		GetUpMusic();

		TBuf<100> nName;
		nName.Copy (iFileNameArray->At(iPlayIndex));

		SetFileName(nName,0);
	}
}

void CMusicPlayManageEngine::PlayNextMusic()
{
	if (iFileNameArray->Count()>1 && iCanPlay==1)
	{
		GetNextMusic();

		TBuf<100> nName;
		nName.Copy (iFileNameArray->At(iPlayIndex));

		SetFileName(nName,0);
	}
}

void CMusicPlayManageEngine::PlayMusic()
{
	AudioPlay();
	iPlay = 1;

	if(iFirstPlay)
	{
		TBuf<100> nName;
		nName.Copy (iFileNameArray->At(iPlayIndex));

		SetFileName(nName,0);
	}
}

void CMusicPlayManageEngine::PlaySelectMusic(TInt aSelectIndex,const TDesC& aMusicFile)
{
	TFileName nFileName;
	nFileName.Copy(iFilePath);
	nFileName.Append(aMusicFile);

	if (iFileNameArray->Count()>0 && iCanPlay==1)
	{
		if(iPlayIndex==aSelectIndex && iPlayMusicName.Compare(nFileName)==0)
		{
			if (iPlay)
			{
				AudioPause();
				iPlay = 0;
			}
			else
			{
				AudioPlay();
				iPlay = 1;

				if(iFirstPlay)
				{
					/*TBuf<100> nName;
					nName.Copy (iFileNameArray->At(iPlayIndex));

					SetFileName(nName,0);*/
					SetFileName(aMusicFile,0);
				}
			}
		}
		else
		{
			iPlayIndex=aSelectIndex;

			SetFileName(aMusicFile,0);
		}
	}
}

void CMusicPlayManageEngine::DecVolume()
{
	if (iFileNameArray->Count()>0)
	{
		iAudio->DecVolume();
	}
}

void CMusicPlayManageEngine::IncVolume()
{
	if (iFileNameArray->Count()>0)
	{
		iAudio->IncVolume();
	}
}

void CMusicPlayManageEngine::SetSliceVolume(TBool aSlice)
{
	if(iIsSliceVolume && !aSlice && iAudio!=NULL && iPlay==1)
	{
		iAudio->SetVolume(iBakVol);	

		iIsSliceVolume=false;
	}
	else if(!iIsSliceVolume && aSlice && iAudio!=NULL && iPlay==1)
	{
		TLex nLex(iVol);
		nLex.Val(iBakVol);
		iAudio->SetVolume(0);	

		iIsSliceVolume=true;
	}
}

void CMusicPlayManageEngine::SetAudioPlayNoticeWindow(MAudioPlayNoticeObserver* aAudioPlayNoticWindow)
{
	iAudioPlayNoticeWindow=aAudioPlayNoticWindow;
}

TInt CMusicPlayManageEngine::GetMaxVoice()
{
	ASSERT(iAudio!=NULL);
	
	return iAudio->MaxVolume();

}

TInt CMusicPlayManageEngine::GetCurVoice()
{
	TInt nVocie;

	TLex nLex(iVol);
	nLex.Val(nVocie);

	return nVocie;
}

TInt CMusicPlayManageEngine::GetIsPlay()
{
	if (iAudio!=NULL && iAudio->iState==3)
	{
		//暂停状态
		return 2;
	} 
	else
	{
		return iPlay;
	}
}