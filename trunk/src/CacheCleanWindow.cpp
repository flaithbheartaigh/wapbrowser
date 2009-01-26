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
	if (aCommand==1)//ȷ�������Ҫ����û�����ݵ�ʱ���𣿣�
	{
		//��ʾ�ȴ��Ի���
		iMainEngine.ControlFactory().CreateWaitDialogWithoutCancel((*iRes->iItem)[ESystemManageRes_Wait],*this);
		if (iSelectGroup->GetSelectCheckBox(_L("a"),1)==true)
		{
			iMainEngine.CleanupCache();//ɾ�������б���
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
				::DeleteDir(path);//ɾ����Ϣ�б���
			}
			fs.Close();
			
		}

		if (1)//��ʾ�ɹ�����ʧ��(ÿ�ζ��ɹ���û��ʧ��)
		{//����Ի���Ḳ�ǵȴ��Ի�����Ȼ�ȴ��򻹴��ڣ�������Ϊ��ֱ�ӷ��ظ���ͼ����Ҫ��
			iMainEngine.ControlFactory().CreateTipDialog(\
				(*iRes->iItem)[ESystemManageRes_CacheCleanSuccess]\
				,*this,3);
			
		}
		//else //û��ʲô����
		//{
		//	iMainEngine.ControlFactory().CreateTipDialog(\
		//		(*iRes->iItem)[ESystemManageRes_CacheCleanFail]\
		//		,*this,2);
		//	//���ɹ������ڻ����������
		//}
		
	}
	if(aCommand==2)//ȡ�����
	{

	}
	if(aCommand==3)
	{
		GoBackToParentWindow();//�ɹ��ص��ϲ����
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