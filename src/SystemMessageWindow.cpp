/*
============================================================================
 Name        : SystemMessageWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CSystemMessageWindow implementation
============================================================================
*/
#include <BAUTILS.H>
#include "SystemMessageWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "TypeDefine.h"

#include "PopUpMenu.h"
#include "Dialog.h"
#include "SystemInfoDialog.h"
#include "ListBox.h"
#include "ListBoxItem.h"
//#include "FileEngine.h"

#include "BitmapFactory.h"
//#include "PictureEngine.h"
#include "TitleBar.h"
#include "StaticCommonTools.h"
#include "FileReadByRow.h"
enum TMenuEventCommand
{
	EDeleteAll,
	EOk,
	EDelete
};
CSystemMessageWindow::CSystemMessageWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CSystemMessageWindow::~CSystemMessageWindow()
{
}

CSystemMessageWindow* CSystemMessageWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSystemMessageWindow* self = new (ELeave)CSystemMessageWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSystemMessageWindow* CSystemMessageWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSystemMessageWindow* self=CSystemMessageWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSystemMessageWindow::ConstructL()
{
	ChangeButton();
	iMainEngine.ControlFactory().SetParentWindow(this);

	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(0);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(0);
	iPageNum=0;//初始总的页数为0，即没有页
	iCurPage=0;

	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iTitle = iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_SystemMessage]);


	/*AddControl(iTitle);*/

	iMsgList=new (ELeave) TSysMsg[MSGLIST_NUM];
	GetMsgInfo();
	if(iMsgNum>0)
	{
		iCurPage=1;
		if (iMsgNum<(ITEM_NUM_PER_PAGE))
		{
			iPageNum=1;
		} 
		else//多于一页的情况
		{
			if ( (iMsgNum%(ITEM_NUM_PER_PAGE))==0)
			{
				iPageNum=iMsgNum/(ITEM_NUM_PER_PAGE);
			} 
			else
			{
				iPageNum=(iMsgNum/(ITEM_NUM_PER_PAGE))+1;
			}
		}


		iSysInfoListbox=iMainEngine.ControlFactory().CreateDefaultListBoxL();
		DisplaySysMsg(iCurPage);
		/*AddControl(iSysInfoListbox);*/
	}	
	DisplayPageNum();//显示页码
	
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSystemMessageWindow::DoActivateL()
{
}
//在title的右边显示页码
void CSystemMessageWindow::DisplayPageNum()
{
	//显示页码
	TBuf<3>page;
	TBuf<7>displayPage;//显示页码，格式：1/4
	page.Zero();
	displayPage.Zero();

	page.Num(iCurPage);
	displayPage.Append(page);//当前页码

	displayPage.Append(_L("/"));

	page.Zero();
	page.Num(iPageNum);//总共的页码
	displayPage.Append(page);

	iTitle->SetRightInfoL(displayPage);
}
void CSystemMessageWindow::DoDeactivate()
{
	//if (iMsgNum>0)//初始化了listbox的情况
	//{
	//	DeleteAllControl();
	//}
}
void CSystemMessageWindow::DisplaySysMsg(TInt aCurPage)
{
	if (iSysInfoListbox->Count()>0)
	{
		iSysInfoListbox->Reset();
	}

	TInt i=0;//要显示的下标
	TInt j=0;//控制循环
	if ((aCurPage>0)&&(aCurPage<=iPageNum)&&(iPageNum>0))
	{
		i=(ITEM_NUM_PER_PAGE)*(aCurPage-1);//起始index
		for (j;(j<ITEM_NUM_PER_PAGE)&&(i<iMsgNum);j++,i++)//初始，列表显示
		{
			iSysInfoListbox->AppendL(CNormalListBoxItem::NewL(iMsgList[i].nShowInfo,iIconBitmap,iIconBitmapMask));

		}
	}


}
void CSystemMessageWindow::DoDraw(CGraphic& aGraphic) const
{

}


TBool CSystemMessageWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	
		switch(aKeyCode)
		{
		case EKeyDevice3://ok键，显示系统信息,根据当前页数来显示
			if (iMsgNum>0)//有数据的情况
			{
				TInt index;
				index=iSysInfoListbox->CurItemIndex()+(iCurPage-1)*(ITEM_NUM_PER_PAGE);
				ShowSystemInfo(iMsgList[index].nFileName,iMsgList[index].nTitle);
			}
			break;
		case EKeyLeftArrow://翻页
			if ((iCurPage>1)&&(iMsgNum>0))
			{
				iCurPage--;
				DisplayPageNum();//更新页码
				DisplaySysMsg(iCurPage);//更新列表显示
			}
			break;
		case EKeyRightArrow://翻页
			if ((iCurPage<iPageNum)&&(iMsgNum>0))
			{
				iCurPage++;
				DisplayPageNum();//更新页码
				DisplaySysMsg(iCurPage);//更新列表显示
			}
			break;
		case KLeftSoftKey:
			{
				InitPopUpMenu();
			}
			break;
		case KRightSoftKey:

			GoBackToParentWindow();
			break;
		default:
			keyResult = EFalse;
			break;
		}

return keyResult;
}

TBool CSystemMessageWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSystemMessageWindow::DoSizeChanged()
{

}

void CSystemMessageWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CSystemMessageWindow::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	
	popUpMenu->AppendItem((*iRes->iItem)[ESystemManageRes_Ok],EOk);
	popUpMenu->AppendItem((*iRes->iItem)[ESystemManageRes_Delete],EDelete);
	popUpMenu->AppendItem((*iRes->iItem)[ESystemManageRes_DeleteAll],EDeleteAll);
	
}
//////////////////////////////////////////////////////////////////////////
//MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CSystemMessageWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case EOk:
		if (iMsgNum>0)//有数据的情况，显示
		{
			TInt index;
			index=iSysInfoListbox->CurItemIndex()+(iCurPage-1)*(ITEM_NUM_PER_PAGE);
			ShowSystemInfo(iMsgList[index].nFileName,iMsgList[index].nTitle);
		}
		break;
	case EDelete://清除
		if (iMsgNum>0)//有数据的情况
		{
			iMainEngine.ControlFactory().CreateInquireDialog(\
			(*iRes->iItem)[ESystemManageRes_Delete_Inquire]\
			,*this,1,2);
		}
		break;
	case EDeleteAll://清除所有
		if (iMsgNum>0)//有数据的情况
		{
			iMainEngine.ControlFactory().CreateInquireDialog(\
			(*iRes->iItem)[ESystemManageRes_DeleteAll_Inquire]\
			,*this,3,4);
		}
		break;
	
	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//MDialodObserver
//////////////////////////////////////////////////////////////////////////
void CSystemMessageWindow::DialogEvent(TInt aCommand)
{
	RFs& fs = CCoeEnv::Static()->FsSession();
	if (aCommand==1)//删除一项的情况,对显示的数据进行调整
	{
		if (iMsgNum>0)//有数据的情况
		{
			TInt index;
			index=iSysInfoListbox->CurItemIndex()+(iCurPage-1)*(ITEM_NUM_PER_PAGE);
			BaflUtils::DeleteFile(fs,iMsgList[index].nFileName);
			GetMsgInfo();
			if (iMsgNum>0)//删除之后仍然有数据
			{
				if (iMsgNum<(ITEM_NUM_PER_PAGE))
				{
					iPageNum=1;
				} 
				else//多于一页的情况
				{
					if ( (iMsgNum%(ITEM_NUM_PER_PAGE))==0)
					{
						iPageNum=iMsgNum/(ITEM_NUM_PER_PAGE);
					} 
					else
					{
						iPageNum=(iMsgNum/(ITEM_NUM_PER_PAGE))+1;
					}
				}
				if (iCurPage>iPageNum)
				{
					iCurPage=iPageNum;
				}
				DisplaySysMsg(iCurPage);

			}
			DisplayPageNum();//更新页码
		}
	}
	if (aCommand==3)//清空
	{
		if (iMsgNum>0)//有数据的情况
		{
			for (TInt i=0;i<iMsgNum;i++)
			{
				BaflUtils::DeleteFile(fs,iMsgList[i].nFileName);

			}
			iMsgNum=0;
			iCurPage=0;
			iPageNum=0;
			DisplayPageNum();//更新页码
			DisplaySysMsg(0);//更新显示，没有数据被显示出来
		}

	}
}
//From MInfoObserver
void CSystemMessageWindow::InfoResponseEvent(TInt aEvent)
{

}

void CSystemMessageWindow::GetMsgInfo()
{
	//从文件中获取系统消息
	TFileName szFilePath;

	szFilePath.Zero ();
	szFilePath.Append (iMainEngine.GetAppPath());
	szFilePath.Append(_L("SysInfo\\"));	
	//判断文件夹是否存在，不存在则建一个
	RFs fs;
	User::LeaveIfError(fs.Connect());
	if(!BaflUtils::PathExists(fs,szFilePath))
	{
		TInt err = fs.MkDir(szFilePath);;
		
		User::LeaveIfError(err);
	}
	fs.Close();

	CArrayFixFlat<TFileName>*	nFileNameArray;
	nFileNameArray= new (ELeave) CArrayFixFlat<TFileName>(10);

	CArrayFixFlat<TUint8>*	nDirArray;
	nDirArray = new (ELeave) CArrayFixFlat<TUint8>(10);

	if (iFileNameArray)
		delete iFileNameArray;

	iFileNameArray = new (ELeave) CArrayFixFlat<TCONTENTTEXT>(10);

	ReadFileNameList(szFilePath, nFileNameArray, nDirArray);

	TBuf<100> nTmp;
	TBuf<100> nInfo;
	TBuf<100> nTitle;

	iMsgNum=0;

	for(int i=0;i<nFileNameArray->Count();i++)
	{
		if(nDirArray->At(i)==0 && i<MSGLIST_NUM)//待改？？
		{
			nTmp.Zero ();
			nTmp.Append (nFileNameArray->At(i));

			nTitle.Zero ();
			nTitle.Append (nTmp.Mid (10));//出去日期以后的数据

			nInfo.Zero ();
			nInfo.Append (nTmp.Mid (0,2));
			nInfo.Append (_L("/"));
			nInfo.Append (nTmp.Mid (2,2));
			nInfo.Append (_L(" "));
			nInfo.Append (nTmp.Mid (4,2));
			nInfo.Append (_L(":"));
			nInfo.Append (nTmp.Mid (6,2));
			nInfo.Append (_L(" "));
			nInfo.Append (nTmp.Mid (10));

			iMsgList[i].nFileName.Zero();
			iMsgList[i].nShowInfo.Zero();
			iMsgList[i].nTitle.Zero();

			iMsgList[i].IsUsed =1;
			iMsgList[i].nFileName.Copy(szFilePath);
			iMsgList[i].nFileName.Append(nTmp);//路径＋文件名
			iMsgList[i].nShowInfo.Copy(nInfo);
			iMsgList[i].nTitle.Copy(nTitle); 

			iMsgNum++;
		}
	}


}

void CSystemMessageWindow::ShowSystemInfo(const TDesC& aFileName,const TDesC& aTitle)
{
	//从文件中获取信息并调用公告信息界面显示
	TFileName fn;
	fn = aFileName;

	RFs rfs;
	rfs.Connect();

	RFile oFile;
	if (KErrNone != oFile.Open(rfs, fn, EFileWrite|EFileShareAny))
	{
		rfs.Close();
		return;
	}

	TBuf<1024>	szContent;
	TBuf8<1024>	szContent1;

	szContent.Zero ();
	szContent1.Zero ();

	oFile.Read(szContent1);
	oFile.Close();
	rfs.Close();

	CnvUtfConverter::ConvertToUnicodeFromUtf8(szContent,szContent1);

	iMainEngine.ControlFactory().CreateInfoDialog(*this,aTitle,szContent,1);

	//显示
	
}
//读取某个目录下的文件列表
void CSystemMessageWindow::ReadFileNameList(const TDesC& strPath, CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TUint8>* pDirArray)
{
	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Append(strPath);	
	RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	

	CDir* dir = NULL;
	User::LeaveIfError(oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));

	int i;
	i=(*dir).Count()-1;
	for(;i>=0; i--)
	{
		const	TEntry & oEntry = (*dir)[i];
		TFileName oName = oEntry.iName;	

		if(oName.Right(2)!=_L("3G"))
		{
			pArray->AppendL(oName);	
			if (oEntry.IsDir()) 
				pDirArray->AppendL(1);
			else
				pDirArray->AppendL(0);
		}
	}

	dir=NULL;
	oRFs.Close();
}