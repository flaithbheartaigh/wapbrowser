/*
============================================================================
 Name        : HandleInfo.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleInfo declaration
============================================================================
*/

#ifndef HANDLEINFO_H
#define HANDLEINFO_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <BADESCA.H>
#include "HttpObserver.h"
// CLASS DECLARATION
class CMainEngine;
class MHandleEventObserver;
class CHttpManager;
/**
*  CHandleInfo
* 
*/
class CHandleInfo : public CBase,public MOperationObserver
{
public: // Constructors and destructor

	~CHandleInfo();
	static CHandleInfo* NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	static CHandleInfo* NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);

private:

	CHandleInfo(MHandleEventObserver& aObserver,CMainEngine& aMainEngine);
	void ConstructL();

public://From MOperationObserver
	virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType);

public:
	const TDesC& GetInfo();

	const TDesC& GetName();
	const TDesC& GetSize();
	const TDesC& GetType();
	const TDesC& GetRemark();

	const TDesC& GetImagePath();

	void  SendRequest(const TDesC& aCode);
	void  CancelSendRequest();
	
	TBool IsImage(){ return iPicBool; }
private:
	void GetTempFile();
	TBool CheckTempFile();
	void HandleXmlData();
	void ClearBuf();
	void SendImageRequest(const TDesC8& aUrl);

	void GetXmlPath();
	void GetPicPath();
private:
	MHandleEventObserver& iObserver;

	CMainEngine& iMainEngine;
	CHttpManager& iHttpManager;

	HBufC* iBuf;

	TFileName iFileName;

	HBufC8* iPicUrl;
	HBufC* iName;
	HBufC* iSize;
	HBufC* iType;
	HBufC* iRemark;
	HBufC* iPicPath;
	HBufC* iXmlPath;
	HBufC* iCode;

	TInt iHttpState;

	TBool iResponseFlag;
	TBool iPicBool;
	TBool iContentError;
	TBool iTempBuf;
	CDesCArray* iDesArray;
};

#endif // HANDLEINFO_H
