/*
============================================================================
 Name        : NotifyTimer.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CNotifyTimer declaration
============================================================================
*/

#ifndef NOTIFYTIMER_H
#define NOTIFYTIMER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

//////////////////////////////////////////////////////////////////////////
//MTimerNotifier:CNotifyTimer中的事件产生时用该接口回调
//当返回值为true时继续请求时间事件,否则停止事件
//////////////////////////////////////////////////////////////////////////
class MTimerNotifier
{
public:
	virtual TBool DoPeriodTask() = 0;
};
//////////////////////////////////////////////////////////////////////////
//CNotifyTimer
//////////////////////////////////////////////////////////////////////////
class CNotifyTimer : public CTimer
{
public: // Constructors and destructor
	static CNotifyTimer* NewL(MTimerNotifier& aTimerNotifier,TInt aPriority = EPriorityStandard);
	static CNotifyTimer* NewLC(TInt aPriority,MTimerNotifier& aTimerNotifier);
	~CNotifyTimer();

private:
	CNotifyTimer(TInt aPriority,MTimerNotifier& aTimerNotifier);
	void ConstructL();

public:
	void Start(TTimeIntervalMicroSeconds32 anInterval);
	void Stop();

protected:
	void RunL();

private:
	TTimeIntervalMicroSeconds32 iInterval;
	MTimerNotifier&	iTimerNotifier;
};

#endif // NOTIFYTIMER_H
