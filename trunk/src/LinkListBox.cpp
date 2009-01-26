/*
============================================================================
 Name        : LinkListBox.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CLinkListBox implementation
============================================================================
*/

#include "LinkListBox.h"
#include "MainEngine.h"

#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "CoCoPreDefine.h"
#include "SearchResultItem.h"
#include "Graphic.h"
#include "MControlObserver.h"

CLinkListBox::CLinkListBox(MLinkListObserver& aObserver,CMainEngine& aMainEngine)
:CControl(ELinkListBox)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iUrl(NULL)
,iName(NULL)
{
	// No implementation required
}


CLinkListBox::~CLinkListBox()
{
	iPointArray.Close();
	DELETE(iUrl);
	DELETE(iName);
}

CLinkListBox* CLinkListBox::NewLC(MLinkListObserver& aObserver,CMainEngine& aMainEngine)
{
	CLinkListBox* self = new (ELeave)CLinkListBox( aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CLinkListBox* CLinkListBox::NewL(MLinkListObserver& aObserver,CMainEngine& aMainEngine)
{
	CLinkListBox* self=CLinkListBox::NewLC( aObserver,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CLinkListBox::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CLinkListBox::Draw(CGraphic& gc)const
{
	gc.SetPenColor(KTextColor);
	TInt index=0;
	TPoint point=iRect.iTl;
	for(TInt i=0;i<iMaxLine;i++)
	{
		index=i+iFirstIndex;
		if(index>iPointArray.Count()||iPointArray.Count()==0)
			break;

		if(iPointArray[index]->IsLink())
		{
			if(iSelectIndex==index)
				gc.DrawText_Red(iPointArray[index]->GetName(),TRect(point+TPoint(0,i*iTextSize.iHeight),iTextSize),CGraphicsContext::ELeft,0);
			else
				gc.DrawText_Blue(iPointArray[index]->GetName(),TRect(point+TPoint(0,i*iTextSize.iHeight),iTextSize),CGraphicsContext::ELeft,0);
		}
		else
		{
			gc.DrawText(iPointArray[index]->GetName(),TRect(point+TPoint(0,i*iTextSize.iHeight),iTextSize),CGraphicsContext::ELeft,0);
		}
	}
}

TBool CLinkListBox::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		HandleUpKey();
		break;
	case KKeyCodeDown:
		HandleDownKey();
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CLinkListBox::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}
void CLinkListBox::SizeChanged(const TRect& /*aScreenRect*/)
{

}
void CLinkListBox::SetRect(TRect aRect)
{
	iRect=aRect;
	InitSize();
}
void CLinkListBox::AppendItem(const TDesC& aName,const TDesC8& aUrl,TBool aBool)
{
	
	if(aBool)
	{
		CSearchResultItem* item=CSearchResultItem::NewL();
		item->SetIsLink(aBool);
		item->SetName(aName);
		item->SetUrl(aUrl);
		iPointArray.Append(item);
	}
	else
	{
		TInt textPos = 0;
 		while(textPos < aName.Length())
		{	
			TInt charNum = iMainEngine.ScreenLayout().Font()->TextCount(aName.Mid(textPos),iRect.Width());
 			const TDesC& tmpDes = aName.Mid(textPos,charNum);

			CSearchResultItem* citem=CSearchResultItem::NewL();
			citem->SetIsLink(aBool);
			citem->SetName(tmpDes);
			iPointArray.Append(citem);

			textPos += charNum;
		}
	}
}

void CLinkListBox::ResetList()
{
	iFirstIndex=0;
	iSelectIndex=0;
	iPointArray.ResetAndDestroy();
}
void CLinkListBox::LayoutList()
{
	for(TInt i=0;i<iPointArray.Count();i++)
	{
		if(iPointArray[i]->IsLink())
		{
			iFirstSelectIndex=i;
			iSelectIndex=i;
			break;
		}
	}
}

void CLinkListBox::InitSize()
{
	iTextSize.iWidth=iRect.Width();
	iTextSize.iHeight=iMainEngine.ScreenLayout().FontHeight()+iMainEngine.ScreenLayout().FontHeight()/2;
	iMaxLine=iRect.Height()/iTextSize.iHeight;
}

TBool CLinkListBox::IsFirst()
{
	return iBool;
}

const TDesC8& CLinkListBox::GetUrl()
{
	DELETE(iUrl);
	iUrl=iPointArray[iSelectIndex]->GetUrl().Alloc();
	return *iUrl;
}
const TDesC& CLinkListBox::GetName()
{
	DELETE(iName);
	iName=iPointArray[iSelectIndex]->GetName().Alloc();
	return *iName;
}
void CLinkListBox::HandleDownKey()
{
	if(iSelectIndex>=iPointArray.Count()-1||iFirstIndex>=iPointArray.Count()-1)
		return;

	if(iFirstIndex==0)
		iObserver.LinkListEvent(KKeyCodeDown,1);

	TInt index=iSelectIndex;

	if(!iPointArray[index+1]->IsLink())
	{
		for(TInt i=0;i<iPointArray.Count()-index-1;i++)
		{
			if(iPointArray[index+1+i]->IsLink())
			{
				index=index+1+i;
				break;
			}
		}
	}
	else
	{
		index++;
	}

	if((index>=iFirstIndex+iMaxLine||iSelectIndex==index)&&iFirstIndex<iPointArray.Count()-iMaxLine)
	{
		iFirstIndex++;
	}	

	if(index<iFirstIndex+iMaxLine)
	{
		if(iPointArray[index]->IsLink())
			iSelectIndex=index;
	}
}
void CLinkListBox::HandleUpKey()
{
	if(iSelectIndex<=0)
	{
		return;
	}

	TInt index=iSelectIndex;

	if(!iPointArray[index-1]->IsLink())
	{
		for(TInt i=0;i<index-iFirstIndex;i++)
		{
			if(iPointArray[index-1-i]->IsLink())
			{
				index=index-1-i;
				break;
			}
		}
	}
	else
	{
		index--;
	}

	if(iFirstIndex==0&&iFirstSelectIndex==iSelectIndex)
	{	
		iObserver.LinkListEvent(KKeyCodeUp,1);
	}
	
	if((index<=iFirstIndex||index==iSelectIndex)&&iFirstIndex>0)
	{
		iFirstIndex--;
	}	

	if(index>=iFirstIndex)
	{
		if(iPointArray[index]->IsLink())
			iSelectIndex=index;
	}
}
