/*
============================================================================
 Name        : SearchResultTempItem.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSearchResultTempItem implementation
============================================================================
*/

#include "SearchResultTempItem.h"
#include "CoCoPreDefine.h"
#include "SearchResultItem.h"
#include <utf.h>
#include "StaticCommonTools.h"

CSearchResultTempItem::CSearchResultTempItem()
:iHomePageUrl(NULL)
,iPrePageUrl(NULL)
,iNextPageUrl(NULL)
,iEndPageUrl(NULL)
,iKeyWord(NULL)
,iUrl(NULL)
,iCurUrl(NULL)
{
	// No implementation required
}


CSearchResultTempItem::~CSearchResultTempItem()
{
	iPointArray.Close();
	DELETE(iHomePageUrl);
	DELETE(iPrePageUrl);
	DELETE(iNextPageUrl);
	DELETE(iEndPageUrl);
	DELETE(iKeyWord);
	DELETE(iUrl);
	DELETE(iCurUrl);
}

CSearchResultTempItem* CSearchResultTempItem::NewLC()
{
	CSearchResultTempItem* self = new (ELeave)CSearchResultTempItem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSearchResultTempItem* CSearchResultTempItem::NewL()
{
	CSearchResultTempItem* self=CSearchResultTempItem::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CSearchResultTempItem::ConstructL()
{
	iCurPage=0;
	iAllPage=0;

	iHomePageUrl=HBufC8::NewL(0);
	iPrePageUrl=HBufC8::NewL(0);
	iNextPageUrl=HBufC8::NewL(0);
	iEndPageUrl=HBufC8::NewL(0);
	iUrl=HBufC8::NewL(0);
}
void CSearchResultTempItem::AddItem(const TDesC& aName,const TDesC8 aUrl,TBool aBool)
{
	CSearchResultItem* item=CSearchResultItem::NewL();
	item->SetName(aName);
	item->SetUrl(aUrl);
	item->SetIsLink(aBool);
	iPointArray.Append(item);
}
void CSearchResultTempItem::CreatNewItem()
{
	CSearchResultItem* item=CSearchResultItem::NewL();
	iPointArray.Append(item);
}
CSearchResultItem& CSearchResultTempItem::GetItem(TInt aIndex)
{
	return *iPointArray[aIndex];
}
TInt CSearchResultTempItem::GetItemCount()
{
	return iPointArray.Count();
}
void CSearchResultTempItem::SetPrePageUrl(const TDesC8& aUrl)
{
	DELETE(iPrePageUrl);
	iPrePageUrl=aUrl.Alloc();
	if(iCurPage==0||iAllPage==0)
	{
		this->ParseUrl(aUrl);

		if(iCurPage>0)
			iCurPage++;
	}
}
void CSearchResultTempItem::SetNextPageUrl(const TDesC8& aUrl)
{
	DELETE(iNextPageUrl);
	iNextPageUrl=aUrl.Alloc();
	if(iCurPage==0||iAllPage==0)
	{
		this->ParseUrl(aUrl);

		if(iCurPage>0)
			iCurPage--;
	}
}
void CSearchResultTempItem::SetKeyWord(const TDesC& aText)
{
	DELETE(iKeyWord);
	iKeyWord=aText.Alloc();
}
void CSearchResultTempItem::SetCurPage(TInt aPage)
{
	iCurPage=aPage;
}	

void CSearchResultTempItem::SetAllPage(TInt aPage)
{
	iAllPage=aPage;	
}
const TDesC8& CSearchResultTempItem::GetGoToPageUrl(TInt aPage,TDes8& aUrl)
{
	if(iUrl->Find(_L8("http://"))!=0)
	{
		aUrl.Append(_L8("http://wap.roboo.com/"));
	}
	aUrl.Append(*iUrl);
	aUrl.Append(_L8("&pid="));
	aUrl.AppendNum(aPage);
	aUrl.Append(_L8("&ap="));
	aUrl.AppendNum(iAllPage);
	ASSERT(EFalse);	//没有返回值,请修改
	return aUrl;
}
void CSearchResultTempItem::ParseUrl(const TDesC8& aUrl)
{
	// Simage.wml?q=mm&pgs=3&or=3316&ap=1106&pid=2
	DELETE(iUrl);
	iUrl=aUrl.Alloc();

	TInt pid=aUrl.Find(_L8("pid="));
	TInt pidL=0;
	TInt ap=aUrl.Find(_L8("ap="));
	TInt apL=0;

	TBuf8<100> tempUrl;
	if(pid>-1)
	{
		tempUrl.Zero();
		tempUrl.Append(aUrl.Right(aUrl.Length()-4-pid));

		TInt po=tempUrl.Find(_L8("&"));
		if(po>-1)
		{
			pidL=po;
			iCurPage=::ConvertTDesC8ToTInt(tempUrl.Left(tempUrl.Find(_L8("&"))));
		}
		else
		{
			iCurPage=::ConvertTDesC8ToTInt(tempUrl);
		}
	}
	if(ap>-1)
	{
		tempUrl.Zero();
		tempUrl.Append(aUrl.Right(aUrl.Length()-3-ap));

		TInt po=tempUrl.Find(_L8("&"));
		if(po>-1)
		{
			apL=po;
			iAllPage=::ConvertTDesC8ToTInt(tempUrl.Left(tempUrl.Find(_L8("&"))));
		}
		else
		{
			iAllPage=::ConvertTDesC8ToTInt(tempUrl);
		}
	}

	pid=aUrl.Find(_L8("&pid="));
	iUrl->Des().Replace(pid,pidL+5,_L8(""));
	ap=aUrl.Find(_L8("&ap="));
	iUrl->Des().Replace(ap,apL+4,_L8(""));


	if(iPointArray.Count()==0||iPointArray[0]->GetName().Find(_L("Page:"))<0)
	{
		TBuf<50> pageBuf;
		pageBuf.Append(_L("Page:"));
		pageBuf.Append(_L("["));
		pageBuf.AppendNum(iCurPage);
		pageBuf.Append(_L("/"));
		pageBuf.AppendNum(iAllPage);
		pageBuf.Append(_L("]"));

		CSearchResultItem* item=CSearchResultItem::NewL();
		item->SetName(pageBuf);
		item->SetUrl(_L8(""));
		item->SetIsLink(EFalse);
		iPointArray.Insert(item,0);
	}
	
}
const TDesC8& CSearchResultTempItem::GetHomePageUrl()
{
	if(iHomePageUrl->Length()==0||iHomePageUrl->Find(_L8("http://"))!=0)
	{
		DELETE(iHomePageUrl);
		iHomePageUrl=HBufC8::NewL(300);
		iHomePageUrl->Des().Append(_L8("http://wap.roboo.com/"));
		iHomePageUrl->Des().Append(*iUrl);
		iHomePageUrl->Des().Append(_L8("&pid="));
		iHomePageUrl->Des().AppendNum(1);
		iHomePageUrl->Des().Append(_L8("&ap="));
		iHomePageUrl->Des().AppendNum(iAllPage);
	}
	return *iHomePageUrl;
}
const TDesC8& CSearchResultTempItem::GetPrePageUrl()
{
	if(iPrePageUrl->Length()==0||iPrePageUrl->Find(_L8("http://"))!=0)
	{
		DELETE(iPrePageUrl);
		iPrePageUrl=HBufC8::NewL(300);
		iPrePageUrl->Des().Append(_L8("http://wap.roboo.com/"));
		iPrePageUrl->Des().Append(*iUrl);
		iPrePageUrl->Des().Append(_L8("&pid="));
		iPrePageUrl->Des().AppendNum(--iCurPage);
		iPrePageUrl->Des().Append(_L8("&ap="));
		iPrePageUrl->Des().AppendNum(iAllPage);
	}

	return *iPrePageUrl;
}
const TDesC8& CSearchResultTempItem::GetNextPageUrl()
{
	if(iNextPageUrl->Length()==0||iNextPageUrl->Find(_L8("http://"))!=0)
	{
		DELETE(iNextPageUrl);
		iNextPageUrl=HBufC8::NewL(300);
		iNextPageUrl->Des().Append(_L8("http://wap.roboo.com/"));
		iNextPageUrl->Des().Append(*iUrl);
		iNextPageUrl->Des().Append(_L8("&pid="));
		iNextPageUrl->Des().AppendNum(++iCurPage);
		iNextPageUrl->Des().Append(_L8("&ap="));
		iNextPageUrl->Des().AppendNum(iAllPage);
	}
	return *iNextPageUrl;
}
const TDesC8& CSearchResultTempItem::GetEndPageUrl()
{
	if(iEndPageUrl->Length()==0||iEndPageUrl->Find(_L8("http://"))!=0)
	{
		DELETE(iEndPageUrl);
		iEndPageUrl=HBufC8::NewL(300);
		iEndPageUrl->Des().Append(_L8("http://wap.roboo.com/"));
		iEndPageUrl->Des().Append(*iUrl);
		iEndPageUrl->Des().Append(_L8("&pid="));
		iEndPageUrl->Des().AppendNum(iAllPage);
		iEndPageUrl->Des().Append(_L8("&ap="));
		iEndPageUrl->Des().AppendNum(iAllPage);
	}
	return *iEndPageUrl;
}
TBool CSearchResultTempItem::IsTurnPage()
{
	TBool tBool=EFalse;
	if(iUrl->Length()>0)
		tBool=ETrue;

	return tBool;
}
void CSearchResultTempItem::SetCurUrl(const TDesC8& aUrl)
{
	DELETE(iCurUrl);
	iCurUrl=aUrl.Alloc();
}
