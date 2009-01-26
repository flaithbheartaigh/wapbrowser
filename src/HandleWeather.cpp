/*
============================================================================
 Name        : HandleWeather.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleWeather implementation
============================================================================
*/

#include "HandleWeather.h"
#include "MainEngine.h"
#include <HttpObserver.h>
#include "HttpManager.h"
#include "CoCoPreDefine.h"
#include "StaticCommonTools.h"
#include <utf.h>
#include "tinyxml.h"

_LIT(KWeatherURL,"http://59.36.96.182/coco/w?p=%S&username=%S&c=");

CHandleWeather::CHandleWeather(CMainEngine& aMainEngine)
:iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
{
	// No implementation required
}


CHandleWeather::~CHandleWeather()
{
	this->CancelSendRequest();
	::DelFile(iFileName);
}

CHandleWeather* CHandleWeather::NewLC(CMainEngine& aMainEngine)
{
	CHandleWeather* self = new (ELeave)CHandleWeather(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleWeather* CHandleWeather::NewL(CMainEngine& aMainEngine)
{
	CHandleWeather* self=CHandleWeather::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleWeather::ConstructL()
{
	iContentError=EFalse;
	iFileName.Append(iMainEngine.GetAppDrive());
	iFileName.Append(KTEMPXML);
}
//From MOperationObserver
void CHandleWeather::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	if(aEventType==EHttpNoError)
	{
		::WriteFile(iFileName,aEventData);
		this->HandleXml();
	}
}
void  CHandleWeather::SendRequest()
{
	TBuf<200> tempUrl;
	tempUrl.Zero();
	tempUrl.Format(KWeatherURL,&iMainEngine.GetP(),&iMainEngine.GetIMEI());
	
	TBuf8<200> tempUrl8;
	tempUrl8.Zero();
	tempUrl8.Copy(tempUrl);
	iHttpManager.SendGetRequest(*this,tempUrl8,EHttpWeather);
}
void  CHandleWeather::AddRequest()
{
	TBuf<200> tempUrl;
	tempUrl.Zero();
	tempUrl.Format(KWeatherURL,&iMainEngine.GetP(),&iMainEngine.GetIMEI());
	
	TBuf8<200> tempUrl8;
	tempUrl8.Zero();
	tempUrl8.Copy(tempUrl);
	iHttpManager.AddGetRequest(*this,tempUrl8,-1,EHttpWeather);
}
void  CHandleWeather::CancelSendRequest()
{
	iHttpManager.CancelTransaction(this,EHttpWeather);
}
void  CHandleWeather::HandleXml()
{
	TiXmlDocument* doc=new TiXmlDocument();
	bool loadOkay;

	TBuf8<100> xmlPath;
	xmlPath.Zero();
	xmlPath.Copy(iFileName);

	char* path = NULL;
	path=(char *)xmlPath.PtrZ();
	loadOkay=doc->LoadFile(path,TIXML_ENCODING_UTF8);

	/*if(iMainEngine.GetAppDrive().Left(1).Compare(_L("e"))==0||iMainEngine.GetAppDrive().Left(1).Compare(_L("E"))==0)
	{
		loadOkay=doc->LoadFile("c:\\insearch.xml",TIXML_ENCODING_UTF8);
	}
	else
	{
		loadOkay=doc->LoadFile("c:\\insearch.xml",TIXML_ENCODING_UTF8);
	}*/

	if(loadOkay==false)
	{
		delete doc;
		
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement();

	TiXmlElement* pRecord=pElement->FirstChildElement("picid");

	TBuf<50> tempWeather;
	tempWeather.Zero();

	if (pRecord!=0)
	{
		TInt type= atoi( pRecord->FirstChild()->Value() );
		tempWeather.AppendNum(type);
		tempWeather.Append(_L("|"));
	}

	pRecord=pElement->FirstChildElement("city");
	if(pRecord!=0)
	{
		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
		CleanupStack::PushL(buf);
		tempWeather.Append(*buf);
		CleanupStack::PopAndDestroy(1);
	}

	pRecord=pElement->FirstChildElement("temperature");

	if(pRecord!=0)
	{
		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
		CleanupStack::PushL(buf);
		tempWeather.Append(*buf);
		CleanupStack::PopAndDestroy(1);
	}

	iMainEngine.SetWeather(tempWeather);
	doc->Clear();
	delete doc;
	
}