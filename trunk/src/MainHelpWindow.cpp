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

	//���������ļ���·����Ҫ��̬�޸ģ�������3��
	//_LIT(KHelpConfig,"C:\\system\\apps\\coco\\help\\helpConfig.txt");
	//LoadHelpConfig(KHelpConfig);//�Ѽ����˵���������Ϣ
	
	TFileName helpConfigName;
	helpConfigName.Append(iMainEngine.GetAppPath());
	helpConfigName.Append(_L("help\\helpConfig.txt"));
	LoadHelpConfig(helpConfigName);//�Ѽ����˵���������Ϣ
	//ͼ��
	CBitmapFactory& bitmapFactory = iMainEngine.GetBitmapFactory();
	iIconBitmap = bitmapFactory.GetFileTypeIcon(EFileTypeOther);
	iIconBitmapMask = bitmapFactory.GetFileTypeIconMask(EFileTypeOther);
	//����������
	CreatNaviPan();//���Ҽ�ͷû�а�
	//�����б�
	iHelpListbox=iMainEngine.ControlFactory().CreateDefaultListBoxL();
	//Ϊ�б���ӳ�ʼ����
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
	case EKeyDevice0:	//�����,ȷ������ʾ����İ�����Ϣ 
	case EKeyDevice3:
		{
			//�û����ok������Ӧ,��ǰ�û���ǰѡ����б����������
			//��ס��Ϣ����Ҫ�����ݴ���MultiText
			//����û�е���CreateChildWindow()����������ʵ����һ����
	
			CDisplayHelpWindow * display = (CDisplayHelpWindow *)iMainEngine.WindowFactory().CreateWindow(EDisplayHelpWindow,this);
	
			SetChildWindow(display);
	
			//����ͼ��Ҫ�����ĸ�������
			//�����������Ŀ��
			//��Ҫ��ʾ����������
			//�����Ӷ�Ӧ��ģ�飨�Ǹ����֣��Ա���룩
			//���а�����Ϣ���ļ��ѱ���ʾ��
	
			//��ȡ��ǰ���б���
			//û�����ݵ����indexΪ����û�п���
			int index=0;
			index=iHelpListbox->CurItemIndex();
			//CNormalListBoxItem& item=(CNormalListBoxItem&)iHelpListbox->CurItem();
			
			if(index>=0)//��������
			{
				//�������е������ļ�ͳһ�����һ�������У���������index
				//Ҫ���Ը�������Ҫ�Ӹ�������
				display->LoadTitle((*iItem)[index*4]);//������Ŀ
				
				TFileName helpContentFile;
				helpContentFile.Append(iMainEngine.GetAppPath());
				//const TPtrC& ptr1 = (*iItem)[index*4+3];
				helpContentFile.Append((*iItem)[index*4+3]);
				display->LoadFileToMultiText(helpContentFile);//�����ļ���·��
				
				display->LoadLinkText((*iItem)[index*4+1]);//Ҫ��ʾ����������
	
				//���ַ�������ʶģ��ģ�ת��Ϊ����
				const TPtrC& ptr = (*iItem)[index*4+2];
				TBuf<4>des;//4Ҫ�ó�������
				des.Append(ptr);
				TLex myLex(des);
				TInt model;
				myLex.Val(model);
	
				display->LoadEnterModel(model);//��Ӧ��ģ��(�����е�����)
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
	//Ҫ������б��Լ�iItem�����ݣ�

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
	iNaviPane->SetObserver(this);//���ù۲���
	for(int i=0;i<iConfigItem->Count();)
	{
		iNaviPane->AppendItem((*iConfigItem)[i]);
		i+=2;//���������е�·����Ϣ��ֻ���ص�������Ϣ

	}

}

void CMainHelpWindow::AddListboxItem(TInt aCurrentTab)
{
	//Ϊ�б����һЩ��ʼ��
	if (iConfigItem->Count()>2)//�������һ������һ��·�����ż���
	{
		////�����漰����·����Ҫ��̬����ӣ�2��
		TFileName helpListContentFile;
		helpListContentFile.Append(iMainEngine.GetAppPath());//·��
		
		helpListContentFile.Append((*iConfigItem)[2*aCurrentTab+1]);//�ļ���
		LoadItem(helpListContentFile);
		//LoadItem((*iConfigItem)[2*aCurrentTab+1]);//��ȡ�б����ݵ�iItem���飨�����Ǹ�·����

		//���б�����ʾ��Ӧ���tab�İ����б���Ϣ
		for(int i=0;i<iItem->Count();)
		{
			iHelpListbox->AppendL(CNormalListBoxItem::NewL( (*iItem)[i],iIconBitmap,iIconBitmapMask));
			i+=4;//ֻ��ʾ�����ı���

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
	//����FF FEǰ׺
	const TDesC& des = ptr.Mid(1);


	//˳���ȡ�����ļ��е����ݣ��������в�֣��õ�ʱ���ڴ���
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//�س�����

		if(charNum == KErrNotFound)
		{
			iConfigItem->AppendL(tmpDes);
			break;//˵���ļ��������˳�

		}
		else
		{
			if (charNum!=0)//��ֹ�ļ����кܶ�س����е����
			{
				iConfigItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//�س����У�

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
	//����FF FEǰ׺
	const TDesC& des = ptr.Mid(1);


	//˳���ȡ�����ļ��е����ݣ��������в�֣��õ�ʱ���ڴ���
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//�س�����

		if(charNum == KErrNotFound)
		{
			iItem->AppendL(tmpDes);
			break;//˵���ļ��������˳�

		}
		else
		{
			if (charNum!=0)//��ֹ�ļ����кܶ�س����е����
			{
				iItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//�س����У�

	}	
	CleanupStack::PopAndDestroy();		//fileContent
}