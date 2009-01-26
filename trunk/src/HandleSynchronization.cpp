/*
============================================================================
 Name        : HandleSynchronization.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleSynchronization implementation
============================================================================
*/

#include "HandleSynchronization.h"
#include "MainEngine.h"
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"
#include "StaticCommonTools.h"
#include "ContentItem.h"
_LIT(KFIRSTURL,"http://59.36.96.182/interface/n1?n=%S&p=%S&f=1");
_LIT(KSECONDURL,"http://59.36.96.182/interface/n1?n=%S&p=%S&f=2&t=%D&uptime=%D&m=1");
//http://218.16.120.159:8080/interface/n1?p=1&n=359753002458942&t=5&uptime=0&f=2&m=1
//_LIT(KFIRSTURL,"http://218.16.120.159:8080/interface/n1?n=%S&p=%S&f=1");
//_LIT(KSECONDURL,"http://218.16.120.159:8080/interface/n1?n=%S&p=%S&f=2&t=%D&uptime=%D&m=1");


_LIT(KXMLHEADER,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?><content>");
_LIT(KXMLUPTIME,"<uptime>%D</uptime>");
_LIT(KXMLFIRSTHEADER,"<first id=\"%D\" name=\"%S\">");
_LIT(KXMLSECONDHEADER,"<second id=\"%D\" name=\"%S\">");
_LIT(KXMLTHIRD,"<third id=\"%D\">%S</third>");
_LIT(KXMLSECONDEND,"</second>");
_LIT(KXMLFIRSTEND,"</first>");
_LIT(KXMLEND,"</content>");

CHandleSynchronization::CHandleSynchronization(CMainEngine& aMainEngine)
:iObserver(NULL)
,iMainEngine(aMainEngine)
,iCurState(-1)
,iTimeFlag(0)
,iPreTimeFlag(0)
{
	// No implementation required

}


CHandleSynchronization::~CHandleSynchronization()
{
	this->CancelSendRequest();
	::DelFile(iTempFileName);
	iArray.Close();

	DELETE(iBuf);
	DELETE(iTempBuf);
	DELETE(iDataBuf);
}

CHandleSynchronization* CHandleSynchronization::NewLC(CMainEngine& aMainEngine)
{
	CHandleSynchronization* self = new (ELeave)CHandleSynchronization(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleSynchronization* CHandleSynchronization::NewL(CMainEngine& aMainEngine)
{
	CHandleSynchronization* self=CHandleSynchronization::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleSynchronization::ConstructL()
{
	iSynFileName.Zero();
	iSynFileName.Append(iMainEngine.GetAppDrive());
	iSynFileName.Append(KSYNXML);

	iTempFileName.Zero();
	iTempFileName.Append(iMainEngine.GetAppDrive());
	iTempFileName.Append(KTEMPXML);

	iContentError=EFalse;
}

void CHandleSynchronization::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	if(aEventType==EHttpNoError)
	{
		::WriteFile(iTempFileName,aEventData);
		if(aType==EHttpFirstContentUpdate)
		{
			this->HandleFirstResponseXmlData();
		}
		else if(aType==EHttpSecondContentUpdate)
		{
			this->HandleSecondResponseXmlData();
		}
		this->ReWriteXMl();
		iArray.Close();
		this->HandleFirstXmlData();
	}
	else
	{
		/*iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);*/
	}

	if(iObserver)
		iObserver->HandleResponseEvent(aEventType,aType);
}
void  CHandleSynchronization::SendFirstRequest()
{
	iArray.ResetAndDestroy();
	iCurState=EHttpFirstContentUpdate;
	TBuf<200> tempBuf;
	tempBuf.Zero();
	tempBuf.Format(KFIRSTURL,&iMainEngine.GetIMEI(),&iMainEngine.GetP());
	TBuf8<200> tempBuf8;
	tempBuf8.Zero();
	tempBuf8.Copy(tempBuf);
	iMainEngine.GetHttpManager()->SendGetRequest(*this,tempBuf8,EHttpFirstContentUpdate);
}
void  CHandleSynchronization::SendSecondRequest(TInt aID)
{
	iCurState=EHttpSecondContentUpdate;
	//_LIT8(,"http://59.36.96.182/interface/n1?n=%S&p=%S&f=2&t=%D&uptime=%D");
	
	TBuf<200> tempBuf;
	tempBuf.Zero();
	tempBuf.Format(KSECONDURL,&iMainEngine.GetIMEI(),&iMainEngine.GetP(),aID,iArray[aID]->GetTimeFlag());
	TBuf8<200> tempBuf8;
	tempBuf8.Zero();
	tempBuf8.Copy(tempBuf);
	
	iMainEngine.GetHttpManager()->SendGetRequest(*this,tempBuf8,EHttpSecondContentUpdate);
}
void  CHandleSynchronization::CancelSendRequest()
{
	if(iCurState>-1)
		iMainEngine.GetHttpManager()->CancelTransaction(this,iCurState);
}

void  CHandleSynchronization::HandleFirstResponseXmlData()
{
	TiXmlDocument* doc=new TiXmlDocument();
	bool loadOkay;
	TBuf8<100> xmlPath;
	xmlPath.Zero();
	xmlPath.Copy(iTempFileName);

	char* path = NULL;
	path=(char *)xmlPath.PtrZ();
	loadOkay=doc->LoadFile(path,TIXML_ENCODING_UTF8);

	if(loadOkay==false)
	{
		
		delete doc;
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement();//content节点

	TiXmlElement* pRecord=pElement->FirstChildElement("uptime");

	if(pRecord!=0)
	{
		iPreTimeFlag=iTimeFlag;
		iTimeFlag=atoi(pRecord->FirstChild()->Value());
	}

	//<l id="1" name="精品下载">10000</l> 
	pRecord=pElement->FirstChildElement("first")->FirstChildElement("l");
	TInt type=0;
	TInt time=0;
	while(pRecord!=0)
	{
		HBufC* bufName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->Attribute("name")));
		CleanupStack::PushL(bufName);

		time=atoi(pRecord->FirstChild()->Value());

		type=atoi(pRecord->Attribute("id"));

		TBool tBool=ETrue;
		for(TInt i=0;i<iArray.Count();i++)
		{
			if(iArray[i]->GetID()==type)
			{
				iArray[i]->SetName(*bufName);
				iArray[i]->SetID(type);
				iArray[i]->SetTimeFlag(time);
				tBool=EFalse;
				break;
			}
		}

		if(tBool)
		{
			CContentItem* contentItem=CContentItem::NewL();
			contentItem->SetName(*bufName);
			contentItem->SetID(type);
			contentItem->SetTimeFlag(time);
			iArray.Append(contentItem);
		}
		
		CleanupStack::PopAndDestroy(1);
		pRecord=pRecord->NextSiblingElement("l");
	}
	doc->Clear();
	delete doc;
	
}
void  CHandleSynchronization::HandleSecondResponseXmlData()
{
	TiXmlDocument* doc=new TiXmlDocument();
	bool loadOkay;

	TBuf8<100> xmlPath;
	xmlPath.Zero();
	xmlPath.Copy(iTempFileName);

	char* path = NULL;
	path=(char *)xmlPath.PtrZ();
	loadOkay=doc->LoadFile(path,TIXML_ENCODING_UTF8);

	if(loadOkay==false)
	{
		delete doc;
		
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement();//content节点

	//<first id="4" name="分类">
	TiXmlElement* pRecord=pElement->FirstChildElement("first");
	TInt type=0;

	while(pRecord!=0)
	{
		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->Attribute("name")));
		CleanupStack::PushL(buf);

		type=atoi(pRecord->Attribute("id"));

		for(TInt i=0;i<iArray.Count();i++)
		{
			if(iArray[i]->GetID()==type)
			{
				iArray[i]->ClearChildItem();
				iArray[i]->SetName(*buf);
				this->HandleSecondXmlData(iArray[i],pRecord);
				break;
			}
		}
		
		CleanupStack::PopAndDestroy(1);
		pRecord=pRecord->NextSiblingElement("first");
	}
	doc->Clear();
	delete doc;
	
}
void  CHandleSynchronization::HandleFirstXmlData()
{

	TiXmlDocument* doc=new TiXmlDocument();
	bool loadOkay;

	TBuf8<100> xmlPath;
	xmlPath.Zero();
	xmlPath.Copy(iSynFileName);

	char* path = NULL;
	path=(char *)xmlPath.PtrZ();
	loadOkay=doc->LoadFile(path,TIXML_ENCODING_UTF8);


	if(loadOkay==false)
	{
		delete doc;
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement();//content节点

	TiXmlElement* pRecord=pElement->FirstChildElement("uptime");

	if(pRecord!=0)
	{
		iTimeFlag=atoi(pRecord->FirstChild()->Value());
		iPreTimeFlag=iTimeFlag;
	}

	pRecord=pElement->FirstChildElement("first");
	TInt type=0;

	while(pRecord!=0)
	{
		CContentItem* contentItem=CContentItem::NewL();
		iArray.Append(contentItem);

		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->Attribute("name")));
		CleanupStack::PushL(buf);

		type=atoi(pRecord->Attribute("id"));

		AddItem(contentItem,pRecord,*buf,type);
		
		CleanupStack::PopAndDestroy(1);
		pRecord=pRecord->NextSiblingElement("first");
	}
	doc->Clear();
	
	delete doc;
}
void  CHandleSynchronization::HandleSecondXmlData(CContentItem* aContentItem,TiXmlElement* aElement)
{
	TiXmlElement* pRecord=aElement->FirstChildElement("uptime");
	if(pRecord!=0)
	{
		aContentItem->SetPreTimeFlag(atoi(pRecord->FirstChild()->Value()));
	}

	//<second id="31" name="图片">
	pRecord=aElement->FirstChildElement("second");
	TInt index=0;
	while(pRecord!=0)
	{
		
		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->Attribute("name")));
		CleanupStack::PushL(buf);
		
		iMainEngine.WriteLog16(_L("-------2-----"));
		iMainEngine.WriteLog16(*buf);

		aContentItem->AppendChildItem(*buf,atoi(pRecord->Attribute("id")),0);
		CleanupStack::PopAndDestroy(1);

		this->HandleThirdXmlData(&aContentItem->GetChildItem(index),pRecord);
		iMainEngine.WriteLog16(aContentItem->GetChildItem(index).GetName());
		iMainEngine.WriteLog16(_L("------2------"));
		index++;
		pRecord=pRecord->NextSiblingElement("second");
	}
}
void  CHandleSynchronization::HandleThirdXmlData(CContentItem* aContentItem,TiXmlElement* aElement)
{
	TiXmlElement* pRecord=aElement->FirstChildElement("uptime");
	if(pRecord!=0)
	{
		aContentItem->SetPreTimeFlag(atoi(pRecord->FirstChild()->Value()));
	}

	// <third id="60">角色扮演</third> 
	pRecord=aElement->FirstChildElement("third");
	TInt index=0;
	while(pRecord!=0)
	{
		

		HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
		CleanupStack::PushL(buf);

		iMainEngine.WriteLog16(_L("------3-----"));
		iMainEngine.WriteLog16(*buf);

		aContentItem->AppendChildItem(*buf,atoi(pRecord->Attribute("id")),0);
		CleanupStack::PopAndDestroy(1);
		iMainEngine.WriteLog16(aContentItem->GetChildItem(index).GetName());
		iMainEngine.WriteLog16(_L("------3-----"));
		index++;
		pRecord=pRecord->NextSiblingElement("third");
	}
}
void  CHandleSynchronization::AddItem(CContentItem* aItem,TiXmlElement* aElement,const TDesC& aName,TInt aID)
{
	aItem->SetName(aName);
	aItem->SetID(aID);
	HandleSecondXmlData(aItem,aElement);
}

void  CHandleSynchronization::ReWriteXMl()
{
	DELETE(iDataBuf);
	TBuf<100> tempBuf;

	tempBuf.Zero();
	tempBuf.Append(KXMLHEADER);
	iDataBuf=HBufC::NewL(tempBuf.Length());
	iDataBuf->Des().Append(KXMLHEADER);

	tempBuf.Zero();
	tempBuf.Format(KXMLUPTIME,iTimeFlag);

	iDataBuf = iDataBuf->ReAllocL(iDataBuf->Length() + tempBuf.Length());
	iDataBuf->Des().Append(tempBuf);

	for(TInt i=0;i<iArray.Count();i++)
	{
		tempBuf.Zero();
		tempBuf.Format(KXMLFIRSTHEADER,iArray[i]->GetID(),&iArray[i]->GetName());
		iDataBuf = iDataBuf->ReAllocL(iDataBuf->Length() + tempBuf.Length());
		iDataBuf->Des().Append(tempBuf);


		AddText(iArray[i]);
		iDataBuf = iDataBuf->ReAllocL(iDataBuf->Length() + iTempBuf->Length());
		iDataBuf->Des().Append(*iTempBuf);

		tempBuf.Zero();
		tempBuf.Append(KXMLFIRSTEND);
		iDataBuf = iDataBuf->ReAllocL(iDataBuf->Length() + tempBuf.Length());
		iDataBuf->Des().Append(tempBuf);
	}
	
	tempBuf.Zero();
	tempBuf.Append(KXMLEND);
	iDataBuf = iDataBuf->ReAllocL(iDataBuf->Length() + tempBuf.Length());
	iDataBuf->Des().Append(tempBuf);

	::WriteFile(iSynFileName,*iDataBuf);
}
void  CHandleSynchronization::AddText(CContentItem* aItem)
{
	DELETE(iTempBuf);
	iTempBuf=HBufC::NewL(0);
	TBuf<100> tempBuf;
	for(TInt i=0;i<aItem->GetChildCount();i++)
	{
		tempBuf.Zero();
		tempBuf.Format(KXMLSECONDHEADER,aItem->GetChildItem(i).GetID(),&aItem->GetChildItem(i).GetName());
		iTempBuf = iTempBuf->ReAllocL(iTempBuf->Length() + tempBuf.Length());
		iTempBuf->Des().Append(tempBuf);
		iMainEngine.WriteLog16(tempBuf);
		for(TInt j=0;j<aItem->GetChildItem(i).GetChildCount();j++)
		{
			tempBuf.Zero();
			tempBuf.Format(KXMLTHIRD,aItem->GetChildItem(i).GetChildItem(j).GetID(),&aItem->GetChildItem(i).GetChildItem(j).GetName());
			iTempBuf = iTempBuf->ReAllocL(iTempBuf->Length() + tempBuf.Length());
			iTempBuf->Des().Append(tempBuf);
			iMainEngine.WriteLog16(tempBuf);
		}
		tempBuf.Zero();
		tempBuf.Append(KXMLSECONDEND);
		iTempBuf = iTempBuf->ReAllocL(iTempBuf->Length() + tempBuf.Length());
		iTempBuf->Des().Append(tempBuf);
		iMainEngine.WriteLog16(tempBuf);
		
	}
	//return *iTempBuf;
}
const TDesC& CHandleSynchronization::GetValue(TInt aOneID,TInt aTwoID,TInt aThreeID)
{
	
	DELETE(iTempBuf);
	
	CContentItem* item=this->GetContentItem(aOneID);
	
	if(aThreeID>-1)
	{
		if(item)
		{
			if(aTwoID<item->GetChildCount())
			{
				if(aThreeID<item->GetChildItem(aTwoID).GetChildCount())
				{
					
					iTempBuf=HBufC::NewL(item->GetChildItem(aTwoID).GetChildItem(aThreeID).GetName().Length());
					iTempBuf->Des().Copy(item->GetChildItem(aTwoID).GetChildItem(aThreeID).GetName());
					
				}
			}
		}
	}
	else if(aTwoID>-1)
	{
		if(item)
		{
			if(aTwoID<item->GetChildCount())
			{
				
				iTempBuf=HBufC::NewL(item->GetChildItem(aTwoID).GetName().Length());
				iTempBuf->Des().Copy(item->GetChildItem(aTwoID).GetName());
			}
		}

	}
	else if(aOneID>-1)
	{
		if(item)
		{
			iTempBuf=HBufC::NewL(item->GetName().Length());
			iTempBuf->Des().Copy(item->GetName());
		}
		
	}

	if(!iTempBuf)
	{
		iTempBuf=HBufC::NewL(0);
		
	}
	
	return *iTempBuf;
}
TInt CHandleSynchronization::GetItemID(TInt aOneID,TInt aTwoID,TInt aThreeID)
{
	TInt id=0;

	CContentItem* item=this->GetContentItem(aOneID);

	if(aThreeID>-1)
	{
		if(item)
		{
			if(aTwoID<item->GetChildCount())
			{
				if(aThreeID<item->GetChildItem(aTwoID).GetChildCount())
				{
					id=item->GetChildItem(aTwoID).GetChildItem(aThreeID).GetID();
				}
			}
		}
	}
	else if(aTwoID>-1)
	{
		if(item)
		{
			if(aTwoID<item->GetChildCount())
			{
				id=item->GetChildItem(aTwoID).GetID();
			}
		}
	}
	else if(aOneID>-1)
	{
		if(item)
		{
			id=item->GetID();
		}
	}
	else
	{
		id=0;
	}
	

	return id;
}

TBool CHandleSynchronization::IsUpdate(TInt aOneID,TInt aTwoID,TInt aThreeID)
{
	TBool tBool=ETrue;
	if(aOneID==0)
	{
		if(iArray.Count()>4)
			tBool=EFalse;
	}
	else
	{
		CContentItem* item=this->GetContentItem(aOneID);

		if(aThreeID>-1)
		{
			if(item)
			{
				if(aTwoID<item->GetChildCount())
				{
					if(aThreeID<item->GetChildItem(aTwoID).GetChildCount())
					{
						tBool=item->GetChildItem(aTwoID).GetChildItem(aThreeID).IsUpdate();
					}
				}
			}
		}
		else if(aTwoID>-1)
		{
			if(item)
			{
				if(aTwoID<item->GetChildCount())
				{
					tBool=item->GetChildItem(aTwoID).IsUpdate();
				}
			}
		}
		else if(aOneID>-1)
		{
			if(item)
			{
				tBool=item->IsUpdate();
			}

			if(item->GetChildCount()==0)
			{
				tBool=ETrue;
			}
		}
		else
		{
			tBool=EFalse;
		}
	}

	return tBool;
}
TInt CHandleSynchronization::GetCount(TInt aOneID,TInt aTwoID,TInt aThreeID)
{
	TInt count=0;
	if(aOneID==0)
	{
		count=iArray.Count();
	}
	else
	{
		CContentItem* item=this->GetContentItem(aOneID);

		if(aThreeID>-1)
		{
			if(item)
			{
				if(aTwoID<item->GetChildCount())
				{
					if(aThreeID<item->GetChildItem(aTwoID).GetChildCount())
					{
						count=item->GetChildItem(aTwoID).GetChildCount();
					}
				}
			}
		}
		else if(aTwoID>-1)
		{
			if(item)
			{
				if(aTwoID<item->GetChildCount())
				{
					count=item->GetChildCount();
				}
			}
		}
		else if(aOneID>-1)
		{
			if(item)
			{
				count=iArray.Count();
			}
		}
		else
		{
			count=0;
		}
	}
	return count;
}
void  CHandleSynchronization::SetObserver(MHandleEventObserver* aObserver)
{
	iObserver=aObserver;
}
CContentItem* CHandleSynchronization::GetContentItem(TInt aID)
{
	CContentItem* item=NULL;
	for(TInt i=0;i<iArray.Count();i++)
	{
		if(iArray[i]->GetID()==aID)
		{
			item=iArray[i];
			break;
		}
	}
	return item;
}