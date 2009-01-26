/*
============================================================================
 Name        : DownloadDirSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadDirSettingWindow implementation
============================================================================
*/

#include "DownloadDirSettingWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "TypeDefine.h"
#include "ScreenLayout.h"
#include "Dialog.h"
#include "FileBrowser.h"
#include "MultiText.h"
#include "StaticCommonTools.h"
#include "FileReadByRow.h"
CDownloadDirSettingWindow::CDownloadDirSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CDownloadDirSettingWindow::~CDownloadDirSettingWindow()
{
}

CDownloadDirSettingWindow* CDownloadDirSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadDirSettingWindow* self = new (ELeave)CDownloadDirSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadDirSettingWindow* CDownloadDirSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadDirSettingWindow* self=CDownloadDirSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadDirSettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_DownloadDir_Setting]);

	InitMultiText();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDownloadDirSettingWindow::DoActivateL()
{
	


}

void CDownloadDirSettingWindow::DoDeactivate()
{
	
}

void CDownloadDirSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadDirSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)//�û�������ĳɹ���ʾ��Ϣ����ġ�ȷ������
	{
		GoBackToParentWindow();//���ظ���ͼ
	}
	if (aCommand==2)//�û�ȷ�ϸ���
	{
		if(iMainEngine.SetDownLoadDir(iNewPathName)==0)//���ĳɹ�
		{
		iMainEngine.ControlFactory().CreateTipDialog((*iRes->iItem)[ESystemManageRes_DownloadDir_SettingSuccess],*this,1);
		}
		
	}
	
}
////From MFileBrowserObserver
void CDownloadDirSettingWindow::DoFileEvent(const TDesC& aFileName,TInt aCommand)
{
	if (aCommand==1)//�û�ѡ�����ļ�·����
	{
		
		iNewPathName=aFileName;//����·������Ա����
		TBuf<200>display;//��ʾ��Ϣ�������ļ���·��+?
		display.Zero();
		display.Append((*iRes->iItem)[ESystemManageRes_DownLoadDir_Makesure]);
		
		display.Append(iNewPathName);
		display.Append(_L("?"));
		
		iMainEngine.ControlFactory().CreateInquireDialog(display,*this,2,3);
	}
}

TBool CDownloadDirSettingWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey://����
		{
			
			CFileBrowser* fileBrowser;
			fileBrowser=iMainEngine.ControlFactory().CreateFolderBrowser(*this,1,2);
			fileBrowser->SetTitle((*iRes->iItem)[ESystemManageRes_DownLoadDir_Select]);
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

TBool CDownloadDirSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDownloadDirSettingWindow::DoSizeChanged()
{

}

void CDownloadDirSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Setting_Change));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CDownloadDirSettingWindow::InitMultiText()
{
	TBuf<64> remainSpace;//��¼���̵�ʣ��ռ�
	TBuf<100>display;//˵����䣫ʣ��ռ�
	TInt64 nDriverSize;

	CMultiText* multiText = iMainEngine.ControlFactory().CreateDefaultMutliText();
	multiText->AppendL((*iRes->iItem)[ESystemManageRes_Current_Dir]);
	multiText->AppendL(iMainEngine.GetDownLoadDir());
	
	remainSpace.Zero();
	display.Zero();

	nDriverSize=::GetFreeSpace (_L("C"));//��λ�Ѿ���KB��
	remainSpace.Num(nDriverSize);
	display.Append((*iRes->iItem)[ESystemManageRes_DownloadDir_C]);
	display.Append(remainSpace);
	display.Append(_L("KB"));
	multiText->AppendL(display);

	remainSpace.Zero();
	display.Zero();

	nDriverSize=::GetFreeSpace (_L("E"));
	remainSpace.Num(nDriverSize);//��λ�Ѿ���KB��
	display.Append((*iRes->iItem)[ESystemManageRes_DownloadDir_E]);
	display.Append(remainSpace);
	display.Append(_L("KB"));

	multiText->AppendL(display);
	
	
}