//
//	TimeOut.cpp
//

#include "TimeOut.h"


CMyTimer::CMyTimer(TInt32 nTimeOutTime)
{
	SetTimeOutTime(nTimeOutTime);
}

CMyTimer::~CMyTimer()
{
}
void CMyTimer::SetTimeOutTime(TInt32 nTimeOutTime)
{
	m_nTimeOutTime = nTimeOutTime;
}
void CMyTimer::Begin()
{
	m_oLastTimeStamp.UniversalTime();
}
void CMyTimer::Begin(TInt32 nTimeOutTime)
{
	m_nTimeOutTime = nTimeOutTime;
	this->Begin();
}
TBool CMyTimer::Check()
{
	TTime timeNow;
	timeNow.UniversalTime();
	TTimeIntervalMicroSeconds	elapsedMicroSec = timeNow.MicroSecondsFrom(m_oLastTimeStamp);
	TInt64	nMs = elapsedMicroSec.Int64()/1000;
	if (nMs >= (TInt64)((TInt)m_nTimeOutTime))
		return	ETrue;
	else
		return	EFalse;
}
