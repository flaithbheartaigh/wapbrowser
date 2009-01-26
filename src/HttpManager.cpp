/*
============================================================================
 Name        : HttpManager.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHttpManager implementation
============================================================================
*/

#include "HttpManager.h"
#include "HTTPEngine.h"
#include "HttpRequestElement.h"
#include "MainEngine.h"
#include "CoCoPreDefine.h"
#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <EIKENV.H>
#include "StaticCommonTools.h"
#include <utf.h>
_LIT(KIAPVALUE,"IAPValue=%d");
_LIT(KIAPERROR,"GetIAPError=%d");
_LIT(KINITCONN,"InitConnState=%d");

CHttpManager::CHttpManager(CMainEngine& aMainEngine) 
:CActive(EPriorityStandard)
,iIAPValue(-1)	// Standard priority
,iMainEngine(aMainEngine)
,iConnectFlag(EFalse)
{
}

void CHttpManager::ConstructL()
{

	iMainEngine.WriteLog16(_L("ConstructL-1"));
	iOneStatus=0;
	iTwoStatus=0;

	iHttpEngOne= CHTTPEngine::NewL(*this);
	iConnectionStatusOne.iState=ENoStartIap;
	iConnectionStatusOne.iThreadIndex=1;
	iHttpEngOne->SetThreadIndex(iConnectionStatusOne.iThreadIndex);


	iHttpEngTwo= CHTTPEngine::NewL(*this);
	iConnectionStatusTwo.iState=ENoStartIap;
	iConnectionStatusTwo.iThreadIndex=2;
	iHttpEngTwo->SetThreadIndex(iConnectionStatusTwo.iThreadIndex);
	iMainEngine.WriteLog16(_L("ConstructL-2"));
	iState = EUninitialized;

	//这里要通过引擎获取一次IAP接入点,以后添加
	iIAPValue=iMainEngine.GetIAPValue();

	iMainEngine.WriteLog16(KIAPVALUE,iIAPValue);

	if(!CheckIAPValue())
	{
		iMainEngine.WriteLog16(_L("ConstructL-3"));
		InitConnectionIap();
		iMainEngine.WriteLog16(_L("ConstructL-4"));
		ChangeConnectionIap();
		iMainEngine.WriteLog16(KIAPVALUE,iIAPValue);
		iMainEngine.WriteLog16(_L("ConstructL-5"));
	}
	else
		iState=EInitialized;

	//User::LeaveIfError(iTimer.CreateLocal());	// Initialize timer
	
	CActiveScheduler::Add(this);				// Add to scheduler
	iMainEngine.WriteLog16(_L("ConstructL-5"));
}

CHttpManager::~CHttpManager()
{
	
	Cancel(); // Cancel any request, if outstanding

	iRequestArray.ResetAndDestroy();
	iRequestArray.Close();

	if(iRequestElementOne)
	{
		delete iRequestElementOne;
		iRequestElementOne=NULL;
	}

	if(iRequestElementTwo)
	{
		delete iRequestElementTwo;
		iRequestElementTwo=NULL;
	}

	if(iHttpEngOne)
	{
		delete iHttpEngOne; 
		iHttpEngOne=NULL;
	}

	if(iHttpEngTwo)
	{
		delete iHttpEngTwo; 
		iHttpEngTwo=NULL;
	}

	if(iErrorBuf)
	{
		delete iErrorBuf; 
		iErrorBuf=NULL;
	}
}

void CHttpManager::DoCancel()
{
	//iTimer.Cancel();
	this->iHttpEngOne->CancelTransaction();
	this->iHttpEngTwo->CancelTransaction();
}

void CHttpManager::StartConncetion()
{
	iMainEngine.WriteLog16(_L("CHttpManager::StartConncetion"));
	if(IsActive())
		return;

	if(iState == EInitialized)
	{
		iMainEngine.WriteLog16(_L("CHttpManager::StartConncetion EInitialized"));
		iHttpEngOne->SetupConnectionL(&iStatus,iIAPValue);
	}
	//iTimer.After(iStatus, aDelay);	// Set for later
	SetActive();					// Tell scheduler a request is active
	iMainEngine.WriteLog16(_L("CHttpManager::StartConncetion end"));
}

void CHttpManager::RunL()
{
	iMainEngine.WriteLog16(KINITCONN,iState);
	if(iStatus==KErrNone)
	{
		if (iState == EInitialized) 
		{
			iConnectFlag=EFalse;

			iState=EConnectIAP;
			iConnectionStatusOne.iState=EStartIap;
			iHttpEngTwo->SetupConnectionL(&iStatus,iIAPValue);
			SetActive();	
		} 
		else if (iState == EConnectIAP) 
		{
			iState=EConnectSuccess;
			iConnectFlag=ETrue;
			this->CheckHttpArray();
		}
		else if (iState == EConnectSuccess) 
		{
			iConnectFlag=ETrue;	
		}
	}
	else
	{
		SetActive();	
	}
}

void CHttpManager::InitConnectionIap()
{
	//初始化IAP接入点
	// open the IAP communications database 
	CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDB);

	// initialize a view 
	CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);
	//CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);

	// go to the first record 
	TInt err=commDBView->GotoFirstRecord();

	if(err==KErrNotFound)
	{
		iState=EIAPNotFound;
		CEikonEnv::Static()->AlertWin(_L("No IAP!"));
	}
	else
	{
		// Declare a prefTableView Object.
		CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;

		// read the connection preferences 
		commDBView->ReadConnectionPreferenceL(pref);
		iIAPValue = (TInt)pref.iBearer.iIapId; 
		iState=EInitialized;
	}
	// pop and destroy the IAP View 
	CleanupStack::PopAndDestroy(commDBView);

	// pop and destroy the database object
	CleanupStack::PopAndDestroy(commDB);
}
TBool CHttpManager::CheckIAPValue()
{
	TBool returnBool=EFalse;
	//检验IAP接入点是否存在

	#ifdef __WINS__
			return ETrue;
	#endif

	CCommsDatabase* lCommDb= CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(lCommDb);  	
	CCommsDbTableView* iaptable = lCommDb->OpenIAPTableViewMatchingBearerSetLC(ECommDbBearerGPRS,ECommDbConnectionDirectionOutgoing);

	TInt retval = iaptable->GotoFirstRecord();

	iMainEngine.WriteLog16(KIAPERROR,retval);
	if(retval==KErrNotFound)
	{
		iMainEngine.WriteLog16(KIAPERROR,retval);
		iState=EIAPNotFound;
		CEikonEnv::Static()->AlertWin(_L("No IAP!"));
	}
	else
	{
		iMainEngine.WriteLog16(KIAPERROR,retval);
		while (retval == KErrNone)
		{ 		

			TUint32 iap_id;
			TRAPD(leaveCode1,iaptable->ReadUintL(TPtrC( COMMDB_ID), iap_id ));

			iMainEngine.WriteLog16(KIAPVALUE,iIAPValue);
			iMainEngine.WriteLog16(KIAPVALUE,(TInt)iap_id);

			if(iIAPValue==((TInt)iap_id))
			{
				returnBool=true;
				break;
			}
			retval = iaptable->GotoNextRecord();
			iMainEngine.WriteLog16(KIAPERROR,retval);
		} 	
	}
	CleanupStack::PopAndDestroy(2); 
	//// open the IAP communications database 
	//CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	//CleanupStack::PushL(commDB);

	//// initialize a view 
	//CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);
	////CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);
	//

	//// go to the first record 
	//TInt err=commDBView->GotoFirstRecord();

	//iMainEngine.WriteLog16(KIAPERROR,err);
	//if(err==KErrNotFound)
	//{
	//	iMainEngine.WriteLog16(KIAPERROR,err);
	//	iState=EIAPNotFound;
	//	CEikonEnv::Static()->AlertWin(_L("No IAP!"));
	//}
	//else
	//{

	//	iMainEngine.WriteLog16(KIAPERROR,err);
	//	while(err==KErrNone)
	//	{
	//		
	//		// Declare a prefTableView Object.
	//		CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;

	//		// read the connection preferences 
	//		commDBView->ReadConnectionPreferenceL(pref);
	//		TUint32 iap= pref.iBearer.iIapId; 

	//		iMainEngine.WriteLog16(KIAPVALUE,iIAPValue);
	//		iMainEngine.WriteLog16(KIAPVALUE,(TInt)iap);

	//		if(iIAPValue==((TInt)iap))
	//		{
	//			//iMainEngine.WriteLog16(KIAPVALUE,iIAPValue);
	//			returnBool=ETrue;
	//			break;
	//		}
	//		err=commDBView->GotoNextRecord();
	//		iMainEngine.WriteLog16(KIAPERROR,err);
	//	}
	//}
	//// pop and destroy the IAP View 
	//CleanupStack::PopAndDestroy(commDBView);

	//// pop and destroy the database object
	//CleanupStack::PopAndDestroy(commDB);

	return returnBool;
}
void CHttpManager::ChangeConnectionIap()
{
	//修改添加IAP接入点
}

void CHttpManager::ClientEvent(const TDesC& aEventDescription,TInt aIndex)
{
	iMainEngine.WriteLog16(_L("CHttpManager::ClientEvent"));
	iMainEngine.WriteLog16(aEventDescription);
	if (aEventDescription.Compare (_L("Connecting..."))==0)
	{
		if (aIndex==1)
		{
			iMainEngine.SetCurrentTraffice_Rev((TInt64)iHttpEngOne->GetSendSize());
		} 
		else
		{
			iMainEngine.SetCurrentTraffice_Rev((TInt64)iHttpEngTwo->GetSendSize());
		}
		
	}

	if(aEventDescription.Left(13).Compare(_L("Content-Type:"))==0)
	{
		HBufC* tempBuf=aEventDescription.Alloc();
		CleanupStack::PushL(tempBuf);
		tempBuf->Des().Delete(0,13);
		tempBuf->Des().TrimAll();
		if(tempBuf->Compare(_L("text/vnd.wap.wml"))==0)
		{
			switch(aIndex)
			{
				case 1:
					{
						iOneStatus=1;
						break;
					}
				case 2:
					{
						iTwoStatus=1;
						break;
					}
			}
		}
		CleanupStack::PopAndDestroy(1);
	}

	if(aEventDescription.Compare(_L("Transaction Successful"))==0/*||aEventDescription.Compare(_L("Transaction Complete"))==0*/)
	{
		switch(aIndex)
		{
			case 1:
				{
					if(iRequestElementOne)
					{
						if(iOneStatus==1)
						{
							//检查处理推送页面
							iOneStatus=0;
							TBuf8<500> tempUrl;
							TInt intValue=this->CheckWapPage(iRequestElementOne->GetUrl(),iRequestElementOne->GetReceiveData(),tempUrl);
							if(intValue==1)
							{	
								if(iRequestElementOne->GetRequestType()==1)
								{
									this->AddGetRequest(*iRequestElementOne->GetObserver(),tempUrl,aIndex,iRequestElementOne->GetHandleType());
								}
								else if(iRequestElementOne->GetRequestType()==2)
								{
									this->AddPostRequest(*iRequestElementOne->GetObserver(),tempUrl,iRequestElementOne->GetContentType(),iRequestElementOne->GetSendData(),aIndex,iRequestElementOne->GetHandleType());
								}
							}
							else
							{
								this->iRequestElementOne->GetObserver()->OperationEvent(EHttpNoError,iRequestElementOne->GetReceiveData(),iRequestElementOne->GetHandleType());
								iMainEngine.RequestDraw();
							}
						}
						else
						{
							HBufC* url=CnvUtfConverter::ConvertToUnicodeFromUtf8L(iRequestElementOne->GetUrl());
							CleanupStack::PushL(url);
							if(iRequestElementOne->GetReceiveData().Find(*url)>-1)
							{
								CleanupStack::PopAndDestroy(1);
								//检查处理推送页面后的跳转页面
								if(iRequestElementOne->GetRequestType()==1)
								{
									this->AddGetRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),aIndex,iRequestElementOne->GetHandleType());
								}
								else if(iRequestElementOne->GetRequestType()==2)
								{
									this->AddPostRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),iRequestElementOne->GetContentType(),iRequestElementOne->GetSendData(),aIndex,iRequestElementOne->GetHandleType());
								}
							}
							else
							{
								CleanupStack::PopAndDestroy(1);
								this->iRequestElementOne->GetObserver()->OperationEvent(EHttpNoError,iRequestElementOne->GetReceiveData(),iRequestElementOne->GetHandleType());
								iMainEngine.RequestDraw();
							}
						}
					}

					break;
				}
			case 2:
				{

					if(iRequestElementTwo)
					{
						if(iTwoStatus==1)
						{
							iTwoStatus=0;
							TBuf8<500> tempUrl;
							TInt intValue=this->CheckWapPage(iRequestElementTwo->GetUrl(),iRequestElementTwo->GetReceiveData(),tempUrl);
							if(intValue==1)
							{
								if(iRequestElementTwo->GetRequestType()==1)
								{
									this->AddGetRequest(*iRequestElementTwo->GetObserver(),tempUrl,aIndex,iRequestElementTwo->GetHandleType());
								}
								else if(iRequestElementTwo->GetRequestType()==2)
								{
									this->AddPostRequest(*iRequestElementTwo->GetObserver(),tempUrl,iRequestElementTwo->GetContentType(),iRequestElementTwo->GetSendData(),aIndex,iRequestElementTwo->GetHandleType());
								}
							}
							else
							{
								this->iRequestElementTwo->GetObserver()->OperationEvent(EHttpNoError,iRequestElementTwo->GetReceiveData(),iRequestElementTwo->GetHandleType());
								iMainEngine.RequestDraw();
							}
						}
						else
						{
							HBufC* url=CnvUtfConverter::ConvertToUnicodeFromUtf8L(iRequestElementTwo->GetUrl());
							CleanupStack::PushL(url);
							if(iRequestElementTwo->GetReceiveData().Find(*url)>-1)
							{
								CleanupStack::PopAndDestroy(1);
								if(iRequestElementTwo->GetRequestType()==1)
								{
									this->AddGetRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),aIndex,iRequestElementTwo->GetHandleType());
								}
								else if(iRequestElementTwo->GetRequestType()==2)
								{
									this->AddPostRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),iRequestElementTwo->GetContentType(),iRequestElementTwo->GetSendData(),aIndex,iRequestElementTwo->GetHandleType());
								}
							}
							else
							{
								CleanupStack::PopAndDestroy(1);
								this->iRequestElementTwo->GetObserver()->OperationEvent(EHttpNoError,iRequestElementTwo->GetReceiveData(),iRequestElementTwo->GetHandleType());
								iMainEngine.RequestDraw();
							}
						}
					}
					break;
				}
		}
		this->CheckHttpArray();	
	}
	else if(aEventDescription.Left(12).Compare(_L("Http-Status:"))==0)
	{
		TInt state=0;
		HBufC* tempBuf=aEventDescription.Alloc();
		CleanupStack::PushL(tempBuf);
		tempBuf->Des().Delete(0,12);
		tempBuf->Des().TrimAll();
		state=::ConvertTDesCToTInt(*tempBuf);
		if(state>499)
		{
			switch(aIndex)
			{
				case 1:
					{
						if(iHttpEngOne)
						{
							iHttpEngOne->CancelTransaction();
							this->iRequestElementOne->GetObserver()->OperationEvent(EHttpErrorStatus,GetErrorBuf(state,_L("")),iRequestElementOne->GetHandleType());
							iMainEngine.RequestDraw();
							this->CheckHttpArray();
						}

						break;
					}
				case 2:
					{

						if(iHttpEngTwo)
						{
							iHttpEngTwo->CancelTransaction();
							this->iRequestElementTwo->GetObserver()->OperationEvent(EHttpErrorStatus,GetErrorBuf(state,_L("")),iRequestElementTwo->GetHandleType());
							iMainEngine.RequestDraw();
							this->CheckHttpArray();
						}

						break;
					}
			}
		}
		CleanupStack::PopAndDestroy(1);
	}
	else if(aEventDescription.Compare(_L("Error:KErrNotReady"))==0)
	{	
		iHttpEngOne->CancelTransaction();
		iHttpEngTwo->CancelTransaction();

		if(iRequestElementTwo)
		{
			if(iRequestElementTwo->GetRequestType()==1)
				this->AddGetRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),0,iRequestElementTwo->GetHandleType());
			else if(iRequestElementTwo->GetRequestType()==2)
				this->AddPostRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),iRequestElementTwo->GetContentType(),iRequestElementTwo->GetSendData(),0,iRequestElementTwo->GetHandleType());
			
			delete iRequestElementTwo;
			iRequestElementTwo=NULL;
		}

		if(iRequestElementOne)
		{
			if(iRequestElementOne->GetRequestType()==1)
				this->AddGetRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),0,iRequestElementOne->GetHandleType());
			else if(iRequestElementOne->GetRequestType()==2)
				this->AddPostRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),iRequestElementOne->GetContentType(),iRequestElementOne->GetSendData(),0,iRequestElementOne->GetHandleType());
			
			delete iRequestElementOne;
			iRequestElementOne=NULL;
		}

		iState = EInitialized;
		this->StartConncetion();
	}
	else if(aEventDescription.Compare(_L("Error:KErrDisconnected"))==0)
	{	
		switch(aIndex)
		{
			case 1:
				{
					if(iHttpEngOne)
					{
						if(iRequestElementOne)
						{
							if(iRequestElementOne->GetRequestType()==1)
								this->AddGetRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),0,iRequestElementOne->GetHandleType());
							else if(iRequestElementOne->GetRequestType()==2)
								this->AddPostRequest(*iRequestElementOne->GetObserver(),iRequestElementOne->GetUrl(),iRequestElementOne->GetContentType(),iRequestElementOne->GetSendData(),0,iRequestElementOne->GetHandleType());
							
							delete iRequestElementOne;
							iRequestElementOne=NULL;
						}
					}

					break;
				}
			case 2:
				{
					if(iHttpEngTwo)
					{
						if(iRequestElementTwo)
						{
							if(iRequestElementTwo->GetRequestType()==1)
								this->AddGetRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),0,iRequestElementTwo->GetHandleType());
							else if(iRequestElementTwo->GetRequestType()==2)
								this->AddPostRequest(*iRequestElementTwo->GetObserver(),iRequestElementTwo->GetUrl(),iRequestElementTwo->GetContentType(),iRequestElementTwo->GetSendData(),0,iRequestElementTwo->GetHandleType());
							
							delete iRequestElementTwo;
							iRequestElementTwo=NULL;
						}
					}

					break;
				}
		}
		this->CheckHttpArray();
	}
	else if(aEventDescription.Compare(_L("Transaction Failed"))==0||aEventDescription.Left(6).Compare(_L("Error:"))==0)
	{
		switch(aIndex)
		{
			case 1:
				{
					if(iHttpEngOne)
					{
						this->iRequestElementOne->GetObserver()->OperationEvent(EHttpConnectionError,GetErrorBuf(400,_L("")),iRequestElementOne->GetHandleType());
						iMainEngine.RequestDraw();
					}

					break;
				}
			case 2:
				{
					if(iHttpEngTwo)
					{
						this->iRequestElementTwo->GetObserver()->OperationEvent(EHttpConnectionError,GetErrorBuf(400,_L("")),iRequestElementTwo->GetHandleType());
						iMainEngine.RequestDraw();
					}

					break;
				}
		}
		this->CheckHttpArray();
	}	
	else if(aEventDescription.Compare(_L("TimeOut"))==0)
	{
		//20070917张晖增加超时处理
		switch(aIndex)
		{
		case 1:
			{
				if(iHttpEngOne)
				{
					this->iRequestElementOne->GetObserver()->OperationEvent(EHttpTimeOut,GetErrorBuf(404,_L("")),iRequestElementOne->GetHandleType());
					iMainEngine.RequestDraw();
				}

				break;
			}
		case 2:
			{
				if(iHttpEngTwo)
				{
					this->iRequestElementTwo->GetObserver()->OperationEvent(EHttpTimeOut,GetErrorBuf(404,_L("")),iRequestElementTwo->GetHandleType());
					iMainEngine.RequestDraw();
				}

				break;
			}
		}
		this->CheckHttpArray();
	}
	iMainEngine.WriteLog16(_L("CHttpManager::ClientEvent End"));
}
const TDesC& CHttpManager::GetErrorBuf(TInt aCode,const TDesC& aText)
{
	if(iErrorBuf)
	{
		delete iErrorBuf;
		iErrorBuf=NULL;
	}
	iErrorBuf=HBufC::NewL(100);
	
	iErrorBuf->Des().Append(iMainEngine.GetDesById(ETurkeyTextRes_Search_NetErrorReturn));
	iErrorBuf->Des().Append(_L("("));
	iErrorBuf->Des().Append(iMainEngine.GetDesById(ECoCoTestRes_ErrorCode));
	if(aCode!=0)
		iErrorBuf->Des().AppendNum(aCode);
	else
		iErrorBuf->Des().Append(aText);

	iErrorBuf->Des().Append(_L(")"));

	return *iErrorBuf;
}
void CHttpManager::ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex)
{
	iMainEngine.SetCurrentTraffice_Rev((TInt64)aBodyData.Length());

	switch(aIndex)
	{
		case 1:
			{
				if(iRequestElementOne)
					iRequestElementOne->SetReceiveData(aBodyData);
				break;
			}
		case 2:
			{
				if(iRequestElementTwo)
					iRequestElementTwo->SetReceiveData(aBodyData);
				break;
			}
	}
}

void CHttpManager::SendGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aType)
{
	iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest"));
	if(CheckIAPValue())
	{
		iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_CheckIAPValue"));
		if((iHttpEngOne->IsRunning()&&iHttpEngTwo->IsRunning())||iState!=EConnectSuccess)
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_ADD"));
			this->AddGetRequest(aBoserver,aUrl,-1,aType);
		}
		else if(!iHttpEngOne->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_One"));
			if(iRequestElementOne)
			{
				delete iRequestElementOne;
				iRequestElementOne=NULL;
			}
			iRequestElementOne=CHttpRequestElement::NewL(aBoserver);
			iRequestElementOne->SetRequestType(1);
			iRequestElementOne->SetUrl(aUrl);
			iRequestElementOne->SetHandleType(aType);

			iHttpEngOne->IssueHTTPGetL(aUrl);
		}
		else if(!iHttpEngTwo->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_Two"));
			if(iRequestElementTwo)
			{
				delete iRequestElementTwo;
				iRequestElementTwo=NULL;
			}
			iRequestElementTwo=CHttpRequestElement::NewL(aBoserver);
			iRequestElementTwo->SetRequestType(1);
			iRequestElementTwo->SetUrl(aUrl);
			iRequestElementTwo->SetHandleType(aType);

			iHttpEngTwo->IssueHTTPGetL(aUrl);
		}
	}
	iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_End"));
}
void CHttpManager::SendGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,const TDesC& aFileName,TInt aType)
{
	iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F"));
	if(CheckIAPValue())
	{
		iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F_CheckIAPValue"));
		if((iHttpEngOne->IsRunning()&&iHttpEngTwo->IsRunning())||iState!=EConnectSuccess)
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F_Add"));
			this->AddGetRequest(aBoserver,aUrl,-1,aFileName,aType);
		}
		else if(!iHttpEngOne->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F_One"));
			if(iRequestElementOne)
			{
				delete iRequestElementOne;
				iRequestElementOne=NULL;
			}
			iRequestElementOne=CHttpRequestElement::NewL(aBoserver);
			iRequestElementOne->SetRequestType(1);
			iRequestElementOne->SetUrl(aUrl);
			iRequestElementOne->SetHandleType(aType);
			iRequestElementOne->SetFileName(aFileName);

			iHttpEngOne->IssueHTTPGetL(aUrl);
		}
		else if(!iHttpEngTwo->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F_Two"));
			if(iRequestElementTwo)
			{
				delete iRequestElementTwo;
				iRequestElementTwo=NULL;
			}
			iRequestElementTwo=CHttpRequestElement::NewL(aBoserver);
			iRequestElementTwo->SetRequestType(1);
			iRequestElementTwo->SetUrl(aUrl);
			iRequestElementTwo->SetHandleType(aType);
			iRequestElementTwo->SetFileName(aFileName);

			iHttpEngTwo->IssueHTTPGetL(aUrl);
		}
	}
	iMainEngine.WriteLog16(_L("CHttpManager::SendGetRequest_F_End"));
}

void CHttpManager::SendPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aType)
{
	iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest"));
	if(CheckIAPValue())
	{
		iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_CheckIAPValue"));
		if((iHttpEngOne->IsRunning()&&iHttpEngTwo->IsRunning())||iState!=EConnectSuccess)
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_Add"));
			this->AddPostRequest(aBoserver,aUrl,aContentType,aData,-1, aType);
		}
		else if(!iHttpEngOne->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_One"));
			if(iRequestElementOne)
			{
				delete iRequestElementOne;
				iRequestElementOne=NULL;
			}
			iRequestElementOne=CHttpRequestElement::NewL(aBoserver);
			iRequestElementOne->SetRequestType(2);
			iRequestElementOne->SetUrl(aUrl);
			iRequestElementOne->SetContentType(aContentType);
			iRequestElementOne->SetSendData(aData);
			iRequestElementOne->SetHandleType(aType);
			

			iHttpEngOne->IssueHTTPPostL(aUrl,aContentType,aData);
		}
		else if(!iHttpEngTwo->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_Two"));
			if(iRequestElementTwo)
			{
				delete iRequestElementTwo;
				iRequestElementTwo=NULL;
			}
			iRequestElementTwo=CHttpRequestElement::NewL(aBoserver);
			iRequestElementTwo->SetRequestType(2);
			iRequestElementTwo->SetUrl(aUrl);
			iRequestElementTwo->SetContentType(aContentType);
			iRequestElementTwo->SetSendData(aData);
			iRequestElementTwo->SetHandleType(aType);
			

			iHttpEngTwo->IssueHTTPPostL(aUrl,aContentType,aData);
		}
	}
	iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_End"));
}
void CHttpManager::SendPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData, const TDesC& aFileName,TInt aType)
{
	iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F"));
	if(CheckIAPValue())
	{
		iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F_CheckIAPValue"));
		if((iHttpEngOne->IsRunning()&&iHttpEngTwo->IsRunning())||iState!=EConnectSuccess)
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F_Add"));
			this->AddPostRequest(aBoserver,aUrl,aContentType,aData,-1,aFileName, aType);
		}
		else if(!iHttpEngOne->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F_One"));
			if(iRequestElementOne)
			{
				delete iRequestElementOne;
				iRequestElementOne=NULL;
			}
			iRequestElementOne=CHttpRequestElement::NewL(aBoserver);
			iRequestElementOne->SetRequestType(2);
			iRequestElementOne->SetUrl(aUrl);
			iRequestElementOne->SetContentType(aContentType);
			iRequestElementOne->SetSendData(aData);
			iRequestElementOne->SetHandleType(aType);
			iRequestElementOne->SetFileName(aFileName);

			iHttpEngOne->IssueHTTPPostL(aUrl,aContentType,aData);
		}
		else if(!iHttpEngTwo->IsRunning())
		{
			iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F_Two"));
			if(iRequestElementTwo)
			{
				delete iRequestElementTwo;
				iRequestElementTwo=NULL;
			}
			iRequestElementTwo=CHttpRequestElement::NewL(aBoserver);
			iRequestElementTwo->SetRequestType(2);
			iRequestElementTwo->SetUrl(aUrl);
			iRequestElementTwo->SetContentType(aContentType);
			iRequestElementTwo->SetSendData(aData);
			iRequestElementTwo->SetHandleType(aType);
			iRequestElementTwo->SetFileName(aFileName);

			iHttpEngTwo->IssueHTTPPostL(aUrl,aContentType,aData);
		}
	}
	iMainEngine.WriteLog16(_L("CHttpManager::SendPostRequest_F_End"));
}
void CHttpManager::CancelTransaction(MOperationObserver* aBoserver,TInt aType)
{
	if(iRequestElementOne)
	{
		if(iRequestElementOne->GetObserver()==aBoserver&&iRequestElementOne->GetHandleType()==aType)
		{
			this->iHttpEngOne->CancelTransaction();
			delete iRequestElementOne;
			iRequestElementOne=NULL;
			return;
		}
	}

	if(iRequestElementTwo)
	{
		if(iRequestElementTwo->GetObserver()==aBoserver&&iRequestElementTwo->GetHandleType()==aType)
		{
			this->iHttpEngTwo->CancelTransaction();
			delete iRequestElementTwo;
			iRequestElementTwo=NULL;
			return;
		}
	}

	for(TInt i=0;i<iRequestArray.Count();i++)
	{
		if(iRequestArray[i]->GetObserver()==aBoserver&&iRequestArray[i]->GetHandleType()==aType)
		{
			CHttpRequestElement* iRequestElement=iRequestArray[i];
			iRequestArray.Remove(i);
			delete iRequestElement;
			iRequestElement=NULL;
			return;
		}
	}
	
}
void CHttpManager::CheckHttpArray()
{
	iMainEngine.WriteLog16(_L("CHttpManager::CheckHttpArray_Start"));
	if(iRequestArray.Count()>0)
	{
		if(!iHttpEngOne->IsRunning())
		{
			if(iRequestElementOne)
			{
				delete iRequestElementOne;
				iRequestElementOne=NULL;
			}
			iRequestElementOne=iRequestArray[0];

			if(iRequestElementOne->GetRequestType()==1)
			{
				if(iRequestElementOne->GetUrl().Length()>0)
				{
					iHttpEngOne->IssueHTTPGetL(iRequestElementOne->GetUrl());
				}
			}
			else if(iRequestElementOne->GetRequestType()==2)
			{
				if(iRequestElementOne->GetUrl().Length()>0)
					iHttpEngOne->IssueHTTPPostL(iRequestElementOne->GetUrl(),iRequestElementOne->GetContentType(),iRequestElementOne->GetSendData());
			}
			iRequestArray.Remove(0);
		}

		if(!iHttpEngTwo->IsRunning()&&iRequestArray.Count()>0)
		{
			if(iRequestElementTwo)
			{
				delete iRequestElementTwo;
				iRequestElementTwo=NULL;
			}

			iRequestElementTwo=iRequestArray[0];

			if(iRequestElementTwo->GetRequestType()==1)
			{
				if(iRequestElementTwo->GetUrl().Length()>0)
				{
					iHttpEngTwo->IssueHTTPGetL(iRequestElementTwo->GetUrl());
				}
			}
			else if(iRequestElementTwo->GetRequestType()==2)
			{
				if(iRequestElementTwo->GetUrl().Length()>0)
					iHttpEngTwo->IssueHTTPPostL(iRequestElementTwo->GetUrl(),iRequestElementTwo->GetContentType(),iRequestElementTwo->GetSendData());
			}
			iRequestArray.Remove(0);
		}
	}
	iMainEngine.WriteLog16(_L("CHttpManager::CheckHttpArray_End"));
}

void CHttpManager::AddGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aIndex,TInt aType)
{
	//创建一个联网请求队列元素
	CHttpRequestElement* httpRequestElement=CHttpRequestElement::NewL(aBoserver);
	httpRequestElement->SetRequestType(1);
	httpRequestElement->SetUrl(aUrl);
	httpRequestElement->SetHandleType(aType);

	if(aIndex<0||(aIndex>iRequestArray.Count()-1))
		iRequestArray.Append(httpRequestElement);
	else
		iRequestArray.Insert(httpRequestElement,aIndex);
}
void CHttpManager::AddGetRequest(MOperationObserver& aBoserver, const TDesC8& aUrl,TInt aIndex,const TDesC& aFileName,TInt aType)
{
	//创建一个联网请求队列元素
	CHttpRequestElement* httpRequestElement=CHttpRequestElement::NewL(aBoserver);
	httpRequestElement->SetRequestType(1);
	httpRequestElement->SetUrl(aUrl);
	httpRequestElement->SetHandleType(aType);
	httpRequestElement->SetFileName(aFileName);

	if(aIndex<0||(aIndex>iRequestArray.Count()-1))
		iRequestArray.Append(httpRequestElement);
	else
		iRequestArray.Insert(httpRequestElement,aIndex);
}
void CHttpManager::AddPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aIndex,TInt aType)
{
	//创建一个联网请求队列元素
	CHttpRequestElement* httpRequestElement=CHttpRequestElement::NewL(aBoserver);
	httpRequestElement->SetRequestType(2);
	httpRequestElement->SetUrl(aUrl);
	httpRequestElement->SetContentType(aContentType);
	httpRequestElement->SetSendData(aData);
	httpRequestElement->SetHandleType(aType);

	if(aIndex<0||(aIndex>iRequestArray.Count()-1))
		iRequestArray.Append(httpRequestElement);
	else
		iRequestArray.Insert(httpRequestElement,aIndex);
}
void CHttpManager::AddPostRequest(MOperationObserver& aBoserver, const TDesC8& aUrl, const TDesC8& aContentType, const TDesC8& aData,TInt aIndex,const TDesC& aFileName,TInt aType)
{
	//创建一个联网请求队列元素
	CHttpRequestElement* httpRequestElement=CHttpRequestElement::NewL(aBoserver);
	httpRequestElement->SetRequestType(2);
	httpRequestElement->SetUrl(aUrl);
	httpRequestElement->SetContentType(aContentType);
	httpRequestElement->SetSendData(aData);
	httpRequestElement->SetHandleType(aType);
	httpRequestElement->SetFileName(aFileName);

	if(aIndex<0||(aIndex>iRequestArray.Count()-1))
		iRequestArray.Append(httpRequestElement);
	else
		iRequestArray.Insert(httpRequestElement,aIndex);
}
//返回值-1推送页面，2非推送页面，3其它错误
TInt CHttpManager::CheckWapPage(const TDesC8& aUrl, const TDesC& aBodyData,TDes8& aNewUrl)
{

	HBufC* newUrl = HBufC::NewLC(500);
	TBuf8<500> newUrl8;

	HBufC* url=CnvUtfConverter::ConvertToUnicodeFromUtf8L(aUrl);
	CleanupStack::PushL(url);

	TInt errT=iMainEngine.ParseMobiWapPage(*url,aBodyData,newUrl);
	if(errT==1)
	{
		CnvUtfConverter::ConvertFromUnicodeToUtf8(newUrl8,*newUrl);
		aNewUrl.Copy(newUrl8);
	}

	CleanupStack::PopAndDestroy(2);

	return errT;
}