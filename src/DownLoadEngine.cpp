// FileMainState.cpp: implementation of the CDownLoadEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "DownLoadEngine.h"
#include "DownLoadNet.h"
#include "DBEngine.h"
#include "utf.h"
#include "MainEngine.h"
#include "UtilityTools.h"
#include "StaticCommonTools.h"
#include "DownLoadHistoryEngine.h"
#include "PreDefine.h"
#include "ControlFactory.h"
#include "FileExecuter.h"
#include "HttpManager.h"
#include "Dialog.h"
#include "TypeDefine.h"
#include "Window.h"
#include "BitmapFactory.h"

#define DOWNLOADING_NUM (10)
#define DOWNLOADTHREAD_NUM (6)
#define DOWNLOADEDHISTORY_NUM (200)

_LIT(KTEXTBACK,"...");

_LIT(KNUM,":");
_LIT(KBEGINPOS,"*");
_LIT(KENDPOS,"!");
_LIT(KTHREAD,"#");

_LIT(KMUSICFILE,"info.wav");

_LIT(KDOWNLOADURL,"/coco/d?");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum TDialogEventType
{
	ECmdNull,
	ECmdOpenFile
};

CDownLoadEngine::CDownLoadEngine(CMainEngine& aMainEngine) : iMainEngine(aMainEngine),
															 iControlFactory(aMainEngine.ControlFactory()),
															 iAudio(NULL),
															 iMetaData(NULL),
															 iDownLoadNoticeWindow(NULL),
															 iPeriodic(NULL),
															 iDownLoadedArray(NULL),
															 iDialog(NULL),
															 iIsPlayMusic(false)
{
}

CDownLoadEngine* CDownLoadEngine::NewLC(CMainEngine& aMainEngine)
{
	CDownLoadEngine* self = new (ELeave)CDownLoadEngine(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownLoadEngine* CDownLoadEngine::NewL(CMainEngine& aMainEngine)
{
	CDownLoadEngine* self=CDownLoadEngine::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownLoadEngine::ConstructL()
{
	iDownLoadingTaskList=new (ELeave) TDownLoadingTask[DOWNLOADING_NUM];
	iDownLoadThread=new (ELeave) TDownLoadThread[DOWNLOADTHREAD_NUM];

	InitDownLoadingList();

	InitDownLoadThreadList();

	iRfs.Connect();

	iPeriodic = CPeriodic::NewL(CActive::EPriorityIdle);
	iPeriodic->Start(1000, 1000 * 1000, TCallBack(Periodic, this));

	iDownLoadedArray = new (ELeave) CArrayFixFlat<TURLTEXT>(10);

	iTimer.Begin (60000);

	iAudio=CAudioPlayerEngine::NewL();
	iAudio->SetUpdateObserver(this);
	iMetaData=new(ELeave) RMetaDataArray();

	//获取提示音文件名
	iMusicFile.Zero ();
	iMusicFile.Append (iMainEngine.GetAppPath());
	iMusicFile.Append (KMUSICFILE);

	//初始化切片的参数
	iMaxFileSize=2048*1024;
	iPerUnitSize=1024*1024;
	iSurplusUnitSize=1500*1024;

	GetInfoFromLog();
}

CDownLoadEngine::~CDownLoadEngine()
{
	delete iDownLoadedArray ;

	delete[] iDownLoadingTaskList;
	delete[] iDownLoadThread;

	delete iPeriodic;

	iRfs.Close();
}

void CDownLoadEngine::InitDownLoadingList()
{
	int i=0;
	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		iDownLoadingTaskList[i].nStatus =0;
		iDownLoadingTaskList[i].nType  =0;
		iDownLoadingTaskList[i].nDownLoadLen =0;
		iDownLoadingTaskList[i].nDownLoadURL.Zero ();
		iDownLoadingTaskList[i].nFileLen =0;
		iDownLoadingTaskList[i].nFileName.Zero ();
		iDownLoadingTaskList[i].nTmpFilePath.Zero ();
		iDownLoadingTaskList[i].nTaskID =-1;
		iDownLoadingTaskList[i].nThreadID =-1;
		iDownLoadingTaskList[i].nIsContinueDownload =0;

		iDownLoadingTaskList[i].nIsMultiThread=false; 
		iDownLoadingTaskList[i].nThreadNum =0;
		iDownLoadingTaskList[i].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[i].nUsedThreadNum=0;

		iDownLoadingTaskList[i].nDownLoadResponseURL.Zero();
	}
}

void CDownLoadEngine::InitDownLoadThreadList()
{
	int i;
	for(i=0;i<DOWNLOADTHREAD_NUM;i++)
	{
		iDownLoadThread[i].nStatus =0;
		iDownLoadThread[i].nDownLoadURL.Zero ();
		iDownLoadThread[i].nFileName.Zero ();
		iDownLoadThread[i].nTaskID =-1;
		iDownLoadThread[i].nDownLoadClass =new (ELeave) CDownLoadNet(iMainEngine);
		iDownLoadThread[i].nDownLoadClass->Activate(); 
		iDownLoadThread[i].nDownLoadClass->SetState (this);

		iDownLoadThread[i].nDownLoadClass->SetHttpIndex(i+100);
	}
}

void CDownLoadEngine::TimeSlice()
{
	iMainEngine.WriteLog16(_L("TimeSlice-1"));
	if(iTimer.Check())
	{
		iTimeCount++;

		CheckDownLoadTaskList();

		iTimer.Begin (2000);

		if(iTimeCount>=3)
		{
			//保存到下载日志
			WriteDownLoadLog();

			iTimeCount=0;
		}
	}
	iMainEngine.WriteLog16(_L("TimeSlice-2"));
}

TInt CDownLoadEngine::AddToDownLoadingList(const TDesC& aUrl, const TDesC& aName,const TInt aFileLen,const TInt aDownLoadLen,const TInt aType,const TInt aSave)
{
	//aType:0-普通增加（需要验证是否重复下载）；1-不需要进行重复验证的增加
	//aSave:0-不保存到日志文件；1-保存到日志文件
	//返回值：0-添加成功；1-队列已满；2-重复下载；3-正在下载；4-磁盘空间不够;5-文件不被软件支持
	TInt i=0;
	TBool nIsOK=false;
	TInt nIndex=0;
	TInt nFileLen;

	TBuf<255> nFileName;
	TBuf<255> nURLFileName;

	iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-1"));
	//首先判断空间是否足够
	if(aFileLen>0)
	{
		//ASSERT(EFalse);	//此处代码有问题
		TBuf<1> nDriver;
		nDriver.Copy ( iMainEngine.GetDownLoadDir().Left(1));

		TInt64 nDriverSize=::GetFreeSpace (nDriver);

		if((TInt64)aFileLen>nDriverSize)
		{
			iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-2"));
			return 4;
		}
	}

	nFileLen=0;

	//判断是否重复下载
	if(aType==0)
	{
		if(CheckIsInHistoryArray(aUrl,aName,nIndex)==1)
		{
			iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-3"));
			return 2;
		}
	}

	nFileName.Zero ();
	if(aName.Length ()==0)
	{
		//如果没有文件名称则从URL中获取文件名
		nURLFileName.Zero ();

		GetFileNameFromURL(aUrl,nURLFileName);

		nFileName.Append (CheckFileName(nURLFileName));
	}
	else
	{
		nFileName.Append (CheckFileName(aName));
	}

	//判断文件是否可以打开
	if(aType==0)
	{
		if(CheckFileIsSupport(nFileName)==0)
		{
			iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-4"));
			return 5;
		}
	}

	//如果文件名称为空则使用默认值Unkown
	if(nFileName.Length ()==0)
	{
		nFileName.Append (CheckFileName(_L("UnKown")));
	}

	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if (iDownLoadingTaskList[i].nDownLoadURL.Compare(aUrl)==0 )
		{
			iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-5"));
			return 3;
		}

		if (iDownLoadingTaskList[i].nStatus == 0)
		{
			iDownLoadingTaskID++;

			iDownLoadingTaskList[i].nTaskID =iDownLoadingTaskID;
			iDownLoadingTaskList[i].nDownLoadURL =aUrl;
			iDownLoadingTaskList[i].nFileName =nFileName;
			iDownLoadingTaskList[i].nStatus =1;
			iDownLoadingTaskList[i].nFileLen =nFileLen;
			iDownLoadingTaskList[i].nDownLoadLen =aDownLoadLen;

			iDownLoadingTaskNum++;
			nIsOK=true;

			if(nFileLen>0)
			{
				//生成下载临时文件
				CreateDownLoadFile(i);				
			}

			//通知界面显示
			if (iDownLoadNoticeWindow)
			{
				TDownLoadShowInfo* nTaskInfo;
				nTaskInfo->nTaskID=iDownLoadingTaskID;
				nTaskInfo->nFileName=nFileName;
				nTaskInfo->nStatus=1;
				nTaskInfo->nFileLen=nFileLen;
				nTaskInfo->nDownLoadLen=aDownLoadLen;

				iDownLoadNoticeWindow->AddDownLoadTask(nTaskInfo);

				delete nTaskInfo;
			}
			

			CheckDownLoadTaskList();

			break;
		}
	}

	if (nIsOK==true)
	{
		if(aSave==1)
		{
			//保存到下载日志
			//WriteDownLoadLog();

			AddToDownLoadedHistoryArray(aUrl);
		}
		iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-6"));
		return 0;
	}
	else
	{
		iMainEngine.WriteLog16(_L("AddToDownLoadingList-1-7"));
		return 1;
	}
}

void CDownLoadEngine::AddToDownLoadingList(TDesC& aUrl, 
														TDesC& aName,
														TInt aFileLen,
														TInt aDownLoadLen,
														TDesC& aFileTempName,
														TInt aIsMulti,
														TInt aThreadNum,
														TDesC& aMultiInfo)
{
	//内部添加（从日志文件中获取）
	TInt i=0;
	
	iMainEngine.WriteLog16(_L("AddToDownLoadingList-2-1"));

	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if (iDownLoadingTaskList[i].nStatus == 0)
		{
			iDownLoadingTaskID++;

			iDownLoadingTaskList[i].nTaskID =iDownLoadingTaskID;
			iDownLoadingTaskList[i].nDownLoadURL =aUrl;
			iDownLoadingTaskList[i].nFileName =aName;
			iDownLoadingTaskList[i].nStatus =1;
			iDownLoadingTaskList[i].nFileLen =aFileLen;
			iDownLoadingTaskList[i].nDownLoadLen =aDownLoadLen;

			iDownLoadingTaskList[i].nTmpFilePath =aFileTempName;
			iDownLoadingTaskList[i].nMultiThreadInfo= aMultiInfo;
			iDownLoadingTaskList[i].nThreadNum= aThreadNum;

			if(aIsMulti==0)
			{
				iDownLoadingTaskList[i].nIsMultiThread=false; 
			}
			else
			{
				iDownLoadingTaskList[i].nIsMultiThread =true;
			}

			iDownLoadingTaskNum++;

			CheckDownLoadTaskList();

			break;
		}
	}
	iMainEngine.WriteLog16(_L("AddToDownLoadingList-2-2"));
}

TInt CDownLoadEngine::AddToDownLoadingList(const TDesC& aUrl,const TDesC& aName)
{
	//用于增加升级下载
	//返回值：0-添加成功；1-队列已满；2-重复下载；3-正在下载
	TInt i=0;
	TBool nIsOK=false;
	TInt nIndex=0;

	TBuf<255> nFileName;
	TBuf<255> nURLFileName;

	iMainEngine.WriteLog16(_L("AddToDownLoadingList-3-1"));
	//判断是否重复下载
	if(CheckIsInHistoryArray(aUrl,aName,nIndex)==1)
	{
		iMainEngine.WriteLog16(_L("AddToDownLoadingList-3-2"));
		return 2;
	}

	nFileName.Zero ();
	if(aName.Length ()==0)
	{
		//如果没有文件名称则从URL中获取文件名
		nURLFileName.Zero ();

		GetFileNameFromURL(aUrl,nURLFileName);

		nFileName.Append (CheckFileName(nURLFileName));
	}
	else
	{
		nFileName.Append (CheckFileName(aName));
	}


	//如果文件名称为空则使用默认值Unkown
	if(nFileName.Length ()==0)
	{
		nFileName.Append (CheckFileName(_L("UnKown")));
	}

	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if (iDownLoadingTaskList[i].nDownLoadURL.Compare(aUrl)==0 )
		{
			iMainEngine.WriteLog16(_L("AddToDownLoadingList-3-3"));
			return 3;
		}

		if (iDownLoadingTaskList[i].nStatus == 0)
		{
			iDownLoadingTaskID++;

			iDownLoadingTaskList[i].nTaskID =iDownLoadingTaskID;
			iDownLoadingTaskList[i].nDownLoadURL =aUrl;
			iDownLoadingTaskList[i].nFileName =nFileName;
			iDownLoadingTaskList[i].nStatus =1;
			iDownLoadingTaskList[i].nType  =1;
			iDownLoadingTaskList[i].nFileLen =0;
			iDownLoadingTaskList[i].nDownLoadLen =0;

			iDownLoadingTaskNum++;
			nIsOK=true;

			//通知界面显示
			if (iDownLoadNoticeWindow)
			{
				TDownLoadShowInfo* nTaskInfo;
				nTaskInfo->nTaskID=iDownLoadingTaskID;
				nTaskInfo->nFileName=nFileName;
				nTaskInfo->nStatus=1;
				nTaskInfo->nFileLen=0;
				nTaskInfo->nDownLoadLen=0;

				iDownLoadNoticeWindow->AddDownLoadTask(nTaskInfo);

				delete nTaskInfo;
			}

			CheckDownLoadTaskList();

			break;
		}
	}

	if (nIsOK==true)
	{
		//保存到下载日志
		WriteDownLoadLog();

		AddToDownLoadedHistoryArray(aUrl);
		
		iMainEngine.WriteLog16(_L("AddToDownLoadingList-3-4"));
		return 0;
	}
	else
	{
		iMainEngine.WriteLog16(_L("AddToDownLoadingList-3-5"));
		return 1;
	}
}
TInt CDownLoadEngine::AddToDownLoadedList(TInt aTaskID)
{
	TInt i=0;
	TBool nIsOK=false;
	TInt nArrayID=-1;
	
	iMainEngine.WriteLog16(_L("AddToDownLoadedList-1"));
	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if(iDownLoadingTaskList[i].nTaskID==aTaskID)
		{
			nArrayID=i;
			break;
		}
	}

	if(nArrayID==-1)
	{
		iMainEngine.WriteLog16(_L("AddToDownLoadedList-2"));
		return 0;
	}

	iMainEngine.GetDownLoadHistoryEngine().AddToDownLoadHistoryList(iDownLoadingTaskList[nArrayID].nDownLoadURL,
																	iDownLoadingTaskList[nArrayID].nFileName,
																	iDownLoadingTaskList[nArrayID].nFileLen);

	nIsOK=true;

	if (nIsOK==true)
	{
		NoticeUserDownLoaded(nArrayID);

		//发送下载成功状态
		if(iDownLoadingTaskList[nArrayID].nDownLoadResponseURL.Length()>0)
		{
			iMainEngine.GetHttpManager()->SendGetRequest(*this,*(UtilityTools::ConvertToHBufC8L(iDownLoadingTaskList[nArrayID].nDownLoadResponseURL)));
		}
		iMainEngine.WriteLog16(_L("AddToDownLoadedList-3"));
		return 0;
	}
	else
	{
		iMainEngine.WriteLog16(_L("AddToDownLoadedList-41"));
		return 1;
	}
}

TInt CDownLoadEngine::DelFromDownLoadingList(TInt aTaskID)
{
	//从正在下载列表中删除列表信息
	TInt i=0;
	TInt nArrayID=-1;
	
	iMainEngine.WriteLog16(_L("DelFromDownLoadingList-1"));

	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if(iDownLoadingTaskList[i].nTaskID==aTaskID)
		{
			nArrayID=i;
			break;
		}
	}

	if(nArrayID==-1)
	{
		iMainEngine.WriteLog16(_L("DelFromDownLoadingList-2"));
		return 0;
	}

	iDownLoadingTaskNum--;

	if(nArrayID==(DOWNLOADING_NUM-1))
	{
		iDownLoadingTaskList[nArrayID].nTaskID =-1;
		iDownLoadingTaskList[nArrayID].nType  =0;
		iDownLoadingTaskList[nArrayID].nFileLen =0;
		iDownLoadingTaskList[nArrayID].nDownLoadLen =0;
		iDownLoadingTaskList[nArrayID].nStatus =0;
		iDownLoadingTaskList[nArrayID].nThreadID =-1;
		iDownLoadingTaskList[nArrayID].nTmpFilePath.Zero (); 

		iDownLoadingTaskList[nArrayID].nIsMultiThread=false; 
		iDownLoadingTaskList[nArrayID].nThreadNum =0;
		iDownLoadingTaskList[nArrayID].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[nArrayID].nUsedThreadNum=0;

		iDownLoadingTaskList[nArrayID].nDownLoadResponseURL.Zero();

		iMainEngine.WriteLog16(_L("DelFromDownLoadingList-3"));
		return 1;
	}

	for(i=nArrayID;i<(DOWNLOADING_NUM-1);i++)
	{
		iDownLoadingTaskList[i]=iDownLoadingTaskList[i+1];

		if(iDownLoadingTaskList[i+1].nStatus ==0)
			break;
	}

	if(i==(DOWNLOADING_NUM-1))
	{
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nTaskID =-1;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nType  =0;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nFileLen =0;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nDownLoadLen =0;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nStatus =0;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nThreadID =-1;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nTmpFilePath.Zero (); 

		iDownLoadingTaskList[DOWNLOADING_NUM-1].nIsMultiThread=false;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nThreadNum =0;
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[DOWNLOADING_NUM-1].nUsedThreadNum=0;

		iDownLoadingTaskList[DOWNLOADING_NUM-1].nDownLoadResponseURL.Zero();
	}
	
	iMainEngine.WriteLog16(_L("DelFromDownLoadingList-4"));
	return 1;
}

void CDownLoadEngine::CheckDownLoadTaskList()
{
	TInt i=0;

	iMainEngine.WriteLog16(_L("CheckDownLoadTaskList-1"));
	for(i=0;i<DOWNLOADING_NUM;i++)
	{
		if (iDownLoadingTaskList[i].nStatus==0 )	
			break;

		if (iDownLoadingTaskList[i].nStatus==1)
		{
			iMainEngine.WriteLog16(_L("CheckDownLoadTaskList-2"));
			TInt nRet=AddTaskToDownLoadThread(i);
			iMainEngine.WriteLog16(_L("CheckDownLoadTaskList-3"));
			if (nRet==0)
			{
				break;
			}
			else if(nRet==1)
			{
				iDownLoadingTaskList[i].nStatus=2;

				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
				}

				break;
			}
			else if(nRet==2)
			{
				iDownLoadingTaskList[i].nStatus=3;

				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
				}
			}
		}

		if(iDownLoadingTaskList[i].nStatus==3)
		{
			//多线程下载
			if(iDownLoadingTaskList[i].nUsedThreadNum <iDownLoadingTaskList[i].nThreadNum 
				&& iDownLoadingTaskList[i].nUsedThreadNum<iMainEngine.GetDownLoadMaxThreadNum() )
			{
				if(AddTaskToDownLoadThread(i)==2)
					i--;
			}
		}
	}
	iMainEngine.WriteLog16(_L("CheckDownLoadTaskList-4"));
}

TInt CDownLoadEngine::AddTaskToDownLoadThread(TInt aListIndex)
{
	//给任务分配线程
	TInt i=0;
	TBool nIsOK=false;

	iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-1"));

	for(i=0;i<DOWNLOADTHREAD_NUM;i++)
	{	
		if (iDownLoadThread[i].nStatus==0 )	
		{
			if(iDownLoadingTaskList[aListIndex].nFileLen >0)
			{
				//是否已生成临时文件
				iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-2"));
				if(iDownLoadingTaskList[aListIndex].nTmpFilePath.Length() ==0)
				{
					iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-3"));
					TInt i=CreateDownLoadFile(aListIndex);
					iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-3"));
					if(i!=0 && i!=4)
					{
						nIsOK=false;
						break;
					}
				}
				
				iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-4"));
				//是否已经进行切片处理
				if(iDownLoadingTaskList[aListIndex].nThreadNum==0)
				{
					SegmentationFile(aListIndex);
				}

				if(iDownLoadingTaskList[aListIndex].nIsMultiThread==false )
				{
					//普通单线程下载
					iDownLoadThread[i].nDownLoadClass->SetUrl (iDownLoadingTaskList[aListIndex].nTaskID,i,iDownLoadingTaskList[aListIndex].nDownLoadURL,iDownLoadingTaskList[aListIndex].nTmpFilePath ,iDownLoadingTaskList[aListIndex].nDownLoadLen,0 );

					iDownLoadThread[i].nTaskID =iDownLoadingTaskList[aListIndex].nTaskID;
					iDownLoadThread[i].nStatus =1;
					iDownLoadThread[i].nFileName =iDownLoadingTaskList[aListIndex].nFileName;
					iDownLoadThread[i].nDownLoadURL =iDownLoadingTaskList[aListIndex].nDownLoadURL;

					iDownLoadingTaskList[aListIndex].nThreadID =i;

					nIsOK=true;
					break;
				}
				else
				{
					//多线程下载
					TInt nBeginPos=0;
					TInt nEndPos=0;
					TInt nNum=0;

					TInt nRet=SetMultiThread(aListIndex,i,nBeginPos,nEndPos,nNum);
					//::WriteLogsL(_L("AddTaskToDownLoadThread-1")); 
					if(nRet==0)
					{
						//::WriteLogsL(_L("AddTaskToDownLoadThread-2")); 
						iDownLoadThread[i].nDownLoadClass->SetUrl (iDownLoadingTaskList[aListIndex].nTaskID,
							i,
							iDownLoadingTaskList[aListIndex].nDownLoadURL,
							iDownLoadingTaskList[aListIndex].nTmpFilePath ,
							nBeginPos,
							nEndPos);
						//::WriteLogsL(_L("AddTaskToDownLoadThread-3")); 
						iDownLoadThread[i].nTaskID =iDownLoadingTaskList[aListIndex].nTaskID;
						iDownLoadThread[i].nStatus =1;
						iDownLoadThread[i].nFileName =iDownLoadingTaskList[aListIndex].nFileName;
						iDownLoadThread[i].nDownLoadURL =iDownLoadingTaskList[aListIndex].nDownLoadURL;
						iDownLoadThread[i].nMultiUnti=nNum; 

						iDownLoadingTaskList[aListIndex].nThreadID =i;
						
						iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-5"));
						return 2;
					}
					else
					{
						nIsOK=false;
						break;
					}
				}
			}
			else
			{
				////iMainEngine.WriteLog16(iDownLoadingTaskList[aListIndex].nDownLoadURL);

				if (iDownLoadingTaskList[aListIndex].nDownLoadURL.Find(KDOWNLOADURL)==KErrNotFound)
				{
					iDownLoadThread[i].nDownLoadClass->GetDownLoadFileSize(iDownLoadingTaskList[aListIndex].nTaskID,i,iDownLoadingTaskList[aListIndex].nDownLoadURL);
				} 
				else
				{
					//新接口不需要采用Head消息获取文件大小
					iDownLoadThread[i].nDownLoadClass->SetUrl (iDownLoadingTaskList[aListIndex].nTaskID,i,iDownLoadingTaskList[aListIndex].nDownLoadURL,iDownLoadingTaskList[aListIndex].nTmpFilePath ,iDownLoadingTaskList[aListIndex].nDownLoadLen,0 );	
				}

				iDownLoadThread[i].nTaskID =iDownLoadingTaskList[aListIndex].nTaskID;
				iDownLoadThread[i].nStatus =1;
				iDownLoadThread[i].nFileName =iDownLoadingTaskList[aListIndex].nFileName;
				iDownLoadThread[i].nDownLoadURL =iDownLoadingTaskList[aListIndex].nDownLoadURL;

				iDownLoadingTaskList[aListIndex].nThreadID =i;

				nIsOK=true;
				break;
			}
		}
	}

	iMainEngine.WriteLog16(_L("AddTaskToDownLoadThread-6"));
	if(nIsOK)
		return 1;
	else
		return 0;
}

void CDownLoadEngine::DownLoadEvent(TInt aTaskID,TInt aThreadID,TInt aEventType)
{
	//aEventType:0-中止下载；1-下载完成；3-下载链接有误；4-文件生成失败终止下载；5-下载临时文件有误，重新下载
	TInt i=0;
	
	iMainEngine.WriteLog16(_L("DownLoadEvent-1"));

	if(aThreadID !=-1)
	{
		if(aEventType==0 || aEventType==2 || aEventType==3)
		{
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID )
				{
					if(aEventType==3)
					{
						iDownLoadingTaskList[i].nStatus=5;
						iDownLoadingTaskList[i].nMultiThreadInfo.Zero ();
						iDownLoadingTaskList[i].nThreadNum =0;
						iDownLoadingTaskList[i].nUsedThreadNum=0; 
						iDownLoadingTaskList[i].nThreadID=-1;

						//错误链接提示通知
						NoticeUserErrURL(i);
					}
					else
					{						
						iDownLoadingTaskList[i].nStatus=4;
						iDownLoadingTaskList[i].nThreadID=-1;

						if(iDownLoadingTaskList[i].nIsMultiThread==true)
						{
							//如果是多线程的需要更新状态
							UpdateMultiThread(i,iDownLoadThread[aThreadID].nMultiUnti,0,2);
						}
					}

					//通知显示界面状态更改
					if (iDownLoadNoticeWindow)
					{
						iDownLoadNoticeWindow->DownLoadEvent(aTaskID,iDownLoadingTaskList[i].nStatus);
					}

					break;
				}
			}
		}

		if(aEventType==1)
		{
			//::WriteLogsL(_L("DownLoadEvent-1")); 
			//下载完成
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID)
				{
					//::WriteLogsL(_L("DownLoadEvent-2")); 
					if(iDownLoadingTaskList[i].nIsMultiThread==true )
					{
						//::WriteLogsL(_L("DownLoadEvent-2-1")); 
						//如果是多线程,需要更新状态
						UpdateMultiThread(i,iDownLoadThread[aThreadID].nMultiUnti,0,1); 
						//::WriteLogsL(_L("DownLoadEvent-2-2")); 
					}
					else
					{
						//::WriteLogsL(_L("DownLoadEvent-2-3")); 
						iDownLoadingTaskList[i].nThreadNum--; 
					}

					//::WriteLogsL(_L("DownLoadEvent-3")); 
					if(iDownLoadingTaskList[i].nThreadNum==0 )
					{
						//::WriteLogsL(_L("DownLoadEvent-4")); 
						RFile	oFile;
						if (KErrNone == oFile.Open(iRfs, iDownLoadingTaskList[i].nTmpFilePath , EFileWrite )) 
						{
							//::WriteLogsL(_L("DownLoadFileReName-Begin")); 
							oFile.Rename(iDownLoadingTaskList[i].nFileName );
							oFile.Close();
							//::WriteLogsL(_L("DownLoadFileReName-End")); 
						}
						//::WriteLogsL(_L("DownLoadEvent-5")); 
						AddToDownLoadedList(aTaskID);
						//::WriteLogsL(_L("DownLoadEvent-6")); 
						DelFromDownLoadingList(aTaskID);
						//::WriteLogsL(_L("DownLoadEvent-7")); 

						//通知显示界面状态更改
						if (iDownLoadNoticeWindow)
						{
							iDownLoadNoticeWindow->DownLoadEvent(aTaskID,0);
						}
					}
					break;
				}
			}

			iDownLoadThread[aThreadID].nStatus =0;
			iDownLoadThread[aThreadID].nMultiUnti=0; 
		}

		if(aEventType==5)
		{
			//下载临时文件有误，重新下载
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID)
				{
					if(iDownLoadingTaskList[i].nIsMultiThread ==false)
					{
						iDownLoadingTaskList[i].nDownLoadLen =0;
						iDownLoadingTaskList[i].nTmpFilePath.Zero();

						//重新生成临时下载文件
						TInt nRet=CreateDownLoadFile(i);

						if(nRet==0 || nRet==4)
						{
							iDownLoadingTaskList[i].nStatus =1;
						}
						else
						{
							iDownLoadingTaskList[i].nStatus =4;
						}
						
						//通知显示界面状态更改
						if (iDownLoadNoticeWindow)
						{
							iDownLoadNoticeWindow->DownLoadEvent(aTaskID,iDownLoadingTaskList[i].nStatus);
						}

						break;
					}
					else
					{
						//多线程下载
						//如果是多线程的需要更新状态
						UpdateMultiThread(i,iDownLoadThread[aThreadID].nMultiUnti,0,2);

						TInt nRet=CreateDownLoadFile(i);

						if(nRet==0)
						{
							iDownLoadingTaskList[i].nDownLoadLen =0;
							iDownLoadingTaskList[i].nStatus =1;
							iDownLoadingTaskList[i].nThreadNum =0;
						}
						else if(nRet==4)
						{
							//已经通过其他线程生成则不做任何操作
						}
						else
						{
							//生成不成功
							iDownLoadingTaskList[i].nDownLoadLen =0;
							iDownLoadingTaskList[i].nTmpFilePath.Zero();
							iDownLoadingTaskList[i].nThreadNum =0;
							iDownLoadingTaskList[i].nStatus =4;
						}

						//通知显示界面状态更改
						if (iDownLoadNoticeWindow)
						{
							iDownLoadNoticeWindow->DownLoadEvent(aTaskID,iDownLoadingTaskList[i].nStatus);
						}
					}
				}
			}
		}

		if(iDownLoadThread[aThreadID].nTaskID==aTaskID)
		{
			iDownLoadThread[aThreadID].nStatus=0;
			iDownLoadThread[aThreadID].nDownLoadURL.Zero ();
			iDownLoadThread[aThreadID].nFileName.Zero ();
			iDownLoadThread[aThreadID].nTaskID =-1;
			iDownLoadThread[aThreadID].nMultiUnti=0; 
		}
	}

	//保存到下载日志
	WriteDownLoadLog();

	iMainEngine.WriteLog16(_L("DownLoadEvent-2"));
}

void CDownLoadEngine::DownLoadReceived(TInt aTaskID,TInt aThreadID,TInt aTotalLen, TInt aReceivedLen)
{
	if(aTaskID==-1)
	{
		return;
	}
	
	iMainEngine.WriteLog16(_L("DownLoadReceived-1"));
	
	for(int i=0;i<DOWNLOADING_NUM;i++)
	{
		if(iDownLoadingTaskList[i].nTaskID==aTaskID)
		{
			if(iDownLoadingTaskList[i].nFileLen==0)
			{
				//获取文件大小成功
				iDownLoadingTaskList[i].nFileLen =aTotalLen;

				//释放下载线程
				iDownLoadThread[aThreadID].nStatus=0;
				iDownLoadThread[aThreadID].nDownLoadURL.Zero ();
				iDownLoadThread[aThreadID].nFileName.Zero ();
				iDownLoadThread[aThreadID].nTaskID =-1;

				//将下载队列状态更改为等待下载
				iDownLoadingTaskList[i].nStatus =1;

				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadReceived(aTaskID,aTotalLen,0);
					iDownLoadNoticeWindow->DownLoadEvent(aTaskID,iDownLoadingTaskList[i].nStatus);
				}

				CheckDownLoadTaskList();
			}
			else
			{
				//::WriteLogsL(_L("DownLoadReceived-2")); 
				iDownLoadingTaskList[i].nDownLoadLen =iDownLoadingTaskList[i].nDownLoadLen+aReceivedLen;

				if(iDownLoadingTaskList[i].nIsMultiThread==true )
				{
					//如果是多线程,需要更新状态
					UpdateMultiThread(i,iDownLoadThread[aThreadID].nMultiUnti,aReceivedLen,0); 
				}

				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadReceived(aTaskID,iDownLoadingTaskList[i].nFileLen,iDownLoadingTaskList[i].nDownLoadLen);
				}
				
				//::WriteLogsL(_L("DownLoadReceived-3")); 
			}
			break;
		}
	}

	iMainEngine.WriteLog16(_L("DownLoadReceived-2"));
}

void CDownLoadEngine::DownLoadReceived(TInt aTaskID,
									   TInt aThreadID,
									   TInt aFileSize, 
									   TDesC& aFileName,
									   TDesC& aDownLoadURL,
									   TDesC& aDownLoadResponseURL)
{
	if(aTaskID==-1)
	{
		return;
	}

	iMainEngine.WriteLog16(_L("Begin"));
	//iMainEngine.WriteLog16(aFileName);
	//iMainEngine.WriteLog16(aDownLoadURL);
	//iMainEngine.WriteLog16(aDownLoadResponseURL);

	TBuf<20> nSize;
	nSize.Zero();
	nSize.AppendNum(aFileSize);

	//iMainEngine.WriteLog16(nSize);

	for(int i=0;i<DOWNLOADING_NUM;i++)
	{
		if(iDownLoadingTaskList[i].nTaskID==aTaskID)
		{
			if(iDownLoadingTaskList[i].nFileLen==0)
			{
				//获取文件大小成功
				iMainEngine.WriteLog16(_L("DownLoadReceived-1-1"));
				iDownLoadingTaskList[i].nFileLen =aFileSize;
				iDownLoadingTaskList[i].nDownLoadResponseURL.Zero();
				iDownLoadingTaskList[i].nDownLoadResponseURL.Append(aDownLoadResponseURL);
				iDownLoadingTaskList[i].nDownLoadURL.Zero();
				iDownLoadingTaskList[i].nDownLoadURL.Append(aDownLoadURL);
				iDownLoadingTaskList[i].nFileName.Zero ();
				iDownLoadingTaskList[i].nFileName.Append(CheckFileName(aFileName));
				
				iMainEngine.WriteLog16(_L("DownLoadReceived-1-2"));
				//释放下载线程
				iDownLoadThread[aThreadID].nStatus=0;
				iDownLoadThread[aThreadID].nDownLoadURL.Zero ();
				iDownLoadThread[aThreadID].nFileName.Zero ();
				iDownLoadThread[aThreadID].nTaskID =-1;

				//将下载队列状态更改为等待下载
				iDownLoadingTaskList[i].nStatus =1;
				
				iMainEngine.WriteLog16(_L("DownLoadReceived-1-3"));
				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadReceived(aTaskID,aFileSize,0);
					iDownLoadNoticeWindow->DownLoadEvent(aTaskID,iDownLoadingTaskList[i].nStatus);
				}
				iMainEngine.WriteLog16(_L("DownLoadReceived-1-4"));
				CheckDownLoadTaskList();
				iMainEngine.WriteLog16(_L("DownLoadReceived-1-5"));
			}
			break;
		}
	}

	iMainEngine.WriteLog16(_L("DownLoadReceived-1-6"));
}

void CDownLoadEngine::NoticeUserErrURL(TInt aTaskArrayID)
{
	//提示用户下载链接URL错误
	iMainEngine.WriteLog16(_L("NoticeUserErrURL-1"));

	TBuf<200> nName;
	nName.Zero ();
	nName.Append (iDownLoadingTaskList[aTaskArrayID].nFileName );

	//20070601张晖更改，增加下载链接错误提示
	if(iDownLoadingTaskList[aTaskArrayID].nStatus==5 )
	{
		nName.Append (iMainEngine.GetDesById(ETurkeyTextRes_DownLoadStatus_ErrURLInfo));

		iControlFactory.CreateTipDialog(nName,*this,ECmdNull);
	}

	iMainEngine.WriteLog16(_L("NoticeUserErrURL-2"));
}
void CDownLoadEngine::NoticeUserNoFreeDisk(TInt aTaskArrayID)
{
	//提示用户磁盘空间不足
	iMainEngine.WriteLog16(_L("NoticeUserNoFreeDisk-1"));

	TBuf<200> nName;
	nName.Zero ();
	nName.Append (iMainEngine.GetDesById(ETurkeyTextRes_MainDown ));
	nName.Append (iDownLoadingTaskList[aTaskArrayID].nFileName );

	if(iDownLoadingTaskList[aTaskArrayID].nStatus==4 )
	{
		nName.Append (iMainEngine.GetDesById(ETurkeyTextRes_DownLoadFail_NoDiskSpace));
		
		iControlFactory.CreateTipDialog(nName,*this,ECmdNull);
	}

	iMainEngine.WriteLog16(_L("NoticeUserNoFreeDisk-2"));
}

void CDownLoadEngine::NoticeUserDownLoaded(TInt aTaskArrayID)
{
	iMainEngine.WriteLog16(_L("NoticeUserDownLoaded-1"));
	//通知用户下载完成
	if(iIsPlayMusic==false && iMainEngine.HaveDownLoadVoice())
	{
		//播放提示音
		iAudio->OpenFileL(iMusicFile,iMetaData);

		iIsPlayMusic=true;
	}

	TBuf<200> nName;
	nName.Zero ();
	if(iDownLoadingTaskList[aTaskArrayID].nType ==0)
	{
		nName.Append (iDownLoadingTaskList[aTaskArrayID].nFileName );
		nName.Append (iMainEngine.GetDesById(ETurkeyTextRes_DownLoadFinishNotice));
	}
	else if(iDownLoadingTaskList[aTaskArrayID].nType ==1)
	{
		nName.Append (iMainEngine.GetDesById(ETurkeyTextRes_DownLoadUpdateFileSuccess));
	}

	//当前界面为音乐播放、视频播放、电子书、图片时，提示界面在父界面显示
	if (iMainEngine.CurWindow()->GetWindowType()==EOpenFileWindow_Pic
		|| iMainEngine.CurWindow()->GetWindowType()==EOpenFileWindow_Book
		|| iMainEngine.CurWindow()->GetWindowType()==EOpenFileWindow_Music
		|| iMainEngine.CurWindow()->GetWindowType()==EOpenFileWindow_Video)
	{
		if (iDialog!=NULL)
		{
			iMainEngine.CurWindow()->GetParentWindow()->CleanupControl(iDialog);
			iDialog=NULL;
		} 

		iDialog=iControlFactory.CreateInquireDialog(nName,*this,ECmdOpenFile,ECmdNull,iMainEngine.CurWindow()->GetParentWindow());
	} 
	else
	{
		if (iDialog!=NULL)
		{
			iMainEngine.CleanupControl(iDialog);
			iDialog=NULL;
		} 

		iDialog=iControlFactory.CreateInquireDialog(nName,*this,ECmdOpenFile,ECmdNull);
	}

	((CDialog*)iDialog)->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_DownLoad_OpenFile));
	((CDialog*)iDialog)->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
	((CDialog*)iDialog)->SetIconType(EDialogSuccess);

	iDownLoadSuccessPath.Zero ();
	iDownLoadSuccessPath.Append (iDownLoadingTaskList[aTaskArrayID].nTmpFilePath.Mid(0,iDownLoadingTaskList[aTaskArrayID].nTmpFilePath.Length()-2 ));

	iMainEngine.WriteLog16(_L("NoticeUserDownLoaded-2"));
}

void CDownLoadEngine::PauseDownLoadingTask(TInt aTaskID)
{
	//中止任务
	TInt i=0;

	iMainEngine.WriteLog16(_L("PauseDownLoadingTask-1"));

	if(iDownLoadingTaskNum>0)
	{
		if(aTaskID==-1)
		{
			//全部中止
			for(i=0;i<iDownLoadingTaskNum;i++)
			{
				if(iDownLoadingTaskList[i].nStatus ==0 || iDownLoadingTaskList[i].nStatus ==5)
					continue;

				if(iDownLoadingTaskList[i].nStatus==2 || iDownLoadingTaskList[i].nStatus==3 )
				{
					StopDownLoadInThread(iDownLoadingTaskList[i].nTaskID,0 );
				}
				//else if(iDownLoadingTaskList[i].nStatus==1)
				//{
					iDownLoadingTaskList[i].nStatus=4;
				//}

				//通知显示界面状态更改
				if (iDownLoadNoticeWindow)
				{
					iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
				}
			}
		}
		else
		{			
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID )
				{
					if(iDownLoadingTaskList[i].nStatus ==0 || iDownLoadingTaskList[i].nStatus ==5)
						break;

					if(iDownLoadingTaskList[i].nStatus==2 || iDownLoadingTaskList[i].nStatus==3 )
					{
						StopDownLoadInThread(iDownLoadingTaskList[i].nTaskID,0 );
					}
					//else if(iDownLoadingTaskList[i].nStatus==1)
					//{
						iDownLoadingTaskList[i].nStatus=4;
					//}

					//通知显示界面状态更改
					if (iDownLoadNoticeWindow)
					{
						iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
					}

					break;
				}
			}
		}
	}

	//保存到下载日志
	WriteDownLoadLog();

	iMainEngine.WriteLog16(_L("PauseDownLoadingTask-2"));
}

void CDownLoadEngine::StopDownLoadInThread(TInt aTaskID,TInt IsDelTmpFile)
{
	//停止下载线程
	TInt i=0;
	
	iMainEngine.WriteLog16(_L("StopDownLoadInThread-1"));

	if(aTaskID==-1)
	{
		//全部停止
		for(i=0;i<DOWNLOADTHREAD_NUM;i++)
		{
			if(iDownLoadThread[i].nStatus !=0)
			{
				iDownLoadThread[i].nDownLoadClass->CancelDownLoad (IsDelTmpFile);
				iDownLoadThread[i].nStatus =0;
				iDownLoadThread[i].nDownLoadURL.Zero ();
				iDownLoadThread[i].nFileName.Zero (); 
				iDownLoadThread[i].nMultiUnti=0;
			}
		}
	}
	else
	{
		for(i=0;i<DOWNLOADTHREAD_NUM;i++)
		{
			if(iDownLoadThread[i].nTaskID ==aTaskID && iDownLoadThread[i].nStatus !=0)
			{
				iDownLoadThread[i].nDownLoadClass->CancelDownLoad (IsDelTmpFile);
				iDownLoadThread[i].nStatus =0;
				iDownLoadThread[i].nDownLoadURL.Zero ();
				iDownLoadThread[i].nFileName.Zero (); 
				iDownLoadThread[i].nMultiUnti=0;
				//break;
			}
		}
	}

	iMainEngine.WriteLog16(_L("StopDownLoadInThread-2"));
}

void CDownLoadEngine::BeginDownLoadingTask(TInt aTaskID)
{
	//开始下载任务
	TInt i=0;

	iMainEngine.WriteLog16(_L("BeginDownLoadingTask-1"));

	if(iDownLoadingTaskNum>0)
	{
		if(aTaskID==-1)
		{
			//全部开始
			for(i=0;i<iDownLoadingTaskNum;i++)
			{
				if(iDownLoadingTaskList[i].nStatus ==0)
					break;

				if(iDownLoadingTaskList[i].nStatus ==4)
				{
					iDownLoadingTaskList[i].nStatus =1;

					//通知显示界面状态更改
					if (iDownLoadNoticeWindow)
					{
						iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
					}
				}
			}
		}
		else
		{
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID )
				{
					if(iDownLoadingTaskList[i].nStatus==4)
					{
						iDownLoadingTaskList[i].nStatus =1;

						//通知显示界面状态更改
						if (iDownLoadNoticeWindow)
						{
							iDownLoadNoticeWindow->DownLoadEvent(iDownLoadingTaskList[i].nTaskID,iDownLoadingTaskList[i].nStatus);
						}
					}
					break;
				}
			}
		}
	}

	iMainEngine.WriteLog16(_L("BeginDownLoadingTask-2"));
}
void CDownLoadEngine::DelDownLoadingTask(TInt aTaskID)
{
	//从正在下载任务列表中删除任务(包括中止下载线程)
	TInt i=0;

	iMainEngine.WriteLog16(_L("DelDownLoadingTask-1"));

	if(iDownLoadingTaskNum>0)
	{
		if(aTaskID==-1)
		{
			//全部删除
			//1、停止下载线程
			StopDownLoadInThread(-1,1);

			//2、删除下载临时文件
			for(i=0;i<iDownLoadingTaskNum;i++)
			{
				if(iDownLoadingTaskList[i].nStatus !=0)
				{
					TFileName szName;
					szName.Zero ();
					szName.Append (iDownLoadingTaskList[i].nFileName);

					//20070531张晖增加,从下载历史列表中删除
					DelFromDownLoadedHistoryArray(iDownLoadingTaskList[i].nDownLoadURL );
					TFileName szName1;
					szName1.Zero ();
					szName1.Append (iDownLoadingTaskList[i].nTmpFilePath );
					//szName1.Append (GetFullFileName(szName));
					//szName1.Append (_L("3G"));
					if(szName1.Length()!=0)
						::DelFile (szName1);
				}
			}

			//清空下载任务
			InitDownLoadingList();

			iDownLoadingTaskNum=0;
		}
		else
		{	
			for(i=0;i<DOWNLOADING_NUM;i++)
			{
				if(iDownLoadingTaskList[i].nTaskID==aTaskID )
				{
					if(iDownLoadingTaskList[i].nStatus ==2 || iDownLoadingTaskList[i].nStatus==3 )
					{
						StopDownLoadInThread(iDownLoadingTaskList[i].nTaskID ,1);
					}

					TFileName szNameTmp;
					szNameTmp.Zero ();
					szNameTmp.Append (iDownLoadingTaskList[i].nFileName);
					//20070531张晖增加,从下载历史列表中删除
					DelFromDownLoadedHistoryArray(iDownLoadingTaskList[i].nDownLoadURL );

					TFileName szNameTmp1;
					szNameTmp1.Zero ();

					szNameTmp1.Append (iDownLoadingTaskList[i].nTmpFilePath );
					//szNameTmp1.Append (GetFullFileName(szNameTmp));
					//szNameTmp1.Append (_L("3G"));
					if(szNameTmp1.Length ()!=0)
						::DelFile (szNameTmp1);

					DelFromDownLoadingList(iDownLoadingTaskList[i].nTaskID );

					break;
				}
			}
		}
	}

	//保存到下载日志
	WriteDownLoadLog();

	iMainEngine.WriteLog16(_L("DelDownLoadingTask-2"));
}

TFileName CDownLoadEngine::GetFullFileName(TFileName aFileName)
{			
	iMainEngine.WriteLog16(_L("GetFullFileName-1"));

	TFileName szName;

	szName.Append(aFileName);

	TFileName szText;
	szText.Append(_L("\\"));
	szText.Append(aFileName);

	TInt nFileNum = 0, nFileExNum = 0;

	::GetFileName(aFileName, nFileNum, nFileExNum);

	TBuf<16> szExName;
	szExName = aFileName.Right(nFileExNum);

	TFileName szTemp;
	szTemp.Append(aFileName);

	aFileName.Delete(0, aFileName.Length());
	aFileName.Append(iMainEngine.GetDownLoadDir());
	aFileName.Append(ParseExPath(szExName));
	aFileName.Append(szTemp);	

	iMainEngine.WriteLog16(_L("GetFullFileName-2"));
	return aFileName;
}
const TDesC&	CDownLoadEngine::ParseExPath(const TDesC& aFile)
{
	iMainEngine.WriteLog16(_L("ParseExPath-1"));

	if (NULL == aFile.Compare(KBMP) 
		||	NULL == aFile.Compare(KJPG)
		||	NULL == aFile.Compare(KGIF)
		||	NULL == aFile.Compare(KPNG)
		||	NULL == aFile.Compare(KWBMP)
		||	NULL == aFile.Compare(KTIFF)
		||	NULL == aFile.Compare(KEXIF)
		)
		return KIMAGES;
	else if (	NULL == aFile.Compare(KMP3)
		||	NULL == aFile.Compare(KWMA)
		||	NULL == aFile.Compare(KMIDI)
		||	NULL == aFile.Compare(KMID)
		||	NULL == aFile.Compare(KWAV)
		||	NULL == aFile.Compare(KAAC)
		||	NULL == aFile.Compare(KOGG)
		||	NULL == aFile.Compare(KMMF)
		||	NULL == aFile.Compare(KIMY)
		||	NULL == aFile.Compare(KAMR)
		)
		return KMUSIC;
	else if (	NULL == aFile.Compare(KSIS)
		||	NULL == aFile.Compare(KSISX)
		||	NULL == aFile.Compare(KJAR)
		||	NULL == aFile.Compare(KJAD)			
		)
		return KAPPS;
	else if (	NULL == aFile.Compare(K3GP)
		||	NULL == aFile.Compare(K3GPP)
		||	NULL == aFile.Compare(KRM)
		||	NULL == aFile.Compare(KAVI)
		||	NULL == aFile.Compare(KMP4)
		||	NULL == aFile.Compare(KMOV)
		||	NULL == aFile.Compare(KASF)
		||	NULL == aFile.Compare(KRMVB)
		||	NULL == aFile.Compare(KWMV)
		)
		return KVIDEO;
	else if (	NULL == aFile.Compare(KTXT)
		||	NULL == aFile.Compare(KUMD)
		||	NULL == aFile.Compare(KPDB)
		||	NULL == aFile.Compare(KPDF)			
		) 
		return KBOOKS;
	else if (	NULL == aFile.Compare(KTHM)
		||	NULL == aFile.Compare(KMTF)
		||	NULL == aFile.Compare(KNTH)
		||	NULL == aFile.Compare(KSDT)
		||	NULL == aFile.Compare(KHME)			
		) 
		return KTOPS;
	return KOTHER;
}

TInt	CDownLoadEngine::ParseExPathNum(const TDesC& aFile)
{
	iMainEngine.WriteLog16(_L("ParseExPathNum-1"));

	TInt nFileNum=0, nFileExNum=0;
	::GetFileName(aFile, nFileNum, nFileExNum);
	TFileName szName;
	szName = aFile.Right(nFileNum);

	TBuf<16> szExName;
	szExName = aFile.Right(nFileExNum);
	szExName.LowerCase();

	iMainEngine.WriteLog16(_L("ParseExPathNum-2"));

	if (NULL == szExName.Compare(KBMP) 
		||	NULL == szExName.Compare(KJPG)
		||	NULL == szExName.Compare(KGIF)
		||	NULL == szExName.Compare(KPNG)
		)
		return 0;
	else if (	NULL == szExName.Compare(KMP3)
		||	NULL == szExName.Compare(KMIDI)
		||	NULL == szExName.Compare(KMID)
		||	NULL == szExName.Compare(KWAV)
		)
		return 1;
	#ifdef __SERIES60_3X__
	else if (	NULL == szExName.Compare(KSISX)
	#else
	else if (	NULL == szExName.Compare(KSIS)
	#endif
		||	NULL == szExName.Compare(KJAR)
		)
		return 2;
	else if (	NULL == szExName.Compare(K3GP)
		||	NULL == szExName.Compare(K3GPP)
		)
		return 3;
	else if (	NULL == szExName.Compare(KTXT)			
		) 
		return 4;
	return 5;
}

void CDownLoadEngine::WriteDownLoadLog()
{
	_LIT(KSettingPathname,"c:\\");
	_LIT(KLogFileName,"3Gestlog.3G");

	iMainEngine.WriteLog16(_L("WriteDownLoadLog-1"));

	TFileName fn;
	fn = KSettingPathname;
	fn.Append(KLogFileName);

	RFile oFile;
	if (KErrNone != oFile.Replace(iRfs, fn, EFileWrite|EFileShareAny))
	{
		iMainEngine.WriteLog16(_L("WriteDownLoadLog-2"));
		return;
	}

	TBuf<4096>	szContent;
	TBuf8<4096>	szContent1;

	szContent.Zero ();
	szContent1.Zero ();
	for (TInt i = 0; i < iDownLoadingTaskNum; i++)
	{	
		if(iDownLoadingTaskList[i].nStatus !=5)
		{
			szContent.Append (iDownLoadingTaskList[i].nFileName );
			szContent.Append (_L(","));
			szContent.Append (iDownLoadingTaskList[i].nDownLoadURL  );
			szContent.Append (_L(","));
			szContent.AppendNum (iDownLoadingTaskList[i].nFileLen  );
			szContent.Append (_L(","));
			szContent.AppendNum (iDownLoadingTaskList[i].nDownLoadLen  );

			szContent.Append (_L(","));
			szContent.Append (iDownLoadingTaskList[i].nTmpFilePath );
			szContent.Append (_L(","));
			if(iDownLoadingTaskList[i].nIsMultiThread==true)
			{
				szContent.AppendNum(1);
			}
			else
			{
				szContent.AppendNum(0);
			}
			szContent.Append (_L(","));
			szContent.AppendNum (iDownLoadingTaskList[i].nThreadNum);
			szContent.Append (_L(","));
			szContent.Append (iDownLoadingTaskList[i].nMultiThreadInfo );
			szContent.Append (_L("*#"));
		}
	}

	if(szContent.Length ()>0)
	{
		CnvUtfConverter::ConvertFromUnicodeToUtf8(szContent1,szContent);

		//TRequestStatus iStatus;

		oFile.Write(szContent1);
	}

	oFile.Close();

	iMainEngine.WriteLog16(_L("WriteDownLoadLog-3"));
}

void CDownLoadEngine::GetInfoFromLog()
{
	_LIT(KSettingPathname,"c:\\");
	_LIT(KLogFileName,"3Gestlog.3G");

	iMainEngine.WriteLog16(_L("GetInfoFromLog-1"));

	TFileName fn;
	fn = KSettingPathname;
	fn.Append(KLogFileName);

	RFile oFile;
	if (KErrNone != oFile.Open(iRfs, fn, EFileWrite|EFileShareAny))
	{
		iMainEngine.WriteLog16(_L("GetInfoFromLog-2"));
		return;
	}

	TBuf<2048>	szContent;
	TBuf8<4096>	szContent1;

	szContent.Zero ();
	szContent1.Zero ();

	oFile.Read(szContent1);
	oFile.Close();

	CnvUtfConverter::ConvertToUnicodeFromUtf8(szContent,szContent1);

	if(szContent.Length ()>1)
	{
		TBuf<500> nInfo;
		TBuf<100> nFileName;
		//TBuf<100> nURL;有可能太小，卞涛修改
		TBuf<250> nURL;

		TBuf<255> nDownLoadTempFilePath;
		TBuf<500> nMultiInfo;
		TInt nIsMulti;
		TInt nThreadNum;

		TInt nFileLen;
		TInt nDownLoadLen;
		TInt i=0,j=0;

		i=szContent.Find (_L("*#"));

		for(j=0;j<DOWNLOADING_NUM;j++)
		{
			if(i<10)
				break;

			nInfo.Zero ();
			nInfo=szContent.Mid(0,i);
			szContent.Delete (0,i+2);

			//卞涛增加了修改，如果没最开始的2个逗号，则认为是错误记录，直接进入下一个历史记录，如果对于后面的增加默认
			TInt pos=0;
			pos=nInfo.Find(_L(","));//文件名称
			nFileName.Zero ();
			if(pos>-1)
			{
				nFileName.Append(nInfo.Mid(0,pos));
				nInfo.Delete(0,pos+1);
			}
			else
			{
				i=szContent.Find (_L("*#"));
				continue;
			}

			pos=nInfo.Find(_L(","));//下载url
			nURL.Zero ();
			if(pos>-1)
			{
				nURL.Append(nInfo.Mid(0,pos));
				nInfo.Delete(0,pos+1);
			}
			else
			{
				i=szContent.Find (_L("*#"));
				continue;
			}

			pos=nInfo.Find(_L(","));//文件大小
			if(pos>-1)
			{
				TLex temp(nInfo.Mid(0,pos));
				temp.Val( nFileLen );
				nInfo.Delete(0,pos+1);
			}

			pos=nInfo.Find(_L(","));//下载大小
			if(pos>-1)
			{
				TLex temp1(nInfo.Mid(0,pos));
				temp1.Val( nDownLoadLen);
				nInfo.Delete(0,pos+1);
			}
			else
			{
				//这里对老的日志文件处理
				TLex temp1(nInfo);
				temp1.Val(nDownLoadLen);

				nDownLoadTempFilePath.Zero();//下载临时文件路径
				nMultiInfo.Zero ();//切片信息
				AddToDownLoadingList(nURL,nFileName,nFileLen,nDownLoadLen,nDownLoadTempFilePath,0,0,nMultiInfo);

				i=szContent.Find (_L("*#"));
				continue;
			}

			//20070712张晖增加
			nDownLoadTempFilePath.Zero ();//下载临时文件路径
			pos=nInfo.Find(_L(","));
			if(pos>-1)
			{
				nDownLoadTempFilePath.Append(nInfo.Mid(0,pos));
				nInfo.Delete(0,pos+1);
			}

			pos=nInfo.Find(_L(","));//是否切片
			if(pos>-1)
			{
				TLex temp2(nInfo.Mid(0,pos));
				temp2.Val( nIsMulti);
				nInfo.Delete(0,pos+1);
			}

			pos=nInfo.Find(_L(","));//切片个数
			if(pos>-1)
			{
				TLex temp3(nInfo.Mid(0,pos));
				temp3.Val( nThreadNum);
				nInfo.Delete(0,pos+1);
			}

			nMultiInfo.Zero ();//切片信息
			nMultiInfo.Append (nInfo);

			//AddToDownLoadingList(nURL,nFileName,nFileLen,nDownLoadLen,1,0);

			if(nIsMulti==1)
				ChecekMultiInfo(nMultiInfo);

			AddToDownLoadingList(nURL,nFileName,nFileLen,nDownLoadLen,nDownLoadTempFilePath,nIsMulti,nThreadNum,nMultiInfo);

			i=szContent.Find (_L("*#"));
		}
	}
	//::WriteLogsL(_L("GetInfoFromLog-End"));
	iMainEngine.WriteLog16(_L("GetInfoFromLog-3"));
}

void CDownLoadEngine::ExitDownLoad()
{
	iMainEngine.WriteLog16(_L("ExitDownLoad-1"));
	//中止下载并保存日志
	StopDownLoadInThread(-1,0);

	//保存到下载日志
	WriteDownLoadLog();

	//保存已下载列表到文件
	//SaveDownLoadedHistoryToFile();
	iMainEngine.WriteLog16(_L("ExitDownLoad-2"));
}

const TDesC& CDownLoadEngine::CheckFileName(const TDesC& aName)
{
	//检查文件名称中是否有特殊字符，如果有则删除
	//20070815张晖增加判断，判断此文件名是否已存在
	TInt i;
	TBuf<255> nTmpName;
	TBuf<255> nFileName;

	iMainEngine.WriteLog16(_L("CheckFileName-1"));

	nFileName.Zero ();
	nFileName.Append (aName);

	i=0;
	nTmpName.Zero ();

	i=nFileName.Find (_L("*"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L(">"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L("<"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L("/"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L("\\"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L("|"));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	i=nFileName.Find (_L("\""));
	if(i>0)
	{
		nTmpName.Zero ();
		nTmpName.Append (nFileName.Mid (0,i));
		nTmpName.Append (nFileName.Mid (i+1,nFileName.Length ()-i-1));

		nFileName.Zero ();
		nFileName.Append (nTmpName);
	}

	//20070815张晖增加判断，判断此文件名是否已存在
	if (nFileName.Length()==0)
	{
		return nFileName;
	} 

	if (::FileIsExist(GetFullFileName(nFileName))==1)
	{
		//如果此文件已存在则获取新文件名称
		GetRealFileName(nFileName);
	}

	iMainEngine.WriteLog16(_L("CheckFileName-2"));
	return nFileName;
}

void CDownLoadEngine::GetRealFileName(TDes& aFileName)
{
	TBuf<255> nFileName;
	TInt i=0;
	TParse parse;
	
	iMainEngine.WriteLog16(_L("GetRealFileName-1"));

	parse.Set(aFileName,NULL,NULL);

	do
	{
		i++;
		nFileName.Zero ();
		nFileName.Append(parse.Name());
		nFileName.Append(_L("("));
		nFileName.AppendNum(i);
		nFileName.Append(_L(")"));
		nFileName.Append(parse.Ext());
	}while (::FileIsExist(GetFullFileName(nFileName))==1);

	aFileName.Zero();
	aFileName.Append(nFileName);

	iMainEngine.WriteLog16(_L("GetRealFileName-2"));
}
//=====================================================================================
//==============//20070531张晖增加重复下载判断=========================================
void CDownLoadEngine::GetDownLoadedHistoryFromFile()
{
	_LIT(KLogFileName,"3GestDownLoadedHistory.3G");

	iMainEngine.WriteLog16(_L("GetDownLoadedHistoryFromFile-1"));

	iDownLoadedArray = new (ELeave) CArrayFixFlat<TURLTEXT>(10);

	TFileName fn;
	fn = iMainEngine.GetAppPath ();
	fn.Append(KLogFileName);

	RFile oFile;
	if (KErrNone != oFile.Open(iRfs, fn, EFileWrite|EFileShareAny))
	{
		iMainEngine.WriteLog16(_L("GetDownLoadedHistoryFromFile-2"));
		return;
	}

	TInt nFileSize;
	TInt nGetSize;

	nGetSize=0;
	oFile.Size(nFileSize);

#define GETINFO_NUM (1024)

	TBuf<1024>	szTmp;
	TBuf<2048>	szContent;
	TBuf8<1024>	szContent1;

	szContent.Zero ();

	while(nGetSize<nFileSize)
	{
		//分批获取数据，每次大小为1024
		szTmp.Zero ();
		szContent1.Zero ();

		oFile.Read(nGetSize,szContent1,GETINFO_NUM);

		CnvUtfConverter::ConvertToUnicodeFromUtf8(szTmp,szContent1);

		nGetSize=nGetSize+GETINFO_NUM;

		if(szTmp.Length ()>1)
		{
			TBuf<200> nURL;
			TInt i=0;

			szContent.Append (szTmp);

			i=szContent.Find (_L("\x0D\x0A"));

			while(i>0)
			{
				nURL.Zero ();
				nURL=szContent.Mid(0,i);
				szContent.Delete (0,i+2);

				iDownLoadedArray->AppendL(nURL);

				i=szContent.Find (_L("\x0D\x0A"));
			}
		}
	}
	oFile.Close();

	iMainEngine.WriteLog16(_L("GetDownLoadedHistoryFromFile-3"));
}

void CDownLoadEngine::AddToDownLoadedHistoryArray(const TDesC& aURL )
{
	//增加已下载URL到下载历史列表中
	iMainEngine.WriteLog16(_L("AddToDownLoadedHistoryArray-1"));

	if(aURL.Length ()>0)
	{
		TInt nIndex;
		TBuf<10> nName;

		nName.Zero();
		TInt i=CheckIsInHistoryArray(aURL,nName,nIndex);

		if(i!=1)
		{
			if(iDownLoadedArray->Count()>=DOWNLOADEDHISTORY_NUM )
			{
				iDownLoadedArray->Delete(0);
			}
			iDownLoadedArray->AppendL(aURL);
		}
	}
	iMainEngine.WriteLog16(_L("AddToDownLoadedHistoryArray-2"));
}

void CDownLoadEngine::DelFromDownLoadedHistoryArray(const TDesC& aURL )
{
	//从已下载URL到下载历史列表中删除
	iMainEngine.WriteLog16(_L("DelFromDownLoadedHistoryArray-1"));
	if(aURL.Length ()>0)
	{
		TInt nIndex;
		TBuf<10> nName;

		nName.Zero();

		TInt i=CheckIsInHistoryArray(aURL,nName,nIndex);

		if(i==1)
		{
			if(nIndex!=-1)
			{
				iDownLoadedArray->Delete(nIndex);
			}
		}
	}
	iMainEngine.WriteLog16(_L("DelFromDownLoadedHistoryArray-2"));
}

void CDownLoadEngine::SaveDownLoadedHistoryToFile()
{
	if(iDownLoadedArray->Count()==0)
		return;
	
	iMainEngine.WriteLog16(_L("SaveDownLoadedHistoryToFile-1"));

	_LIT(KLogFileName,"3GestDownLoadedHistory.3G");

	TFileName fn;
	fn = iMainEngine.GetAppPath ();
	fn.Append(KLogFileName);

	RFile oFile;
	if (KErrNone != oFile.Replace(iRfs, fn, EFileWrite|EFileShareAny))
	{
		iMainEngine.WriteLog16(_L("SaveDownLoadedHistoryToFile-2"));
		return;
	}

	TBuf<200>	szURL;
	TBuf8<200>	szURL1;

	for (TInt i = 0; i < iDownLoadedArray->Count(); i++)
	{
		szURL.Zero ();
		szURL1.Zero ();

		szURL.Append (iDownLoadedArray->At(i));
		szURL.Append (_L("\x0D\x0A"));

		CnvUtfConverter::ConvertFromUnicodeToUtf8(szURL1,szURL);

		oFile.Write(szURL1);
	}

	oFile.Close();

	iMainEngine.WriteLog16(_L("SaveDownLoadedHistoryToFile-3"));
}

TInt  CDownLoadEngine::CheckIsInHistoryArray(const TDesC& aURL,const TDesC& aName,TInt& aIndex)
{
	//检查URL是否在历史列表中,如果在则返回1,并在aIndex返回Index值；不在则返回0
	TInt i;
	TBuf<200> nURL;

	iMainEngine.WriteLog16(_L("CheckIsInHistoryArray-1"));

	for(i=iDownLoadedArray->Count()-1;i>=0;i--)
	{
		nURL=iDownLoadedArray->At(i);

		if(nURL.Compare (aURL)==0)
		{
			aIndex=i;

			iMainEngine.WriteLog16(_L("CheckIsInHistoryArray-2"));
			return 1;
		}
	}

	//查找数据库中是否有已下载的记录
	TInt nRet=iMainEngine.DBEngine().CheckIsDownLoadHistoryByKeyL(aName,1); 

	if(nRet==1)
	{
		aIndex=-1;
	}

	iMainEngine.WriteLog16(_L("CheckIsInHistoryArray-3"));
	return nRet;
}

void CDownLoadEngine::GetFileNameFromURL(const TDesC& aURL,TDes& aFileName)
{
	TInt i=0;
	TBuf<255> nURL;
	TBuf<255> nFileType;

	iMainEngine.WriteLog16(_L("GetFileNameFromURL-1"));

	nURL.Copy (aURL);

	//20070815张晖为儒豹URL变动而更改
	i=nURL.Find (_L("&lu="));

	if (i!=KErrNotFound)
	{
		//为儒豹定制更改
		TInt j=0;

		j=nURL.Find(_L("&t="));

		nFileType.Append(nURL.Mid (i+4,j-i-4)) ;

		//TBuf<50> nFileName;有可能太小，卞涛修改
		TFileName nFileName;
		nFileName.Zero ();
		nFileName.Append(nURL.Mid (j+3));

		i=nFileType.Find (_L("."));

		if(i!=KErrNotFound)
		{
			nFileName.Append(nFileType.Mid(i));
		}
		aFileName.Zero();
		aFileName.Append(nFileName);
	} 
	else
	{
		//正常情况
		i=nURL.Find (_L("/"));

		while(i!=KErrNotFound)
		{
			nURL.Delete (0,i+1);

			i=nURL.Find (_L("/"));
		}

		i=nURL.Find (_L("."));

		if(i!=KErrNotFound)
		{
			nFileType.Zero ();
			nFileType.Append (nURL.Mid(i+1,nURL.Length ()-i-1));

			if(nFileType.Length ()<5 && nFileType.Find(_L("xml"))==KErrNotFound && nFileType.Find (_L("htm"))==KErrNotFound && nFileType.Find (_L("asp"))==KErrNotFound && nFileType.Find (_L("jsp"))==KErrNotFound)
			{
				aFileName.Zero ();
				aFileName.Append (nURL);
			}
		}
	}

	iMainEngine.WriteLog16(_L("GetFileNameFromURL-2"));
}

TInt CDownLoadEngine::CheckFileIsSupport(TDes& aFileName)
{
	//判断文件类型是否能被打开
	if(aFileName.Length ()==0)
		return 0;

	/*TInt dotIndex = aFileName.LocateReverseF('.');
	TFileName fileExtension = aFileName.Right( aFileName.Length() - dotIndex - 1 );
	fileExtension.ZeroTerminate();

	TFileName aAssociatedApp;*/

	/*TAppType aAppType = CFindAppState::FindAssociatedApp( fileExtension, aAssociatedApp );
	if ( EDefaultApp == aAppType )
	{
		return 1;
	}
	else
	{
		if(ParseExPathNum(aFileName)==5)
			return 0;
		else
			return 1;
	}*/
	if(ParseExPathNum(aFileName)==5)
		return 0;
	else
		return 1;
}

TInt CDownLoadEngine::FileIsExist(const TDes& aFileName,TInt& aSize)
{
	//判断文件是否存在,1:存在，0-不存在
	RFile tFile;
	TInt ret =0;
	
	iMainEngine.WriteLog16(_L("FileIsExist-1"));

	ret = tFile.Open(iRfs, aFileName, EFileWrite|EFileShareAny);
	if (KErrNone == ret)
	{
		tFile.Size(aSize);
		tFile.Close();
		ret= 1;
	}
	else
	{
		tFile.Close();
		ret= 0;
	}

	iMainEngine.WriteLog16(_L("FileIsExist-2"));
	return ret;
}

//=====================================================================================================
//======================================20070710张晖增加切片下载=======================================
TInt CDownLoadEngine::CreateDownLoadFile(TInt aListIndex)
{
	iMainEngine.WriteLog16(_L("CreateDownLoadFile-1"));

	TFileName nFileName;

	nFileName.Copy (GetFullFileName(iDownLoadingTaskList[aListIndex].nFileName));
	nFileName.Append (_L("3G"));

	TInt nSize;
	if(FileIsExist(nFileName,nSize)==1)
	{
		//如果已存在且与文件大小相等则直接返回
		if(nSize==iDownLoadingTaskList[aListIndex].nFileLen)
		{
			iDownLoadingTaskList[aListIndex].nTmpFilePath.Zero();
			iDownLoadingTaskList[aListIndex].nTmpFilePath.Append(nFileName);
			
			iMainEngine.WriteLog16(_L("CreateDownLoadFile-2"));
			return 4;
		}
	}

	TBuf<1> nDriver;
	nDriver.Copy ( iMainEngine.GetDownLoadDir().Mid (0,1));

	TInt64 nDriverSize=::GetFreeSpace (nDriver);

	if((TInt64)iDownLoadingTaskList[aListIndex].nFileLen/1024.0>nDriverSize)
	{
		//如果空间不够则将状态设为中止下载
		iDownLoadingTaskList[aListIndex].nStatus=4;
		//提示用户磁盘空间不足
		NoticeUserNoFreeDisk(aListIndex);

		iMainEngine.WriteLog16(_L("CreateDownLoadFile-3"));
		return 1;
	}

	RFile oFile;
	if (KErrNone != oFile.Replace(iRfs, nFileName, EFileWrite|EFileShareAny))
	{
		//如果建立不成功则将状态设为中止下载
		oFile.Close();
		iDownLoadingTaskList[aListIndex].nStatus=4;

		iMainEngine.WriteLog16(_L("CreateDownLoadFile-4"));
		return 2;
	}

	if(KErrNone!=oFile.SetSize(iDownLoadingTaskList[aListIndex].nFileLen ))
	{
		//如果文件创建不成功
		oFile.Close();
		::DelFile( nFileName);

		iDownLoadingTaskList[aListIndex].nStatus=4;

		iMainEngine.WriteLog16(_L("CreateDownLoadFile-5"));
		return 3;
	}

	oFile.Close();

	iDownLoadingTaskList[aListIndex].nTmpFilePath.Zero();
	iDownLoadingTaskList[aListIndex].nTmpFilePath.Append(nFileName);

	iMainEngine.WriteLog16(_L("CreateDownLoadFile-6"));
	return 0;
}

void CDownLoadEngine::SegmentationFile(TInt aListIndex)
{
	//对文件进行切片,切片原理:
	//1、文件大于设定的最大值才能切片（iMaxFileSize）
	//2、切片的大小为设定值（iPerUnitSize）
	//3、切片后对剩下的大小要进行判断，必须大于一个指定值（iSurplusUnitSize）才能继续切片
	//::WriteLogsL(_L("SegmentationFile-1")); 
	if(iDownLoadingTaskList[aListIndex].nFileLen ==0)
		return;

	if(iDownLoadingTaskList[aListIndex].nFileLen <iMaxFileSize)
	{
		iDownLoadingTaskList[aListIndex].nThreadNum=1;
		return;
	}

	iMainEngine.WriteLog16(_L("SegmentationFile-1"));

	//::WriteLogsL(_L("SegmentationFile-2")); 
	TInt nSize=iDownLoadingTaskList[aListIndex].nFileLen;
	TInt nBeginSize=0;
	TInt nEndSize=0;
	TInt i=0;
	TBuf<100> nInfo;

	//::WriteLogsL(_L("SegmentationFile-3")); 
	iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Zero (); 
	do
	{
		i++;

		nEndSize=nEndSize+iPerUnitSize;

		nInfo.Zero ();
		//格式为:Num:BeginPos*EndPos!ThreadID#,当尚未有ThreadID时默认值为-1
		nInfo.AppendNum(i);
		nInfo.Append (KNUM);
		nInfo.Append (_L("-1"));
		nInfo.Append (KTHREAD);
		nInfo.AppendNum(nBeginSize);
		nInfo.Append (KBEGINPOS);
		nInfo.AppendNum(nEndSize);
		nInfo.Append (KENDPOS);

		//::WriteLogsL(nInfo); 
		//::WriteLogsL(_L("SegmentationFile-4")); 

		if(nBeginSize==0)
		{
			//？？？？？？？？？？？
			//是否都需要加1，尚需测试
			nBeginSize=1+iPerUnitSize;
		}
		else
		{
			nBeginSize=nBeginSize+iPerUnitSize;
		}
		nSize=nSize-iPerUnitSize;

		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);
		//::WriteLogsL(_L("SegmentationFile-5")); 
	}while(nSize>iSurplusUnitSize);

	//::WriteLogsL(_L("SegmentationFile-6")); 
	i++;
	nInfo.Zero ();
	//格式为:Num:ThreadID#BeginPos*EndPos!,当尚未有ThreadID时默认值为-1
	nInfo.AppendNum(i);
	nInfo.Append (KNUM);
	nInfo.Append (_L("-1"));
	nInfo.Append (KTHREAD);
	nInfo.AppendNum(nBeginSize);
	nInfo.Append (KBEGINPOS);
	nInfo.Append(_L("0"));
	nInfo.Append (KENDPOS);
	//::WriteLogsL(nInfo); 
	//::WriteLogsL(_L("SegmentationFile-7")); 
	iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);
	iDownLoadingTaskList[aListIndex].nThreadNum =i;
	iDownLoadingTaskList[aListIndex].nUsedThreadNum=0; 
	iDownLoadingTaskList[aListIndex].nIsMultiThread =true;
	//::WriteLogsL(iDownLoadingTaskList[aListIndex].nMultiThreadInfo); 
	//::WriteLogsL(_L("SegmentationFile-7")); 

	iMainEngine.WriteLog16(_L("SegmentationFile-2"));
}

TInt CDownLoadEngine::SetMultiThread(TInt aListIndex,
												  TInt aThreadIndex,
												  TInt& aBeginPos,
												  TInt& aEndPos,
												  TInt& aNum)
{
	//设置多线程下载
	if(iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Length ()==0
		|| iDownLoadingTaskList[aListIndex].nThreadNum==iDownLoadingTaskList[aListIndex].nUsedThreadNum
		|| iDownLoadingTaskList[aListIndex].nIsMultiThread ==false)
	{
		return 1;
	}

	iMainEngine.WriteLog16(_L("SetMultiThread-1"));

	//::WriteLogsL(_L("SetMultiThread-1")); 
	TBuf<500> nInfo;
	nInfo.Copy (iDownLoadingTaskList[aListIndex].nMultiThreadInfo);

	TInt i=0;
	TInt j=0;

	//::WriteLogsL(_L("SetMultiThread-2")); 
	TInt nID=nInfo.Find (_L("-1"));

	if(nID==KErrNotFound)
	{
		iMainEngine.WriteLog16(_L("SetMultiThread-2"));
		return 2;
	}
	//::WriteLogsL(_L("SetMultiThread-3")); 
	TBuf<100> nTmp;
	nTmp.Zero ();

	if(nID>8)
	{
		nInfo.Delete (0,nID-8);

		//获取切片号
		i=nInfo.Find (KENDPOS);
		j=nInfo.Find (KNUM);

		nTmp.Append (nInfo.Mid (i+1,j-i-1));
	}
	else
	{
		//第一个
		j=nInfo.Find (KNUM);

		nTmp.Append (nInfo.Mid (0,j));
	}
	nInfo.Delete (0,j);

	//::WriteLogsL(nTmp); 
	//::WriteLogsL(_L("SetMultiThread-4")); 
	TLex temp(nTmp);
	temp.Val(aNum);

	//获取开始位置
	i=nInfo.Find (KTHREAD);
	j=nInfo.Find (KBEGINPOS);

	nTmp.Zero ();
	nTmp.Append (nInfo.Mid (i+1,j-i-1));
	//::WriteLogsL(nTmp); 
	//::WriteLogsL(_L("SetMultiThread-5")); 
	TLex temp1(nTmp);
	temp1.Val(aBeginPos);

	//获取结束位置
	i=nInfo.Find (KENDPOS);

	TBuf<10> nT;
	nT.Zero ();
	nT.AppendNum(j);
	nT.Append (_L("-"));
	nT.AppendNum (i);
	//::WriteLogsL(nT); 

	nTmp.Zero ();
	nTmp.Append (nInfo.Mid (j+1,i-j-1));

	//::WriteLogsL(nTmp); 
	//::WriteLogsL(_L("SetMultiThread-6")); 
	TLex temp2(nTmp);
	temp2.Val(aEndPos);
	//::WriteLogsL(_L("SetMultiThread-6-1")); 
	//将下载线程ID保存
	nInfo.Zero ();
	nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (0,nID));
	//::WriteLogsL(_L("SetMultiThread-6-2")); 
	nInfo.AppendNum(aThreadIndex);
	//::WriteLogsL(_L("SetMultiThread-6-3")); 
	nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (nID+2));

	//::WriteLogsL(nInfo); 
	//::WriteLogsL(_L("SetMultiThread-7")); 
	iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Zero ();
	iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);

	iDownLoadingTaskList[aListIndex].nUsedThreadNum++;
	//::WriteLogsL(_L("SetMultiThread-8")); 
	iMainEngine.WriteLog16(_L("SetMultiThread-2"));

	return 0;
}

TInt CDownLoadEngine::UpdateMultiThread(TInt aListIndex,
													 TInt aUnitID,
													 TInt aSize,
													 TInt aType)
{
	//aType:0-更新计数;1-完成下载;2-中止下载
	if(iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Length ()==0 
		|| iDownLoadingTaskList[aListIndex].nThreadNum==0 
		|| iDownLoadingTaskList[aListIndex].nUsedThreadNum ==0)
		return 1;

	iMainEngine.WriteLog16(_L("UpdateMultiThread-1"));
	//::WriteLogsL(_L("UpdateMultiThread-1")); 
	TBuf<500> nInfo;
	nInfo.Copy (iDownLoadingTaskList[aListIndex].nMultiThreadInfo);

	TInt i=0;
	TInt j=0;

	TBuf<20> nSearch;
	nSearch.Zero ();
	nSearch.AppendNum (aUnitID);
	nSearch.Append(KNUM);

	//::WriteLogsL(_L("UpdateMultiThread-2")); 
	TInt nPos=nInfo.Find (nSearch);
	//::WriteLogsL(nSearch); 

	if(nPos==KErrNotFound)
	{
		return 2;
	}

	if(nPos>5)
	{
		//如果不是第一个则判断是否为完全匹配
		TBuf<1> nTmp;
		nTmp.Zero ();
		nTmp.Append (nInfo.Mid (nPos-1,1));

		//::WriteLogsL(_L("UpdateMultiThread-X")); 
		//::WriteLogsL(nTmp) ;
		if(nTmp.Compare (KENDPOS)!=0)
		{
			iMainEngine.WriteLog16(_L("UpdateMultiThread-2"));
			return 2;
		}
	}

	//::WriteLogsL(_L("UpdateMultiThread-3")); 
	nInfo.Delete (0,nPos);
	//::WriteLogsL(nInfo); 
	//::WriteLogsL(_L("UpdateMultiThread-4")); 
	if(aType==0)
	{
		//下载计数
		//::WriteLogsL(_L("UpdateMultiThread-4-1")); 
		//获取开始位置
		i=nInfo.Find (KTHREAD);
		j=nInfo.Find (KBEGINPOS);

		TBuf<20> nTmp;
		nTmp.Zero ();
		nTmp.Append (nInfo.Mid (i+1,j-i-1));

		//::WriteLogsL(nTmp); 

		TInt nBeginPos=0;
		TLex temp(nTmp);
		temp.Val(nBeginPos);

		if(nBeginPos==0)
		{
			//？？？？？？？？？？？
			//计数从0开始
			nBeginPos=nBeginPos+aSize-1;
		}
		else
		{
			nBeginPos=nBeginPos+aSize;
		}

		nInfo.Zero ();
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (0,nPos+i+1));
		nInfo.AppendNum(nBeginPos);
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (j+nPos));

		//::WriteLogsL(nInfo); 
		//::WriteLogsL(_L("UpdateMultiThread-4-2")); 
		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);
		
		iMainEngine.WriteLog16(_L("UpdateMultiThread-3"));
		return 0;
	}
	else if(aType==1)
	{
		//完成下载
		//::WriteLogsL(_L("UpdateMultiThread-5-1")); 
		j=nInfo.Find (KENDPOS);

		nInfo.Zero ();
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (0,nPos));
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (nPos+j+1));

		//::WriteLogsL(nInfo); 
		//::WriteLogsL(_L("UpdateMultiThread-5-2")); 

		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);

		iDownLoadingTaskList[aListIndex].nThreadNum --;
		iDownLoadingTaskList[aListIndex].nUsedThreadNum --;
		
		iMainEngine.WriteLog16(_L("UpdateMultiThread-4"));
		return 0;
	}
	else if(aType==2)
	{
		//中止下载
		i=nInfo.Find (KNUM);
		j=nInfo.Find (KTHREAD);

		nInfo.Zero ();
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (0,nPos+i+1));
		nInfo.Append (_L("-1"));
		nInfo.Append (iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Mid (nPos+j));

		//::WriteLogsL(nInfo); 
		//::WriteLogsL(_L("UpdateMultiThread-5-3")); 

		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Zero ();
		iDownLoadingTaskList[aListIndex].nMultiThreadInfo.Append (nInfo);

		iDownLoadingTaskList[aListIndex].nUsedThreadNum --;
		
		iMainEngine.WriteLog16(_L("UpdateMultiThread-5"));
		return 0;
	}

	iMainEngine.WriteLog16(_L("UpdateMultiThread-6"));
	ASSERT(EFalse);
	return 0;
}

void CDownLoadEngine::ChecekMultiInfo(TDes& aMultiInfo)
{
	//非正常退出后处理
	TBuf<500> nInfo;
	TBuf<500> nMulti;
	TBuf<20> nThread;
	TInt i,j;
	
	iMainEngine.WriteLog16(_L("ChecekMultiInfo-1"));

	nMulti.Zero ();
	nInfo.Copy (aMultiInfo);

	i=nInfo.Find (KNUM);

	if(i==KErrNotFound)
	{
		iMainEngine.WriteLog16(_L("ChecekMultiInfo-2"));
		return;
	}

	do
	{
		j=nInfo.Find (KTHREAD);
		nThread.Zero ();
		nThread.Append (nInfo.Mid (i+1,j-i-1));

		if(nThread.Compare (_L("-1"))!=0)
		{
			nMulti.Append (nInfo.Mid (0,i+1));
			nMulti.Append (_L("-1"));
			nMulti.Append (nInfo.Mid (j,1));
		}
		else
		{
			nMulti.Append (nInfo.Mid (0,j+1));
		}

		nInfo.Delete (0,j+1);
		i=nInfo.Find (KNUM);

	}while(i!=KErrNotFound);

	nMulti.Append (nInfo);

	aMultiInfo.Zero ();
	aMultiInfo.Append (nMulti);

	iMainEngine.WriteLog16(_L("ChecekMultiInfo-3"));
}

//=========================================================================
//==========20070625张晖增加时钟事件
void CDownLoadEngine::StopPeriodic()
{
	iMainEngine.WriteLog16(_L("StopPeriodic-1"));
	if(iPeriodic)
		iPeriodic->Cancel();

	iMainEngine.WriteLog16(_L("StopPeriodic-2"));
}

void CDownLoadEngine::StartPeriodic()
{
	iMainEngine.WriteLog16(_L("StartPeriodic-1"));
	if(iPeriodic)
		iPeriodic->Start(1000, 1000 * 1000, TCallBack(Periodic, this));

	iMainEngine.WriteLog16(_L("StartPeriodic-2"));
}

TInt CDownLoadEngine::Periodic(TAny *aPtr)
{
	STATIC_CAST(CDownLoadEngine*, aPtr)->TimeSlice();	
	return 1;
}

void CDownLoadEngine::SetNoticedWindow(MDownLoadNotice* aDownLoadNoticeWindow)
{
	iDownLoadNoticeWindow=aDownLoadNoticeWindow;
}

void CDownLoadEngine::GetDownLoadingList(RArray<TDownLoadShowInfo>& aShowInfo)
{
	//获取下载列表信息
	TDownLoadShowInfo nInfo;
	
	iMainEngine.WriteLog16(_L("GetDownLoadingList-1"));

	TInt i=0;

	if (iDownLoadingTaskNum>0)
	{
		for(i=0;i<iDownLoadingTaskNum;i++)
		{
			if(iDownLoadingTaskList[i].nStatus ==0)
				break;
			
			nInfo.nTaskID=iDownLoadingTaskList[i].nTaskID;
			nInfo.nFileName.Zero ();
			nInfo.nFileName.Append(iDownLoadingTaskList[i].nFileName);
			nInfo.nStatus=iDownLoadingTaskList[i].nStatus;
			nInfo.nFileLen= iDownLoadingTaskList[i].nFileLen;
			nInfo.nDownLoadLen=iDownLoadingTaskList[i].nDownLoadLen;

			aShowInfo.Append(nInfo);
		}
	} 

	iMainEngine.WriteLog16(_L("GetDownLoadingList-2"));
}

void CDownLoadEngine::AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent)
{
}

void CDownLoadEngine::AudioPlayComplete()
{
	iIsPlayMusic=false;
}
void CDownLoadEngine::AudioInitComplete(TInt aError)
{
}

//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CDownLoadEngine::DialogEvent(TInt aCommand)
{
	iMainEngine.WriteLog16(_L("DialogEvent-1"));
	switch(aCommand)
	{
	case ECmdNull:
		break;

	case ECmdOpenFile:
		//打开文件功能
		if(iDownLoadSuccessPath.Length ()>0)
		{
			iMainEngine.FileExecuter(iMainEngine.CurWindow()).OpenFileByDefault(iDownLoadSuccessPath);		
		}
		break;

	default:
		iMainEngine.WriteLog16(_L("DialogEvent-2"));
		ASSERT(EFalse);
		break;
	}
	iMainEngine.WriteLog16(_L("DialogEvent-3"));
}

void CDownLoadEngine::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{

}

TInt CDownLoadEngine::GetDownLoadingTaskNum()
{
	TInt nCount=0;

	if (iDownLoadingTaskNum==0)
	{
		return 0;
	}
	
	for (TInt i=0;i<iDownLoadingTaskNum;i++)
	{
		if (iDownLoadingTaskList[i].nStatus ==2 || iDownLoadingTaskList[i].nStatus ==3)
		{
			nCount++;
		}
	}

	return nCount;
}