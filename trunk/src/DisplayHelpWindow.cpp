/*
============================================================================
Name        : CDisplayHelpWindow.cpp
Author      : smallcat
Version     :
Copyright   : Your copyright notice
Description : CBarCodeHelpWindow implementation
============================================================================
*/

#include "DisplayHelpWindow.h"

#include "MainEngine.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "NaviPane.h"
#include "TitleBar.h"
#include "TypeDefine.h"

CDisplayHelpWindow::CDisplayHelpWindow(CWindow* aParent,CMainEngine& aMainEngine) : CWindow(EHelpWindow,aParent,aMainEngine)
{
}


CDisplayHelpWindow::~CDisplayHelpWindow()
{

	//DELETE(iDisplayText);

}

CDisplayHelpWindow* CDisplayHelpWindow::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDisplayHelpWindow* self = new (ELeave)CDisplayHelpWindow(aParent,aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL( );
	return self;
}

CDisplayHelpWindow* CDisplayHelpWindow::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CDisplayHelpWindow* self=CDisplayHelpWindow::NewLC(aParent,aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CDisplayHelpWindow::ConstructL()
{
	ChangeButton();

	((CControlFactory&)iMainEngine.ControlFactory()).SetParentWindow(this);
	//SetParent��ʾҪΪ�ĸ����洴���ؼ�



}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDisplayHelpWindow::DoActivateL()
{
}

void CDisplayHelpWindow::DoDeactivate()
{
	//DeleteAllControl();

}

void CDisplayHelpWindow::DoDraw(CGraphic& aGraphic) const
{

}

TBool CDisplayHelpWindow::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	//if (iMultiText->GetEndFlag())//���������ֳ��ֵ�ʱ����ʾ��ȷ����
	//{
	//	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	//	SetLeftButtonL(textRes[ETurkeyTextRes_Ok]);
	//}
	switch(aKeyCode)
	{
	case KRightSoftKey:
		GoBackToParentWindow();
		break;
	case KLeftSoftKey:
		
	case EKeyDevice3://ok��
		//�����û�����İ�����Ŀ�Ĳ�ͬ�����벻ͬ��ģ��
		{
			if (iMultiText->GetEndFlag())//�������ұ�ѡ�в�������Ӧ��ģ��
			{
				switch(iModelNumber)
				{
				case EDownloadManagerWindow:
					CreateChildWindow(EDownloadManagerWindow);
					break;
				case EMainSearchView:
					CreateChildWindow(EMainSearchView);
					break;
				case ERecommendWindow:
					CreateChildWindow( ERecommendWindow);
					break;
					
				case EDownloadedWindow://�����أ�
					CreateChildWindow(EDownloadedWindow);
					break;
				case EResultWindow://��������
					CreateChildWindow( EResultWindow);
					break;

				case EFriendLinkView://����
					CreateChildWindow( EFriendLinkView);
					break;
				case ENewestdownView://��������
					CreateChildWindow( ENewestdownView);
					break;
				case ESutraDownloadView://��Ʒ����.
					CreateChildWindow( ESutraDownloadView);
					break;
				case ESystemManageWindow:
					CreateChildWindow( ESystemManageWindow);
					break;
				case EUserAdviseWindow:
					CreateChildWindow( EUserAdviseWindow);
					break;
				default:
					keyResult = EFalse;
					break;				
				}
			}

		}

	default:
		keyResult = EFalse;
		break;
	}
	return keyResult;
}

TBool CDisplayHelpWindow::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDisplayHelpWindow::DoSizeChanged()
{

}

void CDisplayHelpWindow::ChangeButton()
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	//SetLeftButtonL(textRes[ETurkeyTextRes_Option]);
	SetRightButtonL(textRes[ETurkeyTextRes_Back]);
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CDisplayHelpWindow::InitPopUpMenu()
{

}

void  CDisplayHelpWindow::LoadFileToMultiText(const TDesC& aFileName)
{

	//�ļ��ļ��أ������ڰ���������ͼ���õ�ʱ���������·�����ļ�����

	iMultiText=iMainEngine.ControlFactory().CreateDefaultMutliTextFromFile(aFileName);
	
	iMultiText->SetObserver(this);//�������������Ƿ���ֵĹ۲���

}
void CDisplayHelpWindow::LoadTitle(const TDesC& aTitleText)
{
	//_LIT(KName,"help");

	iMainEngine.ControlFactory().CreateDefaultTitleBarL(aTitleText);
}
void CDisplayHelpWindow::LoadEnterModel(TInt aModelNumber)
{
	iModelNumber=aModelNumber;
}

void CDisplayHelpWindow::LoadLinkText(const TDesC& aLinkText)
{
	if(aLinkText.CompareC(_L("no"))!=0)//��û�����ӵ�ʱ�����������ǡ�no��
	{
		iMultiText->AppendEndItem(aLinkText);//��������
	}
}

void CDisplayHelpWindow::MultiEvent(TInt aEvent)
{
	const CArrayFixFlat<TTEXTRES>& textRes = iMainEngine.GetTextRes();
	
	if (aEvent==1)//���֡�ȷ���������
	{
		SetLeftButtonL(textRes[ETurkeyTextRes_Ok]);
	} 
	if (aEvent==0)//ʹ�������ȷ������ʧ
	{
		SetLeftButtonL(_L(""));
	} 
}