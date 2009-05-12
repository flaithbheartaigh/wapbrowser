// BrowserDownLoadState.h: interface for the CDownLoadNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOADSTATE_H__969FBD0C_19BB_4AD7_A9E2_5915A26CF10A__INCLUDED_)
#define AFX_DOWNLOADSTATE_H__969FBD0C_19BB_4AD7_A9E2_5915A26CF10A__INCLUDED_

#include <S32FILE.H>
#include <apgcli.h>
#include "HTTPEngine.h"
#include "HttpObserver.h"

class CMainEngine;
class MDownLoadServer;

class MDownLoadServer 
{
public:

	virtual void DownLoadEvent(TInt aTaskID,TInt aThreadID,TInt aEventType) = 0;

	virtual void DownLoadReceived(TInt aTaskID,TInt aThreadID,TInt aTotalLen, TInt aReceivedLen) = 0;

	//获取下载描述文件后通知接口
	virtual void DownLoadReceived(TInt aTaskID,
									TInt aThreadID,
									TInt aFileSize, 
									TDesC& aFileName,
									TDesC& aDownLoadURL,
									TDesC& aDownLoadResponseURL) = 0;
};

class CDownLoadNet : public  MClientObserver
{
public:
	CDownLoadNet(CMainEngine& aMainEngine);
	virtual ~CDownLoadNet();

	TInt Activate();

	//////////////////////////////////////////////////////////////////////////
	//from MClientObserver
	void ClientEvent(const TDesC& aEventDescription,TInt aIndex) ;
	void ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex) ;

	TInt CancelDownLoad(TInt aType);

	void SetUrl(TInt aTaskID,TInt aThreadID,const TDesC& aUrl, const TDesC& aName,TInt aBeginPos,TInt aEndPos);

	void GetDownLoadFileSize(TInt aTaskID,TInt aThreadID,const TDesC& aUrl);

	inline void SetState(MDownLoadServer* aDownLoadServer)
	{
		iDownLoadServer = aDownLoadServer;
	}

	void SetHttpIndex(TInt aIndex);
private:

	void GetFile();

	void HttpBinaryGet();
	void HttpBinaryGet(TDesC& aURL);

	void HandleDDFile(TDesC& aContent);
	void HandleJADFile(TDesC& aContent);
private:	

	CMainEngine&		iMainEngine;

	enum TNetStatus
	{
		ENone,
		EConnectMenu,
		EConnecting,
		EConnectOver
	};

	TNetStatus iStatus;
	//write file
	RFs ifs;
	RFileWriteStream* iStream;
	//save download file
	TFileName iSaveFileName;
	//http url
	TFileName	iUrl;

	CHTTPEngine*	iHttpBinary;

	TInt	iNewGetFile;

	MDownLoadServer* iDownLoadServer;
	TInt iTaskID;
	TInt iThreadID;

	HBufC8* iReceivedData;
	HBufC* iReceivedTextData;

	TInt iBeginPos;
	TInt iEndPos;

	RFile iSaveFile;

	//是否是文本内容标志
	TInt iContentType;

	//处理类型标识（0：下载，1-获取文件大小）
	TInt iHandleType;

	TBuf<500> iOldURL;
	//超时连接次数
	TInt iBeyondTimeNum;
};

#endif // !defined(AFX_BROWSERDOWNLOADSTATE_H__969FBD0C_19BB_4AD7_A9E2_5915A26CF10A__INCLUDED_)
