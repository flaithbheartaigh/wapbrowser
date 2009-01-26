/*
============================================================================
 Name        : NotifyTimer.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CNotifyTimer implementation
============================================================================
*/

#include "NotifyTimer.h"
#include "PreDefine.h"
#include "UtilityTools.h"

CNotifyTimer::CNotifyTimer(TInt aPriority,MTimerNotifier& aTimerNotifier):CTimer(aPriority),iTimerNotifier(aTimerNotifier)
{
}


CNotifyTimer::~CNotifyTimer()
{
	CTimer::Cancel();
}

CNotifyTimer* CNotifyTimer::NewLC(TInt aPriority,MTimerNotifier& aTimerNotifier)
{
	CNotifyTimer* self = new (ELeave)CNotifyTimer(aPriority,aTimerNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CNotifyTimer* CNotifyTimer::NewL(MTimerNotifier& aTimerNotifier,TInt aPriority)
{
	CNotifyTimer* self=CNotifyTimer::NewLC(aPriority,aTimerNotifier);
	CleanupStack::Pop(); // self;
	return self;
}

void CNotifyTimer::ConstructL()
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this) ;
}

void CNotifyTimer::Start(TTimeIntervalMicroSeconds32 anInterval)
{
	ASSERT(!CTimer::IsActive());
	//CTimer::After()
	iInterval = anInterval;
	CTimer::After(iInterval);
}

void CNotifyTimer::Stop()
{
	CTimer::Cancel();
}

void CNotifyTimer::RunL()
{
	if (iStatus == KErrNone)
	{
		UtilityTools::WriteLogsL(_L("CNotifyTimer::RunL"));
		TBool result = iTimerNotifier.DoPeriodTask();
		UtilityTools::WriteLogsL(_L("CNotifyTimer::RunL End"));
		if(result)
		{
			CTimer::After(iInterval);
		}
	}
}