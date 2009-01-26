/*
============================================================================
 Name        : Window.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CWindow implementation
============================================================================
*/

#ifdef __SERIES60_3X__
	#include <akniconutils.h> 
#endif

#include "Window.h"
#include "Control.h"
#include "MainWindow.h"
#include "MainEngine.h"
//#include "WindowFactory.h"
#include "ControlFactory.h"
#include "ScreenLayout.h"
#include "Graphic.h" 
#include "UtilityTools.h"
#include "TextResourceEngine.h"
#include "TypeDefine.h"
#include "cocobmp.h"
#include "CoCoPreDefine.h"
#include "ControlText.h"
#include "BitmapFactory.h"
#include "DownLoadEngine.h"

//CWindow::CWindow(const CMainEngine& aMainEngine):iMainEngine(aMainEngine)
CWindow::CWindow(TInt aWindowType,CWindow* aParent,CMainEngine& aMainEngine): iWindowType(aWindowType)
																			, iParentWindow(aParent)
																			, iMainEngine(aMainEngine)
																			, iIsNormalWindow(ETrue)
																			, iCanPopUpWindow(ETrue)
																			, iShowControlText(ETrue)
{
}

CWindow::~CWindow()
{
// 	delete iLeftButtonTextArray;
// 	delete iRightButtonTextArray;
	iControlArray.ResetAndDestroy();
	iControlArray.Close();
}
//////////////////////////////////////////////////////////////////////////
//公共方法
//////////////////////////////////////////////////////////////////////////
void CWindow::ActivateL()
{
	//保证激活过不能重新激活
	ASSERT(!iIsActivated);
	iIsActivated = ETrue;

	if(iIsNormalWindow)
	{
		iBackgroundBmp = iMainEngine.CreateBmpById(EMbmCocobmpBg);
		iControlPaneBmp = iMainEngine.GetBitmapFactory().GetBitmapByWidth(EMbmCocobmpMenubg);

		if(iShowControlText)
		{
			CreateControlText();
		}
	}
	SetLayout();
	//ChangeButton();
	DoActivateL();
	ActivateAllControlL();
}

void CWindow::Deactivate()
{
	//保证只有激活过才能冻结
	if(iIsActivated)
	{
		iIsActivated = EFalse;
		delete iBackgroundBmp;
		iBackgroundBmp = NULL;
		delete iControlPaneBmp;
		iControlPaneBmp = NULL;
		if(iShowControlText)
		{
			RemoveControl(iControlText);
			delete iControlText;
			iControlText = NULL;
		}
		DeactivateAllControl();
		DoDeactivate();
	}
}

void CWindow::Draw(CGraphic& aGraphic)const
{
	if(iChildWindow)
	{
		iChildWindow->Draw(aGraphic);
	}
	else
	{
		if(iIsNormalWindow && iIsActivated)
		{
			DrawBackground(aGraphic);
			DrawButton(aGraphic);
		}
		DoDraw(aGraphic);
		DrawEachControl(aGraphic);
	}
}

TBool CWindow::KeyEventL(TInt aKeyCode)
{
	UtilityTools::WriteLogsL(_L("CWindow::KeyEventL"));
	TBool keyResult = ETrue;
	if(iChildWindow)
	{
		UtilityTools::WriteLogsL(_L("CWindow::KeyEventL iChildWindow Start"));
		UtilityTools::WriteLogsL(_L("CWindow::KeyEventL iChildWindow child type = %d"),iChildWindow->GetWindowType());
		keyResult = iChildWindow->KeyEventL(aKeyCode);
		UtilityTools::WriteLogsL(_L("CWindow::KeyEventL iChildWindow End"));
	}
	else
	{
		keyResult = HandleEachControlKeyL(aKeyCode);
		if(!keyResult)
		{
			keyResult = DoKeyEventL(aKeyCode);
		}
		if(!keyResult && iCanPopUpWindow)
		{
			keyResult = ETrue;
			switch(aKeyCode)
			{
			case '*':		//用户按下"*"键,打开播放器界面
				if(EOpenFileWindow_Music != iWindowType)
				{
					CreateChildWindow(EOpenFileWindow_Music);
				}
				break;

			case '#':		//用户按下"#"键,打开下载管理界面
				if(EDownloadManagerWindow != iWindowType&&iMainEngine.GetLoginType()==1)
				{
					CreateChildWindow(EDownloadManagerWindow);
				}
				break;

			case '0':		//用户按下"0"键,打开已下载界面
				if(EDownloadedWindow != iWindowType)
				{
					CreateChildWindow(EDownloadedWindow);
				}
				break;

			default:
				keyResult = EFalse;
				break;
			}
		}
	}
	UtilityTools::WriteLogsL(_L("CWindow::KeyEventL End"));
	return keyResult;
}

TBool CWindow::HandleCommandL(TInt aCommand)
{
	TBool commandResult = EFalse;
	if(iChildWindow)
	{
		commandResult = iChildWindow->HandleCommandL(aCommand);
	}
	else
	{
		commandResult = HandleEachControlCommandL(aCommand);
		if(!commandResult)
		{
			commandResult = DoHandleCommandL(aCommand);
		}
	}
	return commandResult;
}
//////////////////////////////////////////////////////////////////////////
//对该函数的调用机制有待进一步设计
//////////////////////////////////////////////////////////////////////////
void CWindow::SizeChanged()
{
	if(iChildWindow)
	{
		iChildWindow->SizeChanged();
	}
	else
	{
		SetLayout();
		DoSizeChanged();
		EachControlSizeChanged();
	}
}

void CWindow::SetLeftButtonL(const TDesC& aDes)
{
	ASSERT(aDes.Length() <= iLeftButtonText.MaxLength());
	iLeftButtonText.Zero();
	iLeftButtonText.Append(aDes);
}

void CWindow::SetRightButtonL(const TDesC& aDes)
{
	ASSERT(aDes.Length() <= iRightButtonText.MaxLength());
	iRightButtonText.Zero();
	iRightButtonText.Append(aDes);
}

void CWindow::ResetLeftButton()
{
// 	delete iLeftButtonTextArray;
// 	iLeftButtonTextArray = NULL;
}

void CWindow::ResetRightButton()
{
// 	delete iRightButtonTextArray;
// 	iRightButtonTextArray = NULL;
}

TInt CWindow::GetWindowType()const
{
	return iWindowType;
}

CWindow* CWindow::MakeChildWindow(TInt aWindowType)
{
	CreateChildWindow(aWindowType);
	return iChildWindow;
}
//////////////////////////////////////////////////////////////////////////
//实现以下两个接口以响应控件的请求
//////////////////////////////////////////////////////////////////////////
//From MDrawEventObserver
void CWindow::ControlRequestDraw()
{
	iMainEngine.RequestDraw();
}
//From MCleanUpObserver
void CWindow::CleanupControl(CControl* aControl)
{
	TBool result = RemoveAndDeleteControl(aControl);
	//ASSERT(result);
}
//////////////////////////////////////////////////////////////////////////
//聚合控制
//////////////////////////////////////////////////////////////////////////
TBool CWindow::AddControl(const CControl* aControl)
{
	TBool result = ETrue;
	for(TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		if(aControl == iControlArray[i])		//如果相同的指针已存在，则返回
		{
			result = EFalse;
			break;
		}
	}
	if(result)
	{
		TInt err = iControlArray.Append(aControl);
		result = KErrNone == err;
	}
	return result;
}

TBool CWindow::RemoveControl(const CControl* aControl)
{
	TBool result = EFalse;
	for (TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		if(aControl == iControlArray[i])
		{
			iControlArray.Remove(i);
			result = ETrue;
			break;
		}
	}
	return result;
}

TBool CWindow::RemoveAndDeleteControl(const CControl* aControl)
{
	TBool result = RemoveControl(aControl);
	if(result)
	{
		delete aControl;
	}
	return result;
}

void CWindow::DeleteAllControl()
{
	ResetLeftButton();
	ResetRightButton();
	ChangeButton();
	iControlArray.ResetAndDestroy();
}

void CWindow::ActivateAllControlL()
{
	for (TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		iControlArray[i]->ActivateL();
	}
}

void CWindow::DeactivateAllControl()
{
	for (TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		iControlArray[i]->Deactivate();
	}
}
//////////////////////////////////////////////////////////////////////////
//private
//////////////////////////////////////////////////////////////////////////
void CWindow::DrawBackground(CGraphic& aGraphic)const
{
	ASSERT(NULL != iBackgroundBmp);

	aGraphic.BitBlt(TPoint(0,0),iBackgroundBmp);
}

void CWindow::DrawButton(CGraphic& aGraphic)const
{
	ASSERT(NULL != iControlPaneBmp);

	TSize screenSize = UtilityTools::ScreenSize();	//temp code
	TSize size = iControlPaneBmp->SizeInPixels();
	
	TPoint point(0,0);
	point.iY += screenSize.iHeight;
	point.iY -= size.iHeight;
	aGraphic.BitBlt(point,iControlPaneBmp);
	aGraphic.SetPenColor(KTextColor);
	
	TRect controlPaneRect = iMainEngine.ScreenLayout().GetControlPaneRect();
	TInt margin = iMainEngine.ScreenLayout().InnerMargin();

	TPtrC leftButtonPtr;
	TPtrC rightButtonPtr;
	leftButtonPtr.Set(iLeftButtonText);
	rightButtonPtr.Set(iRightButtonText);
	if(iControlArray.Count() > 0)
	{
		for(TInt controlIndex = iControlArray.Count() - 1 ; controlIndex >= 0 ; controlIndex--)
		{
			CControl* lastControl = iControlArray[controlIndex];
			if(lastControl->HasButton())
			{
				leftButtonPtr.Set(lastControl->LeftButton());
				rightButtonPtr.Set(lastControl->RightButton());
				break;
			}
		}
	}
	aGraphic.DrawText(leftButtonPtr,controlPaneRect,CGraphicsContext::ELeft,margin);
	aGraphic.DrawText(rightButtonPtr,controlPaneRect,CGraphicsContext::ERight,margin);
}

void CWindow::DrawEachControl(CGraphic& aGraphic)const
{
	//自底向顶处理
	for (TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		const CControl* control = iControlArray[i];
		ASSERT(NULL != control);
		control->Draw(aGraphic);
	}
}

TBool CWindow::HandleEachControlKeyL(TInt aKeyCode)
{
	iMainEngine.WriteLog16(_L("CWindow::HandleEachControlKeyL"));
	TBool keyResult = EFalse;
	//自顶向底处理
	for (TInt i = iControlArray.Count() - 1 ; i >= 0 ; i--)
	{
		CControl* control = iControlArray[i];
		//UtilityTools::InfoPrint(control->GetControlType());
		keyResult = control->KeyEventL(aKeyCode);
		if(keyResult)
		{			
			break;		//事件已响应,结束处理
		}
	}
//	iMainEngine.WriteLog16(_L("CWindow::HandleEachControlKeyL End"));
	return keyResult;
}

TBool CWindow::HandleEachControlCommandL(TInt aCommand)
{
	TBool cmdResult = EFalse;
	//自顶向底处理
	for (TInt i = iControlArray.Count() - 1 ; i >= 0 ; i--)
	{
		CControl* control = iControlArray[i];
		cmdResult = control->HandleCommandL(aCommand);
		if(cmdResult)
		{			
			break;		//事件已响应,结束处理
		}
	}
	return cmdResult;
}

void CWindow::EachControlSizeChanged()
{
	TRect aScreenRect = iMainEngine.ScreenRect();
	for (TInt i = 0 ; i < iControlArray.Count() ; i++)
	{
		CControl* control = iControlArray[i];
		control->SizeChanged(aScreenRect);
	}
}
//////////////////////////////////////////////////////////////////////////
//界面控制函数
//////////////////////////////////////////////////////////////////////////
void CWindow::GoBackToParentWindow()
{
	if(iParentWindow)
	{
		iParentWindow->ResetChildWindow();
	}
	else
	{
		ASSERT(EMainWindow == iWindowType);		//目前只有CMainWindow没有父界面		
	}
}

void CWindow::GoBackToMainWindow()
{
	iMainEngine.GoBackToMainWindow();
}

void CWindow::CreateChildWindow(TInt aWindowType)
{
	ASSERT(NULL == iChildWindow);				//当前没有子界面时才能设置

	iChildWindow = iMainEngine.CreateWindow(aWindowType,this);
	iMainEngine.SetCurWindow(iChildWindow);
}

void CWindow::SetChildWindow(CWindow* aWindow)
{
	ASSERT(NULL == iChildWindow);				//当前没有子界面时才能设置

	iChildWindow = aWindow;
	iMainEngine.SetCurWindow(iChildWindow);
}

void CWindow::ResetChildWindow()
{
	if(iChildWindow)
	{
		//清空子界面的同时清空子界面的子界面链
		iChildWindow->ResetChildWindow();
		iMainEngine.SetCurWindow(this);
		delete iChildWindow;
		iChildWindow = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
//其它便利函数
//////////////////////////////////////////////////////////////////////////
void CWindow::SetLayout()
{
#ifdef __SERIES60_3X__
	if(iIsNormalWindow && iIsActivated)
	{
		const CScreenLayout& screenLayout = iMainEngine.ScreenLayout();
		TSize screenSize = iMainEngine.ScreenSize();

		AknIconUtils::SetSize(iBackgroundBmp,screenSize,EAspectRatioNotPreserved);
	}
#endif
}

void CWindow::GetLayout()
{

}

void CWindow::CreateControlText()
{
	ASSERT(NULL == iControlText);

	TPoint point=iMainEngine.ScreenLayout().GetControlPaneRect().iTl;
	TSize size=iMainEngine.ScreenLayout().GetControlPaneRect().Size();

	//以下代码中的描述符本应是两个汉字,但汉字无法在代码中正常表示,帮使用,四个英文字符代替
	TInt leftTextWidth =  iMainEngine.ScreenLayout().Font()->TextWidthInPixels(_L("abcd"));
	TInt rightTextWidth =  iMainEngine.ScreenLayout().Font()->TextWidthInPixels(_L("abcd"));

	point.iX=point.iX+leftTextWidth;
	size.iWidth=size.iWidth-leftTextWidth-rightTextWidth;

	TRect rect(point,size);

	iControlText = iMainEngine.ControlFactory().CreateControlText(rect);
}

//////////////////////////////////////////////////////////////////////////
//待修改:
//将创建对话框的操作移至CDownLoadEngine,并将该函数删除
//////////////////////////////////////////////////////////////////////////
TInt CWindow::AddDownLoadList(MDialogObserver& aObserver,TInt aCommond,const TDesC& aUrl, const TDesC& aName,const TInt aFileLen,const TInt aDownLoadLen,const TInt aType,const TInt aSave)
{
	TInt nRet=iMainEngine.GetDownLoadEngine().AddToDownLoadingList(aUrl,aName,aFileLen,aDownLoadLen,aType,aSave);

	if(nRet==0)
	{
		//添加成功 
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadAddSuccess),aObserver,aCommond);
	}
	else if(nRet==1)
	{
		//队列已满，添加不成功 
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadAddFail),aObserver,aCommond);
	}
	else if(nRet==2)
	{
		//重复下载，再次询问用户是否添加 
		iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadAgainInfo),aObserver,aCommond,-98);
	}
	else if(nRet==3)
	{
		//正在下载列表中 
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_URLInDownLoadList),aObserver,aCommond);
	}
	else if(nRet==4)
	{
		//磁盘空间不够 
		iMainEngine.ControlFactory().CreateTipDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoadFail_NoDiskSpace),aObserver,aCommond);
	}
	else if(nRet==5)
	{
		//文件类型不被支持 
		iMainEngine.ControlFactory().CreateInquireDialog(iMainEngine.GetDesById(ETurkeyTextRes_DownLoad_FileTypeNotSupportInfo),aObserver,aCommond,-98);
	}
	return nRet;
}
