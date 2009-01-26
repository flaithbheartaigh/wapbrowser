/*
============================================================================
 Name        : DownloadVoiceSettingWindow.cpp
 Author      : smallcat
 Version     :
 Copyright   : Your copyright notice
 Description : CDownloadVoiceSettingWindow implementation
============================================================================
*/

#include "DownloadVoiceSettingWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "SelectGroup.h"
#include "TypeDefine.h"
#include "SelectBox.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "FileReadByRow.h"
#include "Dialog.h"

CDownloadVoiceSettingWindow::CDownloadVoiceSettingWindow(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CDownloadVoiceSettingWindow::~CDownloadVoiceSettingWindow()
{
}

CDownloadVoiceSettingWindow* CDownloadVoiceSettingWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadVoiceSettingWindow* self = new (ELeave)CDownloadVoiceSettingWindow( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDownloadVoiceSettingWindow* CDownloadVoiceSettingWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDownloadVoiceSettingWindow* self=CDownloadVoiceSettingWindow::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDownloadVoiceSettingWindow::ConstructL()
{
	ChangeButton();
	TFileName path;
	path.Append(iMainEngine.GetAppPath());
	path.Append(_L("systemManage.txt"));
	iRes=CFileReadByRow::NewL();
	iRes->LoadItem(path);
	
	iMainEngine.ControlFactory().SetParentWindow(this);

	iMainEngine.ControlFactory().CreateDefaultTitleBarL((*iRes->iItem)[ESystemManageRes_Download_VoiceSetting]);

	InitSelectGroup();

}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDownloadVoiceSettingWindow::DoActivateL()
{
	

}

void CDownloadVoiceSettingWindow::DoDeactivate()
{
	
}

void CDownloadVoiceSettingWindow::DoDraw(CGraphic& aGraphic) const
{

}
//////////////////////////////////////////////////////////////////////////
//From MDialogObserver
//////////////////////////////////////////////////////////////////////////
void CDownloadVoiceSettingWindow::DialogEvent(TInt aCommand)
{
	if (aCommand==1)
	{
		GoBackToParentWindow();
	}
}



TBool CDownloadVoiceSettingWindow::DoKeyEventL(TInt aKeyCode)
{


	TBool keyResult = ETrue;
	TInt saved=0;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
		{
			if (iSelectGroup->GetSelectCheckBox(_L("a"),0)==true)//ѡ��
			{
				if (iMainEngine.SetDownLoadVoice(true)==0)
				{
					saved=1;
				}
			} 
			else//û��ѡ��
			{
				if (iMainEngine.SetDownLoadVoice(false)==0)
				{
					saved=1;
				}
			}
			if (saved==1)//��ʾ���óɹ�,�û�ѡ��ȷ���ŵ��ϲ����
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_Download_VoiceSettingSuccess]\
					,*this,1);
				break;
			}
			else //����ʧ��
			{
				iMainEngine.ControlFactory().CreateTipDialog(\
					(*iRes->iItem)[ESystemManageRes_Download_VoiceSettingFail]\
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

TBool CDownloadVoiceSettingWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDownloadVoiceSettingWindow::DoSizeChanged()
{

}

void CDownloadVoiceSettingWindow::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CDownloadVoiceSettingWindow::InitSelectGroup()
{


	//iSelectGroup = CSelectGroup::NewL(iMainEngine);

	//TInt topX;//SelectBox���Ͻ�x����
	//TInt topY;//SelectBox���Ͻ�y����
	//TInt height;//SelectBox�ĸ߶�
	//TInt width;//SelectBox�Ŀ��

	//TRect naviRect=iMainEngine.ScreenLayout().GetNaviPaneRect();
	//TRect clientRect=iMainEngine.ScreenLayout().GetClientRect();

	//topX=(naviRect.iTl).iX;
	//topY=(naviRect.iTl).iY+naviRect.Height();

	//height=clientRect.Height()-naviRect.Height();
	//width=naviRect.Width();

	//iSelectGroup->InitLayout( TRect( TPoint(topX,topY),TSize(width,height) ) );
	iSelectGroup=iMainEngine.ControlFactory().CreateDefaultSelectBox();

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_Download_VoiceSetting0]);


	iSelectGroup->AddSelectItem(\
		(*iRes->iItem)[ESystemManageRes_VoiceSetting_Yes]\
		,_L("a"),ESelectCheckBox);//���ǡ�,�������ݴ���
	
	switch(iMainEngine.HaveDownLoadVoice()==true)//
	{
	case 1://�Ѿ������ˣ�ѡ��
		iSelectGroup->SetSelected(_L("a"),0);
		break;
	default://û�����ã���ѡ��
		break;
	}

	iSelectGroup->AddTextItem((*iRes->iItem)[ESystemManageRes_Download_VoiceSetting1]);
	

	//AddControl(iSelectGroup);
}