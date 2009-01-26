/*
============================================================================
 Name        : SelectBoxView.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CSelectBoxView implementation
============================================================================
*/

#include "SelectBoxView.h"
#include "MainEngine.h"
#include "ControlFactory.h"
#include "WindowFactory.h"
#include "SelectGroup.h"
#include "TypeDefine.h"
#include "SelectBox.h"
#include "ScreenLayout.h"

CSelectBoxView::CSelectBoxView(CWindow* aParent,CMainEngine& aMainEngine) 
:CWindow(ESelectBoxView,aParent,aMainEngine)
,iMainEngine(aMainEngine)
{
	// No implementation required
}


CSelectBoxView::~CSelectBoxView()
{
}

CSelectBoxView* CSelectBoxView::NewLC(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSelectBoxView* self = new (ELeave)CSelectBoxView( aParent, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSelectBoxView* CSelectBoxView::NewL(CWindow* aParent,CMainEngine& aMainEngine)
{
	CSelectBoxView* self=CSelectBoxView::NewLC( aParent, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSelectBoxView::ConstructL()
{
	
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSelectBoxView::DoActivateL()
{
	InitSelectGroup();
}

void CSelectBoxView::DoDeactivate()
{
	DeleteAllControl();
}

void CSelectBoxView::DoDraw(CGraphic& aGraphic) const
{

}

TBool CSelectBoxView::DoKeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KLeftSoftKey:
	case KOKKey:
		{

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

TBool CSelectBoxView::DoHandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CSelectBoxView::DoSizeChanged()
{

}

void CSelectBoxView::ChangeButton()
{
	SetLeftButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Ok));
	SetRightButtonL(iMainEngine.GetDesById(ETurkeyTextRes_Back));
}

void CSelectBoxView::InitSelectGroup()
{
	CSelectGroup* selectGroup = CSelectGroup::NewL(iMainEngine);
	selectGroup->InitLayout(iMainEngine.ScreenLayout().GetClientRect());
	selectGroup->AddTextItem(_L("asdfsdafsdf87sd6f bs7f97sdf987sa9f67sa9df6as6df"));
	selectGroup->AddSelectItem(_L("aaaaaaaa1"),_L("a"),ESelectRedio);
	selectGroup->AddSelectItem(_L("aaaaaaaa2"),_L("a"),ESelectRedio);
	selectGroup->AddSelectItem(_L("aaaaaaaa3"),_L("a"),ESelectRedio,ETrue);
	selectGroup->AddTextItem(_L("sdf5s4fd65s4f6s46f4sa6f1s3f54sd6f4s6dfs6df4s6f4"));
	selectGroup->AddSelectItem(_L("bbbbbbbb1"),_L("b"),ESelectCheckBox);
	selectGroup->AddSelectItem(_L("bbbbbbbb2"),_L("b"),ESelectCheckBox);
	selectGroup->AddSelectItem(_L("bbbbbbbb3"),_L("b"),ESelectCheckBox);
	selectGroup->AddTextItem(_L("asdfsdafsdf87sd6f bs7f97sdf987sa9f67sa9df6as6df"));
	selectGroup->AddTextItem(_L("asdfsdafsdf87sd6f bs7f97sdf987sa9f67sa9df6as6df"));
	selectGroup->AddTextItem(_L("asdfsdafsdf87sd6f bs7f97sdf987sa9f67sa9df6as6df"));
	AddControl(selectGroup);
}