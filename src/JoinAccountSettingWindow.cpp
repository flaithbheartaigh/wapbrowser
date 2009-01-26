/*
============================================================================
 Name        : JoinAccountSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CJoinAccountSettingWindow implementation
============================================================================
*/
#include <CommDbConnPref.h>
#include <COMMDB.H>
#include "JoinAccountSettingWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "SelectGroup.h"
#include "TypeDefine.h"
#include "SelectBox.h"
#include "ScreenLayout.h"
#include "Dialog.h"
#include "FileReadByRow.h"

CJoinAccountSettingWindow::CJoinAccountSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CJoinAccountSettingWindow::~CJoinAccountSettingWindow()
{
	/*DELETE(iIapInfoList);*/
}

CJoinAccountSettingWindow* CJoinAccountSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CJoinAccountSettingWindow* self = new (ELeave)CJoinAccountSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CJoinAccountSettingWindow* CJoinAccountSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CJoinAccountSettingWindow* self=CJoinAccountSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CJoinAccountSettingWindow::ConstructL()
{
	ChangeButton();
	iIapInfoList=new (ELeave) TIapInfo[KMaxIapNum];
	iIapNum=0;

	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);

	iMainEngine.ControlFactory().SetParentWindow(this);
	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_JoinAccountSetting]);

	InitSelectGroup();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CJoinAccountSettingWindow::DoActivateL()
{
}

void CJoinAccountSettingWindow::DoDeactivate()
{
	
}

void CJoinAccountSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CJoinAccountSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
}



TBool CJoinAccountSettingWindow::DoKeyEventL(TInt aKeyCode)
{


	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			TInt selected=0;
			selected=iSelectGroup->GetSelectRedio(_L("a"));
			saved=iMainEngine.SetIapNum(iIapInfoList[selected].nIapID);


			if (saved==0)//提示成功，用户选择确定回到上层界面
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_JoinAccountSettingSuccess]\
					,*this,1);
				
				break;
			}
			else 
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_JoinAccountSettingFail]\
					,*this,2);
				break;
			}

		}
	case KRightSoftKey:
		GoBackToParentWindow();
		break;
	default:
		keyResult = EFalse;
		break;
	}

	return keyResult;
}

TBool CJoinAccountSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CJoinAccountSettingWindow::DoSizeChanged()
{

}

void CJoinAccountSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CJoinAccountSettingWindow::InitSelectGroup()
{
	
	GetIapInfo();
	TInt select;//联网方式的种类
	//TInt haveSelected=0;//标识是否已经有单选项被选中项,1表示有，0表示没有
	//					//以便防止网络数据有误的情况也有一个单选Redio显示选中的状态
	
	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_JoinAccountSetting0]);

	
	for(int i=0;i<iIapNum;i++)//动态的添加单选项
	{
		iSelectGroup->AddSelectItem(iIapInfoList[i].nIapName,_L("a"),ESelectRedio);
	}
	
	int i=0;
	select=iMainEngine.GetSavedIapNum();//初始时需要选中的项
	for(i;i<iIapNum;i++)//动态的添加单选项
	{
		if(select==iIapInfoList[i].nIapID)//
		{
			iSelectGroup->SetSelected(_L("a"),i);
			break;
		}

	}
	
	if ((i==iIapNum)&&(iIapNum!=0))//一种情况是从网络取的的数据有误,
	{
		//怎么处理呢？
	}
	if (iIapNum==0)//另一种情况是用户没有接入点
	{
		//提示一下?
	}
	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_JoinAccountSetting1]);

}
//获取联网方式
void  CJoinAccountSettingWindow::GetIapInfo()
{
	TUint32 iap_id;
	TBuf<KMaxIapNameLength> iap_name;
	TInt i=0;

	CCommsDatabase* lCommDb;
	lCommDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(lCommDb);  	
	CCommsDbTableView* iaptable ; 
	iaptable = lCommDb->OpenIAPTableViewMatchingBearerSetLC(ECommDbBearerGPRS,ECommDbConnectionDirectionOutgoing);
	TInt retval = iaptable->GotoFirstRecord();
	//初始化一下
	for(i=0;i<20;i++)
	{
		iIapInfoList[i].nIapID=0;
		iIapInfoList[i].nIapName.Zero();
		iIapInfoList[i].nIsUse=false;
	}
	i=0;
	while (retval == KErrNone)
	{ 		
		iaptable->ReadUintL(TPtrC( COMMDB_ID), iap_id );
		iaptable->ReadTextL(TPtrC( COMMDB_NAME), iap_name );

		iIapInfoList[i].nIapID=(TInt) iap_id;
		iIapInfoList[i].nIapName.Append(iap_name);
		iIapInfoList[i].nIsUse=true;
		i++;
		retval = iaptable->GotoNextRecord();
	} 	

	iIapNum=i;

	CleanupStack::PopAndDestroy(2); 
}
