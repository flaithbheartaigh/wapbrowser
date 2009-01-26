// FileMainState.cpp: implementation of the CDownLoadHistoryEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "DownLoadHistoryEngine.h"
#include "DBEngine.h"
#include "MainEngine.h"
#include "StaticCommonTools.h"
#include "CoCoPreDefine.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDownLoadHistoryEngine::CDownLoadHistoryEngine(CMainEngine& aMainEngine)
:iMainEngine(aMainEngine),
iDownLoadHistoryListNum(0),
iHistoryArray(NULL)
{
	InitDownLoadHistoryList();

	GetTotalPage();
}


CDownLoadHistoryEngine::~CDownLoadHistoryEngine()
{
	if(iHistoryArray)
	{
		iHistoryArray->Reset();
		delete iHistoryArray;
	}
}

void CDownLoadHistoryEngine::InitDownLoadHistoryList()
{
	//从数据库中获取下载历史记录
	iHistoryArray= new (ELeave) CArrayFixFlat<TINFOCONTENTTEXT>(5);

	iMainEngine.DBEngine().GetAllDownLoadHistory(iHistoryArray);  

	iDownLoadHistoryListNum =iHistoryArray->Count();
}

TInt CDownLoadHistoryEngine::AddToDownLoadHistoryList(const TDesC& aDownLoadURL,
																const TDesC& aFileName,
																const TInt aFileLen)
{
	TInt i=0;

	TBuf<500> nInfo;
	nInfo.Zero ();
	nInfo.Append (aFileName);
	nInfo.Append (_L("*#"));
	nInfo.Append (aDownLoadURL);
	nInfo.Append (_L("*#"));
	nInfo.AppendNum (aFileLen);

	if(iDownLoadHistoryListNum>=iMainEngine.GetDownLoadMaxHistoryNum()) 
	{
		//如果超出下载记录的最大值则删除第一条记录
		DelDownLoadHistory(0,0);
	}

	i=iMainEngine.DBEngine().AddDownLoadHistoryWithSql (aFileName,aDownLoadURL,aFileLen);

	if (i==KErrNone)
	{

		iHistoryArray->AppendL(nInfo);

		iDownLoadHistoryListNum++;
		
		//通知界面显示
		if(iDownLoadHistoryNoticeWindow)
		{
			iDownLoadHistoryNoticeWindow->Update();
		}

		GetTotalPage();

		return 0;
	}
	else
	{
		return 1;
	}
}


TInt CDownLoadHistoryEngine::DelFromDownLoadHistoryList(TInt aListIndex)
{
	//从已下载列表中删除列表信息
	TInt i=0;

	iDownLoadHistoryListNum--;

	iHistoryArray->Delete(aListIndex);

	return 1;
}

void CDownLoadHistoryEngine::DelDownLoadHistory(TInt aListIndex,TInt aDelType)
{
	//从已下载列表中删除，aDelType:0-从列表中删除;1-从文件中删除
	TInt i=0;

	if(iDownLoadHistoryListNum>0)
	{
		if(aDelType==0)
		{
			if(aListIndex==-1)
			{
				iMainEngine.DBEngine().RemoveAllDownLoadHistory(i); 
				iHistoryArray->Reset();
				iDownLoadHistoryListNum=0;
			}
			else
			{
				TBuf<500> nInfo;

				//TBuf<50> nFileName;有可能太小，卞涛修改
				TFileName nFileName;
				TBuf<250> nURL;

				nInfo.Zero ();
				nInfo.Append (iHistoryArray->At(aListIndex));

				TInt i=nInfo.Find (_L("*#"));
				nFileName.Zero ();
				nFileName.Append (nInfo.Mid(0,i));

				nInfo.Delete (0,i+2);	
				i=nInfo.Find (_L("*#"));
				nURL.Zero ();
				nURL.Append (nInfo.Mid(0,i));

				iMainEngine.DBEngine().RemoveDownLoadHistory(nFileName,nURL);

				DelFromDownLoadHistoryList(aListIndex);
			}
		}
		else if(aDelType==1)
		{
			if(aListIndex==-1)
			{
				DelDownLoadHistoryFile(-1);

				iMainEngine.DBEngine().RemoveAllDownLoadHistory(i); 

				iHistoryArray->Reset();
				iDownLoadHistoryListNum=0;
			}
			else
			{
				DelDownLoadHistoryFile(aListIndex);

				DelFromDownLoadHistoryList(aListIndex);
			}
		}
	}

	GetTotalPage();
}

void CDownLoadHistoryEngine::DelDownLoadHistoryFile(TInt aListIndex)
{
	//根据已下载列表删除文件
	TInt i=0;

	if(iDownLoadHistoryListNum>0)
	{
		if(aListIndex==-1)
		{
			for(i=0;i<iDownLoadHistoryListNum;i++)
			{
				//TBuf<50> nFileName;有可能太小，卞涛修改
				TFileName nFileName;
				nFileName.Zero ();
				nFileName.Append (GetFileName(i));

				TFileName szName;
				szName.Zero ();
				szName.Append (nFileName);

				TFileName szName1;

				szName1.Append (ParseUrl( szName));

				TBuf8<200> mTmp;
				mTmp.Zero ();

				CnvUtfConverter::ConvertFromUnicodeToUtf8(mTmp,szName1);
				mTmp.Append (_L8(" -Del"));

				::DelFile (szName1);
			}
		}
		else
		{
			//TBuf<50> nFileName;有可能太小，卞涛修改
			TFileName nFileName;
			nFileName.Zero ();
			nFileName.Append (GetFileName(aListIndex));

			TFileName szName;
			szName.Zero ();
			szName.Append (nFileName);

			TFileName szName1;

			szName1.Append (ParseUrl( szName));

			::DelFile (szName1);
		}
	}
}

TFileName CDownLoadHistoryEngine::ParseUrl(const TDesC& aFileName)
{			
	TInt len = aFileName.Length();
	TFileName fileName;//(aFileName);
	fileName.Append(aFileName);
	TFileName szName;

	szName.Append(aFileName);

	TFileName szText;
	szText.Append(_L("\\"));
	szText.Append(aFileName);

	TInt nFileNum = 0, nFileExNum = 0;

	::GetFileName(fileName, nFileNum, nFileExNum);

	TBuf<16> szExName;
	szExName = aFileName.Right(nFileExNum);

	TFileName szTemp;
	szTemp.Append(aFileName);

	fileName.Delete(0, aFileName.Length());
	fileName.Append(iMainEngine.GetDownLoadDir());
	fileName.Append(ParseExPath(szExName));
	fileName.Append(szTemp);	
	return fileName;
}

TInt CDownLoadHistoryEngine::PageCount() const
{
	return iPageCount;
}

const TDesC& CDownLoadHistoryEngine::GetFileName(TInt aListIndex)
{
	//获取文件名称信息
	TBuf<500> nFileName;
	nFileName.Zero ();
	nFileName.Append (iHistoryArray->At(aListIndex));

	TInt i=nFileName.Find (_L("*#"));

	return nFileName.Mid (0,i);
}

const TDesC&	CDownLoadHistoryEngine::ParseExPath(const TDesC& aFile)
{
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

void CDownLoadHistoryEngine::GetDownLoadHistoryInfo(RArray<TDownLoadHistoryShowInfo>& aShowInfo,TInt aPageNum)
{
	//获取下载列表信息
	TDownLoadHistoryShowInfo nInfo;

	TInt i=0;
	TInt nMaxNum=0;

	TBuf<255> nUrl;
	TBuf<255> nFileName;
	TBuf<500> nFileInfo;

	if (iDownLoadHistoryListNum>0 && aPageNum<=iPageCount)
	{
		nMaxNum=aPageNum*KPAGELINE_LIST>iDownLoadHistoryListNum?iDownLoadHistoryListNum:aPageNum*KPAGELINE_LIST;

		for(i=(aPageNum-1)*KPAGELINE_LIST;i<nMaxNum;i++)
		{
			nUrl.Zero ();
			nFileName.Zero ();
			
			nFileInfo.Zero ();
			nFileInfo.Append (iHistoryArray->At(iHistoryArray->Count()-i-1));

			TInt j=nFileInfo.Find (_L("*#"));

			nFileName.Append (nFileInfo.Mid (0,j));
			nFileInfo.Delete (0,j+2);

			j=nFileInfo.Find (_L("*#"));
			nFileInfo.Delete (0,i+2);

			nUrl.Append (nFileInfo);

			nInfo.nListID=iHistoryArray->Count()-i-1;

			nInfo.nFileName.Zero ();
			nInfo.nFileName.Append(nFileName);
			
			nInfo.nURL.Zero ();
			nInfo.nURL.Append(nUrl);

			aShowInfo.Append(nInfo);
		}
	} 
}

void CDownLoadHistoryEngine::SetNoticedWindow(MDownLoadHistoryNotice* aDownLoadNoticeWindow)
{
	iDownLoadHistoryNoticeWindow=aDownLoadNoticeWindow;
}

void CDownLoadHistoryEngine::GetTotalPage()
{
	iPageCount=iDownLoadHistoryListNum/KPAGELINE_LIST;

	if(iDownLoadHistoryListNum%KPAGELINE_LIST>0)
		iPageCount++;
}