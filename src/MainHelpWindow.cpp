/*
============================================================================
Name        : MainHelpWindow.cpp
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : CMainHelpWindow implementation
============================================================================
*/

#include "MainHelpWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "TypeDefine.h"

#include "ListBox.h"
#include "ListBoxItem.h"
#include "DisplayHelpWindow.h"

#include "BitmapFactory.h"
#include "PictureEngine.h"

#include "UtilityTools.h"

CMainHelpWindow::CMainHelpWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EHelpWindow,aParent,aMainEngine)
{
}


CMainHelpWindow::~CMainHelpWindow()
{
	DELETE(iItem);
	
	DELETE(iConfigItem);

}

CMainHelpWindow* CMainHelpWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMainHelpWindow* self = new (ELeave)CMainHelpWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMainHelpWindow* CMainHelpWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CMainHelpWindow* self=CMainHelpWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CMainHelpWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);

	//帮助配置文件的路径需要动态修改，待续（3）
	//_LIT(KHelpConfig,"C:\\system\\apps\\coco\\help\\helpConfig.txt");
	//LoadHelpConfig(KHelpConfig);//已加载了导航栏的信息
	
	TFileName helpConfigName;
	helpConfigName.Append(iMainEngine.GetAppPath());
	helpConfigName.Append(_L("help\\helpConfig.txt"));
	LoadHelpConfig(helpConfigName);//已加载了导航栏的信息
	//图标
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(EFileTypeOther);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(EFileTypeOther);
	//创建导航栏
	CreatNaviPan();//左右箭头没有奥
	//创建列表
	iHelpListbox=iMainEngine.ControlFactory().CreateDefaultListBoxL();
	//为列表添加初始数据
	AddListboxItem(0);

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CMainHelpWindow::DoActivateL()
{
}

void CMainHelpWindow::DoDeactivate()
{
	//DeleteAllControl();
}

void CMainHelpWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CMainHelpWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{//ENonCharacterKeyBase
	case EKeyDevice1:

		GoBackToParentWindow();
		break;
	case EKeyDevice0:	//左软键,确定，显示具体的帮助信息 
	case EKeyDevice3:
		{
			//用户点击ok键，响应,当前用户当前选择的列表项进入具体的
			//帮住信息，还要把数据传到MultiText
			//这里没有调用CreateChildWindow()函数，但其实现是一样的
	
			CDisplayHelpWindow * display = (CDisplayHelpWindow *)iMainEngine.WindowFactory().CreateWindow(EDisplayHelpWindow,this);
	
			SetChildWindow(display);
	
			//子视图需要传入四个东西：
			//具体帮助的题目，
			//所要显示的链接文字
			//和链接对应的模块（是个数字，以便进入）
			//存有帮助信息的文件已便显示，
	
			//获取当前的列表项
			//没有数据的情况index为多少没有考虑
			int index=0;
			index=iHelpListbox->CurItemIndex();
			//CNormalListBoxItem& item=(CNormalListBoxItem&)iHelpListbox->CurItem();
			
			if(index>=0)//有数据项
			{
				//由于所有的数据文件统一存放在一个数组中，所以最后的index
				//要乘以个基数还要加个数才行
				display->LoadTitle((*iItem)[index*4]);//帮助题目
				
				TFileName helpContentFile;
				helpContentFile.Append(iMainEngine.GetAppPath());
				//const TPtrC& ptr1 = (*iItem)[index*4+3];
				helpContentFile.Append((*iItem)[index*4+3]);
				display->LoadFileToMultiText(helpContentFile);//帮助文件的路径
				
				display->LoadLinkText((*iItem)[index*4+1]);//要显示的连接文字
	
				//将字符串（标识模块的）转化为数字
				const TPtrC& ptr = (*iItem)[index*4+2];
				TBuf<4>des;//4要用常量代替
				des.Append(ptr);
				TLex myLex(des);
				TInt model;
				myLex.Val(model);
	
				display->LoadEnterModel(model);//对应的模块(工厂中的数字)
			}
		}
	
		break;


	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;

}

TBool CMainHelpWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CMainHelpWindow::DoSizeChanged()
{

}

void CMainHelpWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	SetLeftButtonL(textRes[ETurkeyTextRes_Ok]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}

void CMainHelpWindow::InitPopUpMenu()
{

}


void CMainHelpWindow::NaviPaneIndexChanged(TInt aNewIndex)
{
	//要先清除列表以及iItem的数据，

	iHelpListbox->Reset();
	if (iItem!=NULL)
	{
		iItem->Reset();
	}

	AddListboxItem(aNewIndex);

}

void CMainHelpWindow::CreatNaviPan()
{
	iNaviPane = iMainEngine.ControlFactory().CreateDefaultNaviPane();
	iNaviPane->SetObserver(this);//设置观察器
	for(int i=0;i<iConfigItem->Count();)
	{
		iNaviPane->AppendItem((*iConfigItem)[i]);
		i+=2;//跳过数组中的路径信息，只加载导航栏信息

	}

}

void CMainHelpWindow::AddListboxItem(TInt aCurrentTab)
{
	//为列表加载一些初始项
	if (iConfigItem->Count()>2)//有数据项（一行文字一行路径）才加载
	{
		////以下涉及到的路径需要动态的添加（2）
		TFileName helpListContentFile;
		helpListContentFile.Append(iMainEngine.GetAppPath());//路径
		
		helpListContentFile.Append((*iConfigItem)[2*aCurrentTab+1]);//文件名
		LoadItem(helpListContentFile);
		//LoadItem((*iConfigItem)[2*aCurrentTab+1]);//读取列表数据到iItem数组（参数是个路径）

		//在列表中显示对应与该tab的帮助列表信息
		for(int i=0;i<iItem->Count();)
		{
			iHelpListbox->AppendL(CNormalListBoxItem::NewL( (*iItem)[i],iIconBitmap,iIconBitmapMask));
			i+=4;//只显示帮助的标题

		}
	}

}

void CMainHelpWindow::LoadHelpConfig(const TDesC& aFileName)
{

	int i=0;
	RFs& fs = CCoeEnv::Static()->FsSession();
	HBufC8* fileContent = UtilityTools::GetFileContentL(fs,aFileName);
	CleanupStack::PushL(fileContent);

	iConfigItem=NULL;
	iConfigItem = new (ELeave) CDesCArrayFlat(10);

	TPtrC ptr((const TUint16*)(fileContent->Ptr()),fileContent->Length()/2);
	//处理FF FE前缀
	const TDesC& des = ptr.Mid(1);


	//顺序读取配置文件中的数据，并不进行拆分，用的时候在处理
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//回车换行

		if(charNum == KErrNotFound)
		{
			iConfigItem->AppendL(tmpDes);
			break;//说明文件结束，退出

		}
		else
		{
			if (charNum!=0)//防止文件中有很多回车换行的情况
			{
				iConfigItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//回车换行，

	}	
	CleanupStack::PopAndDestroy();		//fileContent
}

void CMainHelpWindow::LoadItem(const TDesC& aFileName)
{

	int i=0;
	RFs& fs = CCoeEnv::Static()->FsSession();
	HBufC8* fileContent = UtilityTools::GetFileContentL(fs,aFileName);
	CleanupStack::PushL(fileContent);

	iItem=NULL;
	iItem = new (ELeave) CDesCArrayFlat(10);

	TPtrC ptr((const TUint16*)(fileContent->Ptr()),fileContent->Length()/2);
	//处理FF FE前缀
	const TDesC& des = ptr.Mid(1);


	//顺序读取配置文件中的数据，并不进行拆分，用的时候在处理
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//回车换行

		if(charNum == KErrNotFound)
		{
			iItem->AppendL(tmpDes);
			break;//说明文件结束，退出

		}
		else
		{
			if (charNum!=0)//防止文件中有很多回车换行的情况
			{
				iItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//回车换行，

	}	
	CleanupStack::PopAndDestroy();		//fileContent
}