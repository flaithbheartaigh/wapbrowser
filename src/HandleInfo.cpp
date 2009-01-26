/*
============================================================================
 Name        : HandleInfo.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHandleInfo implementation
============================================================================
*/

#include "HandleInfo.h"
#include "MainEngine.h"
#include <utf.h>
#include "HttpObserver.h"
#include "MControlObserver.h"
#include "HttpManager.h"
#include "CoCoPreDefine.h"
#include "tinyxml.h"
#include "StaticCommonTools.h"
#include <f32file.h>
#include <s32file.h>

_LIT(KSendContetUrl,"http://59.36.96.182/coco/v?username=%S&id=%S");
_LIT(KTEMPCONTENTPATH,"\\system\\Apps\\coco\\tempCInfo\\");
CHandleInfo::CHandleInfo(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
:iObserver(aObserver)
,iMainEngine(aMainEngine)
,iHttpManager(*aMainEngine.GetHttpManager())
,iBuf(NULL)
,iName(NULL)
,iSize(NULL)
,iType(NULL)
,iRemark(NULL)
,iPicPath(NULL)
,iCode(NULL)
,iPicUrl(NULL)
,iXmlPath(NULL)
,iDesArray(NULL)
,iResponseFlag(ETrue)
{
	// No implementation required
}


CHandleInfo::~CHandleInfo()
{
	this->CancelSendRequest();
	this->ClearBuf();
	DELETE(iDesArray);
	DELETE(iCode);
	
}

CHandleInfo* CHandleInfo::NewLC(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleInfo* self = new (ELeave)CHandleInfo(aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHandleInfo* CHandleInfo::NewL(MHandleEventObserver& aObserver,CMainEngine& aMainEngine)
{
	CHandleInfo* self=CHandleInfo::NewLC(aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CHandleInfo::ConstructL()
{
	iContentError=EFalse;

	iFileName.Zero();
	iFileName.Append(iMainEngine.GetAppDrive());
	iFileName.Append(KTEMPCONTENTPATH);
	::MakeDir(iFileName);

	GetTempFile();
}

//From MOperationObserver
void CHandleInfo::OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType)
{
	if(aEventType==EHttpNoError)
	{
		TFileName fileName;
		fileName.Zero();
		if(aType==EHttpContentInfo)
		{
			this->GetXmlPath();

			::WriteFile(*iXmlPath,aEventData);
			this->HandleXmlData();
		}
		else if(aType==EHttpContentInfoImage)
		{
			
		}
	}
	else
	{
		DELETE(iBuf);
		
		iBuf=HBufC::NewL(aEventData.Length());
		iBuf->Des().Copy(aEventData);
	}
	if(iResponseFlag)
	{
		if(iContentError)
			iObserver.HandleResponseEvent(EHttpConnectionError,EHttpContentInfo);
		else
			iObserver.HandleResponseEvent(aEventType,aType);
	}
}

const TDesC& CHandleInfo::GetInfo()
{
	if(iBuf==NULL)
		iBuf=HBufC::NewL(0);

	return *iBuf;
}
void CHandleInfo::SendImageRequest(const TDesC8& aUrl)
{
	iResponseFlag=ETrue;
	this->GetPicPath();

	iHttpState=EHttpContentInfoImage;

	iHttpManager.SendGetRequest(*this,aUrl,*iPicPath,EHttpContentInfoImage);
}
void  CHandleInfo::SendRequest(const TDesC& aCode)
{
	iResponseFlag=ETrue;
	iHttpState=EHttpContentInfo;

	DELETE(iCode);
	iCode=aCode.Alloc();

	iTempBuf=CheckTempFile();
	if(!iTempBuf)
	{
		if(iDesArray->Count()>49)
		{
			TFileName fileName;
			fileName.Zero();
			fileName.Append((*iDesArray)[0]);
			fileName.Append(_L(".xml"));
			::DelFile(fileName);

			fileName.Zero();
			fileName.Append((*iDesArray)[0]);
			fileName.Append(_L(".jpg"));
			::DelFile(fileName);

			iDesArray->Delete(0);

		}
		TBuf<300> tempUrl;
		tempUrl.Zero();
		tempUrl.Format(KSendContetUrl,&iMainEngine.GetIMEI(),&aCode);

		TBuf8<300> tempUrl8;
		tempUrl8.Zero();
		tempUrl8.Copy(tempUrl);
		
		iHttpManager.SendGetRequest(*this,tempUrl8,EHttpContentInfo);
	}
	else
	{
		HandleXmlData();
		if(iResponseFlag)
			iObserver.HandleResponseEvent(EHttpNoError,EHttpTempBuf);
	}
}

void  CHandleInfo::CancelSendRequest()
{
	iResponseFlag=EFalse;
	iHttpManager.CancelTransaction(this,iHttpState);
}
void CHandleInfo::HandleXmlData()
{
	
	TiXmlDocument* doc=new TiXmlDocument();
	bool loadOkay;

	TBuf8<100> xmlPath;
	xmlPath.Zero();
	xmlPath.Copy(*iXmlPath);

	char* path = NULL;
	path=(char *)xmlPath.PtrZ();
	loadOkay=doc->LoadFile(path,TIXML_ENCODING_UTF8);
	/*if(iMainEngine.GetAppDrive().Left(1).Compare(_L("e"))==0||iMainEngine.GetAppDrive().Left(1).Compare(_L("E"))==0)
	{
		loadOkay=doc->LoadFile("c:\\contenttemp.xml",TIXML_ENCODING_UTF8);
	}
	else
	{
		loadOkay=doc->LoadFile("c:\\contenttemp.xml",TIXML_ENCODING_UTF8);
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
	
	TiXmlElement *pElement=doc->RootElement();

	TiXmlElement* pRecord=pElement->FirstChildElement("url");

	if (pRecord!=0)
	{
		if(pRecord->FirstChild()!=0)
		{
			if(iPicBool==EFalse&&iResponseFlag)
			{
				iPicUrl=TPtrC8((const TUint8*)pRecord->FirstChild()->Value()).Alloc();
				SendImageRequest(*iPicUrl);
				iPicBool=ETrue;
			}
		}
	}

	pRecord=pElement->FirstChildElement("name");
	if(pRecord!=0)
	{
		if(pRecord->FirstChild()!=0)
		{
			HBufC* buf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
			CleanupStack::PushL(buf);
			iName = HBufC::NewL(buf->Length()+iMainEngine.GetDesById(ECoCoTextRes_File_Name).Length());
			iName->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Name));
			iName->Des().Append(*buf);
			CleanupStack::PopAndDestroy(1);
		}
	}

	pRecord=pElement->FirstChildElement("size");
	if(pRecord!=0)
	{
		if(pRecord->FirstChild()!=0)
		{
			HBufC* buf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
			CleanupStack::PushL(buf);
			iSize = HBufC::NewL(buf->Length()+iMainEngine.GetDesById(ECoCoTextRes_File_Size).Length());
			iSize->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Size));
			iSize->Des().Append(*buf);
			CleanupStack::PopAndDestroy(1);
		}
	}

	pRecord=pElement->FirstChildElement("type");
	if(pRecord!=0)
	{
		if(pRecord->FirstChild()!=0)
		{
			HBufC* buf=CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
			CleanupStack::PushL(buf);
			iType = HBufC::NewL(buf->Length()+iMainEngine.GetDesById(ECoCoTextRes_File_Type).Length());
			iType->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Type));
			iType->Des().Append(*buf);
			CleanupStack::PopAndDestroy(1);
		}
	}

	pRecord=pElement->FirstChildElement("remark");
	if(pRecord!=0)
	{
		if(pRecord->FirstChild()!=0)
		{
			HBufC* buf= CnvUtfConverter::ConvertToUnicodeFromUtf8L(TPtrC8((const TUint8*)pRecord->FirstChild()->Value()));
			CleanupStack::PushL(buf);
			iRemark = HBufC::NewL(buf->Length()+iMainEngine.GetDesById(ECoCoTextRes_File_Remark).Length());
			iRemark->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Remark));
			iRemark->Des().Append(*buf);
			CleanupStack::PopAndDestroy(1);
		}
	}

	doc->Clear();
	delete doc;
	
}
const TDesC& CHandleInfo::GetName()
{
	if(iName==NULL)
	{
		iName = HBufC::NewL(iMainEngine.GetDesById(ECoCoTextRes_File_Name).Length());
		iName->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Name));
	}

	return *iName;
}
const TDesC& CHandleInfo::GetSize()
{
	if(iSize==NULL)
	{
		iSize = HBufC::NewL(iMainEngine.GetDesById(ECoCoTextRes_File_Size).Length());
		iSize->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Size));
	}

	return *iSize;
}
const TDesC& CHandleInfo::GetType()
{
	if(iType==NULL)
	{
		iType = HBufC::NewL(iMainEngine.GetDesById(ECoCoTextRes_File_Type).Length());
		iType->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Type));
	}

	return *iType;
}
const TDesC& CHandleInfo::GetRemark()
{
	if(iRemark==NULL)
	{
		iRemark = HBufC::NewL(iMainEngine.GetDesById(ECoCoTextRes_File_Remark).Length());
		iRemark->Des().Append(iMainEngine.GetDesById(ECoCoTextRes_File_Remark));
	}

	return *iRemark;
}
const TDesC& CHandleInfo::GetImagePath()
{
	if(iPicPath==NULL)
		iPicPath=HBufC::NewL(0);

	return *iPicPath;
}
void CHandleInfo::ClearBuf()
{
	iContentError=EFalse;
	DELETE(iXmlPath);
	DELETE(iBuf);
	DELETE(iName);
	DELETE(iSize);
	DELETE(iType);
	DELETE(iRemark);
	DELETE(iPicPath);
	DELETE(iPicUrl);
	iPicBool=EFalse;
}
TBool CHandleInfo::CheckTempFile()
{
	ClearBuf();
	TBool tBool=EFalse;
	GetXmlPath();
	TInt err=::FileIsExistL(*iXmlPath);
	if(err==1)
	{
		tBool=ETrue;
		GetPicPath();
		err=::FileIsExistL(*iPicPath);
		if(err==1)
		{
			iPicBool=ETrue;
		}
	}
	return tBool;
}
void CHandleInfo::GetXmlPath()
{
	DELETE(iXmlPath);
	iXmlPath=HBufC::NewL(200);
	iXmlPath->Des().Append(iFileName);
	iXmlPath->Des().Append(*iCode);
	iXmlPath->Des().Append(_L(".xml"));
}

void CHandleInfo::GetPicPath()
{
	DELETE(iPicPath);
	iPicPath=HBufC::NewL(200);
	iPicPath->Des().Append(iFileName);
	iPicPath->Des().Append(*iCode);
	iPicPath->Des().Append(_L(".jpg"));
}
void CHandleInfo::GetTempFile()
{
	DELETE(iDesArray);
	iDesArray = new (ELeave) CDesCArrayFlat(10);

	CDir* dir;
	RFs fs;
	User::LeaveIfError(fs.Connect());

	TInt err=fs.GetDir(iFileName,KEntryAttNormal|KEntryAttDir|KEntryAttHidden,ESortByDate,dir);

	if(err==KErrNone)
	{
		TInt tempInt = dir->Count();
		
		for(TInt i = 0;i < tempInt;i++)
		{
			TEntry iEntry = (*dir)[i];

			TInt p=iEntry.iName.Find(_L(".xml"));
			if(p>-1)
			{
				iDesArray->AppendL(iEntry.iName.Left(p));
			}
		}
	}

	fs.Close();
	if(dir)
	{
		delete dir;
		dir=NULL;
	}
}