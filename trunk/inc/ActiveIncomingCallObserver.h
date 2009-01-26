/*
============================================================================
 Name        : ActiveIncomingCallObserver.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CActiveIncomingCallObserver declaration
============================================================================
*/

#ifndef ACTIVEINCOMINGCALLOBSERVER_H
#define ACTIVEINCOMINGCALLOBSERVER_H

#include "IncomingCallObserver.h"
#ifdef __SERIES60_3X__
	#include <Etel3rdParty.h>
	#include <e32base.h>
#else
	#include <etel.h>
	#include <EIKENV.H>
#endif

class CActiveIncomingCallObserver : public CActive
{
	public:
	CActiveIncomingCallObserver(MIncomingCallObserverInterface* aObserver);
	~CActiveIncomingCallObserver();
	void ConstructL();
	void StartObserver();

	private:
	virtual void DoCancel();
	virtual void RunL();

	private:

		#ifndef __SERIES60_3X__
			TInt InitializePhone();
		#endif

	void ReleasePhone();

	private:

		MIncomingCallObserverInterface* iObserver;

		#ifdef __SERIES60_3X__
			TBuf<CTelephony::KPhoneSerialNumberSize>  m_nIMEI;
			TInt iFlag;  //0»ñÈ¡IMEIÂë×´Ì¬£¬1¼àÌýÀ´µç×´Ì¬
			TInt iState;

			CTelephony::TPhoneIdV1 iPhoneIdV1;
			CTelephony::TPhoneIdV1Pckg iPhoneIdV1Pckg;

   			CTelephony::TCallStatusV1 		iCurrentStatus;
			CTelephony::TCallStatusV1Pckg	iCurrentStatusPckg;	
			CTelephony*	 iTelephony;
		#else
			RCall::TStatus iCallStatus;
			TName iName;

			TBool iLineInited;
			TBuf <40> iTsyName;
			RTelServer iTelServer;
			RPhone iPhone;
			RLine iLine;

			TBool iCallActive;
			RCall iCall;
		#endif
};

#endif // ACTIVEINCOMINGCALLOBSERVER_H
