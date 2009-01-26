

#if !defined(AFX_BROWSERDOWNLOAD_DOWNLOADHISTORY_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_)
#define AFX_BROWSERDOWNLOAD_DOWNLOADHISTORY_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_

#include <apgcli.h>
#include "DBEngine.h"
#ifdef __SERIES60_3X__
	#include <aknserverapp.h> 
#endif
#include "CoCoPreDefine.h"

class CMainEngine;

class MDownLoadHistoryNotice 
{
public:
	virtual void Update() = 0;
};

#define KPAGELINE_LIST (15)

typedef struct TDownLoadHistoryShowInfo DOWNLOADHISTORYSHOWINFO;
struct TDownLoadHistoryShowInfo
{
	TInt		nListID;		//任务ID
	TBuf<130>	nFileName;		//文件名称	
	TBuf<130>	nURL;			//文件下载路径	
} ;

class CDownLoadHistoryEngine : public CBase
{
public:
	CDownLoadHistoryEngine(CMainEngine& aMainEngine);
	virtual ~CDownLoadHistoryEngine();

	TInt AddToDownLoadHistoryList(const TDesC& aDownLoadURL,
		const TDesC& aFileName,
		const TInt aFileLen);

	void DelDownLoadHistory(TInt aListIndex,TInt aDelType);

	//获取信息页数
	inline TInt GetHistoryInfoPageCount()
	{
		return iPageCount;
	};

	//获取下载列表信息
	void GetDownLoadHistoryInfo(RArray<TDownLoadHistoryShowInfo>& aShowInfo,TInt aPageNum);

	//设置通知界面
	void SetNoticedWindow(MDownLoadHistoryNotice* aDownLoadNoticeWindow);

	TFileName ParseUrl(const TDesC& aFileName);

	TInt	PageCount() const;

private:
	TInt iDownLoadHistoryListNum;

	void InitDownLoadHistoryList();

	TInt DelFromDownLoadHistoryList(TInt aListIndex);

	void DelDownLoadHistoryFile(TInt aListIndex);

	const TDesC& ParseExPath(const TDesC& aFile);

	const TDesC& GetFileName(TInt aListIndex);

	void GetTotalPage();
private:
	CMainEngine&		iMainEngine;

	CArrayFixFlat<TINFOCONTENTTEXT>* iHistoryArray;

	MDownLoadHistoryNotice* iDownLoadHistoryNoticeWindow;

	TInt iPageCount;
};

#endif // !defined(AFX_BROWSERDOWNLOADMANAGE_H__2A9A44EB_655D_4AD6_AEB2_086593DFD59B__INCLUDED_)
