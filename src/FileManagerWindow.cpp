/*
============================================================================
 Name        : FileManagerWindow.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileManagerWindow implementation
============================================================================
*/
#include <coemain.h>
#include <bautils.h>

#include "FileManagerWindow.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "FileExecuter.h"
#include "FileEngine.h"
#include "BitmapFactory.h"
#include "BitmapFactory.h"
#include "NaviPane.h"
#include "PopUpMenu.h"
#include "ListBox.h"
#include "FileAttribute.h"
#include "StaticCommonTools.h"
#include "UtilityTools.h"
#include "TypeDefine.h"

enum TEventCmd
{
	ECmdExecute,
	ECmdExecuteWith,
	ECmdShowAttribute,
	ECmdDelete,
	ECmdRename,
	ECmdSaveAs,    
	ECmdRefreshMenu,
	ECmdCreateFolder,
	ECmdCopyFile,
	ECmdPasteFile,
	ECmdGoBackToParentWindow,
	ECmdGoBackToMainWindow,

	ECmdSortByDate,
	ECmdSortByName
};
_LIT(KFILEMANAGERERROR,"fileerror:%d");

CFileManagerWindow::CFileManagerWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EDownloadedWindow,aParent,aMainEngine)
																				  , iFileEngine(aMainEngine.FileEngine())
																				  , iFs(CCoeEnv::Static()->FsSession())
																				  ,	iSortType(0)
{
	iFileEngine.SetObserver(this);
}


CFileManagerWindow::~CFileManagerWindow()
{
}

CFileManagerWindow* CFileManagerWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CFileManagerWindow* self = new (ELeave)CFileManagerWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileManagerWindow* CFileManagerWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CFileManagerWindow* self=CFileManagerWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CFileManagerWindow::ConstructL()
{
	ChangeButton();

	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL1"));
	iMainEngine.ControlFactory().SetParentWindow(this);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL2"));
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL3"));
	iIconBitmap = bitmapFactory.GetFileTypeIcon(0);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL4"));
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(0);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL5"));
	GetFolderPath(0);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL6"));
	InitNaviPane();
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL7"));
	InitListBox();
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ConstructL8"));

}
//////////////////////////////////////////////////////////////////////////
//From CWindow
//////////////////////////////////////////////////////////////////////////
void CFileManagerWindow::DoActivateL()
{
/*
	iMainEngine.ControlFactory().SetParentWindow(this);
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(0);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(0);
	GetFolderPath(0);
	InitNaviPane();
	InitListBox();
*/

}

void CFileManagerWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CFileManagerWindow::DoDraw(CGraphic& /*aGraphic*/) const
{

}

TBool CFileManagerWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case EKeyDevice0:
		InitPopUpMenu();
		break;

	case EKeyDevice1:
		GoBack();
		break;

	case EKeyDevice3:
		Enter();
		break;

		//哪个是删除键?
	case EKeyDelete:
	case EKeyBackspace:
		DeleteCurFile();
		break;

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CFileManagerWindow::DoHandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CFileManagerWindow::DoSizeChanged()
{

}

void CFileManagerWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Option));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}
//////////////////////////////////////////////////////////////////////////
//From MNaviPaneObserver
//////////////////////////////////////////////////////////////////////////
void CFileManagerWindow::NaviPaneIndexChanged(TInt aNewIndex)
{
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(aNewIndex);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(aNewIndex);

	GetFolderPath(aNewIndex);
	InitListBox();
	iIsSubFolder = EFalse;
}
//////////////////////////////////////////////////////////////////////////
//From MPopUpMenuObserver
//////////////////////////////////////////////////////////////////////////
void CFileManagerWindow::DoPopUpMenuCommand(TInt aCommand)
{
	switch(aCommand)
	{
	case ECmdExecute:
		//ExecuteCurFile();
		this->Enter();
		break;

	case ECmdExecuteWith:
		ExecuteCurFileWith();
		break;

	case ECmdShowAttribute:
		ShowCurFileAttribute();
		break;

	case ECmdDelete:
		DeleteCurFile();
		break;

	case ECmdRename:
		RenameCurFile();
		break;

	case ECmdSaveAs:
		SaveAsCurFile();
		break;
  
	case ECmdRefreshMenu:
		RefreshList();
		break;

	case ECmdCreateFolder:
		CreateFolder();
		break;

	case ECmdCopyFile:
		CopyFile();
		break;

	case ECmdPasteFile:
		PasteFile();
		break;

	case ECmdGoBackToParentWindow:
		GoBackToParentWindow();
		break;

	case ECmdGoBackToMainWindow:
		GoBackToMainWindow();
		break;

	case ECmdSortByDate:
		ShowFileByDate();
		break;

	case ECmdSortByName:
		ShowFileByName();
		break;

	default:
		ASSERT(EFalse);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
////From MFileEngineEvent
//////////////////////////////////////////////////////////////////////////
void CFileManagerWindow::Update(TInt /*aReason*/)
{
	//User::InfoPrint(_L("Update"));
	InitListBox();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFileManagerWindow::InitNaviPane()
{
	CNaviPane* naviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane(this);

	//文件类型 0 图片 1 音乐　2程序/游戏/主题　3视频　4电子书　5其他 (主题、程序、游戏属于一类)
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Images));
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Music));
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Game));
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Video));
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Book));	
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Topic));	
	naviPane->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Other));	
}

void CFileManagerWindow::InitPopUpMenu()
{
	CPopUpMenu* popUpMenu = iMainEngine.ControlFactory().CreatePopUpMenu(*this);
	
	if(iTempListBox && iTempListBox->Count() > 0)
	{
		TBool curIsFile = !CurIsFolder();

		CSubMenuItem* subItem = new (ELeave) CSubMenuItem(iMainEngine.GetDesById(ECoCoTextRes_FileBrowser_Sort));
		subItem->Append(iMainEngine.GetDesById(ECoCoTextRes_FileBrowser_SortByDate),ECmdSortByDate);
		subItem->Append(iMainEngine.GetDesById(ECoCoTextRes_FileBrowser_SortByName),ECmdSortByName);

		popUpMenu->AppendSubMenu(subItem);

		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_OpenFile),ECmdExecute);

		if(curIsFile)
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_OpenWith),ECmdExecuteWith);
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Attribute),ECmdShowAttribute);
		}

		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Delete),ECmdDelete);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_ReName),ECmdRename);

		if(curIsFile)
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_SaveAs),ECmdSaveAs);
		}

		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Program_Menu_Refresh),ECmdRefreshMenu);
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_CreateFolder),ECmdCreateFolder);

		if(curIsFile)
		{
			popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_CopyFile),ECmdCopyFile);
		}
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PasteFile),ECmdPasteFile);
	}
	else
	{
		popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_PasteFile),ECmdPasteFile);
	}
	popUpMenu->AppendItem(iMainEngine.GetDesById(ETurkeyTextRes_Back),ECmdGoBackToParentWindow);
	//popUpMenu->AppendItem(_L("main"),ECmdGoBackToMainWindow);	
}

void CFileManagerWindow::InitListBox()
{
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox1"));
	RemoveAndDeleteControl(iTempListBox);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox2"));
	iTempListBox = NULL;
	iTempListBox = iMainEngine.ControlFactory().CreateDefaultListBoxL();

	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox3"));
	CArrayFixFlat<TFileName>* array = new (ELeave) CArrayFixFlat<TFileName>(10);
	CleanupStack::PushL(array);
	CArrayFixFlat<TUint8>* array2 = new (ELeave) CArrayFixFlat<TUint8>(10);
	CleanupStack::PushL(array2);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox4"));
	ReadFileNameList(iCurPath,iSortType, array,array2);
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox5"));
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	for(TInt i = 0 ; i < array->Count() ; i++)
	{
		if(1 == (*array2)[i])//文件夹
		{
			const CFbsBitmap* iconBitmap = bitmapFactory.GetFileTypeIcon(EFileTypeFile);
			const CFbsBitmap* iconBitmapMask = bitmapFactory.GetFileTypeIconMask(EFileTypeFile);
			iTempListBox->AppendL(CNormalListBoxItem::NewL((*array)[i],iconBitmap,iconBitmapMask));
		}
	}
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox6"));
	for(TInt i = 0 ; i < array->Count() ; i++)
	{
		if(0 == (*array2)[i])//文件
		{
			const TDesC& fileName = (*array)[i];
/*
			TInt len = fileName.Length();
			TParse parse;
			parse.Set(fileName,NULL,NULL);
			TBuf<10> buf;
			buf.Append(parse.Ext().Mid(1));
			TInt fileType = ::CheckFileType(buf);

			const CFbsBitmap* iconBitmap = bitmapFactory.GetFileTypeIcon(fileType);
			const CFbsBitmap* iconBitmapMask = bitmapFactory.GetFileTypeIconMask(fileType);
*/

			const CFbsBitmap* iconBitmap = NULL;
			const CFbsBitmap* iconBitmapMask = NULL;
			iMainEngine.GetFileTypeIcon(fileName,iconBitmap,iconBitmapMask);

			iTempListBox->AppendL(CNormalListBoxItem::NewL(fileName,iconBitmap,iconBitmapMask));
		}
	}
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox7"));
	iTempListBox->Layout();
	CleanupStack::PopAndDestroy(2);	//array,array2
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox8"));
	iMainEngine.RequestDraw();
	iMainEngine.WriteLog16(_L("CFileManagerWindow::InitListBox8"));
}

void CFileManagerWindow::GetFolderPath(TInt aIndex)
{
	CDesCArray* desArray = new (ELeave) CDesCArrayFlat(7);
	CleanupStack::PushL(desArray);
	desArray->AppendL(KIMAGES);
	desArray->AppendL(KMUSIC);
	desArray->AppendL(KAPPS);
	desArray->AppendL(KVIDEO);
	desArray->AppendL(KBOOKS);
	desArray->AppendL(KTOPS);
	desArray->AppendL(KOTHER);

	const TDesC& des = (*desArray)[aIndex];
	ASSERT(des.Length() + iMainEngine.GetDownLoadDir().Length() <= iCurPath.MaxLength());

	iCurPath.Zero();
	iCurPath.Append(iMainEngine.GetDownLoadDir());
	iCurPath.Append(des);

	UtilityTools::WriteLogsL(iCurPath);

	ASSERT(BaflUtils::PathExists(iFs,iCurPath));

	CleanupStack::PopAndDestroy();
}
//以下方法需要调整
void CFileManagerWindow::ReadFileNameList(const TDesC& strPath,const TInt aSortType, CArrayFixFlat<TFileName>* pArray, CArrayFixFlat<TUint8>* pDirArray)
{
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList1"));
	if (NULL == strPath.Length())
		return;
	TFileName	szPath;
	szPath.Append(strPath);	
	RFs		oRFs;

	
	TInt eror=oRFs.Connect();
	iMainEngine.WriteLog16(KFILEMANAGERERROR,eror);
	//User::LeaveIfError(oRFs.Connect());	
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList2"));
	CDir* dir;

	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList3"));
	if (aSortType==0)
	{
		iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList=ESortByDate"));
		iMainEngine.WriteLog16(strPath);
		//User::LeaveIfError(oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttMatchMask,ESortByDate,dir));
		eror=oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttDir,ESortByDate,dir);
		iMainEngine.WriteLog16(KFILEMANAGERERROR,eror);
	} 
	else if (aSortType==1)
	{
		iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList=ESortByName"));
		iMainEngine.WriteLog16(strPath);
		//User::LeaveIfError(oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttMatchMask,ESortByName,dir));
		eror=oRFs.GetDir(strPath,KEntryAttNormal|KEntryAttDir,ESortByName,dir);
		iMainEngine.WriteLog16(KFILEMANAGERERROR,eror);
	}
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList4"));
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
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList5"));
	if(dir)
	{
		delete dir;
		dir=NULL;
	}
	oRFs.Close();
	iMainEngine.WriteLog16(_L("CFileManagerWindow::ReadFileNameList6"));
}

void CFileManagerWindow::ExecuteCurFile()
{
	iMainEngine.FileExecuter(this).OpenFileByDefault(GetCurFileFullName());

}

void CFileManagerWindow::ExecuteCurFileWith()
{
	iMainEngine.FileExecuter(this).OpenFileByOpenWith(GetCurFileFullName());
}

void CFileManagerWindow::EnterFolder()
{
	iCurPath = GetCurFileFullName();
	iCurPath.Append(_L("\\"));
	InitListBox();
	iIsSubFolder = ETrue;
}

void CFileManagerWindow::ShowCurFileAttribute()
{
	CFileAttribute* attribute = CFileAttribute::NewL(iMainEngine,GetCurFileFullName());
	AddControl(attribute);
// 	ResetLeftButton();
// 	ResetRightButton();
// 	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
// 	SetRightButtonL(KNullDesC);
}

void CFileManagerWindow::DeleteCurFile()
{
	TFileName fileName = GetCurFileFullName();
	TBool isFolder = EFalse;
	TInt err = BaflUtils::IsFolder(iFs,fileName,isFolder);
	if(KErrNone == err && isFolder)
	{
		fileName.Append(_L("\\"));
	}
	iFileEngine.RequestDeleteFile(fileName);
}

void CFileManagerWindow::RenameCurFile()
{
	iFileEngine.RequestRenameFile(GetCurFileFullName());
}

void CFileManagerWindow::SaveAsCurFile()
{
	iFileEngine.RequestSaveAs(GetCurFileFullName());
}

void CFileManagerWindow::RefreshList()
{
	InitListBox();
}

void CFileManagerWindow::CreateFolder()
{
	iFileEngine.RequestCreateFolder(iCurPath);
}

void CFileManagerWindow::CopyFile()
{
	iFileEngine.CopyFile(GetCurFileFullName());
}

void CFileManagerWindow::PasteFile()
{
	iMainEngine.WriteLog16(_L("CFileManagerWindow::PasteFile"));
	iMainEngine.WriteLog16(GetCurFileFullName());
	iFileEngine.PasteFile(GetCurFileFullName());
}

void CFileManagerWindow::Enter()
{
	if(iTempListBox->Count()>0)
	{
		if(CurIsFolder())
		{
			EnterFolder();
		}
		else
		{
			ExecuteCurFile();
		}
	}
}

void CFileManagerWindow::GoBack()
{
	if(iIsSubFolder)			//子目录,返回上一级目录
	{
		GoBackToParentFolder();
	}
	else						//非子目录,返回父界面
	{
		GoBackToParentWindow();
	}
}

void CFileManagerWindow::GoBackToParentFolder()
{
	iCurPath = UtilityTools::ParseParentPath(iCurPath);

	if(UtilityTools::ParseParentPath(iCurPath).Compare(iMainEngine.GetDownLoadDir()) == 0)
	{
		iIsSubFolder = EFalse;
	}
	InitListBox();
}

TBool CFileManagerWindow::CurIsFolder() const
{
	TFileName fileName = GetCurFileFullName();
	TBool isFolder = EFalse;
	TInt err = BaflUtils::IsFolder(iFs,fileName,isFolder);
	if(KErrNone != err)
	{
		isFolder = EFalse;
	}
	return isFolder;
}

TFileName CFileManagerWindow::GetCurFileFullName() const
{
	//ASSERT(iTempListBox);

	TFileName fullName;
	fullName.Append(iCurPath);

	if(iTempListBox)
	{
		if(iTempListBox->Count()>0)
		{
			CNormalListBoxItem& item = (CNormalListBoxItem&)iTempListBox->CurItem();
			fullName.Append(item.GetText());
		}
	}
	iMainEngine.WriteLog16(fullName);
	ASSERT(BaflUtils::FileExists(iFs,fullName));
	return fullName;
}

void CFileManagerWindow::ShowFileByDate()
{
	if (iSortType!=0)
	{
		iSortType=0;

		InitListBox();
	}
}

void CFileManagerWindow::ShowFileByName()
{
	if (iSortType!=1)
	{
		iSortType=1;

		InitListBox();
	}
}
