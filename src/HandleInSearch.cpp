/*
============================================================================
 Name        : HandleInSearch.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleInSearch implementation

 http://59.36.96.182/3gserver/search?p=1&m=1&username=358361002809113&typeid=0&keyword=%E5%A5%BD&pageid=1
 http://59.36.96.182/3gserver/search?corpid=000000&username=123456&typeid=0&keyword=%E5%A6%B9%E5%A6%B9&pageid=1
============================================================================
*/

#include "HandleInSearch.h"
#include "MainEngine.h"
#include <utf.h>
#include <EscapeUtils.h>
#include "tinyxml.h"
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "SearchResultItem.h"
#include "SearchResultTempItem.h"
#include "StaticCommonTools.h"
#include "CoCoPreDefine.h"

CHandleInSearch::CHandleInSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iBuf(NULL)
,iUrl(NULL)
,iKeyWord(NULL)
{
	// No implementation required
}


CHandleInSearch::~CHandleInSearch()
{
	this->CancelSendRequest();
	::DelFile(iFileName);
	iPointArray.Close();

	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}

	if(iBuf)
	{
		delete iBuf;
		iBuf=NULL;
	}
	if(iKeyWord)
	{
		delete iKeyWord;
		iKeyWord=NULL;
	}
}


CHandleInSearch* CHandleInSearch::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleInSearch* self = new (ELeave)CHandleInSearch( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleInSearch* CHandleInSearch::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleInSearch* self=CHandleInSearch::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleInSearch::ConstructL()
{
	iContentError=EFalse;
	iFileName.Append(iMainEngine.GetAppDrive());
	iFileName.Append(KTEMPXML);
}

void CHandleInSearch::SetSendUrl(const TDesC8& aUrl)
{
	iCurUrl=HBufC8::NewL(aUrl.Length());
	iCurUrl->Des().Copy(aUrl);
}
//From MOperationObserver
void CHandleInSearch::OperationEvent(TInt aEventType,const TDesC& aEventData ,TInt aType)
{
	if(aEventType==EHttpNoError)
	{
		if(iPointArray.Count()>20)
		{
			for(TInt i=0;i<iPointArray.Count()-20;i++)
			{
				iPointArray.Remove(0);
			}
		}

		::WriteFile(iFileName,aEventData);
		this->HandleXmlData();
	}
	else
	{
		if(iBuf)
		{
			delete iBuf;
			iBuf=NULL;
		}
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);

		iObserver.HandleResponseEvent(aEventType,aType);
	}
}

TInt CHandleInSearch::GetCount()
{
	return iPointArray[iCurIndex]->GetItemCount();
}
const TDesC& CHandleInSearch::GetOneItem(TInt aIndex)
{
	return iPointArray[iCurIndex]->GetItem(aIndex).GetName();
}

void CHandleInSearch::HandleXmlData()
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
		loadOkay=doc->LoadFile("e:\\insearch.xml",TIXML_ENCODING_UTF8);
	}
	else
	{
		loadOkay=doc->LoadFile("c:\\insearch.xml",TIXML_ENCODING_UTF8);
	}*/

	if(loadOkay==false)
	{
		if(iBuf)
		{
			delete iBuf;
			iBuf=NULL;
		}
		iBuf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)doc->ErrorDesc()));

		iContentError=ETrue;
		iObserver.HandleResponseEvent(EHttpConnectionError,EHttpInSearch);
		delete doc;
		
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement();

	TiXmlElement* pRecord=pElement->FirstChildElement("cp");

	if (pRecord!=0)
	{
		this->iCurPage = atoi( pRecord->FirstChild()->Value() );
	}

	pRecord=pElement->FirstChildElement("ap");
	if(pRecord!=0)
	{
		this->iAllPage = atoi( pRecord->FirstChild()->Value() );
	}

	pRecord=pElement->FirstChildElement("l");

	CSearchResultTempItem* item=CSearchResultTempItem::NewL();
	item->SetAllPage(iAllPage);
	item->SetCurPage(iCurPage);

	while(pRecord!=0)
	{
		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
		CleanupStack::PushL(buf);
		item->AddItem(*buf,_L8(""),EFalse);
		pRecord=pRecord->NextSiblingElement("l");
		CleanupStack::PopAndDestroy(1);
	}
	iPointArray.Append(item);
	iCurIndex=iPointArray.Count()-1;

	iObserver.HandleResponseEvent(EHttpNoError,EHttpInSearch);

	doc->Clear();
	delete doc;
	
}
const TDesC& CHandleInSearch::GetInfo()
{
	return *iBuf;
}

const TDesC8& CHandleInSearch::GetSendUrl()
{
	return *iUrl;
}

void  CHandleInSearch::SendRequest(const TDesC8& aKeyWord,TInt aIndex,TInt aPage)
{
	if(iKeyWord==NULL)
	{
		iKeyWord=aKeyWord.Alloc();
	}
	else if(iKeyWord->Compare(aKeyWord)!=0)
	{
		ClearTempBuf();
		delete iKeyWord;
		iKeyWord=NULL;
		iKeyWord=aKeyWord.Alloc();
	}
	else
	{
		if(CheckTempBuf(aPage))
		{
			iObserver.HandleResponseEvent(EHttpNoError,EHttpInSearch);
			return;
		}
	}

	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}
	iUrl=HBufC8::NewL(300);

	iUrl->Des().Append(*iCurUrl);
	iUrl->Des().Append(aKeyWord);
	iUrl->Des().Append(_L8("&pageid="));
	iUrl->Des().AppendNum(aPage);
	
	iHttpManager.SendGetRequest(*this,*iUrl,EHttpInSearch);
}

void  CHandleInSearch::CancelSendRequest()
{
	iHttpManager.CancelTransaction(this,EHttpInSearch);
}
void CHandleInSearch::ClearTempBuf()
{
	iPointArray.Close();
}

TBool CHandleInSearch::CheckTempBuf(TInt aPage)
{
	TBool tBool=EFalse;
	for(TInt i=0;i<this->iPointArray.Count();i++)
	{
		if(iPointArray[i]->GetCurPage()==aPage)
		{
			tBool=ETrue;
			iCurIndex=i;
			break;
		}
	}
	return tBool;
}
TInt CHandleInSearch::GetCurPage()
{
	TInt page=1;

	if(iCurIndex<iPointArray.Count())
		page=iPointArray[iCurIndex]->GetCurPage();

	return page;
}
TInt CHandleInSearch::GetAllPage()
{
	TInt page=0;

	if(iCurIndex<iPointArray.Count())
		page=iPointArray[iCurIndex]->GetAllPage();

	return page;
}