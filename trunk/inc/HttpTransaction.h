/*
============================================================================
 Name        : HttpTransaction.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CHttpTransaction declaration
============================================================================
*/

#ifndef HTTPTRANSACTION_H
#define HTTPTRANSACTION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

//class CWapBrowserAppUi;
class CHTTPEngine;

class CHttpTransaction : public CBase
{
public: // Constructors and destructor
	~CHttpTransaction();
	static CHttpTransaction* NewL();
	static CHttpTransaction* NewLC();

private:
	CHttpTransaction();
	void ConstructL();

public:
	CHTTPEngine& HTTPEngine();

private:
	CHTTPEngine* iHTTPEngine;
};

#endif // HTTPTRANSACTION_H
