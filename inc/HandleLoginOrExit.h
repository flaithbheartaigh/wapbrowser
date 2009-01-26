/*
============================================================================
 Name        : HandleLoginOrExit.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleLoginOrExit declaration
============================================================================
*/

#ifndef HANDLELOGINOREXIT_H
#define HANDLELOGINOREXIT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "HttpObserver.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CHttpManager;
class CSmsHandler;
/**
*  CHandleLoginOrExit
* 
*/
class CHandleLoginOrExit : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	
	~CHandleLoginOrExit();
	static CHandleLoginOrExit* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleLoginOrExit* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleLoginOrExit(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public:
	void  AddLoginRequest();
	void  SendLoginRequest();
	void  SendExitRequest();

	void  CancelSendRequest(TInt aType);

	const TDesC& GetSystemInfo();
	const TDesC& GetTitle();

	const TDesC& GetInfo();

	inline TBool IsRegisterPhone(){ return iRegisterPhone; }//�Ƿ�ע���ֻ�����
	inline TBool IsAppUpdate(){ return iAppUpdate; } //�Ƿ��������
	inline TBool IsContentUpdate(){ return iContentUpdate; } //�Ƿ���Ŀ����
	inline TBool IsSystemInfo(){ return iSystemInfoFlag; }//�Ƿ���ϵͳ��Ϣ
	inline TBool IsNextSystemInfo(){ return iNextSystemInfo; }//�Ƿ�����һ��ϵͳ��Ϣ
	inline TBool IsSmallGateWay(){ return iSmallGateWay; }//�Ƿ�ʹ��С����
private:
	void GetNameAndPassword();
	void  SendSms();

	void CheckRegisterPhone(const TDesC& aText);
	void CheckAppUpdate(const TDesC& aText);
	void CheckContentUpdate(const TDesC& aText);
	void CheckSystemInfo(const TDesC& aText);
	void CheckGateWay(const TDesC& aText);
private:
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;
	CHttpManager& iHttpManager;
	
	CSmsHandler* iSmsHandler;

	TInt* iIntNum;

	HBufC8* iUrl;
	HBufC* iSystemInfo;
	HBufC* iTitle;
	HBufC* iBuf;

	TBuf8<60> iNamePassword;
	TBuf8<63> iTBuf;

	TBool iRegisterPhone;
	TBool iAppUpdate;
	TBool iContentUpdate;
	TBool iSystemInfoFlag;
	TBool iNextSystemInfo;
	TBool iSmallGateWay;
};

#endif // HANDLELOGINOREXIT_H
