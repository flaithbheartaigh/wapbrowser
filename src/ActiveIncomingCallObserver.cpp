/*
============================================================================
 Name        : ActiveIncomingCallObserver.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CActiveIncomingCallObserver implementation
============================================================================
*/

#include "ActiveIncomingCallObserver.h"

//--------------------------------------------
// CActiveIncomingCallObserver(MIncomingCallObserverInterface* aObserver)
//--------------------------------------------
#ifdef __SERIES60_3X__
CActiveIncomingCallObserver::CActiveIncomingCallObserver(MIncomingCallObserverInterface* aObserver)
: CActive(20),iCurrentStatusPckg(iCurrentStatus),iPhoneIdV1Pckg(iPhoneIdV1),iFlag(0),iObserver(aObserver)
{

}
#else
CActiveIncomingCallObserver::CActiveIncomingCallObserver(MIncomingCallObserverInterface* aObserver)
: CActive(0),iObserver(aObserver)
{
	iLineInited = EFalse;
	iCallActive = EFalse;
}
#endif

//--------------------------------------------
// ~CActiveIncomingCallObserver()
//--------------------------------------------
CActiveIncomingCallObserver::~CActiveIncomingCallObserver()
{
// Cancel any request, if outstanding
	Cancel();
 
	#ifdef __SERIES60_3X__
		delete iTelephony;
	#endif

}

//--------------------------------------------
// void ConstructL()
//--------------------------------------------
void CActiveIncomingCallObserver::ConstructL()
{
	#ifdef __SERIES60_3X__
		iTelephony = CTelephony::NewL();
	#endif

	CActiveScheduler::Add(this);
}

//--------------------------------------------
// void DoCancel()
//--------------------------------------------
void CActiveIncomingCallObserver::DoCancel()
{
	ReleasePhone();
}

//--------------------------------------------
// void RunL()
//--------------------------------------------
void CActiveIncomingCallObserver::RunL()
{
	if( iStatus==KErrNone )
    {
		#ifdef __SERIES60_3X__
			if(iFlag==0)
			{
				m_nIMEI.Zero();
				m_nIMEI.Append(iPhoneIdV1.iSerialNumber);
				iCurrentStatus.iStatus = CTelephony::EStatusUnknown;
				iObserver->IncomingCallObserverInterface_HandleItL(2,m_nIMEI);
				ReleasePhone();
				iFlag=1;
			}
			else
			{
				if( iCurrentStatus.iStatus == CTelephony::EStatusIdle)
				{
					iObserver->IncomingCallObserverInterface_HandleItL(0,_L("idle"));
				}
				else
				{
					iObserver->IncomingCallObserverInterface_HandleItL(1,_L("incoming"));
				}
			}
			StartObserver();
		#else
			if (iCallActive)
			{
				// We are polling for the RCall status
				if (iCallStatus == RCall::EStatusIdle)
				{
					iCallActive = EFalse;
					iCall.Close();

					iLine.NotifyIncomingCall(iStatus, iName);

					if (iObserver)
					{
						iObserver->IncomingCallObserverInterface_HandleItL(0,_L("phone idle"));
					}
				}
				else
				{
					iCall.NotifyStatusChange(iStatus, iCallStatus);
				}
			}
			else
			{
				// There's an incoming call!
				iCallActive = ETrue;
				iCall.OpenExistingCall(iLine, iName);
				iCall.NotifyStatusChange(iStatus, iCallStatus);
				if (iObserver)
				{
					iObserver->IncomingCallObserverInterface_HandleItL(1,_L("phone incoming"));
				}
			}

			// And we re-enable the polling
			SetActive();
		#endif
    }

}

//--------------------------------------------
// void StartObserver()
//--------------------------------------------
void CActiveIncomingCallObserver::StartObserver()
{
	#ifdef __SERIES60_3X__
		if(iFlag==0)
			iTelephony->GetPhoneId(iStatus, iPhoneIdV1Pckg);
		else
			iTelephony->NotifyChange(iStatus,CTelephony::EVoiceLineStatusChange,iCurrentStatusPckg);

		SetActive();
	#else
		Cancel();
		if (InitializePhone() == KErrNone)
		{
			SetActive();
		}
	#endif
}
#ifndef __SERIES60_3X__
//--------------------------------------------
// TInt InitializePhone()
//--------------------------------------------
TInt CActiveIncomingCallObserver::InitializePhone()
{
	TInt theError;

	if (iLineInited)
	{
		return KErrNone;
	}

	theError = iTelServer.Connect();
	if (theError)
	{
		return theError;
	}

	// Find the number of phones available from the tel server
	TInt numberPhones;
	theError = iTelServer.EnumeratePhones(numberPhones);
	if (theError)
	{
		iTelServer.Close();
		return theError;
	}

	// Check there are available phones
	if (numberPhones < 1)
	{
		iTelServer.Close();
		return KErrNotFound;
	}

	// Read the TSY module name
	theError = iTelServer.GetTsyName(0, iTsyName);
	if (theError != KErrNone)
	{
		iTelServer.Close();
		return theError;
	}

	// Load in the phone device driver
	theError = iTelServer.LoadPhoneModule(iTsyName);
	if (theError)
	{
		iTelServer.Close();
		return theError;
	}

	// Get info about the first available phone
	RTelServer::TPhoneInfo info;
	theError = iTelServer.GetPhoneInfo(0, info);
	if (theError)
	{
		iTelServer.UnloadPhoneModule(iTsyName);
		iTelServer.Close();
		return theError;
	}

	// Use this info to open a connection to the phone, the phone is identified by its name
	theError = iPhone.Open(iTelServer, info.iName);
	if (theError)
	{
		iTelServer.UnloadPhoneModule(iTsyName);
		iTelServer.Close();
		return theError;
	}

	// Get info about the first line from the phone
	RPhone::TLineInfo lineInfo;
	theError = iPhone.GetLineInfo(0, lineInfo);
	if (theError)
	{
		iPhone.Close();
		iTelServer.UnloadPhoneModule(iTsyName);
		iTelServer.Close();
		return theError;
	}

	// Use this to open a line
	theError = iLine.Open(iPhone, lineInfo.iName);
	if (theError)
	{
		iPhone.Close();
		iTelServer.UnloadPhoneModule(iTsyName);
		iTelServer.Close();
		return theError;
	}

	iLine.NotifyIncomingCall(iStatus, iName);
	iLineInited = ETrue;

	return KErrNone;
}

#endif
// ---------------------------------------------------------
// void ReleasePhone() 
// ---------------------------------------------------------
void CActiveIncomingCallObserver::ReleasePhone()
{
	#ifdef __SERIES60_3X__
		if(iFlag==0)
			iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
		else
			iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	#else
		if (iLineInited)
		{
			if (iCallActive)
			{
				iCall.NotifyStatusChangeCancel();
				iCall.Close();
			}
			else
			{
				iLine.NotifyIncomingCallCancel();
			}
			iLine.Close();
			iPhone.Close();
			iTelServer.UnloadPhoneModule(iTsyName);
			iTelServer.Close();

			iLineInited = EFalse;
		}
	#endif
}
