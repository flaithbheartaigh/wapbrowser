/*
============================================================================
 Name        : CacheCleanWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CCacheCleanWindow implementation
============================================================================
*/
#include <BAUTILS.H>
#include "CacheCleanWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "SelectGroup.h"
#include "TypeDefine.h"
#include "SelectBox.h"
#include "ScreenLayout.h"
#include "StaticCommonTools.h"
#include "FileReadByRow.h"
#include "Dialog.h"

CCacheCleanWindow::CCacheCleanWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CCacheCleanWindow::~CCacheCleanWindow()
{
}

CCacheCleanWindow* CCacheCleanWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CCacheCleanWindow* self = new (ELeave)CCacheCleanWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CCacheCleanWindow* CCacheCleanWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CCacheCleanWindow* self=CCacheCleanWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CCacheCleanWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_CacheClean]);

	InitSelectGroup();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CCacheCleanWindow::DoActivateL()
{
	
}

void CCacheCleanWindow::DoDeactivate()
{
	
}

void CCacheCleanWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CCacheCleanWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)//确定清除，要考虑没有内容的时候吗？？
	{
		//显示等待对话框
		iMainEngine.ControlFactory().CreateWaitDialogWithoutCancel((*iRes->iItem)[ESystemManageRes_Wait],*this);
		if (iSelectGroup->GetSelectCheckBox(_L("a"),1)==true)
		{
			iMainEngine.CleanupCache();//删除内容列表缓存
		}
		if (iSelectGroup->GetSelectCheckBox(_L("a"),0)==true)
		{
			TFileName path;
			path.Append(iMainEngine.GetAppPath());
			path.Append(_L("tempCInfo\\"));
			RFs fs;
			User::LeaveIfError(fs.Connect());
			if(BaflUtils::PathExists(fs,path))
			{
				::DeleteDir(path);//删除信息列表缓存
			}
			fs.Close();
			
		}

		if (1)//提示成功或者失败(每次都成功，没有失败)
		{//这个对话框会覆盖等待对话框，虽然等待框还存在，但是因为是直接返回父视图，不要紧
			iMainEngine.ControlFactory().CreateTipDialog(\
				(*iRes->iItem)[ESystemManageRes_CacheCleanSuccess]\
				,*this,3);
			
		}
		//else //没有什么用噢
		//{
		//	iMainEngine.ControlFactory().CreateTipDialog(\
		//		(*iRes->iItem)[ESystemManageRes_CacheCleanFail]\
		//		,*this,2);
		//	//不成功还留在缓存清除界面
		//}
		
	}
	if(aCommand==2)//取消清除
	{

	}
	if(aCommand==3)
	{
		GoBackToParentWindow();//成功回到上层界面
	}
}



TBool CCacheCleanWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			iMainEngine.ControlFactory().CreateInquireDialog((*iRes->iItem)[ESystemManageRes_CacheClean_Makesure],*this,1,2);
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

TBool CCacheCleanWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CCacheCleanWindow::DoSizeChanged()
{

}

void CCacheCleanWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CCacheCleanWindow::InitSelectGroup()
{

	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();
	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_CacheClean0]);

	iSelectGroup->AddSelectItem((*iRes->iItem)[ESystemManageRes_CacheCleanKind1],_L("a"),ESelectCheckBox);
	iSelectGroup->AddSelectItem((*iRes->iItem)[ESystemManageRes_CacheCleanKind2],_L("a"),ESelectCheckBox);

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_CacheClean1]);
	
}