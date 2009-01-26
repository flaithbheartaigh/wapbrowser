/*
============================================================================
 Name        : FileBrowser.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CFileBrowser implementation
============================================================================
*/
#include <coemain.h>
#include <bautils.h>
#ifdef __SERIES60_3X__
	#include <akniconutils.h>
#endif

#include "FileBrowser.h"
#include "FileBrowserObserver.h"
#include "Window.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "BitmapFactory.h"
#include "ScreenLayout.h"
#include "Graphic.h"
#include "ListBox.h"
#include "PreDefine.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "CoCoPreDefine.h"

_LIT(KCDriver,		"C:");
_LIT(KEDriver,		"E:");
_LIT(KZDriver,		"Z:");
_LIT(KBackslash,	"\\");

CFileBrowser::CFileBrowser(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType) 
	: CControl(EFileBrowser)
	, iMainEngine(aMainEngine)
	, iObserver(aFileBrowserObserver)
	, iBrowserType(aBrowserType)
	, iFs(CCoeEnv::Static()->FsSession())
	, iLeftCommond(KCmdEventNull)
	, iRightCommand(KCmdEventNull)
{
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CFileBrowser::~CFileBrowser()
{
	delete iListBox;
	delete iDesArray;
	delete iSaveAsBmp;
	delete iTitleText;
}

CFileBrowser* CFileBrowser::NewLC(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType)
{
	CFileBrowser* self = new (ELeave)CFileBrowser(aMainEngine,aFileBrowserObserver,aBrowserType);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileBrowser* CFileBrowser::NewL(CMainEngine& aMainEngine,MFileBrowserObserver& aFileBrowserObserver,TInt aBrowserType)
{
	CFileBrowser* self=CFileBrowser::NewLC(aMainEngine,aFileBrowserObserver,aBrowserType);
	CleanupStack::Pop(); // self;
	return self;
}

void CFileBrowser::ConstructL()
{
	//iTitleText = aTitleText.AllocL();

	if(EBrowserFolders == iBrowserType)
	{
		iTitleText =iMainEngine.GetDesById(ECoCoTestRes_SelectFolder).AllocL();
	}
	else
	{
		iTitleText = iMainEngine.GetDesById(ECoCoTestRes_SelectFile).AllocL();
	}

	iDesArray = new (ELeave) CDesCArrayFlat(10);

	InitBitmaps();

	InitRootDir();
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CFileBrowser::Draw(CGraphic& aGraphic)const
{
	ASSERT(iListBox);
	ASSERT(iTitleText);

	aGraphic.BitBlt(iStartPoint,iSaveAsBmp);
	//aGraphic.SetPenColor(KRgbBlack);
	aGraphic.SetPenColor(KSelectedTextColor);
	
	aGraphic.SetPenStyle(CGraphicsContext::ESolidPen);
	aGraphic.DrawText(iStartPoint + iTextPoint,*iTitleText);

	iListBox->Draw(aGraphic);
}

TBool CFileBrowser::KeyEventL(TInt aKeyCode)
{
	ASSERT(iListBox);

	TBool keyResult = iListBox->KeyEventL(aKeyCode);
	if(!keyResult)
	{
		keyResult = ETrue;
		switch(aKeyCode)
		{
		case EKeyDevice0:
			DoConfirm();
			break;

		case EKeyLeftArrow:
			ReadParentDir();
			break;

		case EKeyRightArrow:
		case EKeyDevice3:
			//DoReadSub();
			ReadSub();
			break;

		case EKeyDevice1:
			DoCancel();
			break;

		default:
			keyResult = EFalse;
			break;
		}
	}
	return ETrue;	//屏蔽底层操作
}

TBool CFileBrowser::HandleCommandL(TInt aCommand)
{
	ASSERT(iListBox);

	return iListBox->HandleCommandL(aCommand);
}

void CFileBrowser::SizeChanged(const TRect& aScreenRect)
{
	ASSERT(iListBox);

	iListBox->SizeChanged(aScreenRect);
}

const TDesC& CFileBrowser::LeftButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Ok);
}

const TDesC& CFileBrowser::RightButton() const
{
	return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
}
//////////////////////////////////////////////////////////////////////////
//public
//////////////////////////////////////////////////////////////////////////
void CFileBrowser::SetPath(const TDesC& aFilePath)
{
	ASSERT(BaflUtils::PathExists(iFs,aFilePath));
	iCurrentPath.Zero();
	iCurrentPath.Append(aFilePath);
    iMainEngine.WriteLog16(iCurrentPath);
	InitDirList(iCurrentPath);
}

void CFileBrowser::SetTitle(const TDesC& aTitleText)
{
	delete iTitleText;
	iTitleText = NULL;
	iTitleText = aTitleText.AllocL();
}

void CFileBrowser::SetCommandType(TInt aLeftCommand,TInt aRightCommand)
{
	iLeftCommond = aLeftCommand;
	iRightCommand = aRightCommand;
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CFileBrowser::DoConfirm()
{
	if(EBrowserFolders == iBrowserType)
	{
		HBufC* tempFilePath = GetFileOrPathLC();
		iMainEngine.WriteLog16(*tempFilePath);
		ASSERT(BaflUtils::PathExists(iFs,*tempFilePath));
		MFileBrowserObserver& observer = iObserver;
		TInt command = iLeftCommond;
		CleanupSelf();
		observer.DoFileEvent(*tempFilePath,command);
		CleanupStack::PopAndDestroy();	//tempFilePath
	}
	else
	{
		ReadSub();
	}
}

void CFileBrowser::DoCancel()
{
	HBufC* tempFilePath = GetFileOrPathLC();
	MFileBrowserObserver& observer = iObserver;
	TInt command = iRightCommand;
	CleanupSelf();
	observer.DoFileEvent(*tempFilePath,command);
	CleanupStack::PopAndDestroy();	//tempFilePath
}

void CFileBrowser::InitRootDir()
{
	ASSERT(iDesArray && iDesArray->Count() == 0);
	iCurrentPath.Zero();
	iMainEngine.WriteLog16(iCurrentPath);
	//iDesArray->Reset();
	iDesArray->AppendL(KCDriver);
	iFolderNum++;
#ifndef __WINS__
	iDesArray->AppendL(KEDriver);
	iFolderNum++;
#endif
	if(EBroswerFiles == iBrowserType)
	{
		iDesArray->AppendL(KZDriver);
		iFolderNum++;
	}
	InitListBox();
}
//如何处理空列表??
//即空文件夹的情况
void CFileBrowser::ReadSub()
{
	HBufC* tempFilePath = GetFileOrPathLC();

	if(EBroswerFiles == iBrowserType)
	{
		if(BaflUtils::FileExists(iFs,*tempFilePath))	//是一个文件
		{
			MFileBrowserObserver& observer = iObserver;
			TInt command = iLeftCommond;
			CleanupSelf();
			observer.DoFileEvent(*tempFilePath,iLeftCommond);
		}
		else
		{
			ASSERT(BaflUtils::PathExists(iFs,*tempFilePath));
			InitDirList(*tempFilePath);
		}
	}
	else
	{
		ASSERT(BaflUtils::PathExists(iFs,*tempFilePath));
		InitDirList(*tempFilePath);
	}
	CleanupStack::PopAndDestroy();	//tempFilePath
}

void CFileBrowser::ReadParentDir()
{
	if(iCurrentPath.Length() == 0)	//当前路径为空时说明为根目录
	{
		//什么都不做
	}
	else
	{
		if(iCurrentPath.Length() == 3)	//根目录的子目录
		{
			InitRootDir();
		}
		else
		{
			iMainEngine.WriteLog16(iCurrentPath);
			iCurrentPath = ParseParentPath(iCurrentPath);
			iMainEngine.WriteLog16(iCurrentPath);
			InitDirList(iCurrentPath);
			iMainEngine.WriteLog16(iCurrentPath);
		}
	}
}

//--------------------------------------------------------------------------
//InitDirList
//若某目录为空,则不改变当前文件列表
//--------------------------------------------------------------------------
void CFileBrowser::InitDirList(const TDesC& aFilePath)
{
	iMainEngine.WriteLog16(_L("CFileBrowser::InitDirList"));
	iMainEngine.WriteLog16(aFilePath);
	ASSERT(BaflUtils::PathExists(iFs,aFilePath));
	ASSERT(iDesArray && iDesArray->Count() == 0);
	RFs& fs = CCoeEnv::Static()->FsSession();
	CDir* dir = NULL;
	fs.GetDir(aFilePath, KEntryAttNormal|KEntryAttDir, ESortByName, dir);	
	ASSERT(dir);

	TFileName tFilePath;
	

	if(dir->Count() > 0)
	{
		iFolderNum = 0;
		iFileNum = 0;

		for (TInt i = 0 ; i < dir->Count() ; i++)
		{
			const TEntry& entry = (*dir)[i];
			iMainEngine.WriteLog16(entry.iName);
			if (entry.IsDir())
			{
				//卞涛增加对是否可访问的文件夹做判断，防止出现没有访问权限的文件夹
				tFilePath.Zero();
				tFilePath.Append(aFilePath);
				if(aFilePath.Right(1).Compare(_L("\\"))!=0)
				{
					tFilePath.Append(_L("\\"));
				}
				tFilePath.Append(entry.iName);
				tFilePath.Append(_L("\\"));

				TBool tempIsFolder;
				TInt err = BaflUtils::IsFolder(iFs,tFilePath,tempIsFolder);

				if(err==KErrNone)
				{
					iDesArray->AppendL(entry.iName);
					iFolderNum++;
				}
			}
		}
		if(EBroswerFiles == iBrowserType)
		{
			for (TInt i = 0 ; i < dir->Count() ; i++)
			{
				const TEntry& entry = (*dir)[i];
				if (!entry.IsDir())
				{

					if(entry.IsSystem()||entry.IsHidden())
						continue;

					iDesArray->AppendL(entry.iName);	
					iFileNum++;
				}
			}
		}
		if(iDesArray->Count() > 0)
		{
			iCurrentPath = aFilePath;
/*
			iMainEngine.WriteLog16(aFilePath);
			iCurrentPath.Zero();
			iCurrentPath.Append(aFilePath);
			iMainEngine.WriteLog16(iCurrentPath);
			iMainEngine.WriteLog16(aFilePath);
*/

			InitListBox();
		}
	}
	iMainEngine.WriteLog16(_L("CFileBrowser::InitDirList End"));
}

void CFileBrowser::InitListBox()
{
	delete iListBox;
	iListBox = NULL;
	iListBox = iMainEngine.ControlFactory().CreateListBox(iListBoxRect);
	iListBox->SetTextColor(KRgbBlack,KRgbBlack);
	iMainEngine.CurWindow()->RemoveControl(iListBox);
	for (TInt i = 0 ; i < iDesArray->Count() ; i++)
	{
		const CFbsBitmap* icon = NULL;
		const CFbsBitmap* iconMask = NULL;
		const TDesC& fileName = (*iDesArray)[i];
		if(i < iFolderNum)
		{
			icon = iMainEngine.GetBitmapFactory().GetFileTypeIcon(EFileTypeFile);
			iconMask = iMainEngine.GetBitmapFactory().GetFileTypeIconMask(EFileTypeFile);
		}
		else
		{
			const CFbsBitmap* iconBitmap = NULL;
			const CFbsBitmap* iconBitmapMask = NULL;
			iMainEngine.GetFileTypeIcon(fileName,iconBitmap,iconBitmapMask);
		}
		iListBox->AppendL(CNormalListBoxItem::NewL(fileName,icon,iconMask));
	}
	iListBox->Layout();
	iDesArray->Reset();
}

HBufC* CFileBrowser::GetFileOrPathLC() const
{
	ASSERT(iListBox);
	iMainEngine.WriteLog16(iCurrentPath);
	CNormalListBoxItem& curListItem = (CNormalListBoxItem&)(iListBox->CurItem());
	//const TDesC& curFileName = curListItem.GetText();2版本手机长度有问题
	TFileName curFileName;
	curFileName.Copy(curListItem.GetText());

	iMainEngine.WriteLog16(curFileName);
	TInt length = iCurrentPath.Length() + curFileName.Length() + KBackslash.iTypeLength;
	HBufC* tempFilePath = HBufC::NewLC(length);
	tempFilePath->Des().Append(iCurrentPath);

	if(curFileName.Right(1).Compare(_L(":"))!=0||iCurrentPath.Length()==0)
		tempFilePath->Des().Append(curFileName);

	if(tempFilePath->Right(1).Compare(_L("\\"))!=0)
		tempFilePath->Des().Append(KBackslash);

	iMainEngine.WriteLog16(*tempFilePath);
	TBool curIsFolder = EFalse;
	TInt err = BaflUtils::IsFolder(iFs,*tempFilePath,curIsFolder);
	if(KErrNone == err)
	{
		if(curIsFolder)
		{
			if(tempFilePath->Right(1).Compare(_L("\\"))!=0)
				tempFilePath->Des().Append(KBackslash);

			ASSERT(BaflUtils::PathExists(iFs,*tempFilePath));
		}
		else
		{
			ASSERT(BaflUtils::FileExists(iFs,*tempFilePath));
		}
	}
	else
	{
		switch(err)
		{
		case KErrNotFound:	//根目录
			tempFilePath->Des().Append(KBackslash);
			iMainEngine.WriteLog16(*tempFilePath);
			ASSERT(BaflUtils::PathExists(iFs,*tempFilePath));
			break;
			
		default:
			iMainEngine.WriteLog16(*tempFilePath);
			iMainEngine.WriteLog16(_L("CFileBrowser::GetFileOrPathLC err = %d"),err);
			ASSERT(EFalse);
			break;
		}
	}
	return tempFilePath;
}

void CFileBrowser::InitBitmaps()
{
	iSaveAsBmp = iMainEngine.GetBitmapFactory().GetBitmap(EMbmCocobmpSaveas);
	
	const CScreenLayout& screenLayout = iMainEngine.ScreenLayout(); 

	iBmpSize = iSaveAsBmp->SizeInPixels();


	//上下左右居中
	TRect clientRect = screenLayout.GetClientRect();

	iStartPoint = clientRect.iTl;
	iStartPoint.iX += (clientRect.Width() - iBmpSize.iWidth)/2;
	iStartPoint.iY += (clientRect.Height() - iBmpSize.iHeight)/2;

	iLineHeight = screenLayout.LineHeight();

	TPoint listPoint;
	screenLayout.GetSaveAsPos(iTextPoint,listPoint);

	iListBoxRect = iSaveAsBmp->SizeInPixels();
	iListBoxRect.Move(iStartPoint);
	iListBoxRect.iTl.iY += listPoint.iY;
	TInt innerMargin = screenLayout.InnerMargin();
	iListBoxRect.Shrink(innerMargin,innerMargin);

}
//--------------------------------------------------------------------------
//ParseParentPath,该方法的实现比较曲折,不直观
//--------------------------------------------------------------------------
TFileName CFileBrowser::ParseParentPath(const TDesC& aPath) const
{
	//return UtilityTools::ParseParentPath(aPath);
	ASSERT(BaflUtils::PathExists(iFs,aPath));

	HBufC* tempPath = HBufC::NewLC(aPath.Length() - 1);
	tempPath->Des().Append(aPath.Left(aPath.Length() - 1));					//去掉最后一个反斜杠
	TParse parse;
	parse.Set(*tempPath,NULL,NULL);
	const TDesC& curFolder = parse.NameAndExt();
	TFileName newPath;
	TInt length = tempPath->Length() - curFolder.Length();
	newPath.Append(tempPath->Left(length));	//去掉当前文件夹名称
	CleanupStack::PopAndDestroy();

	return newPath;
}