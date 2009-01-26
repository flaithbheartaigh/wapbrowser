

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
	TInt		nTaskID;		//任务ID
	TInt		nType;			//下载任务类型（0-正常下载，1-升级下载）
	TBuf<255>	nFileName;		//下载文件名称	
	TBuf<255>	nTmpFilePath;		//下载文件路径	
	TBuf<255>	nDownLoadURL;	//下载文件URL
	TInt		nFileLen;		//文件大小
	TInt		nDownLoadLen;	//已下载文件大小
	TInt		nThreadID;		//使用的下载线程ID
	TInt		nStatus;		//下载状态：0-空闲，1-等待下载，2-单线下载，3-多线下载,4-中止下载,5-无法下载（例如错误链接）
	TInt		nIsContinueDownload;	//是否为断点续传：0-不是，1-是

	TBool		nIsMultiThread;			//是否为多线程下载
	TInt		nThreadNum;				//需要使用多少线程下载,为0表示尚未做分析
	TBuf<500>	nMultiThreadInfo;		//多线程下载情况记录信息:
	//格式为:Num:ThreadID#BeginPos*EndPos!,当尚未有ThreadID时默认值为-1
	TInt		nUsedThreadNum;			//已使用多少线程进行下载

	TBuf<255>	nDownLoadResponseURL;	//下载成功后的反馈URL
} ;

typedef struct TDownLoadThread DOWNLOADTHREAD;
struct TDownLoadThread
{
	TInt		nTaskID;		//任务ID
	TBuf<255>	nFileName;		//下载文件名称	
	TBuf<255>	nDownLoadURL;	//下载文件URL
	TInt		nStatus;		//下载状态：0-空闲，1-单线下载下载，2-多线下载下载

	TInt		nMultiUnti;		//下载哪一块
	CDownLoadNet* nDownLoadClass;	//下载线程
} ;

//下载状态：0-已完成，1-等待下载，2-正在单线下载，3-正在多线下载，4-中止下载，5-无法下载
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
	TInt		nTaskID;		//任务ID
	TBuf<255>	nFileName;		//下载文件名称	
	TInt		nStatus;		//下载状态：0-已完成，1-等待下载，2-正在单线下载，3-正在多线下载，4-中止下载，5-无法下载
	TInt		nFileLen;		//文件大小
	TInt		nDownLoadLen;	//已下载文件大小
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
	//=================时钟相关==============================================================================
	static TInt Periodic(TAny *aPtr);

	void StopPeriodic();
	void StartPeriodic();

	void TimeSlice();

public:
	//添加下载任务
	TInt AddToDownLoadingList(const TDesC& aUrl,const TDesC& aName);
	TInt AddToDownLoadingList(const TDesC& aUrl,const TDesC& aName,const TInt aFileLen,const TInt aDownLoadLen,const TInt aType,const TInt aSave);

	//获取正在下载的任务个数
	TInt GetDownLoadingTaskNum();

public:
	//======================下载显示界面调用========================================================================================
	//终止下载（-1为终止所有下载）
	void PauseDownLoadingTask(TInt aTaskID);

	//开始下载（-1为开始所有下载）
	void BeginDownLoadingTask(TInt aTaskID);

	//删除下载（-1为删除所有下载）
	void DelDownLoadingTask(TInt aTaskID);

	//获取下载列表信息
	void GetDownLoadingList(RArray<TDownLoadShowInfo>& aShowInfo);

	//设置通知界面
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
	//20070712张晖增加内部添加下载
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

	//停止下载线程
	void StopDownLoadInThread(TInt aTaskID,TInt IsDelTmpFile);

	const TDesC& CheckFileName(const TDesC& aName);

	TFileName GetFullFileName(TFileName aFileName);

	const TDesC& ParseExPath(const TDesC& aFile);
	TInt ParseExPathNum(const TDesC& aFile);

	void WriteDownLoadLog();
	void GetInfoFromLog();

	//20070531张晖增加重复下载判断
	void GetDownLoadedHistoryFromFile();

	void AddToDownLoadedHistoryArray(const TDesC& aURL);

	void DelFromDownLoadedHistoryArray(const TDesC& aURL);

	void SaveDownLoadedHistoryToFile();

	TInt CheckIsInHistoryArray(const TDesC& aURL,const TDesC& aName,TInt& aIndex);

	TInt CheckFileIsSupport(TDes& aFileName);

	//20070601张晖增加获取文件名
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

	TInt iDownLoadingTaskID;	//下载列表ID

	TInt iTimeCount;

	TFileName iDownLoadSuccessPath;

private:
	//20070710张晖增加切片下载
	TInt CreateDownLoadFile(TInt aListIndex);

	TInt iMaxFileSize;		//切片文件最大值,只有大小超过此大小才需要切片
	TInt iPerUnitSize;		//每个切片的大小
	TInt iSurplusUnitSize;	//最后剩余文件最大的大小,如果切片后的剩余大小小于此大小则不进行切片,此值必须比iMaxFileSize小

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
