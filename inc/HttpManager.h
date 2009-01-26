/*
============================================================================
 Name        : HttpManager.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHttpManager declaration
============================================================================
*/

#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#ifdef __SERIES60_3X__
	#include <e32cmn.h>
#endif
#include "HttpObserver.h"
class CHTTPEngine;
class CHttpRequestElement;
class CMOperationObserver;
class CMainEngine;
struct TConnectionStatus
{
	TInt	iThreadIndex;
	TInt	iState;
} ;

class CHttpManager : public CActive,public MClientObserver
{
public:

	CHttpManager(CMainEngine& aMainEngine);
	void ConstructL();
	~CHttpManager();

public: // New functions

	void StartConncetion();

	//aBoserver��ʾ����ҵ��ģ�飻
	//aUrl��ʾ��������URL��
	void SendGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aType = 0);
	void SendGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,const TDesC& aFileName,TInt aType = 0);

	//aBoserver��ʾ����ҵ��ģ�飻
	//aUrl��ʾ��������URL��
	//aContentType��ʾ��������ContentType��
	//aData��ʾ�����������ݣ�
	void SendPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aType = 0);
	void SendPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,const TDesC& aFileName,TInt aType = 0);

	//aBoserver��ʾ����ҵ��ģ�飻
	void CancelTransaction(MOperationObserver* aBoserver,TInt aType = 0);
	
public: // From MClientObserver
	virtual void ClientEvent(const TDesC& aEventDescription,TInt aIndex);
	virtual void ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex);

private: // From CActive
	void RunL();
	
	void DoCancel();
private:
	void InitConnectionIap(); //��ʼ��IAP�����
	TBool CheckIAPValue(); //����IAP������Ƿ����
	void ChangeConnectionIap();//�޸����IAP�����

	void CheckHttpArray();

	TInt CheckWapPage(const TDesC8& aUrl, const TDesC& aBodyData,TDes8& aNewUrl);
public:
	void AddGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aIndex,TInt aType);
	void AddGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aIndex,const TDesC& aFileName,TInt aType);
	void AddPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aIndex,TInt aType);
	void AddPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aIndex,const TDesC& aFileName,TInt aType);
private:
	enum THttpManagerState
	{
		EUninitialized,
		EInitialized,
	    EIAPNotFound,	// û���ҵ�IAP�ڵ�
	    EConnectIAP,	// 
		EConnectSuccess,
	    ENoError			// Error condition
	};
	enum THttpConnectionState
	{
		ENoStartIap,
		EStartIap
	};

	const TDesC& GetErrorBuf(TInt aCode,const TDesC& aText);
private:
	CMainEngine& iMainEngine;
	TConnectionStatus iConnectionStatusOne;
	TConnectionStatus iConnectionStatusTwo;

	RPointerArray<CHttpRequestElement>	iRequestArray;

	CHttpRequestElement* iRequestElementOne;
	CHttpRequestElement* iRequestElementTwo;

	CHTTPEngine* iHttpEngOne; 
	CHTTPEngine* iHttpEngTwo; 

	//MClientObserver&		aObserver;	// Used for passing body data and
	TInt    iState;		// State of the active object
	TBool	iConnectFlag;
	RTimer	iTimer;		// Provides async timing service

	TInt iIAPValue;  //IAP�����ֵ

	TInt iOneStatus;
	TInt iTwoStatus;

	HBufC* iErrorBuf;
};

#endif
