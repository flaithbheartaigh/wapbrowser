/*
============================================================================
 Name        : Control.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CControl implementation
============================================================================
*/

#include "Control.h"
#include "ControlObserver.h"
#include "PreDefine.h"

CControl::CControl(TInt aControlType) : iControlType(aControlType)
{
}


CControl::~CControl()
{
}

void CControl::ActivateL()
{
	iIsActive = ETrue;
}

void CControl::Deactivate()
{
	iIsActive = EFalse;
}

const TDesC& CControl::LeftButton() const
{
	return KNullDesC;
}
const TDesC& CControl::RightButton() const
{
	return KNullDesC;
}

TInt CControl::GetControlType() const
{
	return iControlType;
}

void CControl::SetDrawEventObserver(MDrawEventObserver* aDrawEventObserver)
{
	ASSERT(aDrawEventObserver);
	iDrawEventObserver = aDrawEventObserver;
}

void CControl::SetCleanUpObserver(MCleanUpObserver* aCleanUpObserver)
{
	ASSERT(aCleanUpObserver);
	iCleanUpObserver = aCleanUpObserver;
}

/*
void CControl::SetHasButton(TBool aHasButton)
{
	iHasButton = aHasButton;
}
*/

TBool CControl::HasButton() const
{
	return iHasButton;
}

void CControl::RequestDraw()
{
	ASSERT(iDrawEventObserver);
	iDrawEventObserver->ControlRequestDraw();
}

void CControl::CleanupSelf()
{
	ASSERT(iCleanUpObserver);
	iCleanUpObserver->CleanupControl(this);
}
