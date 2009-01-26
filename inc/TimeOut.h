//
//	TimeOut.h
//
#ifndef	__MYTIMER_H
#define	__MYTIMER_H

#include <e32std.h>

class CMyTimer
{
public:
	CMyTimer(TInt32 nTimeOutTime = 0);
	~CMyTimer();

protected:
	TInt32	m_nTimeOutTime;
	TTime	m_oLastTimeStamp;

public:
	void  Begin(TInt32 nTimeOutTime);
	void  Begin();
	TBool Check();
	void  SetTimeOutTime(TInt32 nTimeOutTime);
};

#endif
