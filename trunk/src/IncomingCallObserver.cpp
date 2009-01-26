/*
============================================================================
 Name        : IncomingCallObserver.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CIncomingCallObserver implementation
============================================================================
*/

#include "IncomingCallObserver.h"
#include "ActiveIncomingCallObserver.h"
//--------------------------------------------
// CIncomingCallObserver()
//--------------------------------------------
CIncomingCallObserver::CIncomingCallObserver()
{
	iActive = NULL;
}

//--------------------------------------------
// ~CIncomingCallObserver()
//--------------------------------------------
CIncomingCallObserver::~CIncomingCallObserver()
{
	delete iActive;
}

//--------------------------------------------
// CIncomingCallObserver* NewL()
//--------------------------------------------
CIncomingCallObserver* CIncomingCallObserver::NewL(MIncomingCallObserverInterface* aObserver)
{
	CIncomingCallObserver* self = NewLC(aObserver);
	CleanupStack::Pop(self);

	return self;
}

//--------------------------------------------
// IncomingCallObserver* NewLC()
//--------------------------------------------
CIncomingCallObserver* CIncomingCallObserver::NewLC(MIncomingCallObserverInterface* aObserver)
{
	CIncomingCallObserver* self = new(ELeave) CIncomingCallObserver();
	CleanupStack::PushL(self);

	self->ConstructL(aObserver);

	return self;
}

//--------------------------------------------
// void ConstructL()
//--------------------------------------------
void CIncomingCallObserver::ConstructL(MIncomingCallObserverInterface* aObserver)
{
	iActive = new (ELeave) CActiveIncomingCallObserver(aObserver);
	iActive->ConstructL();

	iActive->StartObserver();
}
