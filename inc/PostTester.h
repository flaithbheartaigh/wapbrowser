/*
============================================================================
 Name        : PostTester.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPostTester declaration
============================================================================
*/

#ifndef POSTTESTER_H
#define POSTTESTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CWapHttpEngine;

class CPostTester 
	: public CBase
	, public MWapHttpEngineObserver
{
public: // Constructors and destructor
	~CPostTester();
	static CPostTester* NewL();
	static CPostTester* NewLC();

private:
	CPostTester();
	void ConstructL();

public://From MWapHttpEngineObserver
	virtual void HttpOk(const TDesC8& aData);
	virtual void HttpEmpty();
	virtual void HttpFailed();

public:
	void Test();

private:
	CWapHttpEngine* iWapHttpEngine;
};

#endif // POSTTESTER_H
