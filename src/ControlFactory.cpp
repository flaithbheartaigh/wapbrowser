/*
============================================================================
 Name        : ControlFactory.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CControlFactory implementation
============================================================================
*/

#include "ControlFactory.h"

#include "MainEngine.h"
#include "Window.h"

#include "Graphic.h"
#include "ScreenLayout.h"
#include "BitmapFactory.h"

#include "ScrollBar.h"
#include "MultiText.h"
#include "ListBox.h"
#include "NaviPane.h"
#include "TitleBar.h"
#include "Dialog.h"
#include "PopUpMenu.h"
#include "FileBrowser.h"
#include "SlideText.h"
#include "InputDialog.h"
#include "SystemInfoDialog.h"
#include "PreDefine.h"
#include "SearchListBox.h"
#include "CopyToDialog.h"
#include "ControlText.h"
#include "CoCoPreDefine.h"
#include "GradeDialog.h"
#include "ContentInfoDialog.h"
#include "SelectAppWindow.h"
#include "LinkListBox.h"
#include "SelectGroup.h"


CControlFactory::CControlFactory(CMainEngine& aMainEngine) : iMainEngine(aMainEngine)
														   , iScreenLayout(aMainEngine.ScreenLayout())
{
}

CControlFactory::~CControlFactory()
{
	iBitmapCleanArray.ResetAndDestroy();
	iBitmapCleanArray.Close();
}

CControlFactory* CControlFactory::NewLC(CMainEngine& aMainEngine)
{
	CControlFactory* self = new (ELeave)CControlFactory(aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CControlFactory* CControlFactory::NewL(CMainEngine& aMainEngine)
{
	CControlFactory* self=CControlFactory::NewLC(aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CControlFactory::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CControlFactory::SetParentWindow(CWindow* aParentWindow)
{
	iParentWindow = aParentWindow;
}

//////////////////////////////////////////////////////////////////////////
CScrollBar*	CControlFactory::CreateDefaultScrollbarL()const
{
	TRect rect = iScreenLayout.WhiteBackgroundRect();
	return CreateScrollBar(rect);
}



CNaviPane* CControlFactory::CreateDefaultNaviPane(MNaviPaneObserver* aObserver,CWindow* aWindow) const
{
	TRect rect = iScreenLayout.GetNaviPaneRect();
	const CFont* font = iMainEngine.Graphic().GetFont();
	CNaviPane* naviPane = new (ELeave) CNaviPane(rect,*font,iMainEngine,aObserver);
	naviPane->ConstructL();

	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(naviPane);
	return naviPane;
}

CTitleBar* CControlFactory::CreateDefaultTitleBarL(const TDesC& aTitleText,CWindow* aWindow)const
{
	TRect rect = iScreenLayout.GetNaviPaneRect();
	const CFont* font = iMainEngine.Graphic().GetFont();
	CTitleBar* titleBar = CTitleBar::NewL(iMainEngine,rect,font,aTitleText);

	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(titleBar);
	return titleBar;
}

CListBox* CControlFactory::CreateListBox(const TRect& aRect,CWindow* aWindow) const
{
	const CFont* font = iScreenLayout.Font();
	TInt scrollBarWidth = iScreenLayout.ScrollBarWidth();

	CListBox* listBox = new (ELeave) CListBox(iMainEngine,aRect,font,scrollBarWidth);
	listBox->SetTextColor(KRgbWhite,KRgbWhite);

	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(listBox);
	return listBox;
}

CListBox* CControlFactory::CreateDefaultListBoxL(CWindow* aWindow)const
{
	//TRect clientRect = iScreenLayout.WhiteBackgroundRect();

	TRect clientRect = iScreenLayout.GetFullClientRect();
	clientRect.iTl.iY += iScreenLayout.TitleBarRect().Height();
	
	CListBox* listBox = CreateListBox(clientRect,aWindow);
	TInt margin = iScreenLayout.InnerMargin();
	//listBox->SetMargin(margin,margin);
	return listBox;
}

CMultiText* CControlFactory::CreateMultiText(const TRect& aRect,CWindow* aWindow)const
{
	CMultiText* multiText = CMultiText::NewL();
	multiText->SetMargin(5,3);
	const CFont* font = iScreenLayout.Font();
	TInt rowSpacing = font->HeightInPixels();
	rowSpacing = (rowSpacing>>1);
	multiText->Layout(aRect,font,iScreenLayout.ScrollBarWidth(),rowSpacing);
	multiText->SetTextColor(KRgbWhite);

	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(multiText);

	return multiText;
}

CMultiText* CControlFactory::CreateMultiTextFromFile(const TRect& aRect,const TDesC& aFileName,CWindow* aWindow) const
{
	CMultiText* multiText = CreateMultiText(aRect,aWindow);
	multiText->LoadFromFileL(aFileName);
	return multiText;
}

CMultiText* CControlFactory::CreateDefaultMutliText(CWindow* aWindow)const
{
	//TRect clientRect = iScreenLayout.WhiteBackgroundRect();
	TRect clientRect = iScreenLayout.GetFullClientRect();
	clientRect.iTl.iY += iScreenLayout.TitleBarRect().Height();
	CMultiText* multiText = CreateMultiText(clientRect,aWindow);
	TInt margin = iScreenLayout.InnerMargin();
	multiText->SetMargin(margin,margin);
	return multiText;
}

CMultiText* CControlFactory::CreateDefaultMutliTextFromFile(const TDesC& aFileName,CWindow* aWindow) const
{
	CMultiText* multiText = CreateDefaultMutliText(aWindow);
	multiText->LoadFromFileL(aFileName);
	return multiText;
}

CMultiText* CControlFactory::CreateDefaultMutliTextFromFile(const TDesC& aFileName,const TDesC& aText,CWindow* aWindow) const
{
	CMultiText* multiText = CreateDefaultMutliText(aWindow);
	multiText->LoadFromFileL(aFileName);
	multiText->AppendEndItem(aText);
	return multiText;
}
CScrollBar* CControlFactory::CreateScrollBar(const TRect& aRect)const
{
	CScrollBar* scrollbar = new (ELeave) CScrollBar;
	TPoint point = aRect.iTl;
	point.iX += aRect.Width() - iScreenLayout.ScrollBarWidth();
	scrollbar->SetBeginPostion(point);
	scrollbar->SetVerticalSpace(aRect.Height());
	scrollbar->SetScrollWidth(iScreenLayout.ScrollBarWidth());
	scrollbar->SetBackgroundColor(KScrollBackgroundColor);
	scrollbar->SetCursorColor(KScrollSpaceColor);
	scrollbar->SetMaxLinePerPage(iScreenLayout.LineNum());
	scrollbar->SetTotalLineNum(0);
	scrollbar->Layout();
	return scrollbar;
}

CDialog* CControlFactory::CreateTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow) const
{
	ASSERT(iParentWindow);
	CDialog* dialog = CDialog::NewL(aDes,aObserver,iMainEngine,CDialog::EDialogTip);
	dialog->SetIconType(EDialogSmile);
	dialog->SetLeftCommandType(aLeftCommand);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(KNullDesC);
	parentWindow->AddControl(dialog);
	//dialog->SetHasButton(ETrue);
	return dialog;
}

CDialog* CControlFactory::CreateSuccessTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow) const
{
	CDialog* dialog = CreateTipDialog(aDes,aObserver,aLeftCommand,aWindow);
	dialog->SetIconType(EDialogSuccess);
	return dialog;
}

CDialog* CControlFactory::CreateFailTipDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,CWindow* aWindow) const
{
	CDialog* dialog = CreateTipDialog(aDes,aObserver,aLeftCommand,aWindow);
	dialog->SetIconType(EDialogFail);
	return dialog;
}

CDialog* CControlFactory::CreateInquireDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aLeftCommand,TInt aRightCommand,CWindow* aWindow) const
{
	ASSERT(iParentWindow);
	CDialog* dialog = CDialog::NewL(aDes,aObserver,iMainEngine,CDialog::EDialogInquire);
	dialog->SetIconType(EDialogQuestion);
	dialog->SetLeftCommandType(aLeftCommand);
	dialog->SetRightCommandType(aRightCommand);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(dialog);
	//dialog->SetHasButton(ETrue);
	return dialog;
}

/*
CDialog* CControlFactory::CreateInquireDialog(const TDesC& aDes,MDialogObserver& aObserver,MCommondBase* aLeftCommand,MCommondBase* aRightCommand) const
{
	ASSERT(iParentWindow)
	CDialog* dialog = CDialog::NewL(aDes,aObserver,iMainEngine,CDialog::EDialogInquire);
	dialog->SetLeftCommand(aLeftCommand);
	dialog->SetRightCommand(aRightCommand);

	i//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	i//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	iParentWindow->AddControl(dialog);
	return dialog;
}
*/

CDialog* CControlFactory::CreateWaitDialog(const TDesC& aDes,MDialogObserver& aObserver,TInt aRightCommand,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CDialog* dialog = CDialog::NewL(aDes,aObserver,iMainEngine,CDialog::EDialogWaiting);
	dialog->SetRightCommandType(aRightCommand);
	dialog->SetIconType(EDialogWait);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(KNullDesC);
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(dialog);
	//dialog->SetHasButton(ETrue);
	return dialog;
}

CDialog* CControlFactory::CreateWaitDialogWithoutCancel(const TDesC& aDes,MDialogObserver& aObserver,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CDialog* dialog = CDialog::NewL(aDes,aObserver,iMainEngine,CDialog::EDialogWaitingWithoutCancel);
	dialog->SetIconType(EDialogWait);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(KNullDesC);
	//parentWindow->SetRightButtonL(KNullDesC);
	parentWindow->AddControl(dialog);
	//dialog->SetHasButton(ETrue);
	return dialog;
}

CSearchListBox* CControlFactory::CreateSearchListBox(MSearchListObserver& aObserver,CWindow* aWindow) const
{
	CSearchListBox*  sListBox= CSearchListBox::NewL(aObserver,iMainEngine);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(sListBox);
	return sListBox;
}

CPopUpMenu* CControlFactory::CreatePopUpMenu(MPopUpMenuObserver& aObserver,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CPopUpMenu* popUpMenu = CPopUpMenu::NewL(aObserver,iMainEngine);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(popUpMenu);
	//popUpMenu->SetHasButton(ETrue);
	return popUpMenu;
}

CFileBrowser* CControlFactory::CreateFileBrowser(MFileBrowserObserver& aObserver,TInt aLeftCommand,TInt aRightCommand,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CFileBrowser* fileBrowser = CFileBrowser::NewL(iMainEngine,aObserver,CFileBrowser::EBroswerFiles);
	fileBrowser->SetCommandType(aLeftCommand,aRightCommand);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(fileBrowser);
	//fileBrowser->SetHasButton(ETrue);
	return fileBrowser;
}

CFileBrowser* CControlFactory::CreateFolderBrowser(MFileBrowserObserver& aObserver,TInt aLeftCommand,TInt aRightCommand,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CFileBrowser* folderBrowser = CFileBrowser::NewL(iMainEngine,aObserver,CFileBrowser::EBrowserFolders);
	folderBrowser->SetCommandType(aLeftCommand,aRightCommand);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(folderBrowser);
	//folderBrowser->SetHasButton(ETrue);
	return folderBrowser;
}

CInputDialog* CControlFactory::CreateInputTextDialog(MInputObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aCommandType,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CInputDialog* inputDialog = CInputDialog::NewL(aObserver,iMainEngine,aCommandType);
	ASSERT(inputDialog);
	inputDialog->SetTitle(aTitle);
	inputDialog->SetInfo(aInfo);
	inputDialog->SetInputType(1);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(inputDialog);
	//inputDialog->SetHasButton(ETrue);
	return inputDialog;
}

CInputDialog* CControlFactory::CreateInputNumberDialog(MInputObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aCommandType,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CInputDialog* inputDialog = CInputDialog::NewL(aObserver,iMainEngine,aCommandType);
	ASSERT(inputDialog);
	inputDialog->SetTitle(aTitle);
	inputDialog->SetInfo(aInfo);
	inputDialog->SetInputType(2);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(inputDialog);
	//inputDialog->SetHasButton(ETrue);
	return inputDialog;
}
CSystemInfoDialog* CControlFactory::CreateInfoDialog(MInfoObserver& aObserver,const TDesC& aTitle,const TDesC& aInfo,TInt aType,CWindow* aWindow) const
{
	ASSERT(iParentWindow)
	CSystemInfoDialog* infoDialog = CSystemInfoDialog::NewL(aObserver,iMainEngine);
	infoDialog->SetShowInfo(aTitle,aInfo,aType);
	
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_NextStep));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(infoDialog);
	//infoDialog->SetHasButton(ETrue);
	return infoDialog;
}
CSlideText* CControlFactory::CreateSlideText(const TRect& aRect,const TDesC& aDefaultText,TInt aStyle,CWindow* aWindow) const
{
	CSlideText* slideText = CSlideText::NewL(iMainEngine,aStyle);
	slideText->SetFont(iMainEngine.Graphic().GetFont());
	slideText->SetTextColor(KTextColor);
	slideText->SetMoveSpeed(5);
	slideText->SetStopTime(50);
	slideText->Layout(ECenterAlign);
	slideText->SetClientRect(aRect);
	slideText->SetText(aDefaultText);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(slideText);
	return slideText;
}
CControlText* CControlFactory::CreateControlText(const TRect& aRect,CWindow* aWindow) const
{
	CControlText* controlText = CControlText::NewL(iMainEngine,iMainEngine);
	controlText->SetClientRect(aRect);
	controlText->SetSpeed(50);
	controlText->Begin();
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(controlText);
	return controlText;
}
CLinkListBox* CControlFactory::CreateLinkListBox(MLinkListObserver& aObserver,const TRect& aRect,CWindow* aWindow) const
{
	CLinkListBox* linkListBox=CLinkListBox::NewL(aObserver,iMainEngine);
	linkListBox->SetRect(aRect);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	parentWindow->AddControl(linkListBox);
	return linkListBox;
}
CControlText* CControlFactory::CreateDefaultControlText(CWindow* aWindow) const
{
	TPoint point=iScreenLayout.GetControlPaneRect().iTl;
	TSize size=iScreenLayout.GetControlPaneRect().Size();
	TRect rect(point,size);
	return CreateControlText(rect,aWindow);
}

CCopyToDialog* CControlFactory::CreateCopyToDialog(MCopyToObserver& aObserver,const TDesC& aTitle,CWindow* aWindow) const
{
	CCopyToDialog* copyToDialog = CCopyToDialog::NewL(aObserver,iMainEngine,aTitle);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(copyToDialog);
	//copyToDialog->SetHasButton(ETrue);
	return copyToDialog;
}
CGradeDialog* CControlFactory::CreateGradeDialog(MGradeObserver& aObserver,CWindow* aWindow) const
{
	CGradeDialog* gradeDialog = CGradeDialog::NewL(aObserver,iMainEngine);
	gradeDialog->SetTitle(iMainEngine.GetDesById(ETurkeyTextRes_FileGrade));
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(gradeDialog);
	//gradeDialog->SetHasButton(ETrue);
	return gradeDialog;
}
CContentInfoDialog* CControlFactory::CreateContentInfoDialog(MContentInfoObserver& aObserver,const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,TInt aCommand,TBool aBool,CWindow* aWindow) const
{
	CContentInfoDialog* contentInfoDialog = CContentInfoDialog::NewL(aObserver,iMainEngine,aCommand);
	contentInfoDialog->SetName(aName);
	contentInfoDialog->SetSize(aSize);
	contentInfoDialog->SetType(aType);
	contentInfoDialog->SetRemark(aRemark);
	contentInfoDialog->SetBmpShow(aBool);
	contentInfoDialog->InitBitmap();
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_MainDown));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(contentInfoDialog);
	//contentInfoDialog->SetHasButton(ETrue);
	return contentInfoDialog;
}
CSelectGroup* CControlFactory::CreateSelectBox(const TRect& aRect,CWindow* aWindow) const
{
	CSelectGroup* selectGroup = CSelectGroup::NewL(iMainEngine);
	selectGroup->InitLayout(aRect);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(selectGroup);
	//selectGroup->SetHasButton(ETrue);
	return selectGroup;
}
CSelectGroup* CControlFactory::CreateDefaultSelectBox(CWindow* aWindow) const
{
	//TPoint point=iMainEngine.ScreenLayout().GetClientRect().iTl;
	//TSize size=iMainEngine.ScreenLayout().GetClientRect().Size();

	//point.iY=point.iY+iMainEngine.ScreenLayout().TitleBarRect().iBr.iY;
	//size.iHeight=size.iHeight-iMainEngine.ScreenLayout().TitleBarRect().Size().iHeight;

	//return CreateSelectBox(TRect(point,size));
	TRect clientRect = iScreenLayout.GetFullClientRect();
	clientRect.iTl.iY += iScreenLayout.TitleBarRect().Height();
	return CreateSelectBox(clientRect,aWindow);
}
CContentInfoDialog* CControlFactory::CreateContentInfoDialog(MContentInfoObserver& aObserver,const TDesC& aName,const TDesC& aSize,const TDesC& aType,const TDesC& aRemark,const TDesC& aPath,TInt aCommand,TBool aBool,CWindow* aWindow) const
{
	CContentInfoDialog* contentInfoDialog = CContentInfoDialog::NewL(aObserver,iMainEngine,aCommand);
	contentInfoDialog->SetName(aName);
	contentInfoDialog->SetSize(aSize);
	contentInfoDialog->SetType(aType);
	contentInfoDialog->SetRemark(aRemark);
	contentInfoDialog->SetBmpShow(aBool);
	contentInfoDialog->SetBmpPath(aPath);
	contentInfoDialog->InitBitmap();

	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_MainDown));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(contentInfoDialog);
	//contentInfoDialog->SetHasButton(ETrue);
	return contentInfoDialog;
}
CSlideText* CControlFactory::CreateLeftCycleSlideText(const TRect aRect, const TDesC& aDefaultText,CWindow* aWindow) const
{
	return CreateSlideText(aRect,aDefaultText,CSlideText::ELeftCycle,aWindow);
}

CSelectAppWindow* CControlFactory::CreateSelectAppWindow(MSelectAppObserver& aObserver,CWindow* aWindow) const
{
	ASSERT(iParentWindow);
	CSelectAppWindow* selectappwindow = CSelectAppWindow::NewL(iMainEngine,aObserver);
	CWindow* parentWindow = NULL == aWindow ? iParentWindow : aWindow;
	ASSERT(parentWindow);
	//parentWindow->SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	//parentWindow->SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Cancel));
	parentWindow->AddControl(selectappwindow);
	//selectappwindow->SetHasButton(ETrue);
	return selectappwindow;
}

void CControlFactory::AssertClass() const
{
	ASSERT(iParentWindow);
}