

#if !defined(AFX_BROWSERDOWNLOAD_DOWNLOADING_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_)
#define AFX_BROWSERDOWNLOAD_DOWNLOADING_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_

#include <apgcli.h>
#include <S32FILE.H>
#include "DownLoadNet.h"
#include "TimeOut.h"
#include "CoCoPreDefine.h"
#include "DialogObserver.h"
#include "AudioPlayerEngine.h"
#include "HttpObserver.h"

class	CControlFactory;
class	CPeriodic;
class	CControl;

typedef struct TDownLoadingTask DOWNLOADINGTASK;
struct TDownLoadingTask 
{
	TInt		nTaskID;		//����ID
	TInt		nType;			//�����������ͣ�0-�������أ�1-�������أ�
	TBuf<255>	nFileName;		//�����ļ�����	
	TBuf<255>	nTmpFilePath;		//�����ļ�·��	
	TBuf<255>	nDownLoadURL;	//�����ļ�URL
	TInt		nFileLen;		//�ļ���С
	TInt		nDownLoadLen;	//�������ļ���С
	TInt		nThreadID;		//ʹ�õ������߳�ID
	TInt		nStatus;		//����״̬��0-���У�1-�ȴ����أ�2-�������أ�3-��������,4-��ֹ����,5-�޷����أ�����������ӣ�
	TInt		nIsContinueDownload;	//�Ƿ�Ϊ�ϵ�������0-���ǣ�1-��

	TBool		nIsMultiThread;			//�Ƿ�Ϊ���߳�����
	TInt		nThreadNum;				//��Ҫʹ�ö����߳�����,Ϊ0��ʾ��δ������
	TBuf<500>	nMultiThreadInfo;		//���߳����������¼��Ϣ:
	//��ʽΪ:Num:ThreadID#BeginPos*EndPos!,����δ��ThreadIDʱĬ��ֵΪ-1
	TInt		nUsedThreadNum;			//��ʹ�ö����߳̽�������

	TBuf<255>	nDownLoadResponseURL;	//���سɹ���ķ���URL
} ;

typedef struct TDownLoadThread DOWNLOADTHREAD;
struct TDownLoadThread
{
	TInt		nTaskID;		//����ID
	TBuf<255>	nFileName;		//�����ļ�����	
	TBuf<255>	nDownLoadURL;	//�����ļ�URL
	TInt		nStatus;		//����״̬��0-���У�1-�����������أ�2-������������

	TInt		nMultiUnti;		//������һ��
	CDownLoadNet* nDownLoadClass;	//�����߳�
} ;

//����״̬��0-����ɣ�1-�ȴ����أ�2-���ڵ������أ�3-���ڶ������أ�4-��ֹ���أ�5-�޷�����
enum TDownloadStatus
{
	ECompleted,
	EWaiting,
	ESingleThread,
	EMultiThread,
	EPause,
	EError,
};
typedef struct TDownLoadShowInfo DOWNLOADSHOWINFO;
struct TDownLoadShowInfo
{
	TInt		nTaskID;		//����ID
	TBuf<255>	nFileName;		//�����ļ�����	
	TInt		nStatus;		//����״̬��0-����ɣ�1-�ȴ����أ�2-���ڵ������أ�3-���ڶ������أ�4-��ֹ���أ�5-�޷�����
	TInt		nFileLen;		//�ļ���С
	TInt		nDownLoadLen;	//�������ļ���С
} ;

class MDownLoadNotice 
{
public:

	virtual void DownLoadEvent(TInt aTaskID,TInt aEventType) = 0;

	virtual void DownLoadReceived(TInt aTaskID,TInt aTotalLen, TInt aReceivedLen) = 0;

	virtual void AddDownLoadTask(const TDownLoadShowInfo* aAddTask) = 0;
};

class CDownLoadEngine : public CBase , public MDownLoadServer , public MDialogObserver,public MAudioPlayerUpdateObserver, public MOperationObserver
{
public:
	static CDownLoadEngine* NewL(CMainEngine& aMainEngine);
	static CDownLoadEngine* NewLC(CMainEngine& aMainEngine);

	~CDownLoadEngine();

	void DownLoadEvent(TInt aTaskID,TInt aThreadID,TInt aEventType);
	void DownLoadReceived(TInt aTaskID,TInt aThreadID,TInt aTotalLen, TInt aReceivedLen);
	void DownLoadReceived(TInt aTaskID,
							TInt aThreadID,
							TInt aFileSize, 
							TDesC& aFileName,
							TDesC& aDownLoadURL,
							TDesC& aDownLoadResponseURL);

	//MUpLoadServer
	void ExitDownLoad();

	//HTTPManage
	void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public:
	//=================ʱ�����==============================================================================
	static TInt Periodic(TAny *aPtr);

	void StopPeriodic();
	void StartPeriodic();

	void TimeSlice();

public:
	//�����������
	TInt AddToDownLoadingList(const TDesC& aUrl,const TDesC& aName);
	TInt AddToDownLoadingList(const TDesC& aUrl,const TDesC& aName,const TInt aFileLen,const TInt aDownLoadLen,const TInt aType,const TInt aSave);

	//��ȡ�������ص��������
	TInt GetDownLoadingTaskNum();

public:
	//======================������ʾ�������========================================================================================
	//��ֹ���أ�-1Ϊ��ֹ�������أ�
	void PauseDownLoadingTask(TInt aTaskID);

	//��ʼ���أ�-1Ϊ��ʼ�������أ�
	void BeginDownLoadingTask(TInt aTaskID);

	//ɾ�����أ�-1Ϊɾ���������أ�
	void DelDownLoadingTask(TInt aTaskID);

	//��ȡ�����б���Ϣ
	void GetDownLoadingList(RArray<TDownLoadShowInfo>& aShowInfo);

	//����֪ͨ����
	void SetNoticedWindow(MDownLoadNotice* aDownLoadNoticeWindow);

public:
	//From MDialogObserver
	void DialogEvent(TInt aCommand);

	//MAudioPlayerUpdateObserver
	void AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent);
	void AudioInitComplete(TInt aError);
	void AudioPlayComplete();

private:
	CDownLoadEngine(CMainEngine& aMainEngine);

	void ConstructL();

private:
	//20070712���������ڲ��������
	void AddToDownLoadingList(TDesC& aUrl, 
								TDesC& aName,
								TInt aFileLen,
								TInt aDownLoadLen,
								TDesC& aFileTempName,
								TInt aIsMulti,
								TInt aThreadNum,
								TDesC& aMultiInfo);

	void InitDownLoadingList();

	void InitDownLoadThreadList();

	void CheckDownLoadTaskList();

	TInt AddTaskToDownLoadThread(TInt aListIndex);

	TInt DelFromDownLoadingList(TInt aTaskID);

	//ֹͣ�����߳�
	void StopDownLoadInThread(TInt aTaskID,TInt IsDelTmpFile);

	const TDesC& CheckFileName(const TDesC& aName);

	TFileName GetFullFileName(TFileName aFileName);

	const TDesC& ParseExPath(const TDesC& aFile);
	TInt ParseExPathNum(const TDesC& aFile);

	void WriteDownLoadLog();
	void GetInfoFromLog();

	//20070531���������ظ������ж�
	void GetDownLoadedHistoryFromFile();

	void AddToDownLoadedHistoryArray(const TDesC& aURL);

	void DelFromDownLoadedHistoryArray(const TDesC& aURL);

	void SaveDownLoadedHistoryToFile();

	TInt CheckIsInHistoryArray(const TDesC& aURL,const TDesC& aName,TInt& aIndex);

	TInt CheckFileIsSupport(TDes& aFileName);

	//20070601�������ӻ�ȡ�ļ���
	void GetFileNameFromURL(const TDesC& aURL,TDes& aFileName);

	void NoticeUserErrURL(TInt aTaskArrayID);
	void NoticeUserNoFreeDisk(TInt aTaskArrayID);
	void NoticeUserDownLoaded(TInt aTaskArrayID);

	TInt AddToDownLoadedList(TInt aTaskID);

	TInt FileIsExist(const TDes& aFileName,TInt& aSize);

	void GetRealFileName(TDes& aFileName);

private:
	CArrayFixFlat<TURLTEXT>* iDownLoadedArray;

	TDownLoadingTask* iDownLoadingTaskList;

	TDownLoadThread* iDownLoadThread;
	
	MDownLoadNotice*	iDownLoadNoticeWindow;

	CMainEngine&		iMainEngine;

	const CControlFactory&	iControlFactory;
	
	CPeriodic*			iPeriodic;

	CAudioPlayerEngine* iAudio;
	RMetaDataArray* iMetaData;

	CControl* iDialog;

	TBool iIsPlayMusic;
	TBuf<255> iMusicFile;
	
	TInt iDownLoadingTaskNum;

	RFs iRfs;

	CMyTimer	iTimer;

	TInt iDownLoadingTaskID;	//�����б�ID

	TInt iTimeCount;

	TFileName iDownLoadSuccessPath;

private:
	//20070710����������Ƭ����
	TInt CreateDownLoadFile(TInt aListIndex);

	TInt iMaxFileSize;		//��Ƭ�ļ����ֵ,ֻ�д�С�����˴�С����Ҫ��Ƭ
	TInt iPerUnitSize;		//ÿ����Ƭ�Ĵ�С
	TInt iSurplusUnitSize;	//���ʣ���ļ����Ĵ�С,�����Ƭ���ʣ���СС�ڴ˴�С�򲻽�����Ƭ,��ֵ�����iMaxFileSizeС

	void SegmentationFile(TInt aListIndex);

	TInt SetMultiThread(TInt aListIndex,
		TInt aThreadIndex,
		TInt& aBeginPos,
		TInt& aEndPos,
		TInt& aNum);

	TInt UpdateMultiThread(TInt aListIndex,
		TInt aUnitID,
		TInt aSize,
		TInt aType);

	void ChecekMultiInfo(TDes& aMultiInfo);
};

#endif // !defined(AFX_BROWSERDOWNLOADMANAGE_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_)
