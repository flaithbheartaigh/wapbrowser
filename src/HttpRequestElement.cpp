/*
============================================================================
 Name        : HttpRequestElement.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 存放联网请求数据类
============================================================================
*/

#include "HttpRequestElement.h"
#include <utf.h>
#include "StaticCommonTools.h"
#include "CoCoPreDefine.h"

CHttpRequestElement::CHttpRequestElement(MOperationObserver& aObserver)
:iContentType(NULL)
,iUrl(NULL)
,iSendData(NULL)
,iReceiveData(NULL)
,iObserver(aObserver)
,iFileName(NULL)
,iReceiveData8(NULL)
{
	// No implementation required
}


CHttpRequestElement::~CHttpRequestElement()
{
	if(iContentType)
	{
		delete iContentType;
		iContentType=NULL;
	}
	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}
	if(iSendData)
	{
		delete iSendData;
		iSendData=NULL;
	}
	if(iReceiveData)
	{
		delete iReceiveData;
		iReceiveData=NULL;
	}

	if(iFileName)
	{
		delete iFileName;
		iFileName=NULL;
	}

	if(iReceiveData8)
	{
		delete iReceiveData8;
		iReceiveData8=NULL;
	}
}

CHttpRequestElement* CHttpRequestElement::NewLC(MOperationObserver& aObserver)
{
	CHttpRequestElement* self = new (ELeave)CHttpRequestElement(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CHttpRequestElement* CHttpRequestElement::NewL(MOperationObserver& aObserver)
{
	CHttpRequestElement* self=CHttpRequestElement::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
}

void CHttpRequestElement::ConstructL()
{

}

void CHttpRequestElement::SetContentType(const TDesC8& aContentType)
{
	iContentType=HBufC8::NewL(aContentType.Length());
	iContentType->Des().Copy(aContentType);
}
const TDesC8& CHttpRequestElement::GetContentType()
{
	if(!iContentType)
	{
		iContentType=HBufC8::NewL(20);
		iContentType->Des().Copy(_L8("Unkown Error!"));
	}
		 
	return *iContentType;
}

void CHttpRequestElement::SetUrl(const TDesC8& aUrl)
{
	if(iUrl)
	{
		delete iUrl;
		iUrl=NULL;
	}

	iUrl=HBufC8::NewL(aUrl.Length());
	iUrl->Des().Copy(aUrl);
}
const TDesC8& CHttpRequestElement::GetUrl()
{
	if(!iUrl)
	{
		iUrl=HBufC8::NewL(20);
		iUrl->Des().Copy(_L8("Unkown Error!"));
	}
		 
	return *iUrl;
}

void CHttpRequestElement::SetSendData(const TDesC8& aSendData)
{
	iSendData=HBufC8::NewL(aSendData.Length());
	iSendData->Des().Copy(aSendData);
}
const TDesC8& CHttpRequestElement::GetSendData()
{
	if(!iSendData)
	{
		iSendData=HBufC8::NewL(20);
		iSendData->Des().Copy(_L8("Unkown Error!"));
	}
		 
	return *iSendData;
}

void CHttpRequestElement::SetReceiveData(const TDesC8& aBodyData)
{
	if(iFileName)
	{
		this->GetClientBody(aBodyData);
	}

	HBufC* newData = HBufC::NewLC(aBodyData.Length());
	TPtr16 ptr = newData->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,aBodyData);

	if (iReceiveData)
	{
		iReceiveData = iReceiveData->ReAllocL(iReceiveData->Length() + aBodyData.Length());
		iReceiveData->Des() += newData->Des();
	}
	else
	{
		iReceiveData = HBufC::NewL(aBodyData.Length());
		iReceiveData->Des().Copy(*newData);
	}

	
	CleanupStack::PopAndDestroy(newData);
}
const TDesC& CHttpRequestElement::GetReceiveData()
{
	if(!iReceiveData)
	{
		iReceiveData=HBufC::NewL(20);
		iReceiveData->Des().Copy(_L("Unkown Error!"));
	}

	if(iReceiveData8&&iFileName)
	{
		::WriteFile8(*iFileName,*iReceiveData8);
		DELETE(iReceiveData8);
	}

	return *iReceiveData;
}
void CHttpRequestElement::SetFileName(const TDesC& aFileName)
{
	if(iFileName)
	{
		delete iFileName;
		iFileName=NULL;
	}
	iFileName=aFileName.Alloc();
}
void CHttpRequestElement::GetClientBody(const TDesC8& aBody)
{
	if (iReceiveData8)
	{
		iReceiveData8 = iReceiveData8->ReAllocL(iReceiveData8->Length() + aBody.Length());
		iReceiveData8->Des().Append(aBody);
	}
	else
	{
		iReceiveData8 = HBufC8::NewL(aBody.Length());
		iReceiveData8->Des().Copy(aBody);
	}
}