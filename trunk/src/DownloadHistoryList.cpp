/*
============================================================================
 Name        : DownloadHistoryList.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadHistoryList implementation
============================================================================
*/

#include "DownloadHistoryList.h"
#include "DownloadManagerWindow.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "BitmapFactory.h"
#include "FileExecuter.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "StaticCommonTools.h"
#include "PreDefine.h"
#include "TypeDefine.h"

enum TEventCmd
{
	ECmdNull,
	ECmdStartPage,
	ECmdPrePage,
	ECmdNextPage,
	ECmdEndPage,
	ECmdGotoPage,
	ECmdOpenFile,
	ECmdOpenFileWith,
	ECmdDelFromList,
	ECmdDelFromFile,
	ECmdDelAllFromList,
	ECmdDelAllFromFile,
	ECmdSetting,
	ECmdGoBackToParentWindow,
	ECmdGoBackToMainWindow
};
CDownloadHistoryList::CDownloadHistoryList(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow) 
																	: CControl(EDownloadHistoryList)
																	, iMainEngine(aMainEngine)
																	, iDownloadManagerWindow(aDownloadManagerWindow)
																	, iDownLoadHistoryEngine(iMainEngine.GetDownLoadHistoryEngine())
{
	iDownLoadHistoryEngine.SetNoticedWindow(this);
}

CDownloadHistoryList::~CDownloadHistoryList()
{
	TBool success = iDownloadManagerWindow.RemoveControl(iListBox);
	if(success)
	{
		delete iListBox;
	}
	iDownLoadHistoryInfoArray.Close();
	iDownLoadHistoryEngine.SetNoticedWindow(NULL);
}

CDownloadHistoryList* CDownloadHistoryList::NewLC(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow)
{
	CDownloadHistoryList* self = new (ELeave)CDownloadHistoryList(aMainEngine,aDownloadManagerWindow);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadHistoryList* CDownloadHistoryList::NewL(CMainEngine& aMainEngine,CDownloadManagerWindow& aDownloadManagerWindow)
{
	CDownloadHistoryList* self=CDownloadHistoryList::NewLC(aMainEngine,aDownloadManagerWindow);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadHistoryList::ConstructL()
{
	iTotalPageNum = iDownLoadHistoryEngine.PageCount();
	iPageIndex = 0;
	InitListBox(iPageIndex);
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::Draw(CGraphic& /*aGraphic*/)const
{

}
TBool CDownloadHistoryList::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
		InitPopUpMenu();
		break;

		//哪个是删除键?
	case EKeyDelete:
	case EKeyBackspace:
		break;

	case EKeyDevice3:
		OpenCurFile();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CDownloadHistoryList::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CDownloadHistoryList::SizeChanged(const TRect& /*aScreenRect*/)
{
}
//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdStartPage:
		GetStartPage();
		break;

	case ECmdPrePage:
		GetPrePage();
		break;

	case ECmdNextPage:
		GetNextPage();
		break;

	case ECmdEndPage:
		GetEndPage();
		break;

	case ECmdGotoPage:
		GotoPage();
		break;

	case ECmdOpenFile:
		OpenCurFile();
		break;

	case ECmdOpenFileWith:
		OpenCurFileWith();
		break;

	case ECmdDelFromList:
		DelTaskFromList();
		break;

	case ECmdDelFromFile:
		DelTaskFromFile();
		break;

	case ECmdDelAllFromList:
		DelAllTaskFromList();
		//InitListBox()
		break;

	case ECmdDelAllFromFile:	
		DelAllTaskFromFile();
		break;

	case ECmdSetting:
		iDownloadManagerWindow.MakeChildWindow(EDownloadHistorySettingWindow);
		break;

	case ECmdGoBackToParentWindow:
		iDownloadManagerWindow.GoBackToParentWindow();
		break;

	case ECmdGoBackToMainWindow:
		iDownloadManagerWindow.GoBackToMainWindow();
		break;

	//如何添加返回功能

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//From MDownLoadHistoryNotice
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::Update()
{
	iTotalPageNum = iDownLoadHistoryEngine.PageCount();
	InitListBox(iPageIndex);
}
//////////////////////////////////////////////////////////////////////////
//From MInputObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::InputResponseEvent(TInt aEvent,const TDesC& aText)
{
	if(aEvent == 0)
	{
		TLex lex(aText);
		TInt index = 0;
		TInt err = lex.Val(index);
		if(KErrNone == err)
		{
			if(index > 0 && index <= iTotalPageNum)
			{
				if(index - 1 != iPageIndex)
				{
					iPageIndex = index - 1;
					InitListBox(iPageIndex);
				}
			}
			else
			{
				iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ECoCoTestRes_UnKownContentError),*this,ECmdNull);	//输入超出范围
			}
		}
		else
		{
			iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_NumError),*this,ECmdNull);			//请输入有效的阿拉伯数字/无效的输入
		}
	}
	else
	{
		ASSERT(EFalse);
	}
}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::DialogEvent(TInt /*aCommand*/)
{

}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CDownloadHistoryList::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);


	if(iListBox->Count() > 0)
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_OpenFile),ECmdOpenFile);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_OpenWith),ECmdOpenFileWith);
	}
	if(iTotalPageNum > 1)	//只有多于等于一页才显示页控制选项
	{
		if(iPageIndex > 0)
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_StartPage),ECmdStartPage);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_UpPage),ECmdPrePage);
		}
		if(iPageIndex < iTotalPageNum - 1)
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownPage),ECmdNextPage);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_EndPage),ECmdEndPage);
		}
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_GoToPage),ECmdGotoPage);
	}

	if(iListBox->Count() > 0)
	{
		//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_OpenFile),ECmdOpenFile);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelFromList),ECmdDelFromList);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelFromFile),ECmdDelFromFile);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelAllFromList),ECmdDelAllFromList);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadMenu_DelAllFromFile),ECmdDelAllFromFile);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadHistorySet1),ECmdSetting);
	}
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToParentWindow);
	//popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToMainWindow);
}

void CDownloadHistoryList::InitListBox(TInt aPageIndex)
{
	iDownLoadHistoryInfoArray.Reset();
	iDownLoadHistoryEngine.GetDownLoadHistoryInfo(iDownLoadHistoryInfoArray,aPageIndex + 1);

/*
	TBool success = iDownloadManagerWindow.RemoveControl(iListBox);
	if(success)
	{
		delete iListBox;
		iListBox = NULL;
	}
	iListBox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
*/
	if(NULL == iListBox)
	{
		iListBox = iMainEngine.ControlFactory().CreateDefaultListBoxL();
	}
	else
	{
		iListBox->Reset();
	}

	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	for (TInt i = 0 ; i < iDownLoadHistoryInfoArray.Count() ; i++)
	{
		TBuf<130>& fileName = iDownLoadHistoryInfoArray[i].nFileName;

		TInt len = fileName.Length();
		TParse parse;
		parse.Set(fileName,NULL,NULL);
		TBuf<10> buf;
		buf.Append(parse.Ext().Mid(1));
		TInt fileType = ::CheckFileType(buf);

		const CFbsBitmap* iconBitmap = bitmapFactory.GetFileTypeIcon(fileType);
		const CFbsBitmap* iconBitmapMask = bitmapFactory.GetFileTypeIconMask(fileType);

		iListBox->AppendL(CNormalListBoxItem::NewL(fileName,iconBitmap,iconBitmapMask));
	}
	iListBox->Layout();
}

void CDownloadHistoryList::GetStartPage()
{
	if(iPageIndex != 0)
	{
		iPageIndex = 0;
		InitListBox(iPageIndex);
	}
}

void CDownloadHistoryList::GetPrePage()
{
	if(iPageIndex > 0)
	{
		iPageIndex--;
		InitListBox(iPageIndex);
	}
}

void CDownloadHistoryList::GetNextPage()
{
	if(iPageIndex < iTotalPageNum - 1)
	{
		iPageIndex++;
		InitListBox(iPageIndex);
	}
}

void CDownloadHistoryList::GetEndPage()
{
	if(iPageIndex != iTotalPageNum - 1)
	{
		iPageIndex = iTotalPageNum - 1;
		InitListBox(iPageIndex);
	}
}

void CDownloadHistoryList::GotoPage()
{
	const TInt KCmd = 0;
	iMainEngine.ControlFactory().CreateInputNumberDialog(*this,iMainEngine.GetDesById(ETurkeyTextRes_Search_GoPage),iMainEngine.GetDesById(ETurkeyTextRes_NumError),KCmd);
}

void CDownloadHistoryList::OpenCurFile()
{
	if(iListBox && iListBox->Count() > 0)
	{
		CNormalListBoxItem& curListItem = (CNormalListBoxItem&)(iListBox->CurItem());
		const TDesC& des = curListItem.GetText();
		TFileName tmp;
		tmp.Append(des.Left(tmp.MaxLength()));
		TInt len = tmp.Length();
		len = des.Length();
		//User::InfoPrint(des);
		TFileName fileName = iDownLoadHistoryEngine.ParseUrl(tmp);
		//TFileName fileName = iDownLoadHistoryEngine.ParseUrl(iListBox->GetCurItemDes());
		TFileName fileFullName;
		fileFullName.Append(iMainEngine.GetDownLoadDir());
		fileFullName.Append(fileName);
		//iMainEngine.ControlFactory().CreateTipDialog(fileName,*this,ECmdNull);
		iMainEngine.FileExecuter(&iDownloadManagerWindow).OpenFileByDefault(fileName);
	}
}

void CDownloadHistoryList::OpenCurFileWith()
{
	if(iListBox && iListBox->Count() > 0)
	{
		CNormalListBoxItem& curListItem = (CNormalListBoxItem&)(iListBox->CurItem());
		const TDesC& des = curListItem.GetText();
		TFileName tmp;
		tmp.Append(des.Left(tmp.MaxLength()));
		TInt len = tmp.Length();
		len = des.Length();
		//User::InfoPrint(des);
		TFileName fileName = iDownLoadHistoryEngine.ParseUrl(tmp);
		//TFileName fileName = iDownLoadHistoryEngine.ParseUrl(iListBox->GetCurItemDes());
		TFileName fileFullName;
		fileFullName.Append(iMainEngine.GetDownLoadDir());
		fileFullName.Append(fileName);
		//iMainEngine.ControlFactory().CreateTipDialog(fileName,*this,ECmdNull);
		iMainEngine.FileExecuter(&iDownloadManagerWindow).OpenFileByOpenWith(fileName);
	}
}

void CDownloadHistoryList::DelTaskFromList()
{
	TInt taskId = iDownLoadHistoryInfoArray[iListBox->CurItemIndex()].nListID;
	iDownLoadHistoryEngine.DelDownLoadHistory(taskId,0);
	InitListBox(iPageIndex);
}

void CDownloadHistoryList::DelTaskFromFile()
{
	TInt taskId = iDownLoadHistoryInfoArray[iListBox->CurItemIndex()].nListID;
	iDownLoadHistoryEngine.DelDownLoadHistory(taskId,1);
	InitListBox(iPageIndex);
}

void CDownloadHistoryList::DelAllTaskFromList()
{
	iDownLoadHistoryEngine.DelDownLoadHistory(-1,0);
	InitListBox(iPageIndex);
}

void CDownloadHistoryList::DelAllTaskFromFile()
{
	iDownLoadHistoryEngine.DelDownLoadHistory(-1,1);
	InitListBox(iPageIndex);
}