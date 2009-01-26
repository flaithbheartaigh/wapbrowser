/*
============================================================================
 Name        : IncomingCallObserver.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CIncomingCallObserver declaration
============================================================================
*/

#ifndef INCOMINGCALLOBSERVER_H
#define INCOMINGCALLOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CActiveIncomingCallObserver;

class MIncomingCallObserverInterface
{
public: // MIncomingCallObserverInterface
virtual void IncomingCallObserverInterface_HandleItL(TInt aType,const TDesC& aText) = 0;
};


class CIncomingCallObserver : public CBase
{
public:
static CIncomingCallObserver* NewL(MIncomingCallObserverInterface* aObserver);
static CIncomingCallObserver* NewLC(MIncomingCallObserverInterface* aObserver);
~CIncomingCallObserver();

protected:
CIncomingCallObserver();

private:
void ConstructL(MIncomingCallObserverInterface* aObserver);

private:
CActiveIncomingCallObserver* iActive;
};

#endif // INCOMINGCALLOBSERVER_H
