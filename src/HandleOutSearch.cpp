/*
============================================================================
 Name        : HandleOutSearch.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleOutSearch implementation
============================================================================
*/

#include "HandleOutSearch.h"
#include "MainEngine.h"

#include <utf.h>
#include <EscapeUtils.h>
#include "tinyxml.h"
#include <HttpObserver.h>
#include "HttpManager.h"
#include "MControlObserver.h"
#include "SearchResultItem.h"
#include "SearchResultTempItem.h"
#include "MainEngine.h"
#include "StaticCommonTools.h"
#include "CoCoPreDefine.h"

_LIT8(KXMLP,"p"); //p标签
_LIT8(KXMLA,"a"); //超链接标签
_LIT8(KXMLBR,"br"); //换行标签

_LIT8(KXMLEM,"em"); //文本标签
_LIT8(KXMLB,"b"); //文本标签
_LIT8(KXMLI,"i"); //文本标签
_LIT8(KXMLSTRONG,"strong"); //文本标签
_LIT8(KXMLU,"u"); //文本标签
_LIT8(KXMLBIG,"big"); //文本标签
_LIT8(KXMLSMALL,"small"); //文本标签

_LIT8(KROBOOURL,"http://wap.roboo.com/");

_LIT8(KURLRINGS,"Sring.wml");  //铃声搜索,1
_LIT8(KURLRINGV,"Vring.wml");  //铃声详情,1
_LIT8(KURLRINGD,"RingDown.wml");  //铃声下载,1

_LIT8(KURLIMAGES,"Simage.wml");  //图片搜索,2
_LIT8(KURLIMAGEV,"Vimage.wml");  //图片详情,2
_LIT8(KURLIMAGED,"ImageDown.wml");  //图片下载,2

_LIT8(KURLVIDEOS,"Svideo.wml");  //视频搜索,4
_LIT8(KURLVIDEOV,"Vvideo.wml");  //视频详情,4
_LIT8(KURLVIDEOD,"VideoDown.wml");  //视频下载,4

_LIT8(KURLGAMES,"Sgame.wml");  //游戏搜索,3
_LIT8(KURLGAMEV,"Vgame.wml");  //游戏详情,3
_LIT8(KURLGAMED,"GameDown.wml");  //游戏下载,3

_LIT8(KURLERROR,"http://59.36.96.182/interface/err?t=1&m=1"); //获取错误信息
_LIT8(KURLKEYWORD,"http://59.36.96.182/interface/kw?ty=1&n="); //获取错误信息


_LIT8(KNBSP,"nbsp;"); //
_LIT8(KAMP,"amp;"); //

CHandleOutSearch::CHandleOutSearch(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iCurUrl(NULL)
,iErrorInfo(NULL)
,iBuf(NULL)
{
	// No implementation required
}


CHandleOutSearch::~CHandleOutSearch()
{
	this->CancelSendRequest();
	::DelFile(iFileName);
	iPointArray.Close();
	DELETE(iCurUrl);
	DELETE(iErrorInfo);
	DELETE(iBuf);
}
CHandleOutSearch* CHandleOutSearch::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleOutSearch* self = new (ELeave)CHandleOutSearch( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleOutSearch* CHandleOutSearch::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleOutSearch* self=CHandleOutSearch::NewLC( aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleOutSearch::ConstructL()
{
	iFileName.Zero();
	iFileName.Append(iMainEngine.GetAppDrive());
	iFileName.Append(KTEMPXML);

	iFlag=ETrue;
	iContentError=EFalse;
}

//From MOperationObserver
void CHandleOutSearch::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	TInt eventType=aEventType;

	if(aEventType==EHttpNoError)
	{
		if(aType==EHttpOutSearch)
		{
			::WriteFile(iFileName,aEventData);
			this->HandlePageWml();
		}
		else if(aType==EHttpOutSearchError)
		{
			DELETE(iErrorInfo);
			iErrorInfo=aEventData.Alloc();

			DELETE(iBuf);
			iBuf=aEventData.Alloc();
			eventType=EHttpContentError;
		}
	}
	else
	{
		DELETE(iBuf);
		iBuf=aEventData.Alloc();
	}
	if(aType!=EHttpOutSearchKeyWord)
	{
		if(iContentError)
		{
			iObserver.HandleResponseEvent(EHttpContentError,aType);
			iContentError=EFalse;
		}
		else
			iObserver.HandleResponseEvent(eventType,aType);
	}
}

CSearchResultTempItem& CHandleOutSearch::GetCurItem()
{
	return *iPointArray[iCurIndex];
}

void CHandleOutSearch::HandlePageWml()
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
		loadOkay=doc->LoadFile("c:\\wapxml.xml",TIXML_ENCODING_UTF8);
	}
	else
	{
		loadOkay=doc->LoadFile("c:\\wapxml.xml",TIXML_ENCODING_UTF8);
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
		delete doc;
		
		return;
	}
	
	TiXmlElement *pElement=doc->RootElement()->FirstChildElement("card");

	TiXmlNode* pRecord=pElement->FirstChild();

	iFlag=ETrue;

	if(pRecord!=0)
	{
		CSearchResultTempItem* item=CSearchResultTempItem::NewL();
		item->SetCurUrl(*iCurUrl);
		this->iPointArray.Append(item);

		if(iPointArray.Count()>30)
			iPointArray.Remove(0);

		iCurIndex=iPointArray.Count()-1;
	}
	while(pRecord!=0)
	{
		HandleChildElement(pRecord);
		pRecord=pRecord->NextSibling();
	}

	doc->Clear();
	delete doc;
	
}

void CHandleOutSearch::HandleChildElement(TiXmlNode* aElement)
{
	
	TInt index=iPointArray.Count()-1;
	TInt itemIndex=iPointArray[index]->GetItemCount()-1;
	if(itemIndex<0)
	{
		iPointArray[index]->CreatNewItem();
		itemIndex=0;
	}
	if(aElement->NoChildren())
	{
		if(iFlag)
		{
			HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)aElement->Value()));
			CleanupStack::PushL(buf);

			if(buf->Compare(_L("br"))==0)
			{
				if(!iPointArray[index]->GetItem(itemIndex).IsNull())
					iPointArray[index]->CreatNewItem();
			}

			if(buf->Compare(_L("br"))!=0&&buf->Compare(_L("img"))!=0&&buf->Compare(_L("input"))!=0&&buf->Compare(_L("|"))!=0)
			{
				if(buf->Left(2).Compare(_L("--"))==0)
				{
					TInt f=0;
					for(TInt i=0;i<buf->Length();i++)
					{
						if(buf->Mid(i,1).Compare(_L("-"))!=0)
						{
							f=1;
							break;
						}
					}
					
					if(f==0)
					{
						iFlag=EFalse;
					}
				}
				if(iFlag)
					iPointArray[index]->GetItem(itemIndex).SetName(*buf);
				else
				{
					if(!iPointArray[index]->GetItem(itemIndex).IsNull())
						iPointArray[index]->CreatNewItem();
				}
			}

			iMainEngine.WriteLog16(*buf);
			CleanupStack::PopAndDestroy(1);
		}
	}
	else
	{
		HBufC8* tag=TPtrC8((const TUint8*)aElement->Value()).Alloc();
		CleanupStack::PushL(tag);
		TInt value=CheckTag(*tag);
		CleanupStack::PopAndDestroy(1);
		if(value>0)
		{
			switch(value)
			{
				case 1:  //P标签
					{
						TiXmlNode* element=aElement->FirstChild();
							while(element!=0)
							{
								HandleChildElement(element);
								element=element->NextSibling();
							}
					}
					
					break;
				case 2: //连接标签
					{
						HBufC8* bufUrl = TPtrC8((const TUint8*)aElement->ToElement()->Attribute("href")).Alloc();
						CleanupStack::PushL(bufUrl);
						TInt urlFlag=CheckUrl(*bufUrl);

						if(urlFlag>0)
						{
							iPointArray[index]->GetItem(itemIndex).SetUrl(*bufUrl);
						}
						CleanupStack::PopAndDestroy(1);

						if(urlFlag==1||urlFlag==2)
						{
							HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)aElement->FirstChild()->Value()));
							CleanupStack::PushL(buf);
							iPointArray[index]->GetItem(itemIndex).SetName(*buf);
							iMainEngine.WriteLog16(*buf);
							CleanupStack::PopAndDestroy(1);

							iPointArray[index]->GetItem(itemIndex).SetIsLink(ETrue);
							iPointArray[index]->CreatNewItem();
						}
						else
						{
							HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)aElement->FirstChild()->Value()));
							CleanupStack::PushL(buf);

							HBufC8* bufUrl = TPtrC8((const TUint8*)aElement->ToElement()->Attribute("href")).Alloc();
							CleanupStack::PushL(bufUrl);

							if(CheckUrl(*bufUrl)==3)
							{
								if(buf->Compare(iMainEngine.GetDesById(ETurkeyTextRes_DownPage))==0||buf->Compare(iMainEngine.GetDesById(ETurkeyTextRes_HXML_DPage))==0)
								{
									iPointArray[index]->SetNextPageUrl(*bufUrl);
								}
								else if(buf->Compare(iMainEngine.GetDesById(ETurkeyTextRes_UpPage))==0||buf->Compare(iMainEngine.GetDesById(ETurkeyTextRes_HXML_UPage))==0)
								{
									iPointArray[index]->SetPrePageUrl(*bufUrl);
								}
							}
							iMainEngine.WriteLog16(*buf);

							CleanupStack::PopAndDestroy(2);
						}
					}
					break;
				case 4://换行标签

					if(!iPointArray[index]->GetItem(itemIndex).IsNull()&&iFlag)
						iPointArray[index]->CreatNewItem();
					break;
				case 3://字体标签
					{
						if(iFlag)
						{
							HBufC* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)aElement->FirstChild()->Value()));
							CleanupStack::PushL(buf);
							iPointArray[index]->GetItem(itemIndex).SetName(*buf);
							iMainEngine.WriteLog16(*buf);
							CleanupStack::PopAndDestroy(1);
						}
					}
					break;
			}
		}
	}
}
TInt CHandleOutSearch::CheckUrl(const TDesC8& aUrl)
{
	//iMainEngine.WriteLog8(aUrl);
	TInt rValue=0;
	if(aUrl.Find(_L8("http://"))>-1)
	{
		if(aUrl.Find(KURLRINGV)>-1||aUrl.Find(KURLIMAGEV)>-1||aUrl.Find(KURLVIDEOV)>-1||aUrl.Find(KURLGAMEV)>-1)
		{
			rValue=1;
		}
		else if(aUrl.Find(KURLRINGD)>-1||aUrl.Find(KURLIMAGED)>-1||aUrl.Find(KURLVIDEOD)>-1||aUrl.Find(KURLGAMED)>-1)
		{
			rValue=2;
		}
		else if(aUrl.Find(KURLRINGS)>-1||aUrl.Find(KURLIMAGES)>-1||aUrl.Find(KURLVIDEOS)>-1||aUrl.Find(KURLGAMES)>-1)
		{
			rValue=3;
		}
	}
	else
	{
		if(aUrl.Find(KURLRINGV)==0||aUrl.Find(KURLIMAGEV)==0||aUrl.Find(KURLVIDEOV)==0||aUrl.Find(KURLGAMEV)==0)
		{
			rValue=1;
		}
		else if(aUrl.Find(KURLRINGD)==0||aUrl.Find(KURLIMAGED)==0||aUrl.Find(KURLVIDEOD)==0||aUrl.Find(KURLGAMED)==0)
		{
			rValue=2;
		}
		else if(aUrl.Find(KURLRINGS)==0||aUrl.Find(KURLIMAGES)==0||aUrl.Find(KURLVIDEOS)==0||aUrl.Find(KURLGAMES)==0)
		{
			rValue=3;
		}
	}
	return rValue;
}
TInt CHandleOutSearch::CheckTag(const TDesC8& aTagData)
{
	iMainEngine.WriteLog8(aTagData);
	TInt rValue=0;
	if(aTagData.Compare(KXMLP)==0)
		rValue= 1;
	else if(aTagData.Compare(KXMLA)==0)
		rValue= 2;
	else if(aTagData.Compare(KXMLBR)==0)
		rValue= 4;
	else if(aTagData.Compare(KXMLEM)==0)
		rValue= 3;
	else if(aTagData.Compare(KXMLB)==0)
		rValue= 3;
	else if(aTagData.Compare(KXMLSTRONG)==0)
		rValue= 3;
	else if(aTagData.Compare(KXMLU)==0)
		rValue= 3;
	else if(aTagData.Compare(KXMLBIG)==0)
		rValue= 3;
	else if(aTagData.Compare(KXMLSMALL)==0)
		rValue= 3;

	return rValue;
}
void CHandleOutSearch::ClearTempBuf()
{
	iPointArray.Close();
	//iPointArray.Remove();
}
void  CHandleOutSearch::SendKeyWordRequest(const TDesC8& aKeyWord)
{
	TBuf8<200> tempUrl;
	tempUrl.Zero();
	tempUrl.Append(KURLKEYWORD);
	tempUrl.Append(iMainEngine.GetIMEI());
	tempUrl.Append(_L("&m="));


	if(iCurUrl->Find(KURLRINGS)>-1)
		tempUrl.AppendNum(1);
	else if(iCurUrl->Find(KURLIMAGES)>-1)
		tempUrl.AppendNum(2);
	else if(iCurUrl->Find(KURLGAMES)>-1)
		tempUrl.AppendNum(3);
	else if(iCurUrl->Find(KURLVIDEOS)>-1)
		tempUrl.AppendNum(4);

	tempUrl.Append(_L("&p="));
	tempUrl.Append(iMainEngine.GetP());
	tempUrl.Append(_L("&k="));
	tempUrl.Append(aKeyWord);

	iState=EHttpOutSearchKeyWord;
	iHttpManager.SendGetRequest(*this,tempUrl,EHttpOutSearchKeyWord);
}
void  CHandleOutSearch::SendRequest(const TDesC8& aUrl)
{
	DELETE(iCurUrl);
	iCurUrl=aUrl.Alloc();
	if(CheckTemp())
	{
		iState=EHttpOutSearch;
		iHttpManager.SendGetRequest(*this,aUrl,EHttpOutSearch);
	}
	else
	{
		iObserver.HandleResponseEvent(0,EHttpOutSearch);
	}
}
void  CHandleOutSearch::SendErrorInfoRequest()
{
	iState=EHttpOutSearchError;
	iHttpManager.SendGetRequest(*this,KURLERROR,EHttpOutSearchError);
}
void  CHandleOutSearch::CancelSendRequest()
{
	iHttpManager.CancelTransaction(this,iState);
}
void  CHandleOutSearch::SendHomePageRequest()
{
	this->SendRequest(iPointArray[iCurIndex]->GetHomePageUrl());
}
void  CHandleOutSearch::SendPrePageRequest()
{
	this->SendRequest(iPointArray[iCurIndex]->GetPrePageUrl());
}
void  CHandleOutSearch::SendNextPageRequest()
{
	this->SendRequest(iPointArray[iCurIndex]->GetNextPageUrl());
}
void  CHandleOutSearch::SendEndPageRequest()
{
	this->SendRequest(iPointArray[iCurIndex]->GetEndPageUrl());
}
void  CHandleOutSearch::SendGoToPageRequest(TInt aPage)
{
	TBuf8<300> tempUrl;
	iPointArray[iCurIndex]->GetGoToPageUrl(aPage,tempUrl);
	this->SendRequest(tempUrl);
}
TBool CHandleOutSearch::CheckTemp()
{
	TBool tBool=ETrue;
	for(TInt i=0;i<this->iPointArray.Count();i++)
	{
		if(iCurUrl->Compare(iPointArray[i]->GetCurUrl())==0)
		{
			tBool=EFalse;
			iCurIndex=i;
			break;
		}
	}
	return tBool;
}